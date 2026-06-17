#!/bin/sh
# dtstyle_sysapply.sh - merge font resources into /etc/dt/app-defaults/Dtstyle
#
# Invoked by pkexec from RequestSystemWideApply() in Protocol.c
# (programs/dtstyle/Protocol.c). The caller writes the new Xrm resource
# lines to a per-invocation temp file, then execs this script as root
# with the temp file path as the single argument.
#
# Usage: dtstyle_sysapply.sh /tmp/dtstyle-sysfont-XXXXXXXXXX
#
# This script:
#   1. Validates the input file exists and is readable
#   2. Verifies it is being run as root
#   3. Creates /etc/dt/app-defaults if needed
#   4. Merges the new resources into /etc/dt/app-defaults/Dtstyle using
#      an Xrm-compatible merge: existing + new are combined, and the
#      last value for any duplicated key wins. This is NOT a raw
#      `cat >>`; it uses awk to deduplicate by key.
#   5. Removes the temp file on success.
#
# Exit codes:
#   0  success
#   1  invalid usage / missing input file
#   2  not root
#   3  destination directory creation failed
#   4  merge failed

set -eu

TMPFILE=${1:-}
DESTDIR=/etc/dt/app-defaults
DESTFILE=$DESTDIR/Dtstyle

# 1. Validate input
if [ -z "$TMPFILE" ] || [ ! -f "$TMPFILE" ] || [ ! -r "$TMPFILE" ]; then
    echo "Usage: $0 <tmpfile>" >&2
    exit 1
fi

# 2. Root check. The script must be pkexec-launched; reject any other
#    invocation that is not from uid 0.
if [ "$(id -u)" -ne 0 ]; then
    echo "$0: must be run as root" >&2
    exit 2
fi

# 3. Ensure destination directory exists.
if [ ! -d "$DESTDIR" ]; then
    mkdir -p "$DESTDIR" || exit 3
fi

# 4. Merge.
#
# Strategy: concatenate existing + new and run them through awk to
# produce an Xrm-style merge where the last value for each resource key
# wins. Keys are identified by the leading `*<qualifier>.<name>:` token
# on a line (Xrm loose-binding format used by CDE app-defaults).
#
# Comments (lines starting with '!') and blank lines are preserved
# verbatim; only resource lines (starting with '*') participate in
# dedup.
if [ -f "$DESTFILE" ]; then
    cat "$DESTFILE" "$TMPFILE" | awk '
        NF == 0 || $1 ~ /^!/ {
            print
            next
        }
        $1 ~ /^\*/ {
            key = $1
            sub(/:.*$/, "", key)
            data[key] = $0
            if (!(key in seen)) {
                order[++n] = key
                seen[key] = 1
            }
            next
        }
        {
            print
        }
        END {
            for (i = 1; i <= n; i++)
                print data[order[i]]
        }
    ' > "${DESTFILE}.tmp" || { rm -f "${DESTFILE}.tmp"; exit 4; }
    if [ ! -s "${DESTFILE}.tmp" ]; then
        echo "$0: merge produced empty result, aborting" >&2
        rm -f "${DESTFILE}.tmp"
        exit 4
    fi
    mv "${DESTFILE}.tmp" "$DESTFILE"
else
    # No existing file - just install the temp file as the new content.
    cp "$TMPFILE" "$DESTFILE" || exit 4
fi

# Best-effort mode fix; ignore failures (e.g. immutable fs).
chmod 644 "$DESTFILE" 2>/dev/null || true

# 5. Clean up the temp file the caller passed in.
rm -f "$TMPFILE"

exit 0
