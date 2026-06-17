# Learnings: per-family custom font overrides in dtstyle

## Approach
- FontData struct already had `CustomFontEntry customFont[MAX_FONT_FAMILIES]`; the task was purely mechanical replacement of flat field accesses with `font.customFont[fam].*`.
- Introduced `int family` parameter to all public FontData accessors and to `FontDataSetCustomFont`.
- Added `fontPicker.targetFamily` in `FontPickerData` so the picker records which family it was opened for; `PopupFontPicker` stores the parameter and `FontPickerApply` passes it to the setter.
- `ButtonCB` Cancel now loops over all 8 families and calls `FontDataSetCustomFont(fam, NULL, NULL, ...)` to clear every override.
- `saveFonts`/`restoreFonts` now persist per-family resources named `*Fonts.customSysStr.%d:` and `*Fonts.customUserStr.%d:`.
- `changeSampleFontCB` and `changeFamilyCB` use the family derived from `font.selectedFontIndex` to pick the current family's override for preview widgets.
- `Protocol.c BuildFontResourceString` passes `selectedFamily` to the read accessors.

## Build verification
- `make -C cde/programs/dtstyle` succeeded with all object files compiled and linked.
- `make -C cde/lib/DtFont` was already up to date (no code changes in DtFont).

## CDE style notes
- C89-compatible: declarations at block start; no mixed declarations.
- Existing section/header comments preserved/updated rather than removed.
- `MAX_FONT_FAMILIES` / `FONT_FAMILY(idx)` macros from Main.h used consistently.

## Pitfall avoided
- `CustomFontEntry` typedef must appear before `FontData` struct because `FontData` embeds `CustomFontEntry customFont[MAX_FONT_FAMILIES]`.

## 2025-06-17: Revert DrawingArea Xft workaround back to XmLabelGadget

- Reverted previewLabel from XmDrawingArea (with manual DtFont_DrawImageString in expose callback) back to XmLabelGadget.
- Removed `<Dt/DtFont.h>` includes from both `FontPicker.h` and `FontPicker.c`; the public DtFont API is no longer used here.
- Restored `XmFontList currentFontList` + `XmRenderTable currentRenderTable` fields in `FontPickerData`.
- `UpdatePreview` now calls `DtGetFontXmFontList(style.display, pattern)` once and applies the resulting XmFontList to both `previewLabel` and `previewText` via `XmNfontList`.
- Cleanup in `PopdownFontPicker` uses `XmFontListFree(fontPicker.currentFontList)`; no more `DtFontDestroy` / `XFreeGC`.
- Build `make -C cde/programs/dtstyle` succeeds. Relying on the patched openmotif 2.3.8-5 `XmFONT_IS_XFT` fix.

## Findings: dtstyle font save/persistence path (post-search)

### Where font settings are written
1. **Immediate per-session propagation** - Font.c ButtonCB OK case (lines 610-755)
   - Builds `fontres` resource string with `*systemFont`, `*userFont`, `*FontList`, `*buttonFontList`, `*labelFontList`, `*textFontList`, `*XmText*FontList`, `*XmTextField*FontList`, `*DtEditor*textFontList`, `*Font`, `*FontSet`, `*FontFamily`
   - With custom font: adds `*CustomSysFont`, `*CustomUserFont`, `*CustomFamily`, `*CustomSize`
   - If `style.xrdb.writeXrdbImmediate` is true, calls `_DtAddToResource(style.display, fontres)` which merges into the X server's RESOURCE_MANAGER property (implemented in `/home/kyiimn/Projects/cdesktopenv/cde/lib/DtSvc/DtUtil2/addToRes.c`).
   - Always calls `SmNewFontSettings(fontres)` which writes the same resource string to the dtsession window as the `DT_SM_FONT_INFO` property (XChangeProperty, XA_STRING).

2. **Session persistence to disk** - dtsession writes the font resource string to:
   - Path: `<savePath>/<current|home>.font/<LANG>/dt.font.<l|m|h>` (low/med/high resolution suffix)
   - Determined by `SetFontSavePath()` in `/home/kyiimn/Projects/cdesktopenv/cde/programs/dtsession/SmGlobals.c`
   - Written by `SmSave.c` lines 1651-1686 via `XrmPutFileDatabase(db, smGD.fontPath)` after parsing the DT_SM_FONT_INFO property.

3. **Dtstyle dialog session save/restore** - SaveRestore.c `saveSessionCB` / `restoreSession`
   - `saveFonts()` in `/home/kyiimn/Projects/cdesktopenv/cde/programs/dtstyle/Font.c` writes dialog state to a per-session file via `DtSessionSavePath()` using the `WRITE_STR2FD(fd, bufr)` macro.
   - It saves:
     - `*Fonts.ismapped: True/False`
     - `*Fonts.x:` and `*Fonts.y:` geometry
     - `*Fonts.familyIndex: <n>`
     - For each family with a custom font override: `*Fonts.customSysStr.<fam>: ...` and `*Fonts.customUserStr.<fam>: ...`
   - `restoreFonts()` reads the same Xrm database and restores `font.selectedFontIndex`, `font.customFont[fam]` strings, and recreates XmFontLists via `DtGetFontXmFontList()`.

4. **System-wide app-defaults apply** - Protocol.c `RequestSystemWideApply()` / `SystemApplyFontDirect()`
   - Writes to `<CDE_CONFIGURATION_TOP>/app-defaults/Dtstyle` (default `/etc/dt/app-defaults/Dtstyle`)
   - Uses `XrmGetFileDatabase()`, `XrmMergeDatabases()`, `XrmPutFileDatabase()` to merge rather than raw concatenation.
   - For non-root users, spawns `pkexec <CDE_INSTALLATION_TOP>/bin/dtstyle_sysapply.sh <tmpfile>` after writing resources to a `/tmp/dtstyle-sysfont-XXXXXX` temp file.
   - Helper script `/home/kyiimn/Projects/cdesktopenv/cde/programs/dtstyle/dtstyle_sysapply.sh` performs an awk-based key deduplication merge.

### Key data structures
- `CustomFontEntry` (Font.c lines 122-129): per-family override holding `hasCustom`, `customSysStr`, `customUserStr`, `customSysFont`, `customUserFont`, `customSource`.
- `FontData.customFont[MAX_FONT_FAMILIES]` (Font.c line 147): array of per-family custom entries.
- Read accessors in Font.h: `FontDataGetSelectedIndex`, `FontDataHasCustomFont`, `FontDataGetCustomSysStr`, `FontDataGetCustomUserStr`.
- Write accessor: `FontDataSetCustomFont()` in Font.c (lines 1198-1245), used by `FontPickerApply()` in FontPicker.c.

### Resource loading
- `GetApplicationResources()` in Resource.c loads the fontChoice table from legacy `SystemFont1-7`/`UserFont1-7` and new per-family resources (`fontFamily0SystemFont0..6`, etc.).
- `GetCustomFontResources()` loads `customSysFont`, `customUserFont`, `customFamily`, `customSize` from app-defaults into `ApplicationData`.

### Relevant files
- `/home/kyiimn/Projects/cdesktopenv/cde/programs/dtstyle/Font.c`
- `/home/kyiimn/Projects/cdesktopenv/cde/programs/dtstyle/Font.h`
- `/home/kyiimn/Projects/cdesktopenv/cde/programs/dtstyle/FontPicker.c`
- `/home/kyiimn/Projects/cdesktopenv/cde/programs/dtstyle/FontPicker.h`
- `/home/kyiimn/Projects/cdesktopenv/cde/programs/dtstyle/Protocol.c`
- `/home/kyiimn/Projects/cdesktopenv/cde/programs/dtstyle/Protocol.h`
- `/home/kyiimn/Projects/cdesktopenv/cde/programs/dtstyle/SaveRestore.c`
- `/home/kyiimn/Projects/cdesktopenv/cde/programs/dtstyle/Resource.c`
- `/home/kyiimn/Projects/cdesktopenv/cde/programs/dtstyle/Resource.h`
- `/home/kyiimn/Projects/cdesktopenv/cde/programs/dtstyle/dtstyle_sysapply.sh`
- `/home/kyiimn/Projects/cdesktopenv/cde/lib/DtSvc/DtUtil2/addToRes.c`
- `/home/kyiimn/Projects/cdesktopenv/cde/lib/DtSvc/DtUtil1/SmComm.c`
- `/home/kyiimn/Projects/cdesktopenv/cde/programs/dtsession/SmSave.c`
- `/home/kyiimn/Projects/cdesktopenv/cde/programs/dtsession/SmGlobals.c`
- `/home/kyiimn/Projects/cdesktopenv/cde/programs/dtsession/SmRestore.c`
- `/home/kyiimn/Projects/cdesktopenv/cde/programs/dtsession/SmCommun.c`


## Fix: per-family font persistence in dtstyle

**Problem:** Xrm stores `Dtstyle.customSysFont.0` as three quarks (`Dtstyle`, `customSysFont`, `0`) because `.` is a hierarchy separator. The original `Resource.c:GetCustomFontResources` built a two-quark name list with `XrmStringToQuark("customSysFont.0")`, which never matched the stored resource.

**Fix:** Replaced the `XrmQGetResource` + manual quark assembly with `XrmGetResource` string API, passing the full resource string `Dtstyle.customSysFont.0` / `Dtstyle.customUserFont.0`. This lets Xrm parse the dots correctly. Both sys and user per-family lookups updated.

**Files changed:** `cde/programs/dtstyle/Resource.c` (only).

**Build verification:** `cd cde && make -j4` exits 0; `programs/dtstyle/.libs/dtstyle` produced.

**Note:** `ApplyXrdbCustomFonts` in `Font.c` already correctly iterates `style.xrdb.customSysFontResArr[fam]` and calls `FontDataSetCustomFont`; it just needed the array to be populated correctly by the lookup fix.

## 2026-06-18: Xt type converter for XmRString → XmRFontList

- Implemented `DtFontInit(Display *)` in `lib/DtFont/XftWrapper.c` (USE_XFT) and `lib/DtFont/DtFont.c` (non-XFT stub).
- `DtFontInit` registers `CvtStringToDtFontList` via `XtSetTypeConverter(XmRString, XmRFontList, ..., XtCacheByDisplay, destroy_proc)`.
- Converter signature matches `XtTypeConverter`: `Boolean (*)(Display*, XrmValue*, Cardinal*, XrmValue*, XrmValue*, XtPointer*)`.
- `_XmGetDisplayArg` is a static helper inside Motif's `lib/Xm/ResConvert.c` and is not exported; CDE must supply its own display-arg procedure (`DtFontGetDisplayArg`) that returns `DisplayOfScreen(XtScreenOfObject(widget))`.
- There is no public API to retrieve Motif's previously registered converter, so the XLFD fallback is implemented directly with `XmFontListEntryLoad(dpy, name, XmFONT_IS_FONT, XmFONTLIST_DEFAULT_TAG)`. This covers the single-font strings used by CDE font resources.
- `_DtFontCreateXmFontList` already creates an `XmFONT_IS_XFT` entry using `XmFontListEntryCreate_r` and `XmGetXmDisplay(dpy)`, so the converter reuses it unchanged.
- `DtFontInit` must be called after `XtOpenDisplay` / `XtInitialize` but before widgets read `XmNfontList` resources. Added calls in:
  - `programs/dtstyle/Main.c` right after `style.display = XtDisplay(style.shell)`.
  - `programs/dtwm/WmMain.c` right after `InitWmGlobal()` returns (display is opened inside `InitWmGlobal`).
- Build verified with `cd cde && make -j4` exiting 0.

## 2025-06-18: DtFontInit timing and CvtStringToDtFontStruct fontconfig resolution

### Bug 1: DtFontInit called too late for shell widget font conversion
- `XtVaAppInitialize()` creates the shell widget and converts `*font:` resources inside that call.
- `DtFontInit()` was only registered inside `DtBigInitialize()` (via `DtInitialize()`), which is called AFTER `XtVaAppInitialize()`.
- For CDE programs that have `*font:` resources on the shell, Motif's default `CvtStringToFontStruct` runs before our converter is registered, producing:
  `Warning: Cannot convert string "Monospace-14" to type FontStruct`
- Fix in `dtcalc/motif.c`: call `XtToolkitInitialize(); DtFontInit(NULL);` immediately before `XtVaAppInitialize()`.
  - `DtFontInit(NULL)` is safe: the converter's display-arg callback (`DtFontGetDisplayArg`) gets the Display from the widget at conversion time, so a NULL display argument is fine.
- `dtstyle/Main.c` and `dtwm/WmMain.c` already call `DtFontInit()` explicitly after the shell/display is available and before child widgets are created; that timing is sufficient for child-widget font resources.
- Kept `DtFontInit(display)` inside `DtBigInitialize()` as a safety net for apps that don't call it explicitly. The `static Boolean initialized` guard prevents double registration.

### Bug 2: CvtStringToDtFontStruct did not handle fontconfig patterns
- `XmRFontStruct` resources expect an `XFontStruct*`, which is a core X11 bitmap font. Fontconfig patterns like "Monospace-14" cannot be loaded directly via `XLoadQueryFont()`.
- Added fontconfig-based XLFD resolution in `CvtStringToDtFontStruct()`:
  1. XLFD patterns (leading `-`) still go directly to `XLoadQueryFont()`.
  2. Non-XLFD patterns are parsed with `FcNameParse()`, matched with `FcFontMatch()`, and the matched pattern's `FC_FAMILY` and `FC_PIXEL_SIZE` are used to construct candidate XLFD strings.
  3. If XLFD construction fails, `FC_FILE` (if present) is tried as a last fontconfig-provided core-font path.
  4. The original pattern is tried as-is in case it names a core font.
  5. Final fallback is `"fixed"`.
- All fontconfig code is inside `#ifdef USE_XFT`. Non-XFT builds continue to use the stub in `DtFont.c`.
- Did NOT change `CvtStringToDtFontList` (already works) or `_DtFontCreateXmFontList` (signature/behavior preserved).

### Build verification
- `cd cde && make -j4` exits 0.
- No new compile errors/warnings from the changed files.
