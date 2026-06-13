#!/bin/bash
# CDE Xft Migration - Memory Leak Test (valgrind)
# Task 57: valgrind memory leak test for all migrated programs
#
# This script documents the valgrind test procedure for Xft migration.
# Actual execution requires valgrind and a running X11 display.
#
# PROCEDURE:
# 1. Build CDE with --enable-xft
# 2. For each migrated program, run under valgrind:
#    valgrind --leak-check=full --show-leak-kinds=definite,indirect \
#      --suppressions=cde.supp \
#      <program> [args]
# 3. Check for XftFont/XftDraw/XftColor leaks:
#    - XftFont: Must be freed with XftFontClose()
#    - XftDraw: Must be freed with XftDrawDestroy()
#    - XftColor: Must be freed with XftColorFree()
#
# PROGRAMS TO TEST:
# - dtwm (window manager - long-running, test for leaks over time)
# - dtterm (terminal emulator - Xft text rendering)
# - dthello (hello world - simple startup/shutdown)
# - dtlogin (login manager - Xft in chooser/greeter)
# - dtfile (file manager - save/undef/restore)
# - dtstyle (style manager - save/undef/restore)
# - dtcalc (calculator - save/undef/restore)
#
# SUPPRESSIONS FILE:
# Create cde.supp with suppressions for:
# - X11 library leaks (known Xlib issues)
# - Motif leaks (known Xm issues)
# - fontconfig initialization leaks (one-time)
#
# EXPECTED RESULTS:
# - Zero definite leaks in CDE code
# - XftFont/XftDraw/XftColor all freed correctly
# - Indirect leaks only from X11/Motif (not CDE code)

echo "valgrind test procedure - see comments for detailed instructions"
echo ""
echo "Quick test: Start dtwm under valgrind, open/close windows for 30s, check for leaks"
echo "valgrind --leak-check=full --suppressions=cde.supp /usr/dt/bin/dtwm"