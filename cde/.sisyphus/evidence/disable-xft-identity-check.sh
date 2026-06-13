#!/bin/bash
# ==============================================================================
# disable-xft-identity-check.sh
# Verify that --disable-xft build has no Xft symbol leakage
#
# Guardrail G2: --disable-xft must produce byte-identical binaries to
# the pre-migration code. This script checks that no Xft/fontconfig
# symbols leak into the built libraries and programs.
#
# USAGE:
#   ./cde/.sisyphus/evidence/disable-xft-identity-check.sh
#
# PREREQUISITES:
#   - Clean checkout of CDE source
#   - X11 development libraries installed
#   - Autotools (autoconf, automake, libtool) installed
#   - nproc available (or edit JOBS below)
#
# EXPECTED RESULT:
#   Exit 0 = PASS (no Xft symbols found)
#   Exit 1 = FAIL (Xft symbols leaked into --disable-xft build)
# ==============================================================================

set -e

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
CDE_ROOT="$(cd "$SCRIPT_DIR/../../" && pwd)"
JOBS="${JOBS:-$(nproc 2>/dev/null || echo 4)}"

cd "$CDE_ROOT"

echo "============================================================"
echo "disable-xft-identity-check"
echo "CDE root: $CDE_ROOT"
echo "Jobs: $JOBS"
echo "============================================================"

# Step 1: Build with --disable-xft
echo ""
echo ">>> Building with --disable-xft..."
./autogen.sh > /dev/null 2>&1
./configure --disable-xft > /dev/null 2>&1
make -j"$JOBS" > /dev/null 2>&1

# Step 2: Check for Xft symbol leakage in key libraries
echo ""
echo ">>> Checking for Xft symbol leakage in libraries..."
LEAKED=0

for lib in lib/DtFont/.libs/libDtFont.so \
            lib/DtTerm/TermPrim/.libs/libDtTermPrim.so \
            lib/DtTerm/TermView/.libs/libDtTermView.so \
            lib/DtHelp/.libs/libDtHelp.so \
            lib/DtWidget/.libs/libDtWidget.so; do
    if [ -f "$lib" ]; then
        if nm -D "$lib" 2>/dev/null | grep -q "Xft\|FcInit\|FcFontMatch"; then
            echo "FAIL: Xft/fontconfig symbols found in $lib"
            nm -D "$lib" 2>/dev/null | grep "Xft\|FcInit\|FcFontMatch" | head -5
            LEAKED=1
        else
            echo "PASS: No Xft symbols in $lib"
        fi
    else
        echo "SKIP: $lib not found (not built)"
    fi
done

# Step 3: Check for Xft symbol leakage in key programs
echo ""
echo ">>> Checking for Xft symbol leakage in programs..."

for prog in programs/dtwm/.libs/dtwm \
            programs/dtlogin/.libs/dtlogin \
            programs/dthello/.libs/dthello \
            programs/dtcm/dtcm/.libs/dtcm \
            programs/dtksh/.libs/dtksh; do
    if [ -f "$prog" ]; then
        if nm -D "$prog" 2>/dev/null | grep -q "Xft\|FcInit"; then
            echo "FAIL: Xft/fontconfig symbols found in $prog"
            nm -D "$prog" 2>/dev/null | grep "Xft\|FcInit" | head -5
            LEAKED=1
        else
            echo "PASS: No Xft symbols in $prog"
        fi
    else
        echo "SKIP: $prog not found (not built)"
    fi
done

# Step 4: Report result
echo ""
echo "============================================================"
if [ $LEAKED -eq 1 ]; then
    echo "RESULT: FAIL - Xft symbols leaked into --disable-xft build"
    echo "Guardrail G2 violation: --disable-xft must produce"
    echo "byte-identical binaries to pre-migration code."
    echo "============================================================"
    exit 1
else
    echo "RESULT: PASS - No Xft symbol leakage in --disable-xft build"
    echo "Guardrail G2 satisfied."
    echo "============================================================"
    exit 0
fi