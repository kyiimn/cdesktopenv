
## WmGraphics.c Xft Migration Decisions (2026-06-16)

### Decision 1: Use DtFont API instead of raw Xft API for DrawStringInBox
**Rationale**: The task context explicitly points to `DtFont_DrawString`, `DtFont_DrawImageString`, `DtFont_TextWidth` as the Xft-aware drawing functions. Using the abstraction layer is cleaner and more maintainable than raw Xft calls. The previous raw Xft code in `DrawStringInBox` created/destroyed `XftDraw` per call and manually handled color allocation — DtFont handles all of this internally.

### Decision 2: Module-scoped wm_xft_font + WmSetXftFont() for WmDrawString
**Rationale**: `WmDrawString`'s public signature cannot be changed (called from WmFeedback.c, WmGraphics.c). Xft rendering requires an explicit font handle. A module-scoped static variable with a setter function preserves the existing API while enabling Xft for callers that have a DtFont available. When `wm_xft_font == NULL`, the function falls back to core X11, ensuring backward compatibility.

### Decision 3: Keep XftDraw cache but use it via DtFont API
**Rationale**: The XftDraw cache (`get_xft_draw()`) is kept for potential direct Xft usage but the primary rendering path now goes through `DtFont_DrawString`/`DtFont_DrawImageString` which manage their own XftDraw internally.

### Decision 4: Don't modify WmFeedback.c in this task
**Rationale**: WmFeedback.c calls both `WmDrawString` and `XTextExtents` directly. Making it fully Xft-aware requires changes beyond WmGraphics.c scope. The `WmDrawString` fallback to core X11 handles it gracefully for now.
