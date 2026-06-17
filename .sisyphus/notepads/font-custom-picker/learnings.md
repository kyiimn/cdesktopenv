## 2026-06-17 Wave 1 Exploration: Build System & Font Architecture

### configure.ac Xft/fontconfig Pattern
- Xft detection: `AC_ARG_ENABLE([xft])` with `--enable-xft`/`--disable-xft` (default: auto)
- fontconfig check: `AC_CHECK_HEADERS([fontconfig/fontconfig.h])` + `AC_CHECK_LIB(fontconfig, FcInit)` — uses AC_CHECK_LIB, NOT PKG_CHECK_MODULES
- Variables set: XFTLIB (`-lXft`), FONTCONFIGLIB (`-lfontconfig`), FREETYPE2_INCS (`-I<dir>`), HAVE_XFT (AM_CONDITIONAL), USE_XFT (AC_DEFINE)
- All already exist in configure.ac — NO new configure.ac changes needed for fontconfig

### lib/DtFont/Makefile.am
- SOURCES: `DtFont.c XftWrapper.c` — add `FontEnum.c`
- CPPFLAGS: `-I./include -DMULTIBYTE -DNLS16 -I../include @DT_INCDIR@`
- HAVE_XFT conditional adds FREETYPE2_INCS to CPPFLAGS
- LIBADD: `$(XFTLIB) $(FONTCONFIGLIB) $(XTOOLLIB)` (unconditional — empty when Xft disabled)
- FontEnum.c should follow same pattern as DtFont.c/XftWrapper.c: always in SOURCES, #ifdef guarded inside

### programs/dtstyle/Makefile.am
- dtstyle_SOURCES: `Main.c MainWin.c Font.c Audio.c ...` — add `FontPicker.c` after `Font.c`
- noinst_HEADERS: `Audio.h Backdrop.h ... Font.h ...` — add `FontPicker.h` after `Font.h`
- HAVE_XFT conditional: `dtstyle_CPPFLAGS += $(FREETYPE2_INCS)` and `dtstyle_LDADD += $(XFTLIB) $(FONTCONFIGLIB)`
- dtstyle links `$(DTCLIENTLIBS) $(XTOOLLIB)` — need to verify libDtFont is in DTCLIENTLIBS

### FontData struct (Font.c:107-123)
- Already has: fontWkarea, fontpictLabel, previewTB, previewForm, systemLabel, userText, familyTB, familyList, sizeTB, sizeList, originalFontIndex, selectedFontIndex, selectedFontStr, userTextChanged
- NEW fields (from plan): customizeButton, hasCustomFont, customSysStr, customUserStr, customSysFont, customUserFont, customSource
- MUST be added at END of struct (guardrail G2: XtOffset dependency)

### widget_list[7] (Font.c:200)
- 5 slots used (0-4), 2 free (5-6)
- Plan says use XtManageChild() directly for new buttons (customizeButton, systemWideButton)
- Do NOT add to widget_list[] — per Momus fix (G11)

### ButtonCB fontres (Font.c:636-652)
- 12 resource keys in specific order: systemFont, userFont, FontList, buttonFontList, labelFontList, textFontList, XmText*FontList, XmTextField*FontList, DtEditor*textFontList, Font, FontSet, FontFamily
- sprintf into fontres[8192] buffer
- Followed by: _DtAddToResource(style.display, fontres) and SmNewFontSettings(fontres)

### SmNewFontSettings (Protocol.c:761-781)
- Sends fontres as XChangeProperty to dtsession window
- Property atom: xaDtSmFontInfo (interned from _XA_DT_SM_FONT_INFO)

### _DtAddToResource (addToRes.c:544-550)
- Merges into BOTH XA_RESOURCE_MANAGER and _DT_SM_PREFERENCES
- Uses sorted merge: newer values win, old tags preserved

### _DtFontCreateXmFontList (DtFont.h:111)
- Signature: `extern XmFontList _DtFontCreateXmFontList(Display *dpy, const char *pattern);`
- Accepts both XLFD and fontconfig patterns
- USE_XFT include guard dance in DtFont.h (lines 31-55)

### numFamilies <= 1 UI hiding (Font.c:504-512)
- XtUnmanageChild(font.familyTB) and re-attach sizeTB to form
- customizeButton MUST attach to form, not familyTB (Momus fix G12)
## 2026-06-17 Wave 1 Task 3+4: FontEnum.c Implementation

### Build Verification
- Builds clean BOTH with `--enable-xft` and `--disable-xft`
- File: `cde/lib/DtFont/FontEnum.c` (864 lines)
- `make libDtFont_la-FontEnum.lo` succeeds with both configurations
- Full library link succeeds: `libDtFont.la` produces `libDtFont.so.2.1.0`

### Pre-existing Bug in FontEnum.h
- **Bug**: FontEnum.h forward-declares `XmFontList` as `struct _XmFontListRec *`, guarded by `_XmFontList_H`
- **Problem**: Motif 2.3+ `Xm.h` typedef is `struct __XmRenderTableRec **` (pointer-to-pointer), and uses `_Xm_h` as its guard, NOT `_XmFontList_H`
- **Result**: Forward declaration is NOT suppressed when Xm.h is included first → type conflict
- **Workaround in FontEnum.c**: pre-define `_XmFontList_H` before including FontEnum.h, so the broken forward declaration is skipped and the real Xm.h typedef from Dt/DtFont.h stands
- **Documented with comment block** explaining the workaround so future devs don't "clean it up"

### Coding Style Adopted from DtFont.c/XftWrapper.c
- USE_XFT save/undef/restore dance at top of file (mirror DtFont.h:42-55)
- `XmFontList` typedefs come from Dt/DtFont.h (via Xm.h), not the public header
- Unused parameters silenced with `(void)param;`
- Static helpers in same file (no shared internal header needed)
- `extern` declarations for FC-only functions placed inside `#ifdef USE_XFT` blocks where they're called
- `extern` decls for FC-only helpers used at file scope

### XLFD Parsing Pattern (from dtcm/font.c)
- `sscanf(font_names[i], "-%*[^-]-%*[^-]-%*[^-]-%*[^-]-%*[^-]-%*[^-]-%d", &pixel_size)` — extracts pixel size (7th field)
- Family name is 2nd field after leading '-', so use custom `xlfd_to_family()` to extract it (sscanf %[^-] doesn't safely handle NULL termination in nested calls)

### FC API Patterns
- `FcPatternCreate()` / `FcPatternDestroy(pat)` — must destroy
- `FcObjectSetBuild(FC_FAMILY, ..., (char *)NULL)` — variadic, requires explicit `(char *)NULL` cast
- `FcFontList(NULL, pat, os)` — returns `FcFontSet*`; must `FcFontSetDestroy(fs)`
- `FcPatternGetString(pat, FC_FAMILY, 0, &value)` — returns `FcResult`; ignore result (we just check the out-param is non-NULL)
- `FcPatternGetBool(pat, FC_SCALABLE, 0, &val)` — out param is `FcBool*` not `int*`
- `FcPatternGetDouble(pat, FC_PIXEL_SIZE, 0, &val)` — out param is `double*`

### DtFontList Memory Ownership
- `DtEnumerateFontFamilies` returns ALLOCATED list; caller MUST call `DtFreeFontList` to free
- DtFreeFontList frees: family_name, full_name, xlfd, fc_pattern (per entry), fonts array, list struct
- DtMergeFontLists does NOT free inputs; only the result needs DtFreeFontList
- FC helpers (Families, Sizes) use `strdup` for family_dup/full_dup/fc_dup — all freed by DtFreeFontList

### Edge Cases Handled
- NULL dpy / NULL pattern checks at function entry
- XListFonts returning 0 fonts → NULL return
- FC init failure (FcPatternCreate returns NULL) → NULL return
- Empty result set (count == 0) → free arrays and return NULL
- Scalable font → pixel_size = 0 in DtFontInfo

## 2026-06-17 Task: Customize Button in Font.c

### Implementation Verified
- `make dtstyle` builds clean (only pre-existing mktemp warning, unrelated)
- Font.c compiles with USE_XFT enabled

### Required Fixes Beyond the Plan
1. **Xm/PushBG.h must be explicitly included** for `XmCreatePushButtonGadget` — `Xm/Xm.h` does not transitively pull it in on this Motif 2.3+ setup. Other dtstyle files (Audio.c:51) follow this pattern.
2. **`_XmFontList_H` pre-define workaround is required in Font.c too** — the broken forward declaration in `Dt/FontEnum.h` triggers when `FontPicker.h` is included (since FontPicker.h pulls in `Dt/FontEnum.h`). Without the workaround, compilation fails with `conflicting types for 'XmFontList'`. Same workaround as `FontPicker.c:101-103` and `lib/DtFont/FontEnum.c:73-75`.

### Placement Insight
- Button creation is placed AFTER `XtManageChildren(widget_list, count);` but BEFORE `XtManageChild(font.fontWkarea);` (the form). Order matters: widget_list children are managed first, then customizeButton is managed standalone, then the parent form. This keeps the button visible regardless of widget_list capacity (5 used out of 7, 2 free).
- CustomizeCB uses the same selector pattern as `changeSampleFontCB` and `changeFamilyCB`: clamp `selectedFontIndex` to 0 when < 0, then derive family/size via FONT_FAMILY/FONT_SIZE macros.

## 2026-06-17 Bug Fixes: restoreFonts XmFontList recreation + saveFonts buffer size

### Bug H1 Fixed
- `restoreFonts()` in `cde/programs/dtstyle/Font.c` restored `customSysStr`/`customUserStr` strings from session DB but never created the corresponding `XmFontList` objects (`customSysFont`/`customUserFont`). The result: preview widgets had `NULL` font lists after session restore.
- Fix: After `font.hasCustomFont = ...` (line ~1041), added 4 lines that call `DtGetFontXmFontList(style.display, ...)` for each non-empty string — exactly mirroring the pattern in `FontDataSetCustomFont` (lines 1200-1206).
- Both NULL check AND `str[0] != '\0'` check are needed: XtNewString("") returns a non-NULL empty string in some restoration paths.

### Bug M2 Fixed
- `saveFonts()` `char bufr[1024]` was too small for modern fontconfig patterns. Long patterns like `-*-medium-r-normal--0-0-0-0-p-0-iso8859-1,-*-medium-r-normal--0-0-0-0-p-0-iso8859-1` (when user picks a list of fonts in FontPicker) plus 50 bytes of format wrapping overflows 1024.
- Fix: increased buffer to `char bufr[4096]`.
- All `snprintf` calls already use `sizeof(bufr)` as the size limit, so no other changes were needed. They previously silently truncated; now they have 4x headroom.

### (void)customFamily / (void)customSize — cosmetic only
- These values are read into local variables and immediately discarded. The actual custom state is already captured by `selectedFontIndex` (via `FONT_FAMILY`/`FONT_SIZE` macros — see saveFonts lines 1107-1110 which recompute them from `selectedFontIndex`).
- Decision: keep the read+discard pattern with a brief comment, for forward compatibility (if a future change decouples custom metadata from the selected family index, the Xrm round-trip won't break existing session files).
