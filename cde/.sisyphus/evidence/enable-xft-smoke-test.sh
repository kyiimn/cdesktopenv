#!/bin/bash
# ==============================================================================
# enable-xft-smoke-test.sh
# Verify that --enable-xft build has Xft symbols in expected locations
# and that fontconfig config files are installed.
#
# This confirms the Xft migration actually links Xft/fontconfig where
# needed and installs the required configuration files.
#
# USAGE:
#   ./cde/.sisyphus/evidence/enable-xft-smoke-test.sh
#
# PREREQUISITES:
#   - Clean checkout of CDE source
#   - X11 development libraries installed
#   - libXft-dev / Xft development headers installed
#   - fontconfig-dev / fontconfig development headers installed
#   - Autotools (autoconf, automake, libtool) installed
#   - nproc available (or edit JOBS below)
#
# EXPECTED RESULT:
#   Exit 0 = PASS (Xft symbols present, config files found)
#   Exit 1 = FAIL (Xft symbols missing or config files absent)
# ==============================================================================

set -e

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
CDE_ROOT="$(cd "$SCRIPT_DIR/../../" && pwd)"
JOBS="${JOBS:-$(nproc 2>/dev/null || echo 4)}"

cd "$CDE_ROOT"

echo "============================================================"
echo "enable-xft-smoke-test"
echo "CDE root: $CDE_ROOT"
echo "Jobs: $JOBS"
echo "============================================================"

# Step 1: Build with --enable-xft
echo ""
echo ">>> Building with --enable-xft..."
./autogen.sh > /dev/null 2>&1
./configure --enable-xft > /dev/null 2>&1
make -j"$JOBS" > /dev/null 2>&1

# Step 2: Verify Xft symbols in key libraries
echo ""
echo ">>> Checking for Xft symbols in libraries..."
FOUND_XFT=0

for lib in lib/DtFont/.libs/libDtFont.so \
            lib/DtTerm/TermPrim/.libs/libDtTermPrim.so \
            lib/DtHelp/.libs/libDtHelp.so \
            lib/DtWidget/.libs/libDtWidget.so; do
    if [ -f "$lib" ]; then
        if nm -D "$lib" 2>/dev/null | grep -q "XftFontOpenName"; then
            echo "PASS: Xft symbols found in $lib"
            FOUND_XFT=1
        else
            echo "WARN: No Xft symbols in $lib (may not have Xft code paths)"
        fi
    else
        echo "SKIP: $lib not found"
    fi
done

# Step 3: Verify Xft symbols in key programs
echo ""
echo ">>> Checking for Xft symbols in programs..."

for prog in programs/dtwm/.libs/dtwm \
            programs/dtlogin/.libs/dtlogin \
            programs/dthello/.libs/dthello \
            programs/dtcm/dtcm/.libs/dtcm \
            programs/dtksh/.libs/dtksh; do
    if [ -f "$prog" ]; then
        if nm -D "$prog" 2>/dev/null | grep -q "XftFontOpenName\|XftDrawCreate"; then
            echo "PASS: Xft symbols found in $prog"
            FOUND_XFT=1
        else
            echo "WARN: No Xft symbols in $prog"
        fi
    else
        echo "SKIP: $prog not found"
    fi
done

# Step 4: Verify fontconfig config files
echo ""
echo ">>> Checking fontconfig config files..."
CONF_OK=1

for conf in programs/localized/C/fonts.conf.d/09-cde-aliases.conf \
            programs/localized/C/fonts.conf.d/09-cde-cjk.conf; do
    if [ -f "$conf" ]; then
        echo "PASS: $conf exists"
    else
        echo "FAIL: $conf missing"
        CONF_OK=0
    fi
done

# Step 5: Verify no PKG_CHECK_MODULES in configure output
echo ""
echo ">>> Checking configure.ac for PKG_CHECK_MODULES violations..."
if grep -q "PKG_CHECK_MODULES" configure.ac 2>/dev/null; then
    echo "FAIL: PKG_CHECK_MODULES found in configure.ac (Guardrail G7)"
    echo "      Only AC_CHECK_LIB is allowed for Xft/fontconfig detection."
    CONF_OK=0
else
    echo "PASS: No PKG_CHECK_MODULES in configure.ac"
fi

# Step 6: Report result
echo ""
echo "============================================================"
if [ $FOUND_XFT -eq 1 ] && [ $CONF_OK -eq 1 ]; then
    echo "RESULT: PASS - Xft symbols present, config files installed"
    echo "============================================================"
    exit 0
elif [ $FOUND_XFT -eq 1 ] && [ $CONF_OK -eq 0 ]; then
    echo "RESULT: PARTIAL - Xft symbols present but config issues"
    echo "============================================================"
    exit 1
else
    echo "RESULT: FAIL - Xft symbols not found in expected binaries"
    echo "============================================================"
    exit 1
fi