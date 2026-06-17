
## 2026-06-17 Wave 1 Task 3+4: FontEnum.c Decisions

### Decision: Work around FontEnum.h bug in FontEnum.c (do not modify header)
- Task explicitly said "Do NOT modify FontEnum.h (already created)"
- Bug: type conflict between forward-declared XmFontList and real Xm.h typedef
- Decision: pre-define `_XmFontList_H` in FontEnum.c BEFORE including FontEnum.h
  - Effect: the broken `#ifndef _XmFontList_H ... #endif` block in FontEnum.h is skipped
  - The real XmFontList typedef from Xm.h (included via Dt/DtFont.h) is used
- Documented with a comment block in FontEnum.c explaining the workaround
- Trade-off: if a future developer "cleans up" the unused macro, the compile error returns
- This is acceptable because the comment makes the intent obvious

### Decision: Static helpers, no shared internal header
- `xlfd_to_family()`, `family_strcasecmp()`, `is_duplicate_family()`, `build_list_from_xlfd()` are all `static`
- FC-only helpers (`build_list_from_xlfd_no_free`) are also `static` but forward-declared
- Avoids polluting the global namespace
- Keeps the public API (FontEnum.h) clean

### Decision: Use `extern` declarations for FC-only functions at call site
- `DtEnumerateFontFamiliesFC`, `DtEnumerateFontSizesFC`, `DtMergeFontLists` are defined under USE_XFT
- The public `DtEnumerateFontFamilies`/`DtEnumerateFontSizes` are NOT under USE_XFT
- Use `extern` declarations inside the USE_XFT blocks where these are called
- Avoids polluting the public API in FontEnum.h
- Avoids needing a separate internal header

### Decision: DtMergeFontLists takes ownership semantics
- Inputs are NOT freed (caller's responsibility)
- Result is a fresh allocation that needs DtFreeFontList
- This matches the convention of other CDE APIs (e.g., SmNewFontSettings)
