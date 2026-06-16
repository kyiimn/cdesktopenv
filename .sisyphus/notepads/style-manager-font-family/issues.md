# Issues - Style Manager Font Family

## F2/F3 Bug Fix Wave (2026-06-15)
- F2 reported buffer overread: `GetSysFontResource(7)` accessed past end of 7-element arrays
  - Fixed by adding family1*_resources tables (4 tables × 7 entries each)
- F3 reported NULL derefs in 3 places: Font.c 227-228, 713, 776, 871
  - Fixed with NULL guards / ?: fallbacks
- F3 also reported Cancel flicker from double preview update
  - Fixed by reordering: family revert BEFORE size revert
- numFamilies clamping added as defense against bogus Xrdb values

## R1/R2/R3 Deep Verification Bug Fix Wave (2026-06-15)
- C1 (CRITICAL): famStr use-after-free — XmStringFree(famStr) was called before XmCreateScrolledList used it
  - Fixed by splitting into if/else: famStr created+used+freed inside if branch, else branch creates without famStr
- C2 (CRITICAL): restoreFonts familyIndex overwritten by CreateFontDlg resetting selectedFontIndex to -1
  - Fixed by saving selectedFontIndex in popup_fontBB before CreateFontDlg, restoring after, with XmListSelectPos(False)
- C3 (CRITICAL): numFamilies<=1 sizeTB layout collapse — leftWidget familyTB unmanaged → dangling attachment
  - Fixed by re-attaching sizeTB left to XmATTACH_FORM after XtUnmanageChild(font.familyTB)
- C4 (CRITICAL): ButtonCB userStr NULL dereference for Family 2+ indices (no resources loaded)
  - Fixed by adding NULL guard for userStr/sysStr before strcspn/memcpy in OK handler
- C5 (CRITICAL): restoreFonts x/y value.addr NULL — same pattern as ismapped fix
  - Fixed by adding `&& value.addr != NULL` to x and y XrmQGetResource checks
- M1 (MEDIUM): XmString memory leak in familyItems/sizeItems — individual XmStrings not freed before array XtFree
  - Fixed by adding XmStringFree loops before XtFree for both arrays
- M2 (MEDIUM): Legacy resource name override — GetFamily0FontResources re-loaded sysStr/userStr, clobbering admin customizations
  - Fixed by removing sysStr/userStr XtGetApplicationResources calls from GetFamily0FontResources (already loaded by GetFontStrResources)
- M3 (MEDIUM): Cancel doesn't deselect familyList when originalFontIndex < 0
  - Fixed by adding XtVaSetValues(font.familyList, XmNselectedItemCount, 0, NULL) in Cancel else branch
- M4 (MEDIUM): snprintf sizeof(style.tmpBigStr) instead of sizeof(bufr) — technically same size but incorrect practice
  - Fixed by changing to sizeof(bufr) in both x and y snprintf calls

## 2026-06-16 font->fid Crash Fixes
- Fixed `cde/lib/DtWidget/Icon.c:3431` — wrapped GCFont/fid in USE_XFT guard
- Fixed `cde/lib/DtWidget/Control.c:1744` — wrapped GCFont/fid in USE_XFT guard
- Fixed `cde/programs/dtprintinfo/libUI/MotifUI/Icon.c:863` — restructured if/else for Xft vs core path
- Fixed `cde/lib/DtPrint/PrintSetupB.c:3404-3418` — replaced XGetFontProperty block with XftFont cast for char_width
- All 4 files now have `#include <X11/Xft/Xft.h>` inside `#ifdef USE_XFT` (with `HAVE_X11_XFT_XFT_H` double-guard matching Editor.c pattern)
- PrintSetupB.c required adding the full USE_XFT save/restore dance (it was the only DtPrint file lacking it)

## 2026-06-16 Comprehensive XFontStruct Crash Audit (USE_XFT enabled)

### Methodology
Searched ENTIRE cde/ tree for `->fid`, `->ascent`/`->descent`, `->max_bounds`/`->min_bounds`/`->per_char`/`->n_glyphs`, `XGetFontProperty`, `XTextExtents`, `XTextWidth`, `(XFontStruct *)` casts. For each hit, traced the variable's source and verified whether USE_XFT guard is present.

### CRASH Candidates (Unguarded access to XFontStruct fields when pointer could be XftFont*)

#### 1. `cde/programs/dtinfo/dtinfo/src/Widgets/TabButton.c:643`
- Pattern: `values.font = fs->fid;`
- Variable: `fs` from `_XmFontListSearch(tabw->label.font, XmFONTLIST_DEFAULT_TAG, &myindex, &fs)`
- NO USE_XFT guard
- dtinfo IS built with USE_XFT (Makefile:209)
- Under USE_XFT, `fs` could be `XftFont*` cast → CRASH (XftFont has no `fid`)
- Severity: **CRASH**
- Fix: wrap in `#ifdef USE_XFT` like Control.c:1750-1755

#### 2. `cde/programs/dtappbuilder/src/ab/ui_util.c:764-774`
- Pattern: `font = objxm_fontlist_to_font(fontlist);` then `XGetFontProperty(font, ...)`, `font->per_char`, `font->min_char_or_byte2`, `font->max_char_or_byte2`, `font->max_bounds.width/ascent/descent`
- Variable: `font` from `objxm_fontlist_to_font()` which calls `XmFontListGetNextFont` without type check
- NO USE_XFT guard, NO NULL check, NO type check
- Under USE_XFT, `font` could be `XftFont*` cast → CRASH
- Even without USE_XFT, if `XmFontListGetNextFont` returns FALSE, font=NULL → CRASH
- Severity: **CRASH** (dual risk)
- Fix: 
  - `objxm_fontlist_to_font` needs to add type-check (use `XmFontListEntryGetFont` instead, or check for `XmFONT_IS_FONT` via `XmFontListInitFontContext` + `XmFontListEntryGetFont`)
  - Or guard callers with `#ifdef USE_XFT`

#### 3. `cde/programs/dtwm/WmFeedback.c:295, 641, 661`
- Pattern: `XTextExtents(pSD->feedbackAppearance.font, ...)` (lines 295, 641, 661 in file — NOT 255, 590, 599 from grep offset confusion)
- Variable: `pSD->feedbackAppearance.font` is `XFontStruct*` (AppearanceData:917 in WmGlobal.h)
- Source: from `XmeRenderTableGetDefaultFont(pAData->fontList, &(pAData->font))` in WmResource.c:4358
- NO USE_XFT guard
- Under USE_XFT, `pAData->font` can be `XftFont*` cast → CRASH on XTextExtents (Xft has no `fid`/no extents API)
- Severity: **CRASH**
- Note: The `->fid` at WmResource.c:4694 IS guarded. The feedback appearance XTextExtents calls were missed.
- Fix: 
  - In `UpdateFeedbackText` (line 285) and `UpdateFeedbackSize` (line 635, 651) — add `#ifdef USE_XFT` blocks that use `XftTextExtents8` like the lines 287-292, 634-639, 651-659 patterns already there, OR
  - Verify `XTextExtents` calls are inside `#else` of USE_XFT (they currently are NOT)

### NULL/Index-Out-Of-Bounds Risks (not strictly XFT, but related)

#### 4. `cde/lib/DtHelp/XUICreate.c:580`
- Pattern: `value = XGetFontProperty(__DtHelpFontStructGet(pDAS->font_info, n), xa_ave_width, ...)`
- `n` from `__DtHelpDefaultFontIndexGet(pDAS)` — under USE_XFT can be >= 10000 (Xft)
- `__DtHelpFontStructGet` returns NULL for >= 10000 under USE_XFT
- `XGetFontProperty(NULL, ...)` → CRASH
- Severity: **CRASH** (when default font is Xft-loaded)
- Note: The follow-on code at lines 590-602 handles the n>=10000 case via `__DtHelpFontXftGet`. The XGetFontProperty call at 580 was missed.
- Fix: add check `if (n >= 10000) value = False;` before line 580 (or restructure to use Xft path)

### SAFE / Already Guarded (verified, NOT bugs)

- `cde/lib/DtFont/DtFont.c` — entire file wrapped in `#ifndef USE_XFT`
- `cde/lib/DtFont/XftWrapper.c` — entire file wrapped in `#ifdef USE_XFT`
- `cde/lib/DtWidget/Control.c:1754` — guarded with USE_XFT
- `cde/lib/DtWidget/Icon.c:3442` — guarded with USE_XFT
- `cde/lib/DtWidget/Editor.c:1888-1900, 1930-1937, 1943-1952` — properly type-routed (FONTSET, XFT, FONT branches)
- `cde/lib/DtPrint/PrintSetupB.c:3416-3436` — properly guarded
- `cde/programs/dtprintinfo/libUI/MotifUI/Icon.c:868-874` — properly guarded
- `cde/programs/dtwm/Clock.c:1050-1062` — properly guarded
- `cde/programs/dtwm/WmResource.c:4694` — properly guarded
- `cde/programs/dtwm/WmResource.c:4306, 4415, 4431` — properly guarded (Xft case in FallbackMakeTitleHeight)
- `cde/programs/dtwm/WmGraphics.c:974-991` — properly guarded
- `cde/programs/dtwm/WmFeedback.c:287-293, 634-639, 651-659` — Xft path is inside `#ifdef USE_XFT`
- `cde/programs/dtwm/WmPresence.c:1240-1252` — properly type-routed
- `cde/programs/dtksh/dtkcmds.c:2514, 2760` — inside `#ifndef USE_XFT` (core path) or uses XftTextExtents8
- `cde/programs/dtcm/dtcm/font.c` — all accesses inside `cf_type == XmFONT_IS_FONT` blocks
- `cde/programs/dtcm/dtcm/x_graphics.c:1123-1125` — inside `XmFONT_IS_FONT` branch
- `cde/programs/dtfile/FileMgr.c:883-897, 1761-1780` — properly type-routed (FONTSET, XFT, FONT branches)
- `cde/programs/dtfile/File.c:6631-6633` — inside comment block (not compiled)
- `cde/programs/dtfile/ChangeDirP.c:944-946, 1381` — properly type-routed
- `cde/programs/dtappbuilder/src/ab/brws.c:1894-1895` — `sm_font` is from `XLoadQueryFont` (always real XFontStruct)
- `cde/programs/dtappbuilder/src/ab/brws_mthds.c:2342-2347, 2519-2533, 2816, 3349` — `sm_font` from XLoadQueryFont
- `cde/programs/dthelp/dthelpprint/PrintTopics.c:2784, 2800` — `defaultFont`/`pageNumberFont` from XLoadQueryFont
- `cde/lib/DtHelp/Font.c:1129-1136` — inside `if (font_index < 0)` fontset branch, uses `XFontsOfFontSet` results
- `cde/lib/DtHelp/Font.c:1159-1161` — inside else branch (struct_cnt check)
- `cde/lib/DtHelp/XInterface.c:808-810` — inside Xft path block
- `cde/lib/DtHelp/XInterface.c:851` — protected by line 763 Xft case + font_index bounds
- `cde/lib/DtHelp/XInterface.c:2478` — protected by line 2461 Xft case + font_index bounds
- `cde/lib/DtHelp/HelpUtil.c:2429` — inside `XmFONT_IS_FONT` branch
- `cde/lib/DtHelp/HelpUtil.c:2470` — inside FONTSET branch
- `cde/lib/DtHelp/XUICreate.c:254, 303-321` — inside `XmFONT_IS_FONTSET` (254) or XFT-guarded (303-321)
- `cde/lib/DtHelp/Format.c:184, 352-358` — not dereferenced or type-checked before use
- `cde/lib/DtTerm/TermPrim/TermPrimRenderFont.c:53, 81-83, 115, 125, 163-172` — only used for core font path (FontRenderFunction dispatched via vtable)
- `cde/lib/DtTerm/TermPrim/TermPrimRenderFontSet.c:164-273` — fontset path
- `cde/lib/DtTerm/TermPrim/TermPrimRenderXft.c:192, 276, 308-309` — Xft path
- `cde/lib/DtTerm/TermPrim/TermPrimRenderLineDraw.c` — custom struct
- `cde/lib/DtTerm/TermPrim/TermPrimLineDraw.c:392, 442, 561-627` — custom struct (lineDrawFont)
- `cde/lib/DtTerm/TermPrim/TermPrimRender.c:104` — TermFont struct
- `cde/lib/DtTerm/TermPrim/TermPrimCursor.c` — TermFont struct
- `cde/lib/DtTerm/TermPrim/TermPrim.c:797-799, 853-856, 1153-1154, 1274` — Xft-guarded early return (lines 790-804) or type-checked `else` branch
- `cde/lib/DtTerm/TermPrim/TermPrim.c:709, 724, 843-847, 852-859` — proper type-routing
- `cde/lib/DtTerm/TermView/TermViewMenu.c:558-560` — inside USE_XFT block
- `cde/programs/dtudcfonted/libfal/falfont.c` — internal FAL (Fujitsu Abstraction Layer) struct, not XFontStruct
- `cde/programs/dtudcfonted/libfal/_falomGeneric.c` — FAL internal
- `cde/programs/dtudcfonted/libfal/readpcf.c` — PCF file parser
- `cde/programs/dtudcfonted/dtgpftobdf/gpftobdf.c` — GPF parser
- `cde/programs/dthello/dthello.c:1170` — inside USE_XFT block
- `cde/examples/dtdnd/icon.c:248, 256` — dtdnd example, NOT in autotools build (NO_RISK for production)

### Summary

| File | Severity | Status |
|------|----------|--------|
| `dtinfo/src/Widgets/TabButton.c:643` | **CRASH** | Needs fix |
| `dtappbuilder/src/ab/ui_util.c:764-774` | **CRASH** | Needs fix (and objxm_util.c:382) |
| `dtwm/WmFeedback.c:295, 641, 661` | **CRASH** | Needs fix |
| `lib/DtHelp/XUICreate.c:580` | **CRASH** | Needs fix (when default font is Xft) |

### Fix Priority

1. **HIGHEST**: `dtinfo/dtinfo/src/Widgets/TabButton.c:643` — TabButton is fundamental UI in dtinfo
2. **HIGH**: `dtwm/WmFeedback.c:295, 641, 661` — Window manager feedback (resize/move) — user would crash on every window move
3. **MEDIUM**: `dtappbuilder/src/ab/ui_util.c:764-774` — App builder builder panes, may not be in default desktop session
4. **MEDIUM**: `lib/DtHelp/XUICreate.c:580` — Help system, only triggers if default font is Xft-loaded

## 2026-06-16 XUICreate.c Font.c Crash Fix (fontType uninitialized + wrong type)

### Bug Description
In `cde/lib/DtHelp/XUICreate.c:562`, when `default_list` is NULL or `XmFontListInitFontContext` fails, `fontType` remains uninitialized and `default_font` is NULL. Then `__DtHelpFontDatabaseInit(pDAS, NULL, fontType_uninitialized, tmpFont)` is called. Inside `Font.c:906-912`, when `string_font == NULL`, the fallback sets `string_font = user_font` (which is `tmpFont` from `XmeRenderTableGetDefaultFont`) and hardcodes `type = XmFONT_IS_FONT`. Under USE_XFT, `XmeRenderTableGetDefaultFont` returns `XftFont*`, so passing it as `XmFONT_IS_FONT` to `SaveFontStruct` casts it as `XFontStruct*` → crash.

### Fix
In `cde/lib/DtHelp/Font.c:906-913`, when `string_font == NULL` and we fall back to `user_font`:
- Under `USE_XFT`: if `user_font != NULL`, set `type = XmFONT_IS_XFT` (since `XmeRenderTableGetDefaultFont` returns XftFont* under USE_XFT)
- If `user_font == NULL`, fall through to `type = XmFONT_IS_FONT` (nothing gets saved, safe default)
- Without USE_XFT: unchanged, `type = XmFONT_IS_FONT` as before

This is the correct fix because under USE_XFT, all render table entries store fontconfig-pattern fonts as XmFONT_IS_XFT, so the default font IS always XftFont*.
