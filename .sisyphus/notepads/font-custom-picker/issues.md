
## 2026-06-17 Wave 1 Task 3+4: FontEnum.c Issues

### Pre-existing: FontEnum.h has broken XmFontList forward declaration
- Guard `_XmFontList_H` is not used by Motif 2.3+ Xm.h
- Motif uses `_Xm_h` (lowercase h, different name)
- Forward-declared type `struct _XmFontListRec *` is incompatible with Motif's `struct __XmRenderTableRec **`
- Workaround applied in FontEnum.c: pre-define `_XmFontList_H` to suppress the bad fwd decl
- **Should be fixed in FontEnum.h in a future change**: replace the forward declaration with inclusion of Xm.h using the same USE_XFT guard dance pattern
- See notepad/decisions.md for rationale on why we worked around it in FontEnum.c

## 2026-06-17 Task: Customize Button in Font.c

### Issues Resolved During Implementation
1. **FontEnum.h broken forward declaration is MORE WIDESPREAD than expected** — even just including `FontPicker.h` (which transitively pulls `Dt/FontEnum.h`) breaks compilation if the host Motif 2.3+ Xm.h is included first. The `_XmFontList_H` workaround needs to be applied in every .c file that includes FontPicker.h, not just FontPicker.c itself. **Future fix**: either patch FontEnum.h to use the actual Motif guard, or have FontPicker.h apply the workaround internally so consumers don't need to.
2. **XmCreatePushButtonGadget is NOT in Xm.h** on Motif 2.3+ — must explicitly include `<Xm/PushBG.h>`. Easy to miss because most other Xm widget creation functions ARE exposed via Xm.h transitively.

## 2026-06-17 Bug Fixes: restoreFonts XmFontList recreation + saveFonts buffer size

### Bug H1: restoreFonts did not recreate XmFontList
- **Symptom**: When dtsession restores a session where a custom font was selected, the Font dialog's preview widgets showed no font (NULL `customSysFont`/`customUserFont`).
- **Root cause**: `restoreFonts()` (Font.c) called `XtNewString()` on the saved custom font strings but never called `DtGetFontXmFontList()` to convert them back into XmFontList objects for the preview widgets.
- **Affected code**: lines ~1018-1041 (pre-fix). `FontDataSetCustomFont` had the matching creation logic (lines 1200-1206) but `restoreFonts` did not.
- **Fix applied**: 4 lines added after `font.hasCustomFont = ...`, mirroring the FontDataSetCustomFont pattern.
- **Lesson**: any function that restores struct state from Xrm must also recreate the derived resources (XmFontLists, Pixmaps, GCs, etc.) — string restoration is NOT enough.

### Bug M2: saveFonts buffer too small
- **Symptom**: `char bufr[1024]` in `saveFonts()` (Font.c:1067) was the CDE original size, designed for short XLFD names (~70 chars). Modern fontconfig patterns (which can be CSV-concatenated XLFDs from FontPicker) easily exceed 1024 bytes.
- **Risk**: `snprintf` with `sizeof(bufr)` prevented overflow but silently truncated the saved string, breaking session restore (the truncated pattern is invalid).
- **Fix applied**: buffer increased to 4096 (4x headroom).
- **Lesson**: when adding multi-XLFD CSV patterns to session state, audit every buffer that holds them.
