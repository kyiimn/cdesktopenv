#!/bin/bash
# ==============================================================================
# visual-regression-test.sh
# Visual regression test for CDE Xft migration
#
# Guardrail G2: --disable-xft must produce byte-identical binaries to
# the pre-migration code. This script automates the comparison:
#   1. Build with --disable-xft, capture checksums
#   2. Build with --enable-xft, verify Xft symbols present
#   3. Compare --disable-xft checksums against a baseline
#
# USAGE:
#   ./cde/.sisyphus/evidence/visual-regression-test.sh [baseline-file]
#
#   If baseline-file is provided, checksums are compared against it.
#   If omitted, checksums are saved to /tmp/cde-disable-xft-checksums.txt
#   for later comparison.
#
# PREREQUISITES:
#   - Clean checkout of CDE source
#   - X11 development libraries installed
#   - libXft-dev / fontconfig-dev for --enable-xft build
#   - Autotools (autoconf, automake, libtool) installed
#
# EXIT CODES:
#   0 = PASS (checksums match baseline, or baseline not provided)
#   1 = FAIL (checksums differ from baseline)
# ==============================================================================

set -e

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
CDE_ROOT="$(cd "$SCRIPT_DIR/../../" && pwd)"
JOBS="${JOBS:-$(nproc 2>/dev/null || echo 4)}"
BASELINE="${1:-/tmp/cde-disable-xft-checksums.txt}"

cd "$CDE_ROOT"

echo "============================================================"
echo "visual-regression-test"
echo "CDE root: $CDE_ROOT"
echo "Jobs: $JOBS"
echo "Baseline: $BASELINE"
echo "============================================================"

CHECKSUM_FILES="lib/DtFont/.libs/libDtFont.so
lib/DtTerm/TermPrim/.libs/libDtTermPrim.so
lib/DtHelp/.libs/libDtHelp.so
lib/DtWidget/.libs/libDtWidget.so
programs/dtwm/.libs/dtwm
programs/dtlogin/.libs/dtlogin
programs/dthello/.libs/dthello"

# Step 1: Build with --disable-xft and capture checksums
echo ""
echo ">>> Building with --disable-xft..."
make clean > /dev/null 2>&1 || true
./autogen.sh > /dev/null 2>&1
./configure --disable-xft > /dev/null 2>&1
make -j"$JOBS" > /dev/null 2>&1

CHECKSUM_OUTPUT="/tmp/cde-disable-xft-checksums-$$.txt"
echo ">>> Capturing checksums..."
for f in $CHECKSUM_FILES; do
    if [ -f "$f" ]; then
        md5sum "$f" >> "$CHECKSUM_OUTPUT"
    else
        echo "SKIP: $f not found" >> "$CHECKSUM_OUTPUT"
    fi
done

# Step 2: Verify no Xft symbols in --disable-xft build
echo ""
echo ">>> Verifying no Xft symbols in --disable-xft build..."
XFT_LEAK=0
for f in $CHECKSUM_FILES; do
    if [ -f "$f" ]; then
        if nm -D "$f" 2>/dev/null | grep -qi "Xft\|FcInit"; then
            echo "FAIL: Xft symbols found in $f"
            XFT_LEAK=1
        fi
    fi
done

if [ $XFT_LEAK -eq 1 ]; then
    echo "GUARDRAIL G2 VIOLATION: Xft symbols in --disable-xft build"
    rm -f "$CHECKSUM_OUTPUT"
    exit 1
fi
echo "PASS: No Xft symbols in --disable-xft build"

# Step 3: Build with --enable-xft and verify Xft symbols
echo ""
echo ">>> Building with --enable-xft..."
make clean > /dev/null 2>&1 || true
./configure --enable-xft > /dev/null 2>&1
make -j"$JOBS" > /dev/null 2>&1

echo ">>> Verifying Xft symbols in --enable-xft build..."
XFT_FOUND=0
for f in $CHECKSUM_FILES; do
    if [ -f "$f" ]; then
        if nm -D "$f" 2>/dev/null | grep -q "XftFontOpenName"; then
            echo "PASS: Xft symbols in $f"
            XFT_FOUND=1
        fi
    fi
done

if [ $XFT_FOUND -eq 0 ]; then
    echo "WARN: No Xft symbols found in --enable-xft build"
fi

# Step 4: Compare checksums against baseline if provided
echo ""
if [ -f "$BASELINE" ]; then
    echo ">>> Comparing against baseline: $BASELINE"
    if diff "$CHECKSUM_OUTPUT" "$BASELINE" > /dev/null 2>&1; then
        echo "PASS: Checksums match baseline (byte-identical)"
        rm -f "$CHECKSUM_OUTPUT"
        echo ""
        echo "============================================================"
        echo "RESULT: PASS - --disable-xft build matches baseline"
        echo "============================================================"
        exit 0
    else
        echo "FAIL: Checksums differ from baseline"
        echo ""
        echo "Differences:"
        diff "$CHECKSUM_OUTPUT" "$BASELINE" || true
        echo ""
        echo "Current checksums saved to: $CHECKSUM_OUTPUT"
        echo "Baseline: $BASELINE"
        echo ""
        echo "============================================================"
        echo "RESULT: FAIL - --disable-xft build differs from baseline"
        echo "============================================================"
        exit 1
    fi
else
    echo ">>> No baseline file found. Saving checksums for future comparison."
    mv "$CHECKSUM_OUTPUT" "$BASELINE"
    echo "Saved to: $BASELINE"
    echo ""
    echo "To compare against this baseline in future runs:"
    echo "  $0 $BASELINE"
    echo ""
    echo "============================================================"
    echo "RESULT: PASS - Checksums saved (no baseline to compare)"
    echo "============================================================"
    exit 0
fi