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
