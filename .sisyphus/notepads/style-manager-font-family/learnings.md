# Learnings - Style Manager Font Family

## 2026-06-15 Session Start
- CDE dtstyle uses Autotools build system (not CMake/Meson)
- XtOffset constraint: 2D arrays cannot be used with XtOffset → must use flat 1D + FONT_INDEX macro
- Font.c has USE_XFT include-guard dance (lines 46-65) that MUST be preserved (G5)
- SmNewFontSettings() in Protocol.c passes fontres string as opaque blob → only content changes, never protocol (G4)
- fontres buffer is char[8192], currently ~2KB used
- 8 locale app-defaults/Dtstyle files exist (C + de_DE, es_ES, fr_FR, it_IT, ja_JP, zh_CN, zh_TW)
- ko_KR and sv_SE have .tmsg only, NO app-defaults/Dtstyle files
- Message catalog set 5 currently uses messages 17-24; 25 and 26 are available
- Family 0 defaults MUST exactly match existing SystemFont1..7/UserFont1..7
- numFamilies <= 1 → family TitleBox hidden (backward compat)
- Fontset struct field order MUST NOT change (XtOffset depends on it)

## 2026-06-15 F2/F3 Bug Fix Session
- Family 1 XtResource tables: FONT_INDEX(1, sz) = 7..13, requires 4 separate tables (sysStr/sysFont/userStr/userFont) for the new fontChoice slots
- Family 1 defaults: `-dt-interface user-medium-r-normal-{xxs,xs,s,m,l,xl,xxl}*-*-*-*-*-*-*-*-*:` from Dtstyle app-defaults
- XrmQGetResource can return success with value.addr==NULL (resource not set) — always check both return value AND value.addr before strcmp
- Cancel revert order: revert family first, then size — single XmListSelectPos for size triggers changeSampleFontCB which queries fontChoice[idx].sysStr (NULL until family is set)
- GetSysFontResource/GetUserFontResource are LEGACY functions that index into 7-element arrays — must add bounds guard for idx >= MAX_FONT_SIZES
- XtNumber() macro is the correct way to get array size in Xt code (not sizeof)

## 2026-06-15 R1/R2/R3 Deep Verification Bug Fix Session
- XmNselectedItems with XtSetArg only stores a pointer — the actual copy happens during XmCreateScrolledList. XmStringFree must come AFTER widget creation, not before (use-after-free C1)
- CreateFontDlg resets font.selectedFontIndex = -1 — any caller that sets selectedFontIndex before CreateFontDlg must save/restore (C2)
- When a Motif widget is unmanaged, widgets attached to it via XmATTACH_WIDGET get dangling references — must re-attach to XmATTACH_FORM (C3)
- Family 2+ fontChoice entries have NULL sysStr/userStr because only Family 0 and 1 resource tables exist — any code using userStr/sysStr must NULL-guard (C4)
- XrmQGetResource value.addr NULL check applies to ALL resource types, not just ismapped (C5)
- CMPSTR/XmString created in arrays must be freed individually before XtFree of the array pointer (M1)
- GetFontStrResources already loads Family 0 sysStr/userStr via legacy resource names — GetFamily0FontResources should NOT re-load them to avoid clobbering admin customizations (M2)
- Cancel must deselect BOTH familyList and sizeList when no original selection exists (M3)
- snprintf sizeof should match the actual buffer being written to, not a different buffer of the same size (M4)
- popup_fontBB now saves/restores selectedFontIndex around CreateFontDlg with XmListSelectPos(False) to avoid callback triggers
## 2026-06-16 USE_XFT Crash Fix Wave (font->fid)
- 4 files had XmeRenderTableGetDefaultFont return being dereferenced as XFontStruct->fid, crashing under USE_XFT
- Pattern from `cde/programs/dtwm/Clock.c:1050-1063` is the canonical fix: `#ifdef USE_XFT` skip GCFont, `#else` use font->fid
- DtWidget files (Icon.c, Control.c) already have the USE_XFT save/restore dance around Xm/XmP.h include — just need to add Xft.h after the restore
- dtprintinfo/libUI/MotifUI/Icon.c has the same save/restore dance but Xft.h was not included
- PrintSetupB.c (DtPrint) had NO USE_XFT references at all — required adding the full save/restore dance + Xft.h include for the first time in this file
- Use `((XftFont *)font)->max_advance_width` as the Xft equivalent of core font's `max_bounds.width` for char width estimation
- For files that just skip GCFont (no Xft-specific code), the Xft.h include is technically optional but kept for consistency with Editor.c pattern using HAVE_X11_XFT_XFT_H double guard
- TabButton.c (dtinfo/Widgets) had same fs->fid crash pattern — no USE_XFT guard existed at all; required adding full save/restore dance + Xft.h include at top of file AND the #ifdef USE_XFT guard around fs->fid access

## ui_util.c XftFont crash fix (2026-06-16)

- `objxm_fontlist_to_font()` returns `XFontStruct*` but under USE_XFT, `XmFontListGetNextFont` returns an `XftFont*` cast as `XFontStruct*`
- The crash occurs at `font->per_char`, `font->max_bounds`, and `XGetFontProperty(font, ...)` — these are `XFontStruct` fields that don't exist in `XftFont`
- CDE-wide pattern: cast with `(XftFont *)font` and use `xftfont->max_advance_width`, `xftfont->ascent`, `xftfont->descent`
- USE_XFT is defined globally in `cde_config.h` (via configure.ac), not per-Makefile
- Only one call site for `objxm_fontlist_to_font()` in dtappbuilder: `ui_util.c:764`
- No include guard dance needed for Xft.h in ui_util.c — direct `#ifdef USE_XFT` / `#include <X11/Xft/Xft.h>` / `#endif` suffices (matches CDE convention)

## 2026-06-17 Font Save Flow Research Session

### _DtAddToResource() architecture (lib/DtSvc/DtUtil2/addToRes.c)
- `void _DtAddToResource(Display *dpy, const char *data)` (line 545-550) - PUBLIC entry point
- `void _DtAddResString(Display *dpy, const char *data, unsigned int flags)` (line 552-604) - flag-controlled version
- Internally calls `_DtAddToResProp()` for each of: `_DT_ATR_RESMGR` (XA_RESOURCE_MANAGER) and `_DT_ATR_PREFS` (_DT_SM_PREFERENCES)
- Implementation lifted from xrdb(1X) — parses buffer as X resource lines, merges with existing root property
- Uses XA_STRING type, format 8, PropModeReplace to write to root window
- **Critical**: It MERGES into existing root properties by tag (overwrites entries with same tag, preserves others)
- Format: standard X resource format: `*tag: value\n` separated by newlines

### fontres buffer format (Font.c line 639-651)
- Stack-allocated `char fontres[8192]`
- sprintf format:
  `*systemFont: %s\n*userFont: %s\n*FontList: %s\n*buttonFontList: %s\n*labelFontList: %s\n*textFontList: %s\n*XmText*FontList: %s\n*XmTextField*FontList: %s\n*DtEditor*textFontList: %s\n*Font: %s\n*FontSet: %s\n*FontFamily: %d\n`
- 12 distinct resource tags
- `*systemFont`/`*userFont` are CDE custom resources (read by dtsession for sys.font matching)
- The 9 other tags are Motif standard: `*FontList`, `*buttonFontList`, `*labelFontList`, `*textFontList`, `*XmText*FontList`, `*XmTextField*FontList`, `*DtEditor*textFontList`, `*Font`, `*FontSet`
- `*FontFamily: N` is the new family index (added when family feature was added)

### saveFonts() writes to a session file (Font.c line 914-957)
- File descriptor `fd` passed in (no path in saveFonts itself)
- Written via `WRITE_STR2FD` macro
- Format: X resource DB format
  - `*Fonts.ismapped: True|False\n`
  - `*Fonts.x: %d\n` (X position, frame-corrected)
  - `*Fonts.y: %d\n` (Y position, frame-corrected)
  - `*Fonts.familyIndex: %d\n` (NEW — only written if font dialog is open)
- The fd is the dtstyle session file (path: `~/.dt/sessions/current/dtstyle.dtXXXXXX` or `~/.dt/<display>/dtstyle.dtXXXXXX`)

### saveSessionCB flow (SaveRestore.c line 93-164)
- Receives WM_SAVE_YOURSELF ICCCM message
- Calls `DtSessionSavePath(w, &longpath, &name)` to get session file path
- `longpath` = `~/.dt/sessions/current/dtXXXXXX` (or `~/.dt/<display>/dtXXXXXX`)
- `name` = `dtXXXXXX` (basename)
- Creates file with `creat(longpath, 0644)` — S_IRUSR|S_IRGRP|S_IWUSR|S_IWGRP
- Calls all sub-savers: saveMain, saveColor, saveColorEdit, saveFonts, saveBackdrop, saveKeybd, saveMouse, saveAudio, saveScreen, saveDtwm, saveStartup, saveI18n
- Final XSetCommand sets `-session <name>` argv for restart

### restoreSession() (SaveRestore.c line 175-218)
- DtSessionRestorePath returns full path
- `XrmGetFileDatabase(longpath)` opens as XrmDatabase
- Calls restoreMain first, then per-dialog restore functions
- restoreFonts (Font.c line 855-900) reads:
  - `*Fonts.x` and `*Fonts.y` → save.posArgs for geometry
  - `*Fonts.familyIndex` → font.selectedFontIndex (with bounds clamping)
  - `*Fonts.ismapped` → popup_fontBB if "True"

### _DT_SM_PREFERENCES property flow (Protocol.c)
- `SmNewFontSettings(fontres)` (Protocol.c line 761-781)
- XChangeProperty to dtsession's smWindow with atom `_DT_SM_FONT_INFO` (XA_STRING)
- dtsession receives via PropertyChange event in SmCommun.c line 588 — sets `smCust.fontChange = True`
- dtsession later reads property via `_DtGetSmFont()` → reads from `xaDtSmFontInfo` atom

### dtsession save path (SmSave.c line 1651-1696)
- When `smCust.fontChange == True` at session end:
  - `_DtGetSmFont` reads the fontres blob from _DT_SM_FONT_INFO property
  - Calls `SetFontSavePath(langPtr)` to construct directory
  - Writes to `~/.dt/sessions/<current|home>/<lang>/dt.font.<l|m|h>` (resolution extension)
  - Also calls `_DtAddToResource(smGD.display, fontBuf)` to merge into RESOURCE_MANAGER
- File names constants in SmStrDefs.c:
  - `SM_FONT_FILE = "dt.font"`
  - `SM_CURRENT_FONT_DIRECTORY = "current.font"`
  - `SM_HOME_FONT_DIRECTORY = "home.font"`
  - `SM_LOW_RES_EXT = "l"`, `SM_MED_RES_EXT = "m"`, `SM_HIGH_RES_EXT = "h"`
  - `SM_SYSTEM_FONT_FILE = "sys.font"` (the default file)

### Restore on session start (SmRestore.c line 1566-1787)
- `RestoreIndependentResources()` called when language or resolution has changed
- Path: `~/.dt/sessions/<current|home>/<lang>/dt.font.<l|m|h>` (user-saved) OR `<etcpath>/<lang>/sys.font` (system default)
- Calls `RestoreResources(True, "-merge", "-file", smGD.fontPath, NULL)` (forks dtsession_res / xrdb)
- After merging, reads `*fontList` and XtSetValues on topLevelWid
- For each tag in `fonttype[]` array (SmRestore.c line 282-294), reads resource and writes back to `_DT_SM_PREFERENCES` via `_DtAddResString(dpy, resdata, _DT_ATR_PREFS)` — so fonts persist across login under same lang

### Consumer apps: how they read font resources
- **dtwm** (WmResource.c line 2141-2152): declares `XmNfontList` resource on `AppearanceData.fontList` (default "fixed"). Uses Motif XtGetSubresources to inherit from server db. Reads at startup from X resource database (which is RESOURCE_MANAGER).
- **dtfile** (Main.c line 777-780): declares `userFont` resource on `ApplicationArgsPtr.user_font` (default "Fixed"). Standard Xt resource lookup.
- **dtterm** (DtTermMain.c line 278): `"-fn"` command-line option mapped to `*userFont` resource. App-defaults file has `*userFontList:` for the font picker menu. Reads from X resources at startup.
- **dthelp**: no direct userFont/systemFont resource — falls back to default XmNtextFontList / XmNlabelFontList which are set by the system wildcard resources.
- **Common pattern**: All apps inherit font settings from the root window's RESOURCE_MANAGER property, which dtsession populates from sys.font or dt.font via xrdb at session start.

### file write path summary
- Application-defaults: `/usr/dt/app-defaults/<LANG>/Dtstyle` (read-only, system)
- System default fonts: `/etc/dt/config/<LANG>/sys.font` (read at session start)
- Per-session saved: `~/.dt/sessions/<current|home>/<lang>/dt.font.<res>` (read at next session start)
- Per-app session: `~/.dt/sessions/current/dtstyle.dtXXXXXX` (Xrm DB format with *Fonts.* keys)
- Server properties: `RESOURCE_MANAGER` (root window) and `_DT_SM_PREFERENCES` (root window, written via _DtAddResString)
