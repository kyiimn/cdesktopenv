
## WmGraphics.c Xft Migration (2026-06-16)

### Key Pattern: Stack-allocated DtFont from type-punned XFontStruct*
When `USE_XFT` is defined, `pAData->font` (typed as `XFontStruct*` in `_AppearanceData`) is actually an `XftFont*`. To use the DtFont API without changing struct definitions or function signatures, build a stack-allocated `struct _DtFont`:
```c
struct _DtFont _dtf_buf;
_dtf_buf.display = dpy;
_dtf_buf.screen  = DefaultScreen(dpy);
_dtf_buf.use_xft = True;
_dtf_buf.xft_font = (XftFont *) pfs;  /* type-punned */
_dtf_buf.xfs   = pfs;
_dtf_buf.fontset = NULL;
DtFont dtfont = &_dtf_buf;
```
This lets `DtFont_TextWidth()`, `DtFont_DrawString()`, etc. dispatch to Xft automatically.

### WmDrawString signature constraint
`WmDrawString(Display*, Drawable, GC, int, int, char*, unsigned int)` has no font parameter. The font is implicit in the GC (core X11) but Xft needs an explicit font. Solution: module-scoped `static DtFont wm_xft_font` set via `WmSetXftFont()` before calling `WmDrawString`, cleared afterwards. When `wm_xft_font == NULL`, falls back to core X11 rendering.

### XftDraw cache
Raw Xft rendering requires `XftDraw*` objects. Creating/destroying per-call is wasteful. Use a cached `XftDraw` with `XftDrawChange()` to retarget to different drawables.

### WmFeedback.c not yet Xft-aware
`WmFeedback.c` calls `WmDrawString` and `XTextExtents` directly. It doesn't call `WmSetXftFont`, so `WmDrawString` falls back to core X11. This needs a separate task.

### Motif USE_XFT save/restore dance
Every dtwm .c file that uses USE_XFT must save/undef/restore it around `#include <Xm/Xm.h>` because Motif 2.3+ defines USE_XFT unconditionally. `WmGraphics.h` includes `<Dt/DtFont.h>` under `#ifdef USE_XFT`; DtFont.h has its own save/restore.

## XFT Crash-Prone Code Path Audit (2026-06-16)

### Files auditing XmFONT_IS_FONT/FONTSET/XFT in .c files
Searched the entire CDE codebase (16 .c files contain `XmFONT_IS_` references). The Motif 2.3+ Motif/Xm header defines `XmFONT_IS_XFT` numerically (3) so an XFT-typed entry can silently fall through to "treat as XFontStruct" branches.

### Confirmed bugs (crash or wrong-type cast on XFT)

| Location | Pattern | Status |
|---|---|---|
| `dtfile/FileMgr.c:883-891` | `XmFontListEntryGetFont` then if-else on FONT/FONTSET only; casts `entry_font` as `XFontStruct*` on XFT | BUG |
| `dtfile/ChangeDirP.c:930-940` | if-else on `cd_fonttype` FONTSET vs default; XFT falls to default (cast as XFontStruct, then `cd_font->ascent`) | BUG |
| `dtcm/dtcm/font.c:68-125` (`get_font`) | function returns first FONT/FONTSET entry; no XFT case in initial `*type_return = XmFONT_IS_FONT` path | BUG (XFT entries saved with type_return = XmFONT_IS_XFT, then caller's XFT branch sees data correctly only if USE_XFT is defined) |
| `lib/DtHelp/Font.c:870-886` (`__DtHelpFontDatabaseInit`) | `if (string_font == NULL) type = XmFONT_IS_FONT; ... if (type == XmFONT_IS_FONTSET) SaveFontSet else SaveFontStruct` — XFT cast as XFontStruct** | BUG (corrupted type stored) |
| `lib/DtHelp/Font.c:1106-1111` (`__DtHelpFontMetrics`) | `else if (font_index < font_info.struct_cnt)` — does not check `font_index >= 10000` for XFT. With USE_XFT disabled would segfault; with USE_XFT enabled, `__DtHelpFontStructGet` returns NULL for >=10000 so first check is OK | OK (defensive) |
| `lib/DtTerm/TermPrim/TermPrim.c:679-703` | while loop: if FONTSET do X, else cast as XFontStruct + call XGetFontProperty; XFT case unhandled | BUG (XGetFontProperty on XftFont*) |
| `lib/DtWidget/Icon.c:3431-3434` | `XmeRenderTableGetDefaultFont` then `font->fid`; no XFT check | BUG (similar in Control.c, dtprintinfo Icon.c, PrintSetupB.c) |
| `lib/DtWidget/Control.c:1744-1747` | same pattern | BUG |
| `programs/dtprintinfo/libUI/MotifUI/Icon.c:863-864` | same pattern | BUG |
| `lib/DtPrint/PrintSetupB.c:3404-3418` | XGetFontProperty + font->per_char / font->max_bounds access | BUG |
| `programs/dtwm/WmResource.c:4280-4308` (`FallbackMakeTitleHeight`) | switch handles FONT/FONTSET only; XFT falls to default (just `break`); titleHeight not calculated | FUNCTIONAL BUG (silent miscalc) |
| `programs/dtwm/WmResource.c:4400,4411` | `(pAData->font)->ascent + (pAData->font)->descent` — if pAData->font is XftFont, this dereferences XftFont fields as XFontStruct | CRASH BUG |
| `programs/dtwm/WmResource.c:4665` (`GetAppearanceGCs`) | `font->fid` | CRASH BUG (called from lines 4506, 4596 with pAData->font) |

### Files that handle XFT correctly
- `lib/DtHelp/HelpUtil.c:2410-2456` (else-if XFT in #ifdef USE_XFT)
- `lib/DtHelp/Format.c:320-360` (XmFontListEntryCreate with all 3 types)
- `lib/DtHelp/GlobSearch.c:378-385` (XmFontListEntryLoad)
- `lib/DtWidget/Editor.c:1789-1963` (extractFontMetrics handles all 3)
- `programs/dtwm/WmPresence.c:1230-1264` (switch with XFT case)
- `programs/dtfile/ChangeDirP.c:1368-1390, 1401-1480` (get_textwidth and draw_imagestring have XFT switch case)
- `programs/dtfile/FileMgr.c:1755-1774` (cd_font init handles XFT)
- `programs/dtcm/dtcm/x_graphics.c:1116-1142` (FontList iteration handles XFT)
- `programs/dtcm/dtcm/font.c:144-151, 265-294, 357-396, 414-457, 464-499` (all if-else chains handle XFT in #ifdef USE_XFT)
- `programs/dtcm/dtcm/calendarA.c:3342-3374` (assigns cf_type, no crash)
- `lib/DtFont/XftWrapper.c:420-434` (creates XFT entry)
- `programs/dtappbuilder/src/ab/brws_edit.c:131-134` (creates FONT entry)
- `programs/dtwm/Clock.c:1050-1063` (XmeRenderTableGetDefaultFont + USE_XFT guard)

### File.c:6623 is in a comment
`File.c` line 6623 is inside a `/* ... */` block (lines 6619-6640). Not active code. No fix needed.

### Pattern to remember
When XmeRenderTableGetDefaultFont is called and the result is treated as XFontStruct* (via `font->fid`, `font->ascent`, etc.), the function must be guarded by USE_XFT (per Clock.c pattern) OR the return value must be type-checked via XmFontListEntryGetFont + XmFontType.
