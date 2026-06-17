# Style Manager Font Custom Picker — User Font Selection & System-Wide Application

## TL;DR

> **Quick Summary**: CDE Style Manager Font 대화상자에 "Customize..." 버튼을 추가하여 사용자가 시스템의 폰트를 직접 조회·선택할 수 있게 한다. 선택한 폰트는 현재 family+size 슬롯에 적용되며, _DtAddToResource를 통해 CDE 전체에 즉시 전파되고, SmNewFontSettings를 통해 세션 영속화된다. USE_XFT 활성 시 fontconfig 폰트도 열거 가능하다. "Apply System-Wide" 기능으로 root 권한으로 /etc/dt/app-defaults/Dtstyle에 기록하여 모든 사용자의 기본값을 변경할 수 있다.
>
> **Deliverables**:
> - Font enumeration library (lib/DtFont/FontEnum.c): XListFonts + FcFontList 래퍼
> - Custom Font Picker 다이얼로그 (FontPicker.c): Motif 기반 폰트 선택 UI
> - Font.c 통합: "Customize..." 버튼 + fontres 생성 로직 확장
> - Resource 전파: _DtAddToResource + SmNewFontSettings 기존 패턴 재사용
> - System-wide 적용: XrmPutFileDatabase()로 /etc/dt/app-defaults/Dtstyle 기록 (root)
> - configure.ac + Makefile.am 업데이트: fontconfig lib 의존성 추가
>
> **Estimated Effort**: Large
> **Parallel Execution**: YES - 5 waves
> **Critical Path**: Wave 1 (font enum lib) → Wave 2 (picker UI) → Wave 3 (integration) → Wave 4 (system-wide) → Wave 5 (i18n + verification)

---

## Context

### Original Request
UI에서 사용자가 직접 시스템의 폰트를 조회하여 설정할 수 있는 기능을 추가. CDE 전체에 유연하게 폰트가 적용될 수 있도록 _DtAddToResource 메커니즘을 활용. 시스템 전체 적용 루틴도 추가 (root 권한 필요). USE_XFT 시 fontconfig 폰트도 설정 가능하도록.

### Interview Summary

**Key Discussions**:
- **리소스 전파 메커니즘**: _DtAddToResource()는 X 서버 RESOURCE_MANAGER 속성에 병합 (즉시 효과). SmNewFontSettings()는 dtsession에 DT_SM_FONT_INFO 속성 전송. dtsession이 세션 저장 시 ~/.dt/sessions/current/dt.font.<res>에 기록.
- **사용자 app-defaults 기록은 비표준**: CDE는 파일이 아닌 X 서버 속성 + 세션 매니저 IPC를 사용. 파일 기록은 dtsession만 담당.
- **fontconfig 의존성**: CDE 코드베이스에 FcFontList/FcObjectSetCreate 전혀 없음. 새로 추가 필요. USE_XFT ifdef로 가드.
- **선택 적용 범위**: 사용자가 선택한 폰트는 해당 family+size 슬롯에만 적용 (예: "System" family의 "Medium" size). 전체 family 일괄 변경은 별도 UI로.

**Research Findings**:
- CDE 코드베이스에 폰트 열거 UI 없음 — 모든 프로그램이 고정 목록 사용
- XListFonts()는 dtterm, dtcm, dtinfo에서 이미 사용 (단일 폰트 조회/최적 매치)
- DtHelp/Font.c: FcNameParse + XftFontOpenPattern 사용 (단일 폰트 로드만, 열거 없음)
- DtFont/XftWrapper.c: FcNameParse 검증만, 열거 없음
- _DtAddToResource 구현: addToRes.c — X 서버 RESOURCE_MANAGER + _DT_SM_PREFERENCES 속성에 병합
- SmNewFontSettings 구현: Protocol.c:762 — dtsession 윈도우에 XChangeProperty
- dtsession SmSave.c:1651-1686 — fontBuf를 XrmPutFileDatabase로 파일에 기록

### Metis Review (Critical Findings Applied)

**Identified Gaps (addressed)**:
1. **Custom font → 슬롯 적용 방식**: fontChoice[] 배열은 정적 크기. 선택된 폰트는 해당 슬롯의 sysStr/userStr을 덮어쓰고, XmFontList를 재생성. → **해결**: fontChoice[FONT_INDEX(fam,sz)]의 필드를 직접 교체.
2. **Xft 폰트를 XmFontList로 변환**: Motif는 XftFont*를 직접 받지 않음. _DtFontCreateXmFontList() (DtFont.h:111) 사용. → **해결**: USE_XFT 시 이 함수로 XmFontList 생성.
3. **리소스 문자열 포맷**: fontres는 XLFD 또는 fontconfig 패턴 모두 가능. Motif가 자동 처리. → **확인 필요**: USE_XFT 비활성 시 fontconfig 패턴이 작동하는지.
4. **RESOURCE_MANAGER 속성 크기 제한**: X 서버 속성은 최대 ~100MB이므로 폰트 몇 개 추가는 문제 없음.
5. **시스템 전체 적용 권한**: /etc/dt/app-defaults/Dtstyle은 root만 쓰기 가능. dtstyle은 일반 사용자로 실행되므로, 별도 setuid helper 또는 pkexec 필요. → **해결**: XrmPutFileDatabase()로 임시 파일에 쓴 후 system("dtstyle_syswide_apply ...") 호출. helper 스크립트는 root 소유 setuid 또는 pkexec 사용.
6. **fontres 형식**: 기존 12개 리소스 키에 *CustomFont<N> 추가 필요. → **해결**: 대신 *systemFont/*userFont 값을 직접 교체하는 방식이 CDE 아키텍처에 부합.

### Momus Review (High Accuracy — Revision Applied)

**MUST-FIX Issues (fixed before implementation)**:
1. **Task 14+16 System-wide flow broken** (Issue 6+7): C process `mkstemp` in `/etc/dt` fails for non-root. `xrdb -merge` writes to X server, not file. → **Fixed**: Restructured to C code writes to `/tmp`, pkexec helper merges via awk dedup (not raw cat, not xrdb).
2. **Task 8 XLFD↔fontconfig conversion is dead code** (Issue 8): `_DtFontCreateXmFontList()` accepts both formats. Conversion is lossy and unnecessary. → **Fixed**: Dropped conversion functions. Replaced with `detect_pattern_type()` and `pattern_to_display_name()` only. Pass raw patterns to `_DtFontCreateXmFontList()`.
3. **Task 9 "Customize..." button layout bug** (Issue 2): Button attached to `familyTB` which is unmanaged when `numFamilies <= 1`. → **Fixed**: Attach to `XmATTACH_FORM` instead. Use `XtManageChild()` directly (not `widget_list[]`).

**SHOULD-FIX Issues (fixed before implementation)**:
4. **Task 9 `widget_list[7]` overflow** (Issue 1): Array at capacity. → **Fixed**: New buttons use `XtManageChild()` directly.
5. **Task 7 duplicate preview** (Issue 3): FontPicker preview vs Font dialog preview. → **Fixed**: Clarified that FontPicker has its own preview for isolated font viewing.
6. **Task 20 build verification** (Issue 10): `CFLAGS=-DUSE_XFT` doesn't work. → **Fixed**: Use `./configure --enable-xft` / `--disable-xft`.
7. **Task 7 BUG inline comments** (Issue 4): Removed confusing review noise. → **Fixed**.
8. **Task 17 msg slot audit** (Issue 9): Need to audit existing set 5 occupancy. → **Fixed**: Added audit requirement.
9. **Task 18 Dtstyle.src empty defaults** (Issue 9): Empty string vs NULL in XtResource. → **Fixed**: Use `(caddr_t) ""` as default, not `NULL`.

**Guardrails Applied**:
- G1: 기존 SystemFont1..7/UserFont1..7 리소스명 변경 금지
- G2: Fontset 필드 순서/제거 금지 (XtOffset 의존성)
- G3: ButtonCB fontres 형식 변경 금지 (다른 CDE 앱이 읽음) — 추가만 가능
- G4: SmNewFontSettings 프로토콜 변경 금지
- G5: USE_XFT include guard dance 보존
- G6: dtstyle은 fontconfig/Xft 직접 의존 금지 (lib/DtFont 통해서만)
- G7: numFamilies<=1일 때 기존 UI와 동일
- NEW-G8: Xft/fontconfig 코드는 반드시 #ifdef USE_XFT 내에 배치
- NEW-G9: fontChoice[] 배열 크기 변경 금지
- NEW-G10: _DtAddToResource/SmNewFontSettings 호출 시그니처 변경 금지
- NEW-G11: widget_list[7]에 새 버튼 추가 금지 — XtManageChild() 직접 사용
- NEW-G12: customizeButton는 familyTB가 아닌 form에 직접 첨부 (numFamilies≤1 시에도 표시)
- NEW-G13: 시스템 전체 적용 시 /etc/dt에 직접 쓰지 않음 — /tmp에 쓰고 pkexec로 root 이관
- NEW-G14: Xrm 병합은 XrmGetFileDatabase+XrmMergeDatabases 사용 (raw cat >> 금지)
- NEW-G15: XLFD↔fontconfig 변환 함수 추가 금지 — _DtFontCreateXmFontList가 두 포맷 모두 처리
- NEW-G16: USE_XFT는 configure로 설정 (CFLAGS 아님) — 빌드 검증은 ./configure --enable/disable-xft 사용

---

## Work Objectives

### Core Objective
CDE Style Manager Font 대화상자에 폰트 피커를 추가하여 사용자가 시스템에 설치된 폰트를 직접 선택할 수 있게 한다. 선택한 폰트는 CDE 전체에 전파되며, root 권한으로 시스템 전체 기본값으로 설정할 수도 있다.

### Concrete Deliverables
- `cde/lib/DtFont/FontEnum.c` + `FontEnum.h`: X11/fontconfig 폰트 열거 라이브러리
- `cde/programs/dtstyle/FontPicker.c` + `FontPicker.h`: 폰트 피커 다이얼로그
- `cde/programs/dtstyle/Font.c` 수정: "Customize..." 버튼 + 피커 연동 + fontres 확장
- `cde/programs/dtstyle/Resource.c`/`Resource.h` 수정: customFont 리소스 추가
- `cde/programs/dtstyle/Main.h` 수정: CustomFontData 구조체 추가
- `cde/programs/dtstyle/Protocol.c`/`Protocol.h`: system-wide 적용 함수 추가
- `cde/programs/dtstyle/dtstyle.msg`: 새 메시지 추가
- `cde/configure.ac`: fontconfig/libfontconfig 의존성 체크 추가
- `cde/programs/dtstyle/Makefile.am`: FontEnum, FontPicker 소스 추가
- `cde/lib/DtFont/Makefile.am`: FontEnum 소스 추가

### Definition of Done
- [ ] Font 피커 다이얼로그가 X11 core 폰트를 열거하고 선택할 수 있음
- [ ] USE_XFT 활성 시 fontconfig 폰트도 열거 가능
- [ ] 선택한 폰트가 기존 family+size 슬롯에 적용됨
- [ ] _DtAddToResource를 통해 X 서버 RESOURCE_MANAGER에 전파됨
- [ ] SmNewFontSettings를 통해 dtsession에 전파됨
- [ ] "Apply System-Wide" 버튼이 /etc/dt/app-defaults/Dtstyle에 폰트 리소스를 기록함 (root 필요)
- [ ] USE_XFT 비활성 시에도 빌드 가능 (fontconfig 코드가 ifdef 내에 있음)
- [ ] 기존 패밀리 선택 기능이 정상 작동함

### Must Have
- 폰트 피커 UI (family/size 외 "Customize..." 버튼)
- X11 core 폰트 열거 (XListFonts)
- fontconfig 폰트 열거 (USE_XFT 시)
- 선택 폰트의 XmFontList 생성 및 슬롯 적용
- _DtAddToResource + SmNewFontSettings 전파
- 세션 저장/복원에 customFont 정보 포함
- "Apply System-Wide" 기능 (root 권한)
- USE_XFT ifdef 가드

### Must NOT Have (Guardrails)
- ❌ fontChoice[] 배열 크기 변경
- ❌ _DtAddToResource/SmNewFontSettings 시그니처 변경
- ❌ ButtonCB fontres 기존 12개 키 순서/형식 변경
- ❌ dtstyle이 fontconfig/Xft 헤더를 직접 include (lib/DtFont 통해서만)
- ❌ USE_XFT 비활성 시 fontconfig 코드 실행
- ❌ 다른 CDE 앱(dtterm/dtfile)의 독립 폰트 UI 변경
- ❌ Motif XmFontSelectionBox 사용

---

## Verification Strategy (MANDATORY)

> **ZERO HUMAN INTERVENTION** - ALL verification is agent-executed. No exceptions.

### Test Decision
- **Infrastructure exists**: NO (CDE has no automated test suite)
- **Automated tests**: NONE (build verification only)
- **QA Policy**: Build verification + static analysis + code review for every task

### QA Tool Mapping
- **Build**: `Bash` (`make` 명령)
- **Static analysis**: `Grep` (ifdef 가드, 심볼 참조)
- **Resource verification**: `Bash` (`xprop -root` 또는 코드 리뷰로 전파 로직 검증)
- **Code review**: 정적 분석 (XtOffset 정확성, NULL 처리, ifdef 가드)

---

## Execution Strategy

### Parallel Execution Waves

```
Wave 1 (Foundation - font enumeration library, MAX PARALLEL):
├── Task 1: Add fontconfig dependency to configure.ac + Makefile.am
├── Task 2: Create FontEnum.h public API header
├── Task 3: Create FontEnum.c — X11 core font enumeration (XListFonts)
└── Task 4: Create FontEnum.c — fontconfig enumeration (#ifdef USE_XFT)

Wave 2 (Font Picker UI):
├── Task 5: Create FontPicker.h — dialog data structures
├── Task 6: Create FontPicker.c — dialog layout (family list, preview, buttons)
├── Task 7: Create FontPicker.c — font enumeration integration + preview rendering
└── Task 8: Create FontPicker.c — XLFD ↔ fontconfig pattern conversion

Wave 3 (Integration with Font dialog):
├── Task 9: Add "Customize..." button to Font.c CreateFontDlg
├── Task 10: Add CustomFontData to Main.h + resource loading in Resource.c
├── Task 11: Update Font.c ButtonCB OK to include custom font in fontres
├── Task 12: Update Font.c saveFonts/restoreFonts for custom font state
└── Task 13: Update Font.c changeFamilyCB/changeSampleFontCB for custom override

Wave 4 (System-wide application):
├── Task 14: Add SystemApplyFont() to Protocol.c (XrmPutFileDatabase to /etc/dt)
├── Task 15: Add "Apply System-Wide" button to Font dialog
└── Task 16: Create helper script for setuid/pkexec system-wide apply

Wave 5 (i18n + Verification):
├── Task 17: Update dtstyle.msg with new messages
├── Task 18: Update Dtstyle.src app-defaults (NumCustomFonts resource)
├── Task 19: Update nlsREADME.txt and dtstyle.man
└── Task 20: Full build verification + static analysis

Critical Path: Task 1→3→6→7→9→11→14→20
Parallel Speedup: ~55% faster than sequential
Max Concurrent: 4 (Waves 1, 2)
```

### Dependency Matrix

| Task | Depends On | Blocks |
|------|-----------|--------|
| 1 | - | 2, 3, 4, 5, 6, 7, 8 |
| 2 | 1 | 5, 6, 7 |
| 3 | 1 | 6, 7, 8 |
| 4 | 1 | 7, 8 |
| 5 | 2 | 6, 7 |
| 6 | 2, 3, 5 | 7, 9 |
| 7 | 3, 4, 6 | 9, 11 |
| 8 | 3, 4 | 7, 11 |
| 9 | 6, 7 | 11, 13 |
| 10 | 2 | 11, 12 |
| 11 | 7, 8, 9 | 14 |
| 12 | 10 | 14 |
| 13 | 9 | 11 |
| 14 | 11 | 15 |
| 15 | 14 | 20 |
| 16 | 14 | 20 |
| 17 | - | 20 |
| 18 | - | 20 |
| 19 | - | 20 |
| 20 | all | - |

### Agent Dispatch Summary

- **Wave 1**: 4 tasks — `quick` ×2 (configure, header), `deep` ×2 (X11 enum, fc enum)
- **Wave 2**: 4 tasks — `unspecified-high` ×2 (layout, enumeration), `deep` ×2 (preview, conversion)
- **Wave 3**: 5 tasks — `unspecified-high` ×3 (button, resources, callbacks), `quick` ×2 (save/restore, familyCB)
- **Wave 4**: 3 tasks — `deep` ×1 (Protocol.c), `quick` ×2 (button, helper)
- **Wave 5**: 4 tasks — `quick` ×3 (msg, app-defaults, docs), `unspecified-high` ×1 (verification)

---

## TODOs

---

### Wave 1: Foundation — Font Enumeration Library (PARALLEL)

- [x] 1. Add fontconfig/libfontconfig dependency to configure.ac and Makefile.am

  **What to do**:
  - In `cde/configure.ac`, add fontconfig library check:
    ```m4
    AC_ARG_WITH([fontconfig], [AS_HELP_STRING([--with-fontconfig], [Enable fontconfig support for font enumeration])])
    if test "x$with_fontconfig" != "xno"; then
      PKG_CHECK_MODULES([FONTCONFIG], [fontconfig], [have_fontconfig=yes], [have_fontconfig=no])
    fi
    AM_CONDITIONAL([HAVE_FONTCONFIG], [test "x$have_fontconfig" = "xyes"])
    if test "x$have_fontconfig" = "xyes"; then
      AC_DEFINE([HAVE_FONTCONFIG], [1], [Define if fontconfig is available])
    fi
    ```
  - In `cde/lib/DtFont/Makefile.am`, add `FontEnum.c` to source list and `$(FONTCONFIG_CFLAGS)` / `$(FONTCONFIG_LIBS)` conditionally.
  - In `cde/programs/dtstyle/Makefile.am`, add FontPicker.c and link against libDtFont.

  **Must NOT do**:
  - Do not make fontconfig a hard dependency — USE_XFT already gates Xft; fontconfig should be optional too
  - Do not modify existing library dependencies

  **Recommended Agent Profile**:
  - **Category**: `quick`
  - **Skills**: `[]`

  **Parallelization**:
  - **Can Run In Parallel**: YES
  - **Parallel Group**: Wave 1
  - **Blocks**: Tasks 2, 3, 4, 5, 6, 7, 8
  - **Blocked By**: None

  **References**:
  - `cde/configure.ac:1-996` — existing configure structure
  - `cde/lib/DtFont/Makefile.am` — current DtFont build
  - `cde/programs/dtstyle/Makefile.am` — current dtstyle build

  **Acceptance Criteria**:
  - [ ] configure.ac has fontconfig PKG_CHECK_MODULES with --with-fontconfig option
  - [ ] AM_CONDITIONAL HAVE_FONTCONFIG defined
  - [ ] HAVE_FONTCONFIG AC_DEFINE when fontconfig found
  - [ ] `./configure --with-fontconfig` succeeds on system with libfontconfig-dev
  - [ ] `./configure --without-fontconfig` succeeds on system without libfontconfig

  **QA Scenarios (MANDATORY)**:

  ```
  Scenario: configure.ac syntax is valid
    Tool: Bash (autoconf)
    Preconditions: configure.ac modified
    Steps:
      1. cd cde && autoconf --warnings=all 2>&1 | head -20
    Expected Result: No errors
    Failure Indicators: syntax errors, undefined macros
    Evidence: .sisyphus/evidence/task-1-autoconf.log

  Scenario: Makefile.am syntax is valid
    Tool: Bash (automake)
    Preconditions: Makefile.am modified
    Steps:
      1. cd cde/lib/DtFont && automake --warnings=all 2>&1 | head -20
    Expected Result: No errors
    Failure Indicators: syntax errors, missing sources
    Evidence: .sisyphus/evidence/task-1-automake.log
  ```

  **Commit**: YES
  - Message: `feat(dtstyle): add fontconfig dependency to build system`
  - Files: `cde/configure.ac`, `cde/lib/DtFont/Makefile.am`, `cde/programs/dtstyle/Makefile.am`

---

- [x] 2. Create FontEnum.h — public API header for font enumeration

  **What to do**:
  - Create `cde/include/Dt/FontEnum.h` with:
    ```c
    #ifndef _DtFontEnum_H
    #define _DtFontEnum_H

    #include <X11/Xlib.h>

    #ifdef __cplusplus
    extern "C" {
    #endif

    typedef enum {
        DtFontEnumCoreX11,    /* XListFonts — core X11 fonts only */
        DtFontEnumFontconfig,  /* FcFontList — fontconfig (requires USE_XFT) */
        DtFontEnumAll          /* Both, with fontconfig preferred */
    } DtFontEnumSource;

    typedef struct {
        char *family_name;     /* e.g., "Helvetica", "DejaVu Sans" */
        char *full_name;        /* e.g., "Helvetica Bold", "DejaVu Sans" */
        char *xlfd;            /* XLFD name, NULL if not available */
        char *fc_pattern;       /* fontconfig pattern, NULL if USE_XFT off */
        int   is_scalable;     /* True if scalable font */
        int   pixel_size;      /* 0 if scalable */
        DtFontEnumSource source; /* Where this font was found */
    } DtFontInfo;

    typedef struct {
        DtFontInfo *fonts;
        int          count;
    } DtFontList;

    /* Enumerate available font families */
    extern DtFontList *DtEnumerateFontFamilies(Display *dpy, int screen);

    /* Enumerate sizes for a specific family */
    extern DtFontList *DtEnumerateFontSizes(Display *dpy, int screen,
                                             const char *family_name,
                                             DtFontEnumSource source);

    /* Free font list */
    extern void DtFreeFontList(DtFontList *list);

    /* Get XmFontList for a font pattern (uses DtFont internally) */
    extern XmFontList DtGetFontXmFontList(Display *dpy, const char *pattern);

    #ifdef __cplusplus
    }
    #endif

    #endif /* _DtFontEnum_H */
    ```

  **Must NOT do**:
  - Do not include fontconfig headers directly — use forward declarations
  - Do not include Xm headers — DtFontInfo is plain C data

  **Recommended Agent Profile**:
  - **Category**: `quick`
  - **Skills**: `[]`

  **Parallelization**:
  - **Can Run In Parallel**: YES
  - **Parallel Group**: Wave 1
  - **Blocks**: Tasks 5, 6, 7
  - **Blocked By**: Task 1

  **References**:
  - `cde/include/Dt/DtFont.h` — existing DtFont API pattern
  - `cde/lib/DtFont/DtFont.c` — Xlib fallback implementation

  **Acceptance Criteria**:
  - [ ] DtFontInfo struct defined with all fields
  - [ ] DtFontList struct defined
  - [ ] 4 public functions declared: DtEnumerateFontFamilies, DtEnumerateFontSizes, DtFreeFontList, DtGetFontXmFontList
  - [ ] No fontconfig/Xft includes in header
  - [ ] C++ extern guard present

  **QA Scenarios (MANDATORY)**:

  ```
  Scenario: Header compiles standalone
    Tool: Bash (gcc)
    Preconditions: FontEnum.h created
    Steps:
      1. echo '#include "FontEnum.h"' | gcc -I cde/include -I cde/include/Dt -fsyntax-only -xc -
    Expected Result: No errors
    Failure Indicators: missing includes, type errors
    Evidence: .sisyphus/evidence/task-2-header-compile.log
  ```

  **Commit**: NO (groups with Task 3, 4)

---

- [x] 3. Create FontEnum.c — X11 core font enumeration (XListFonts)

  **What to do**:
  - Create `cde/lib/DtFont/FontEnum.c` with X11 core font enumeration:
    ```c
    #include <X11/Xlib.h>
    #include <X11/Xutil.h>
    #include <Dt/FontEnum.h>
    #include <stdlib.h>
    #include <string.h>

    /* Internal: parse XLFD to extract family name */
    static char *xlfd_to_family(const char *xlfd) {
        /* XLFD: -foundry-family-weight-slant-width-... */
        /* Extract 'family' field (2nd field after leading '-') */
    }

    /* Internal: deduplicate family names */
    static DtFontList *deduplicate_families(char **names, int count, ...) {
    }

    DtFontList *
    DtEnumerateFontFamilies(Display *dpy, int screen)
    {
        char **font_names;
        int count;
        DtFontList *result;

        /* First try: enumerate all scalable + fixed fonts */
        font_names = XListFonts(dpy, "-", 8192, &count);
        if (!font_names || count == 0) {
            if (font_names) XFreeFontNames(font_names);
            return NULL;
        }

        /* Parse XLFD names, extract unique family names */
        result = deduplicate_families(font_names, count, DtFontEnumCoreX11);
        XFreeFontNames(font_names);
        return result;
    }

    DtFontList *
    DtEnumerateFontSizes(Display *dpy, int screen,
                         const char *family_name,
                         DtFontEnumSource source)
    {
        char pattern[256];
        char **font_names;
        int count;
        DtFontList *result;

        /* Build pattern: -foundry-family_name-* */
        snprintf(pattern, sizeof(pattern), "-*-%s-*", family_name);
        font_names = XListFonts(dpy, pattern, 512, &count);

        /* Parse pixel sizes from XLFD, create DtFontList */
        result = parse_sizes(font_names, count, family_name, source);
        XFreeFontNames(font_names);
        return result;
    }

    void
    DtFreeFontList(DtFontList *list)
    {
        int i;
        if (!list) return;
        for (i = 0; i < list->count; i++) {
            free(list->fonts[i].family_name);
            free(list->fonts[i].full_name);
            free(list->fonts[i].xlfd);
            if (list->fonts[i].fc_pattern) free(list->fonts[i].fc_pattern);
        }
        free(list->fonts);
        free(list);
    }
    ```

  **Must NOT do**:
  - Do not include fontconfig headers in this section (ifdef-guarded section is Task 4)
  - Do not call Xft/fontconfig APIs outside #ifdef USE_XFT

  **Recommended Agent Profile**:
  - **Category**: `deep`
  - **Skills**: `[]`

  **Parallelization**:
  - **Can Run In Parallel**: YES
  - **Parallel Group**: Wave 1
  - **Blocks**: Tasks 6, 7, 8
  - **Blocked By**: Task 1

  **References**:
  - `cde/programs/dtcm/dtcm/font.c:190` — XListFonts usage for closest-match (pattern)
  - `cde/lib/DtHelp/Font.c:456-506` — Xft font loading pattern (for USE_XFT section)

  **Acceptance Criteria**:
  - [ ] XListFonts-based family enumeration works
  - [ ] XLFD parsing extracts family name, weight, slant, pixel size
  - [ ] Deduplication produces unique family names
  - [ ] DtFreeFontList properly frees all allocated memory
  - [ ] No fontconfig/Xft code outside #ifdef USE_XFT

  **QA Scenarios (MANDATORY)**:

  ```
  Scenario: FontEnum.c compiles without fontconfig
    Tool: Bash (make)
    Preconditions: FontEnum.c created
    Steps:
      1. cd cde/lib/DtFont && make FontEnum.o 2>&1 | tee /tmp/build-task3.log
    Expected Result: 0 errors, 0 fontconfig-related warnings
    Failure Indicators: fontconfig include errors, undefined references
    Evidence: .sisyphus/evidence/task-3-build.log
  ```

  **Commit**: NO (groups with Task 4)

---

- [x] 4. Create FontEnum.c — fontconfig enumeration (#ifdef USE_XFT section)

  **What to do**:
  - Add `#ifdef USE_XFT` section to `FontEnum.c`:
    ```c
    #ifdef USE_XFT
    #include <fontconfig/fontconfig.h>
    #include <X11/Xft/Xft.h>

    /* Internal: fontconfig family enumeration */
    static DtFontList *
    enumerate_fc_families(Display *dpy, int screen)
    {
        FcPattern *pat;
        FcObjectSet *os;
        FcFontSet *fs;
        DtFontList *result;
        int i, unique_count = 0;
        char **seen_families = NULL;

        pat = FcPatternCreate();
        os = FcObjectSetBuild(FC_FAMILY, FC_STYLE, FC_FULLNAME, FC_SCALABLE, NULL);
        fs = FcFontList(NULL, pat, os);
        FcObjectSetDestroy(os);
        FcPatternDestroy(pat);

        if (!fs) return NULL;

        /* Deduplicate families, create DtFontList */
        result = calloc(1, sizeof(DtFontList));
        result->fonts = calloc(fs->nfont, sizeof(DtFontInfo));

        for (i = 0; i < fs->nfont; i++) {
            FcChar8 *family = NULL;
            FcBool scalable = FcFalse;

            FcPatternGetString(fs->fonts[i], FC_FAMILY, 0, &family);
            FcPatternGetBool(fs->fonts[i], FC_SCALABLE, 0, &scalable);

            if (!is_duplicate_family(seen_families, unique_count, (char *)family)) {
                result->fonts[result->count].family_name = strdup((char *)family);
                result->fonts[result->count].fc_pattern = strdup((char *)family);
                result->fonts[result->count].is_scalable = scalable;
                result->fonts[result->count].source = DtFontEnumFontconfig;
                result->count++;
                /* Add to seen list */
            }
        }

        FcFontSetDestroy(fs);
        free(seen_families);
        return result;
    }

    /* Merge X11 core and fontconfig families, preferring fontconfig */
    static DtFontList *
    merge_font_lists(DtFontList *x11, DtFontList *fc)
    {
        /* Merge, deduplicating by family_name, preferring fc entries */
    }
    #endif /* USE_XFT */
    ```
  - Modify `DtEnumerateFontFamilies()` to call fontconfig when USE_XFT:
    ```c
    DtFontList *
    DtEnumerateFontFamilies(Display *dpy, int screen)
    {
        DtFontList *x11_list = NULL;
        DtFontList *result = NULL;

        x11_list = enumerate_x11_families(dpy, screen);

    #ifdef USE_XFT
        {
            DtFontList *fc_list = enumerate_fc_families(dpy, screen);
            if (fc_list && x11_list) {
                result = merge_font_lists(x11_list, fc_list);
                DtFreeFontList(x11_list);
                DtFreeFontList(fc_list);
            } else if (fc_list) {
                result = fc_list;
                DtFreeFontList(x11_list);
            } else {
                result = x11_list;
            }
        }
    #else
        result = x11_list;
    #endif
        return result;
    }
    ```

  **Must NOT do**:
  - Do not include fontconfig headers outside #ifdef USE_XFT
  - Do not call FcFontList when USE_XFT is not defined
  - G8: fontconfig code only inside #ifdef USE_XFT

  **Recommended Agent Profile**:
  - **Category**: `deep`
  - **Skills**: `[]`

  **Parallelization**:
  - **Can Run In Parallel**: YES
  - **Parallel Group**: Wave 1
  - **Blocks**: Tasks 7, 8
  - **Blocked By**: Task 1

  **References**:
  - `cde/lib/DtHelp/Font.c:456-506` — existing Xft/fontconfig usage in CDE
  - `cde/lib/DtFont/XftWrapper.c:206-211` — FcNameParse pattern validation

  **Acceptance Criteria**:
  - [ ] fontconfig enumeration works when USE_XFT is defined
  - [ ] Code compiles when USE_XFT is NOT defined (no fontconfig refs)
  - [ ] Merge function deduplicates X11 and fontconfig families
  - [ ] FcPattern, FcObjectSet, FcFontSet properly freed

  **QA Scenarios (MANDATORY)**:

  ```
  Scenario: Compiles without USE_XFT (no fontconfig)
    Tool: Bash (make)
    Preconditions: FontEnum.c complete
    Steps:
      1. cd cde/lib/DtFont && CFLAGS="-DUSE_XFT=0" make FontEnum.o 2>&1 | tee /tmp/build-task4-noxft.log
    Expected Result: 0 errors, 0 warnings about fontconfig
    Failure Indicators: FcFontList, FcPatternCreate references in non-XFT build
    Evidence: .sisyphus/evidence/task-4-noxft-build.log

  Scenario: Compiles with USE_XFT (fontconfig)
    Tool: Bash (make)
    Preconditions: FontEnum.c complete, libfontconfig-dev installed
    Steps:
      1. cd cde/lib/DtFont && CFLAGS="-DUSE_XFT=1" make FontEnum.o 2>&1 | tee /tmp/build-task4-xft.log
    Expected Result: 0 errors
    Failure Indicators: fontconfig header not found, Xft header not found
    Evidence: .sisyphus/evidence/task-4-xft-build.log
  ```

  **Commit**: YES
  - Message: `feat(dtstyle): add font enumeration library (X11 core + fontconfig)`
  - Files: `cde/include/Dt/FontEnum.h`, `cde/lib/DtFont/FontEnum.c`, `cde/lib/DtFont/Makefile.am`

---

### Wave 2: Font Picker UI

- [x] 5. Create FontPicker.h — dialog data structures

  **What to do**:
  - Create `cde/programs/dtstyle/FontPicker.h`:
    ```c
    #ifndef _FONTPICKER_H
    #define _FONTPICKER_H

    #include <X11/Xlib.h>
    #include <Xm/Xm.h>
    #include "Main.h"

    typedef struct {
        Widget       pickerDialog;    /* Shell for the picker */
        Widget       familyList;      /* ScrolledList of font families */
        Widget       sizeList;        /* ScrolledList of available sizes */
        Widget       previewLabel;    /* Label showing sample text in selected font */
        Widget       previewText;     /* Text showing sample text in selected font */
        Widget       sourceOption;    /* OptionMenu: Core X11 / Fontconfig / All */
        Widget       applyButton;     /* Apply to current slot */
        Widget       systemWideButton; /* Apply system-wide (root) */
        Widget       cancelButton;

        DtFontList  *availableFonts; /* Current enumeration result */
        char        *selectedFamily;  /* Currently selected family name */
        char        *selectedXlfd;    /* Currently selected XLFD */
        char        *selectedFcPattern;/* Currently selected fc pattern (USE_XFT) */
        XmFontList   selectedSysFont; /* Resolved XmFontList for system font */
        XmFontList   selectedUserFont;/* Resolved XmFontList for user font */
        int           selectedSize;    /* Currently selected pixel size */
        DtFontEnumSource currentSource;/* Current enumeration source */
        Boolean       pickerActive;   /* True while picker is shown */
    } FontPickerData;

    extern FontPickerData fontPicker;

    /* Public functions */
    extern void CreateFontPicker(Widget parent);
    extern void PopupFontPicker(int familyIdx, int sizeIdx);
    extern void PopdownFontPicker(void);
    extern void FontPickerApply(void);
    extern void FontPickerApplySystemWide(void);

    #endif /* _FONTPICKER_H */
    ```

  **Must NOT do**:
  - Do not include Dt/FontEnum.h in this header (use forward declaration if needed)
  - Do not declare widget variables that are already in Style or FontData structs

  **Recommended Agent Profile**:
  - **Category**: `quick`
  - **Skills**: `[]`

  **Parallelization**:
  - **Can Run In Parallel**: YES
  - **Parallel Group**: Wave 2
  - **Blocks**: Tasks 6, 7
  - **Blocked By**: Task 2

  **References**:
  - `cde/programs/dtstyle/Font.h` — existing FontData struct pattern
  - `cde/programs/dtstyle/Main.h` — Style struct and ApplicationData

  **Acceptance Criteria**:
  - [ ] FontPickerData struct defined with all fields
  - [ ] 5 public functions declared
  - [ ] No widget types that conflict with existing FontData

  **Commit**: NO (groups with Task 6, 7, 8)

---

- [x] 6. Create FontPicker.c — dialog layout (family list, preview, buttons)

  **What to do**:
  - Create `cde/programs/dtstyle/FontPicker.c` with Motif dialog layout:
    ```c
    #include "FontPicker.h"
    #include "Main.h"
    #include "Resource.h"
    #include "Help.h"
    #include <Dt/Message.h>
    #include <Dt/FontEnum.h>

    FontPickerData fontPicker = {0};

    static void
    CreateFontPickerWidgets(Widget parent)
    {
        Widget form, familyFrame, familyTB, sizeTB, previewForm;
        Widget buttonForm;
        Arg args[20];
        int n;
        XmString string;

        /* Create dialog shell */
        fontPicker.pickerDialog = XtVaCreatePopupShell(
            "fontPickerDialog", xmDialogShellWidgetClass,
            parent,
            XmNtitle, GETMESSAGE(5, 27, "Select Font"),
            XmNallowShellResize, True,
            NULL);

        form = XtVaCreateWidget("fontPickerForm", xmFormWidgetClass,
                                 fontPicker.pickerDialog, NULL);

        /* Source option menu: Core X11 / Fontconfig / All */
        /* ... XmOptionMenu with 3 items ... */

        /* Family list (left side) */
        familyTB = _DtCreateTitleBox(form, "familyTitleBox", ...);
        fontPicker.familyList = XmCreateScrolledList(familyTB, "fontFamilyList", ...);

        /* Size list (right side) */
        sizeTB = _DtCreateTitleBox(form, "sizeTitleBox", ...);
        fontPicker.sizeList = XmCreateScrolledList(sizeTB, "fontSizeList", ...);

        /* Preview area (bottom) */
        previewForm = XtVaCreateWidget("previewForm", xmFormWidgetClass, form, ...);
        fontPicker.previewLabel = XtVaCreateManagedWidget("systemSample", ...);
        fontPicker.previewText = XmCreateText(previewForm, "userSample", ...);

        /* Buttons: Apply, Apply System-Wide, Cancel */
        buttonForm = XtVaCreateWidget("buttonForm", xmFormWidgetClass, form, ...);
        fontPicker.applyButton = XtVaCreateManagedWidget("applyButton", ...);
        fontPicker.systemWideButton = XtVaCreateManagedWidget("systemWideButton", ...);
        fontPicker.cancelButton = XtVaCreateManagedWidget("cancelButton", ...);

        /* Callbacks */
        XtAddCallback(fontPicker.familyList, XmNbrowseSelectionCallback, FamilySelectCB, NULL);
        XtAddCallback(fontPicker.sizeList, XmNbrowseSelectionCallback, SizeSelectCB, NULL);
        XtAddCallback(fontPicker.applyButton, XmNactivateCallback, ApplyCB, NULL);
        XtAddCallback(fontPicker.systemWideButton, XmNactivateCallback, SystemWideCB, NULL);
        XtAddCallback(fontPicker.cancelButton, XmNactivateCallback, CancelCB, NULL);

        XtManageChild(form);
    }
    ```
  - Follow the same Motif widget creation pattern as CreateFontDlg() in Font.c
  - Use _DtCreateTitleBox for labeled sections (same as family list)

  **Must NOT do**:
  - Do not create a separate ApplicationShell — use xmDialogShellWidgetClass
  - Do not use deprecated Motif functions
  - G5: USE_XFT include guard dance must be preserved if including Font.c headers

  **Recommended Agent Profile**:
  - **Category**: `unspecified-high`
  - **Skills**: `[]`

  **Parallelization**:
  - **Can Run In Parallel**: YES
  - **Parallel Group**: Wave 2
  - **Blocks**: Tasks 9, 11
  - **Blocked By**: Tasks 2, 3, 5

  **References**:
  - `cde/programs/dtstyle/Font.c:172-430` — CreateFontDlg pattern
  - `cde/programs/dtstyle/Main.h:76-78` — MAX_ARGS, CMPSTR macros

  **Acceptance Criteria**:
  - [ ] FontPicker dialog shell created
  - [ ] Family list (XmScrolledList) populated on popup
  - [ ] Size list (XmScrolledList) populated on family select
  - [ ] Preview area with systemLabel + userText
  - [ ] Apply / Apply System-Wide / Cancel buttons
  - [ ] Source option menu (Core X11 / Fontconfig / All)

  **QA Scenarios (MANDATORY)**:

  ```
  Scenario: FontPicker.c compiles
    Tool: Bash (make)
    Preconditions: FontPicker.c created
    Steps:
      1. cd cde/programs/dtstyle && make dtstyle-FontPicker.o 2>&1 | tee /tmp/build-task6.log
    Expected Result: 0 errors
    Failure Indicators: undefined references, missing headers
    Evidence: .sisyphus/evidence/task-6-build.log
  ```

  **Commit**: NO (groups with Task 5, 7, 8)

---

- [x] 7. Create FontPicker.c — font enumeration integration + preview rendering

  **What to do**:
  - Add enumeration and preview functions to FontPicker.c:
    ```c
    static void
    PopulateFamilyList(void)
    {
        DtFontList *list;
        XmStringTable items;
        int i;

        list = DtEnumerateFontFamilies(style.display, DefaultScreen(style.display));
        if (!list) return;

        fontPicker.availableFonts = list;
        items = (XmStringTable)XtMalloc(list->count * sizeof(XmString));

        for (i = 0; i < list->count; i++) {
            items[i] = CMPSTR(list->fonts[i].family_name);
        }

        XtVaSetValues(fontPicker.familyList,
                      XmNitems, items,
                      XmNitemCount, list->count,
                      NULL);

        for (i = 0; i < list->count; i++) XmStringFree(items[i]);
        XtFree((char *)items);
    }

    static void
    PopulateSizeList(const char *family_name, DtFontEnumSource source)
    {
        DtFontList *sizes;
        XmStringTable items;
        char buf[32];
        int i;

        sizes = DtEnumerateFontSizes(style.display, DefaultScreen(style.display),
                                      family_name, source);
        if (!sizes) return;

        items = (XmStringTable)XtMalloc(sizes->count * sizeof(XmString));
        for (i = 0; i < sizes->count; i++) {
            snprintf(buf, sizeof(buf), "%d", sizes->fonts[i].pixel_size);
            items[i] = CMPSTR(buf);
        }

        XtVaSetValues(fontPicker.sizeList,
                      XmNitems, items,
                      XmNitemCount, sizes->count,
                      NULL);

        for (i = 0; i < sizes->count; i++) XmStringFree(items[i]);
        XtFree((char *)items);
        DtFreeFontList(sizes);
    }

    static void
    UpdatePreview(void)
    {
        /* Load selected font, create XmFontList, set on previewLabel and previewText.
         * _DtFontCreateXmFontList() accepts both XLFD and fontconfig patterns.
         * No conversion needed — pass the selected pattern through verbatim. */
        XmFontList fontList = DtGetFontXmFontList(style.display,
                                                    fontPicker.selectedXlfd ?
                                                    fontPicker.selectedXlfd :
                                                    fontPicker.selectedFcPattern);
        if (fontList) {
            XtVaSetValues(fontPicker.previewLabel, XmNfontList, fontList, NULL);
            XtVaSetValues(fontPicker.previewText, XmNfontList, fontList, NULL);
            /* Note: XmFontList is reference-counted. The list is shared between
             * previewLabel and previewText. Free only after both widgets are destroyed
             * or have their fontList replaced. */
            fontPicker.currentFontList = fontList;  /* track for later free */
        }
    }
    ```
  - **Preview scope clarification**: The FontPicker dialog has its OWN preview area (previewLabel + previewText) within the picker popup. This is NOT a duplicate of the Font dialog's main preview. The FontPicker preview shows how the selected font looks in isolation, while the Font dialog's preview shows the combined system/user font. When the user clicks "Apply" in the picker, the Font dialog's preview updates to reflect the custom override.

  **Must NOT do**:
  - Do not call FcFontList outside #ifdef USE_XFT
  - Do not hold XmFontList references without freeing them
  - Do not free XmFontList while widgets still reference it
  - G8: All fontconfig code inside #ifdef USE_XFT
  - Do not add XLFD↔fontconfig conversion — _DtFontCreateXmFontList handles both

  **Recommended Agent Profile**:
  - **Category**: `unspecified-high`
  - **Skills**: `[]`

  **Parallelization**:
  - **Can Run In Parallel**: YES
  - **Parallel Group**: Wave 2
  - **Blocks**: Tasks 9, 11
  - **Blocked By**: Tasks 3, 4, 6

  **References**:
  - `cde/programs/dtstyle/Font.c:333-360` — existing family list population pattern
  - `cde/include/Dt/FontEnum.h` — FontEnum API

  **Acceptance Criteria**:
  - [ ] Family list populated from DtEnumerateFontFamilies()
  - [ ] Size list populated on family select
  - [ ] Preview updated on family+size select
  - [ ] XmFontList properly created and freed
  - [ ] Memory management: DtFontList freed when dialog closes

  **Commit**: NO (groups with Task 5, 6, 8)

---

- [x] 8. Create FontPicker.c — pattern detection and display name formatting

  **What to do**:
  - Add **pattern detection** and **display name** helper functions to FontPicker.c:
    ```c
    /* Determine if a pattern is XLFD or fontconfig */
    static DtFontEnumSource
    detect_pattern_type(const char *pattern)
    {
        if (pattern[0] == '-')
            return DtFontEnumCoreX11;
    #ifdef USE_XFT
        /* fontconfig patterns don't start with '-' */
        return DtFontEnumFontconfig;
    #else
        return DtFontEnumCoreX11;
    #endif
    }

    /* Convert XLFD or fontconfig pattern to a human-readable display name */
    static char *
    pattern_to_display_name(const char *pattern)
    {
        if (pattern[0] == '-') {
            /* XLFD: extract family name from "-foundry-family-..." */
            return xlfd_to_family(pattern);
        }
    #ifdef USE_XFT
        /* fontconfig: pattern IS the display name (e.g., "DejaVu Sans") */
        return XtNewString(pattern);
    #else
        return XtNewString(pattern);
    #endif
    }
    ```
  - **DO NOT add XLFD↔fontconfig conversion functions.** `_DtFontCreateXmFontList()` (in `DtFont.h:111`) accepts both XLFD and fontconfig pattern strings verbatim. The DtFont library handles resolution internally. No conversion is needed.
  - The `xlfd_to_family()` helper (used by `pattern_to_display_name`) reuses the same function from FontEnum.c. Consider making it a shared internal helper or duplicating as a static function.

  **Must NOT do**:
  - Do NOT add `fc_pattern_to_xlfd()` or `xlfd_to_fc_pattern()` — these are unnecessary lossy conversions
  - Do NOT add fontconfig includes outside #ifdef USE_XFT
  - G8: All fontconfig code inside #ifdef USE_XFT

  **Recommended Agent Profile**:
  - **Category**: `deep`
  - **Skills**: `[]`

  **Parallelization**:
  - **Can Run In Parallel**: YES
  - **Parallel Group**: Wave 2
  - **Blocks**: Task 11
  - **Blocked By**: Tasks 3, 4

  **References**:
  - `cde/include/Dt/DtFont.h:111` — `_DtFontCreateXmFontList()` accepts both XLFD and fontconfig patterns
  - `cde/lib/DtFont/XftWrapper.c:206-211` — FcNameParse validation pattern
  - `cde/lib/DtFont/FontEnum.c:xlfd_to_family()` — existing XLFD family extraction (reuse this)

  **Acceptance Criteria**:
  - [ ] detect_pattern_type() correctly identifies XLFD vs fontconfig patterns
  - [ ] pattern_to_display_name() returns human-readable name for both pattern types
  - [ ] NO fc_pattern_to_xlfd() or xlfd_to_fc_pattern() conversion functions exist
  - [ ] _DtFontCreateXmFontList() used directly with raw patterns (no conversion)
  - [ ] All fontconfig code inside #ifdef USE_XFT

  **Commit**: YES
  - Message: `feat(dtstyle): add font picker dialog with enumeration and preview`
  - Files: `cde/programs/dtstyle/FontPicker.h`, `cde/programs/dtstyle/FontPicker.c`

---

### Wave 3: Integration with Font Dialog

- [x] 9. Add "Customize..." button to Font.c CreateFontDlg

  **What to do**:
  - In `cde/programs/dtstyle/Font.c`, modify `CreateFontDlg()`:
    1. Add a "Customize..." button (XmPushButtonGadget) **attached to the form directly**, NOT to `font.familyTB`:
       ```c
       /* Create Customize button — attached to form independently of familyTB.
        * This ensures the button remains visible even when numFamilies <= 1
        * causes familyTB to be unmanaged. */
       n = 0;
       XtSetArg(args[n], XmNlabelString, CMPSTR(GETMESSAGE(5, 28, "Customize..."))); n++;
       XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
       XtSetArg(args[n], XmNrightAttachment, XmATTACH_NONE); n++;
       XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
       font.customizeButton =
           XmCreatePushButtonGadget(font.fontWkarea, "customizeButton", args, n);
       XmStringFree(string);
       XtManageChild(font.customizeButton);
       XtAddCallback(font.customizeButton, XmNactivateCallback, CustomizeCB, NULL);
       ```
       **IMPORTANT**: Use `XtManageChild()` directly, NOT `widget_list[]`. The `widget_list[7]` array is at capacity (5 slots used + 2 from TitleBox). New buttons must be managed individually. The system-wide button (Task 15) should also use `XtManageChild()`.
    2. Add `Widget customizeButton;` to FontData struct in Font.h (at the END, after existing fields)
    3. Implement `CustomizeCB`:
       ```c
       static void
       CustomizeCB(Widget w, XtPointer client_data, XtPointer call_data)
       {
           int familyIdx = (font.selectedFontIndex >= 0) ?
                           FONT_FAMILY(font.selectedFontIndex) : 0;
           int sizeIdx = (font.selectedFontIndex >= 0) ?
                         FONT_SIZE(font.selectedFontIndex) : 0;
           PopupFontPicker(familyIdx, sizeIdx);
       }
       ```
    4. **CRITICAL**: When `numFamilies <= 1`, the customize button MUST remain visible because `familyTB` is unmanaged but the button is attached to the form. This is the only way to select a custom font when family selection is hidden.

  **Must NOT do**:
  - Do not change existing callback signatures
  - Do not modify existing widget layout when customizeButton is not shown
  - Do not add customizeButton to `widget_list[]` — it's at capacity (7 slots). Use `XtManageChild()` directly instead.
  - Do not attach customizeButton to `font.familyTB` — it's unmanaged when `numFamilies <= 1`, which would hide the button
  - G5: USE_XFT include guard dance preserved

  **Recommended Agent Profile**:
  - **Category**: `unspecified-high`
  - **Skills**: `[]`

  **Parallelization**:
  - **Can Run In Parallel**: YES
  - **Parallel Group**: Wave 3
  - **Blocks**: Tasks 11, 13
  - **Blocked By**: Tasks 6, 7

  **References**:
  - `cde/programs/dtstyle/Font.c:172-430` — CreateFontDlg layout
  - `cde/programs/dtstyle/FontPicker.h` — FontPickerData and PopupFontPicker

  **Acceptance Criteria**:
  - [ ] "Customize..." button added below family list
  - [ ] CustomizeCB calls PopupFontPicker with current family+size
  - [ ] Button visible when numFamilies <= 1
  - [ ] Button label uses message catalog (5, 28)
  - [ ] G5: USE_XFT guard dance preserved

  **Commit**: NO (groups with Tasks 10-13)

---

- [x] 10. Add CustomFontData to Main.h + resource loading in Resource.c

  **What to do**:
  - In `cde/programs/dtstyle/Main.h`, add to FontData struct:
    ```c
    /* Custom font override data (new) */
    Widget     customizeButton;      /* "Customize..." button */
    Boolean    hasCustomFont;        /* True if current slot has custom override */
    String     customSysStr;         /* Custom system font XLFD or fc pattern */
    String     customUserStr;        /* Custom user font XLFD or fc pattern */
    XmFontList customSysFont;       /* Resolved XmFontList for custom system font */
    XmFontList customUserFont;      /* Resolved XmFontList for custom user font */
    DtFontEnumSource customSource;   /* Core X11 or Fontconfig */
    ```
  - Add XtResource for custom font persistence:
    ```c
    /* In ApplicationData */
    String     customSysFontRes;      /* *CustomSysFont resource */
    String     customUserFontRes;    /* *CustomUserFont resource */
    int        customFamilyRes;       /* *CustomFamily resource */
    int        customSizeRes;        /* *CustomSize resource */
    ```
  - In `cde/programs/dtstyle/Resource.c`, add resource table:
    ```c
    XtResource customFont_resources[] = {
      {"customSysFont", "CustomSysFont", XmRString, sizeof(String),
          XtOffset(ApplicationDataPtr, customSysFontRes), XmRString, NULL},
      {"customUserFont", "CustomUserFont", XmRString, sizeof(String),
          XtOffset(ApplicationDataPtr, customUserFontRes), XmRString, NULL},
      {"customFamily", "CustomFamily", XmRInt, sizeof(int),
          XtOffset(ApplicationDataPtr, customFamilyRes), XmRImmediate, (caddr_t) 0},
      {"customSize", "CustomSize", XmRInt, sizeof(int),
          XtOffset(ApplicationDataPtr, customSizeRes), XmRImmediate, (caddr_t) 0},
    };
    ```

  **Must NOT do**:
  - Do not change existing ApplicationData field order (XtOffset dependency)
  - Do not add new fields between existing fields (add at END only)

  **Recommended Agent Profile**:
  - **Category**: `quick`
  - **Skills**: `[]`

  **Parallelization**:
  - **Can Run In Parallel**: YES
  - **Parallel Group**: Wave 3
  - **Blocks**: Tasks 11, 12
  - **Blocked By**: Task 2

  **References**:
  - `cde/programs/dtstyle/Main.h:140-170` — ApplicationData struct
  - `cde/programs/dtstyle/Resource.c:217-271` — existing resources[] table

  **Acceptance Criteria**:
  - [ ] CustomFontData fields added to FontData struct
  - [ ] XtResource table for custom font persistence
  - [ ] GetCustomFontResources() function added
  - [ ] Existing field order unchanged

  **Commit**: NO (groups with Tasks 9, 11-13)

---

- [x] 11. Update Font.c ButtonCB OK to include custom font in fontres

  **What to do**:
  - In `cde/programs/dtstyle/Font.c`, modify the `ButtonCB` OK case:
    - After the `sprintf(fontres, ...)` block, if `font.hasCustomFont` is True, append custom font resources:
      ```c
      if (font.hasCustomFont) {
          char customRes[1024];
          snprintf(customRes, sizeof(customRes),
              "*CustomSysFont: %s\n*CustomUserFont: %s\n"
              "*CustomFamily: %d\n*CustomSize: %d\n",
              font.customSysStr ? font.customSysStr : "",
              font.customUserStr ? font.customUserStr : "",
              FONT_FAMILY(font.selectedFontIndex),
              FONT_SIZE(font.selectedFontIndex));
          strcat(fontres, customRes);
      }
      ```
    - The existing `*systemFont`, `*userFont` lines in fontres should be replaced with the custom font values when a custom font is active:
      ```c
      /* Replace systemFont/userFont with custom override */
      if (font.hasCustomFont && font.customSysStr) {
          /* Use customSysStr instead of fontChoice[idx].sysStr */
          sysStr = font.customSysStr;
      } else {
          sysStr = style.xrdb.fontChoice[font.selectedFontIndex].sysStr;
      }
      /* Similarly for userStr */
      ```

  **Must NOT do**:
  - G3: Do not change existing fontres line format order — only replace values and append new lines
  - G4: SmNewFontSettings call signature unchanged (still passes fontres string)
  - Do not exceed 8192-byte fontres buffer (custom append adds ~200 bytes max)

  **Recommended Agent Profile**:
  - **Category**: `unspecified-high`
  - **Skills**: `[]`

  **Parallelization**:
  - **Can Run In Parallel**: NO
  - **Parallel Group**: Wave 3 (sequential after Task 9)
  - **Blocks**: Task 14
  - **Blocked By**: Tasks 7, 8, 9, 10

  **References**:
  - `cde/programs/dtstyle/Font.c:555-665` — current ButtonCB OK case
  - `cde/programs/dtstyle/Protocol.c:761-781` — SmNewFontSettings

  **Acceptance Criteria**:
  - [ ] Custom font values override systemFont/userFont in fontres when active
  - [ ] *CustomSysFont, *CustomUserFont, *CustomFamily, *CustomSize appended when custom font active
  - [ ] Existing fontres line order preserved
  - [ ] fontres buffer does not overflow (verify < 8192 bytes)
  - [ ] SmNewFontSettings call unchanged

  **Commit**: NO (groups with Tasks 9, 10, 12, 13)

---

- [x] 12. Update Font.c saveFonts/restoreFonts for custom font state

  **What to do**:
  - In `saveFonts()`, add custom font state:
    ```c
    if (font.hasCustomFont) {
        snprintf(bufr, sizeof(bufr), "*Fonts.customSysFont: %s\n", font.customSysStr ? font.customSysStr : "");
        WRITE_STR2FD(fd, bufr);
        snprintf(bufr, sizeof(bufr), "*Fonts.customUserFont: %s\n", font.customUserStr ? font.customUserStr : "");
        WRITE_STR2FD(fd, bufr);
        snprintf(bufr, sizeof(bufr), "*Fonts.customFamily: %d\n", FONT_FAMILY(font.selectedFontIndex));
        WRITE_STR2FD(fd, bufr);
        snprintf(bufr, sizeof(bufr), "*Fonts.customSize: %d\n", FONT_SIZE(font.selectedFontIndex));
        WRITE_STR2FD(fd, bufr);
        snprintf(bufr, sizeof(bufr), "*Fonts.hasCustomFont: True\n");
        WRITE_STR2FD(fd, bufr);
    }
    ```
  - In `restoreFonts()`, add custom font restoration:
    ```c
    xrm_name[1] = XrmStringToQuark("customSysFont");
    if (XrmQGetResource(db, xrm_name, xrm_name, &rep_type, &value) && value.addr != NULL) {
        font.customSysStr = XtNewString((char *)value.addr);
    }
    /* Similarly for customUserFont, customFamily, customSize, hasCustomFont */
    ```

  **Must NOT do**:
  - Do not change existing save/restore format for x, y, familyIndex, ismapped

  **Recommended Agent Profile**:
  - **Category**: `quick`
  - **Skills**: `[]`

  **Parallelization**:
  - **Can Run In Parallel**: YES
  - **Parallel Group**: Wave 3
  - **Blocks**: Task 14
  - **Blocked By**: Task 10

  **References**:
  - `cde/programs/dtstyle/Font.c:854-957` — current saveFonts/restoreFonts

  **Acceptance Criteria**:
  - [ ] Custom font state saved to session file
  - [ ] Custom font state restored from session file
  - [ ] NULL handling for customSysStr/customUserStr
  - [ ] Existing session data format unchanged

  **Commit**: NO (groups with Tasks 9-11, 13)

---

- [x] 13. Update Font.c changeFamilyCB/changeSampleFontCB for custom override

  **What to do**:
  - When user selects a different family or size, clear the custom font override:
    ```c
    /* In changeFamilyCB and changeSampleFontCB: */
    if (font.hasCustomFont) {
        font.hasCustomFont = False;
        if (font.customSysStr) { XtFree(font.customSysStr); font.customSysStr = NULL; }
        if (font.customUserStr) { XtFree(font.customUserStr); font.customUserStr = NULL; }
        if (font.customSysFont) { XmFontListFree(font.customSysFont); font.customSysFont = NULL; }
        if (font.customUserFont) { XmFontListFree(font.customUserFont); font.customUserFont = NULL; }
    }
    ```
  - When FontPicker Apply callback returns with a custom font, update the current slot:
    ```c
    void
    FontPickerApply(void)
    {
        int idx = FONT_INDEX(font.selectedFontIndex >= 0 ?
                            FONT_FAMILY(font.selectedFontIndex) : 0,
                            font.selectedFontIndex >= 0 ?
                            FONT_SIZE(font.selectedFontIndex) : 0);

        /* Free previous custom font if any */
        if (font.customSysStr) XtFree(font.customSysStr);
        if (font.customUserStr) XtFree(font.customUserStr);
        if (font.customSysFont) XmFontListFree(font.customSysFont);
        if (font.customUserFont) XmFontFontFree(font.customUserFont);

        /* Set custom override */
        font.hasCustomFont = True;
        font.customSysStr = XtNewString(fontPicker.selectedXlfd);
        font.customUserStr = XtNewString(fontPicker.selectedXlfd); /* same font for both, or separate */
        font.customSysFont = fontPicker.selectedSysFont;
        font.customUserFont = fontPicker.selectedUserFont;

        /* Update preview */
        UpdateFontPreview();
    }
    ```

  **Must NOT do**:
  - Do not change existing changeFamilyCB/changeSampleFontCB signatures
  - Do not modify fontChoice[] array contents (custom override is parallel)

  **Recommended Agent Profile**:
  - **Category**: `unspecified-high`
  - **Skills**: `[]`

  **Parallelization**:
  - **Can Run In Parallel**: YES
  - **Parallel Group**: Wave 3
  - **Blocks**: Task 11
  - **Blocked By**: Task 9

  **References**:
  - `cde/programs/dtstyle/Font.c:779-829` — changeFamilyCB
  - `cde/programs/dtstyle/Font.c:618-670` — changeSampleFontCB
  - `cde/programs/dtstyle/FontPicker.h` — FontPickerApply

  **Acceptance Criteria**:
  - [ ] Custom override cleared on family/size change
  - [ ] Custom override applied when FontPicker returns with selection
  - [ ] Preview updated with custom font
  - [ ] Memory properly freed when custom override is cleared or replaced

  **Commit**: YES
  - Message: `feat(dtstyle): integrate font picker with Font dialog`
  - Files: `cde/programs/dtstyle/Font.c`, `cde/programs/dtstyle/Main.h`, `cde/programs/dtstyle/Resource.c`, `cde/programs/dtstyle/Resource.h`

---

### Wave 4: System-Wide Application

- [x] 14. Add BuildFontResourceString() and RequestSystemWideApply() to Protocol.c

  **What to do**:
  - In `cde/programs/dtstyle/Protocol.c`, add two functions:

  **Function 1: BuildFontResourceString** — Constructs the font resource string from current settings:
    ```c
    /************************************************************************
     * BuildFontResourceString
     *
     * Build the font resource string from current font settings.
     * This is the same string that ButtonCB OK builds for _DtAddToResource,
     * but extracted into a reusable function.
     *
     * Returns: Newly allocated string (caller must XtFree), or NULL on error
     ************************************************************************/
    char *
    BuildFontResourceString(void)
    {
        /* Build fontres string identically to ButtonCB OK case.
         * This is extracted from Font.c ButtonCB for reuse by SystemWide. */
        /* ... sprintf fontres from current fontChoice/selectedFontIndex ... */
        return XtNewString(fontres);
    }
    ```

  **Function 2: RequestSystemWideApply** — Writes to user-writable temp, invokes pkexec helper:
    ```c
    /************************************************************************
     * RequestSystemWideApply
     *
     * Write the current font resource string to a user-writable temp file,
     * then invoke pkexec dtstyle_sysapply to merge it into
     * /etc/dt/app-defaults/Dtstyle as root.
     *
     * Returns: 0 on success (pkexec launched), -1 on failure
     *
     * NOTE: This does NOT write to /etc/dt directly. The C process runs as
     * a normal user and cannot write to /etc/dt. The pkexec helper script
     * handles the privileged write.
     *
     * System-wide font changes only affect NEW X sessions. The current
     * session is already updated via _DtAddToResource (immediate) and
     * SmNewFontSettings (session-persistent).
     ************************************************************************/
    int
    RequestSystemWideApply(char *fontResourceString)
    {
        char tmppath[PATH_MAX];
        int fd;
        FILE *fp;
        int ret;
        char cmd[PATH_MAX + 128];

        /* Write resource string to a user-writable temp file */
        snprintf(tmppath, sizeof(tmppath), "/tmp/dtstyle-sysfont-XXXXXX");
        fd = mkstemp(tmppath);
        if (fd < 0) return -1;

        fp = fdopen(fd, "w");
        if (!fp) { close(fd); unlink(tmppath); return -1; }

        fputs(fontResourceString, fp);
        fclose(fp);

        /* Check if pkexec is available */
        if (access(PKEXECPATH, X_OK) != 0) {
            /* No pkexec — try direct write (will work if user is root) */
            ret = SystemApplyFontDirect(fontResourceString);
            unlink(tmppath);
            return ret;
        }

        /* Invoke pkexec helper */
        snprintf(cmd, sizeof(cmd), "pkexec --disable-internal-agent "
                 "%s/dtstyle_sysapply.sh \"%s\"",
                 CDE_INSTALLATION_TOP "/bin", tmppath);
        ret = system(cmd);

        /* Clean up temp file */
        unlink(tmppath);

        return (ret == 0) ? 0 : -1;
    }

    /************************************************************************
     * SystemApplyFontDirect
     *
     * Direct write to /etc/dt/app-defaults/Dtstyle (root only).
     * Used as fallback when pkexec is unavailable and user is root.
     * Uses XrmGetFileDatabase + XrmMergeDatabases + XrmPutFileDatabase
     * for proper Xrm merge (not raw concatenation).
     ************************************************************************/
    static int
    SystemApplyFontDirect(char *fontResourceString)
    {
        char filepath[PATH_MAX];
        char tmppath[PATH_MAX];
        XrmDatabase db;
        int fd;
        mode_t old_umask;

        snprintf(filepath, sizeof(filepath), "%s/app-defaults/Dtstyle",
                 CDE_CONFIGURATION_TOP);

        /* Check write permission */
        if (access(filepath, W_OK) != 0 && errno != ENOENT) {
            return -1;
        }

        /* Parse existing file + merge new resources */
        db = XrmGetFileDatabase(filepath);
        if (!db) {
            db = XrmGetStringDatabase(fontResourceString);
        } else {
            XrmDatabase newdb = XrmGetStringDatabase(fontResourceString);
            XrmMergeDatabases(newdb, &db);
        }

        /* Write to temp file first (atomic) */
        snprintf(tmppath, sizeof(tmppath), "%s/app-defaults/Dtstyle.XXXXXX",
                 CDE_CONFIGURATION_TOP);
        old_umask = umask(0022);
        fd = mkstemp(tmppath);
        umask(old_umask);
        if (fd < 0) {
            XrmDestroyDatabase(db);
            return -1;
        }
        close(fd);

        XrmPutFileDatabase(db, tmppath);
        XrmDestroyDatabase(db);

        if (rename(tmppath, filepath) != 0) {
            unlink(tmppath);
            return -1;
        }

        return 0;
    }
    ```
  - Add declarations to `cde/programs/dtstyle/Protocol.h`

  **Must NOT do**:
  - Do NOT write to /etc/dt from the C process when running as non-root (mkstemp in /etc/dt fails for normal users)
  - Do NOT use raw `cat >> file` for merging — Xrm merge must be used to avoid duplicate lines
  - Do NOT hardcode paths — use CDE_CONFIGURATION_TOP and CDE_INSTALLATION_TOP
  - Do NOT modify SmNewFontSettings signature

  **Recommended Agent Profile**:
  - **Category**: `deep`
  - **Skills**: `[]`

  **Parallelization**:
  - **Can Run In Parallel**: NO
  - **Parallel Group**: Wave 4
  - **Blocks**: Tasks 15, 16
  - **Blocked By**: Task 11

  **References**:
  - `cde/programs/dtstyle/Protocol.c:761-781` — SmNewFontSettings pattern
  - `cde/programs/dtsession/SmSave.c:1651-1696` — XrmPutFileDatabase pattern
  - `cde/include/Dt/DtPStrings.h:46-47` — CDE paths

  **Acceptance Criteria**:
  - [ ] BuildFontResourceString() extracts fontres construction from ButtonCB
  - [ ] RequestSystemWideApply() writes to /tmp (user-writable), invokes pkexec
  - [ ] SystemApplyFontDirect() uses XrmGetFileDatabase + XrmMergeDatabases (not cat >>)
  - [ ] Fallback to direct write when pkexec unavailable and user is root
  - [ ] Temp file cleaned up after pkexec completes
  - [ ] CDE_CONFIGURATION_TOP and CDE_INSTALLATION_TOP used for paths
  - [ ] Comment documents that system-wide changes only affect NEW X sessions

  **QA Scenarios (MANDATORY)**:

  ```
  Scenario: SystemApplyFontDirect uses Xrm merge (not raw concat)
    Tool: Bash (grep)
    Preconditions: Protocol.c modified
    Steps:
      1. grep -n "XrmGetFileDatabase\|XrmMergeDatabases\|XrmPutFileDatabase" cde/programs/dtstyle/Protocol.c
    Expected Result: All three Xrm calls found in SystemApplyFontDirect
    Failure Indicators: Xrm calls missing, or raw cat/append used
    Evidence: .sisyphus/evidence/task-14-xrm-merge.log

  Scenario: RequestSystemWideApply writes to /tmp (not /etc/dt)
    Tool: Bash (grep)
    Preconditions: Protocol.c modified
    Steps:
      1. grep -n "mkstemp\|/tmp/dtstyle" cde/programs/dtstyle/Protocol.c
    Expected Result: mkstemp with /tmp/dtstyle-sysfont pattern found
    Failure Indicators: No /tmp path found (means C process writes to /etc/dt)
    Evidence: .sisyphus/evidence/task-14-tmp-path.log

  Scenario: pkexec invocation found
    Tool: Bash (grep)
    Preconditions: Protocol.c modified
    Steps:
      1. grep -n "pkexec" cde/programs/dtstyle/Protocol.c
    Expected Result: pkexec invocation found in RequestSystemWideApply
    Failure Indicators: No pkexec reference
    Evidence: .sisyphus/evidence/task-14-pkexec.log
  ```

  **Commit**: NO (groups with Task 15, 16)

---

- [x] 15. Add "Apply System-Wide" button to Font dialog

  **What to do**:
  - In `cde/programs/dtstyle/Font.c`, add system-wide apply button to the font dialog (after "OK"/"Cancel"/"Help" buttons):
    ```c
    /* In CreateFontDlg, add after helpButton.
     * Use XtManageChild() directly — do NOT add to widget_list[] (at capacity). */
    font.systemWideButton = XtVaCreateManagedWidget(
        "systemWideButton", xmPushButtonGadgetClass,
        buttonForm,
        XmNlabelString, CMPSTR(GETMESSAGE(5, 29, "Apply System-Wide...")),
        NULL);
    XmStringFree(string);
    XtAddCallback(font.systemWideButton, XmNactivateCallback, SystemWideCB, NULL);
    ```
  - Implement `SystemWideCB`:
    ```c
    static void
    SystemWideCB(Widget w, XtPointer client_data, XtPointer call_data)
    {
        char *fontres;
        int result;

        /* Build the font resource string */
        fontres = BuildFontResourceString();
        if (!fontres) {
            ShowError(style.shell, GETMESSAGE(5, 31,
                "Unable to build font resource string."));
            return;
        }

        result = RequestSystemWideApply(fontres);
        if (result == 0) {
            /* Success */
            ShowMessage(style.shell, GETMESSAGE(5, 30,
                "Font settings applied system-wide.\n"
                "Changes will take effect for new X sessions."));
        } else {
            /* Failure — likely pkexec unavailable or permission denied */
            ShowError(style.shell, GETMESSAGE(5, 31,
                "Unable to apply font settings system-wide.\n"
                "Ensure PolicyKit (pkexec) is installed, or\n"
                "run dtstyle with administrator privileges."));
        }
        XtFree(fontres);
    }
    ```
  - **Important**: The system-wide button should ONLY be visible if either:
    1. The user has write permission to `/etc/dt/app-defaults/` (root), OR
    2. `pkexec` is available on the system (for privilege escalation).
    ```c
    /* Show system-wide button if pkexec is available or user is root */
    if (access(PKEXECPATH, X_OK) == 0 || access(CDE_CONFIGURATION_TOP "/app-defaults", W_OK) == 0) {
        XtManageChild(font.systemWideButton);
    } else {
        XtUnmanageChild(font.systemWideButton);  /* Hide if no way to elevate */
    }
    ```
  - **Important**: System-wide changes only affect NEW X sessions. The current session is already updated via `_DtAddToResource` (immediate) and `SmNewFontSettings` (session-persistent). This should be documented in the success message.

  **Must NOT do**:
  - Do not use sudo/pkexec from within dtstyle directly — the C code calls `system("pkexec ...")` which prompts for auth via PolicyKit
  - Do not prompt for password in dtstyle UI — let pkexec handle that
  - Do not modify the existing OK/Cancel/Help button layout
  - Do not add systemWideButton to `widget_list[]` — use `XtManageChild()` directly

  **Recommended Agent Profile**:
  - **Category**: `quick`
  - **Skills**: `[]`

  **Parallelization**:
  - **Can Run In Parallel**: YES
  - **Parallel Group**: Wave 4
  - **Blocks**: Task 20
  - **Blocked By**: Task 14

  **References**:
  - `cde/programs/dtstyle/Font.c:475-570` — existing button layout
  - `cde/programs/dtstyle/ColorMain.c:1811-1813` — existing _DtAddToResource pattern

  **Acceptance Criteria**:
  - [ ] "Apply System-Wide..." button added to font dialog
  - [ ] Button hidden when user lacks write permission
  - [ ] SystemWideCB builds fontres string and calls SystemApplyFont()
  - [ ] Success message shown on successful write
  - [ ] Error message shown on permission denied

  **Commit**: NO (groups with Task 14, 16)

---

- [x] 16. Create helper script for pkexec system-wide apply

  **What to do**:
  - Create `cde/programs/dtstyle/dtstyle_sysapply.sh`:
    ```bash
    #!/bin/sh
    # dtstyle_sysapply - Apply font settings system-wide
    # Usage: dtstyle_sysapply <font-resource-file>
    #
    # This script is intended to be called via pkexec from dtstyle.
    # It merges the font resource file into /etc/dt/app-defaults/Dtstyle.
    #
    # The C process writes the resource string to a temp file in /tmp,
    # then invokes: pkexec dtstyle_sysapply.sh /tmp/dtstyle-sysfont-XXXXXX
    #
    # This script runs as root (via pkexec). It performs proper Xrm merge
    # (not raw concatenation) to avoid duplicate resource lines.

    DESTDIR="${CDE_CONFIGURATION_TOP:-/etc/dt}/app-defaults"
    DESTFILE="$DESTDIR/Dtstyle"

    if [ "$(id -u)" != "0" ]; then
        echo "Error: This script must be run as root." >&2
        exit 1
    fi

    if [ -z "$1" ] || [ ! -f "$1" ]; then
        echo "Error: Resource file not found: $1" >&2
        exit 1
    fi

    # Create directory if needed
    if [ ! -d "$DESTDIR" ]; then
        mkdir -p "$DESTDIR" || exit 1
    fi

    # Merge resource file into Dtstyle using Xrm-compatible approach.
    # Xrm uses "last definition wins" semantics, so we can concatenate
    # and then deduplicate: new resources override old ones.
    if [ -f "$DESTFILE" ]; then
        # Concatenate new after old, then sort by resource key (first field)
        # and keep only the last occurrence of each key (which is the new value).
        # Xrm format: "*resource.key: value" — sort by everything before ":"
        cat "$DESTFILE" "$1" | awk -F': ' '!/^!/ && !/^$/ && !/^#/ {
            key = $1; gsub(/^[[:space:]]+|[[:space:]]+$/, "", key); print key "\x00" $0
        }' | sort -t$'\x00' -k1,1 -u | awk -F'\x00' '{print $2}' > "$DESTFILE.tmp"
        mv "$DESTFILE.tmp" "$DESTFILE"
        chmod 644 "$DESTFILE"
    else
        # No existing file — just copy
        cp "$1" "$DESTFILE"
        chmod 644 "$DESTFILE"
    fi

    # Clean up the temp file
    rm -f "$1"

    echo "Font settings applied system-wide to $DESTFILE"
    echo "Changes will take effect for new X sessions."
    exit 0
    ```
  - Add corresponding pkexec policy file `cde/programs/dtstyle/org.cde.dtstyle.font.policy`:
    ```xml
    <?xml version="1.0" encoding="UTF-8"?>
    <!DOCTYPE policyconfig PUBLIC "-//freedesktop//DTD PolicyKit Policy Configuration 1.0//EN"
      "http://www.freedesktop.org/standards/PolicyKit/1/policyconfig.dtd">
    <policyconfig>
      <vendor>CDE Project</vendor>
      <action id="org.cde.dtstyle.font">
        <description>Apply font settings system-wide</description>
        <message>Authentication is required to apply font settings system-wide</message>
        <defaults>
          <allow_any>auth_admin</allow_any>
          <allow_inactive>auth_admin</allow_inactive>
          <allow_active>auth_admin_keep</allow_active>
        </defaults>
      </action>
    </policyconfig>
    ```
  - Add both files to `cde/programs/dtstyle/Makefile.am`

  **Must NOT do**:
  - Do not make dtstyle itself setuid — security risk (pkexec is the escalation mechanism)
  - Do not store passwords in the script
  - Do not use `xrdb -merge` — it writes to X server RESOURCE_MANAGER, not to a file
  - Do not use raw `cat >> $DESTFILE` — this creates duplicate Xrm lines without deduplication
  - Do not use `/tmp` for the final destination — use mkstemp only in the C code for the input file

  **Recommended Agent Profile**:
  - **Category**: `quick`
  - **Skills**: `[]`

  **Parallelization**:
  - **Can Run In Parallel**: YES
  - **Parallel Group**: Wave 4
  - **Blocks**: Task 20
  - **Blocked By**: Task 14

  **References**:
  - `cde/programs/dtstyle/Makefile.am` — existing install targets

  **Acceptance Criteria**:
  - [ ] Helper script checks for root permission
  - [ ] PolicyKit policy file defined
  - [ ] Both files added to Makefile.am
  - [ ] Script uses CDE_CONFIGURATION_TOP for path

  **Commit**: YES
  - Message: `feat(dtstyle): add system-wide font application with PolicyKit support`
  - Files: `cde/programs/dtstyle/Protocol.c`, `cde/programs/dtstyle/Protocol.h`, `cde/programs/dtstyle/Font.c`, `cde/programs/dtstyle/dtstyle_sysapply.sh`, `cde/programs/dtstyle/org.cde.dtstyle.font.policy`

---

### Wave 5: i18n + Verification

- [x] 17. Update dtstyle.msg with new messages (5-27 through 5-32)

  **What to do**:
  - In `cde/programs/dtstyle/dtstyle.msg`, add messages 5-27 through 5-32:
    ```
    $ _DtMessage 27 is the label of the "Customize..." button
    27 Customize...

    $ _DtMessage 28 is the title of the font picker dialog
    28 Select Font

    $ _DtMessage 29 is the label of the "Apply System-Wide..." button
    29 Apply System-Wide...

    $ _DtMessage 30 is the success message for system-wide application
    30 Font settings applied system-wide.\\nChanges will take effect for new X sessions.

    $ _DtMessage 31 is the error message when system-wide application fails
    31 Unable to apply font settings system-wide.\\nEnsure PolicyKit (pkexec) is installed, or\\nrun dtstyle with administrator privileges.

    $ _DtMessage 32 is the label for the font source option menu
    32 Font Source
    ```
  - **IMPORTANT**: Audit existing dtstyle.msg set 5 message numbers before assigning 27-32. Verify that slots 23, 24, 26 are still free (23=FontFamily, 25=Family are used; 22, 23, 24, 26 may be used by other features). If any slot is occupied, shift the new messages to the next available slots.

  **Must NOT do**:
  - Do not change existing message numbers (1-26 in set 5)
  - Do not use special characters that would break gencat

  **Recommended Agent Profile**:
  - **Category**: `quick`
  - **Skills**: `[]`

  **Parallelization**:
  - **Can Run In Parallel**: YES
  - **Parallel Group**: Wave 5
  - **Blocks**: Task 20
  - **Blocked By**: None

  **Acceptance Criteria**:
  - [ ] Messages 5-27 through 5-32 added
  - [ ] gencat succeeds: `gencat dtstyle.cat dtstyle.msg`
  - [ ] No duplicate message numbers

  **Commit**: NO (groups with Task 18, 19)

---

- [x] 18. Update Dtstyle.src + Dtstyle with CustomFont resources

  **What to do**:
  - In `cde/programs/dtstyle/Dtstyle.src`, add custom font resources:
    ```
    !### Custom Font Override Resources
    Dtstyle*customSysFont:
    Dtstyle*customUserFont:
    Dtstyle*customFamily: 0
    Dtstyle*customSize: 0
    ```
  - In `cde/programs/dtstyle/Dtstyle`, add the same resources with default values

  **Must NOT do**:
  - Do not change existing resource lines
  - Do not add fontconfig-specific resources (those come from the picker, not from defaults)

  **Recommended Agent Profile**:
  - **Category**: `quick`
  - **Skills**: `[]`

  **Parallelization**:
  - **Can Run In Parallel**: YES
  - **Parallel Group**: Wave 5
  - **Blocks**: Task 20
  - **Blocked By**: None

  **Acceptance Criteria**:
  - [ ] CustomSysFont, CustomUserFont, CustomFamily, CustomSize resources added
  - [ ] Default values are empty/0 (no custom override by default)

  **Commit**: NO (groups with Task 17, 19)

---

- [x] 19. Update nlsREADME.txt and dtstyle.man documentation

  **What to do**:
  - In `cde/programs/dtstyle/nlsREADME.txt`, add documentation for:
    - `customSysFont` resource
    - `customUserFont` resource
    - `customFamily` resource
    - `customSize` resource
    - Font enumeration feature
  - In `cde/programs/dtstyle/dtstyle.man`, update resources section with new resources and the "Customize..." button description

  **Recommended Agent Profile**:
  - **Category**: `writing`
  - **Skills**: `[]`

  **Parallelization**:
  - **Can Run In Parallel**: YES
  - **Parallel Group**: Wave 5
  - **Blocks**: Task 20
  - **Blocked By**: None

  **Acceptance Criteria**:
  - [ ] nlsREADME.txt documents new custom font resources
  - [ ] dtstyle.man documents "Customize..." button and font enumeration
  - [ ] No existing documentation removed

  **Commit**: YES
  - Message: `feat(dtstyle): add font picker documentation and i18n`
  - Files: `cde/programs/dtstyle/dtstyle.msg`, `cde/programs/dtstyle/Dtstyle.src`, `cde/programs/dtstyle/Dtstyle`, `cde/programs/dtstyle/nlsREADME.txt`, `cde/programs/dtstyle/dtstyle.man`

---

- [x] 20. Full build verification + static analysis

  **What to do**:
  - Build without Xft/fontconfig:
    ```bash
    cd cde && ./configure --disable-xft && make clean && make 2>&1 | tee /tmp/build-noxft.log
    ```
  - Build with Xft/fontconfig:
    ```bash
    cd cde && ./configure --enable-xft && make clean && make 2>&1 | tee /tmp/build-xft.log
    ```
  - Static analysis:
    - All #ifdef USE_XFT guards are properly closed
    - No fontconfig/Xft symbols linked when configured without Xft
    - All XtOffset references still valid
    - No memory leaks (DtFreeFontList, XtFree, XmFontListFree)
  - Manual simulation: Font dialog → Customize... → select font → Apply → verify preview

  **Note on USE_XFT**: USE_XFT is defined via `configure` (autoconf substitution into `config.h`), NOT via user CFLAGS. Using `CFLAGS="-DUSE_XFT=0"` only affects compile flags but does not change `config.h`'s `#define USE_XFT 1`. Always use `./configure --enable-xft` / `./configure --disable-xft` to switch modes.

  **Must NOT do**:
  - Do not skip any verification step

  **Recommended Agent Profile**:
  - **Category**: `unspecified-high`
  - **Skills**: `[]`

  **Parallelization**:
  - **Can Run In Parallel**: NO (final integration)
  - **Parallel Group**: Wave 5
  - **Blocks**: Final Verification Wave
  - **Blocked By**: Tasks 1-19

  **Acceptance Criteria**:
  - [ ] `make clean && make` succeeds with USE_XFT=0 (0 errors, 0 warnings)
  - [ ] `make clean && make` succeeds with USE_XFT=1 (0 errors, 0 warnings)
  - [ ] `nm dtstyle | grep -iE "FcFont|Xft"` returns nothing when USE_XFT=0
  - [ ] Font dialog shows "Customize..." button
  - [ ] Font picker opens with family list populated
  - [ ] Custom font overrides current slot

  **Commit**: NO (final verification wave)

---

## Final Verification Wave (MANDATORY — after ALL implementation tasks)

- [x] F1. **Plan Compliance Audit** — `oracle`
  Read the plan end-to-end. For each "Must Have": verify implementation exists. For each "Must NOT Have": search codebase for forbidden patterns. Check evidence files. Compare deliverables against plan.

- [x] F2. **Code Quality Review** — `unspecified-high`
  Run `make` with both USE_XFT=0 and USE_XFT=1. Check for: #ifdef USE_XFT guard completeness, memory leaks (XmFontListFree, DtFreeFontList, XtFree), NULL pointer checks, buffer overflows (fontres 8192 byte limit).

- [x] F3. **Feature Verification** — `unspecified-high`
  Verify: Font picker opens, enumerates fonts (X11 + fontconfig), preview updates, Apply propagates to RESOURCE_MANAGER (xprop -root), SmNewFontSettings called, system-wide write works with root permission.

- [x] F4. **Scope Fidelity Check** — `deep`
  For each task: verify implementation matches spec. Check no cross-task contamination. Verify guardrails G1-G10.

---

## Commit Strategy

- **Commit 1**: Wave 1 — `feat(dtstyle): add font enumeration library and fontconfig dependency`
- **Commit 2**: Wave 2 — `feat(dtstyle): add font picker dialog with enumeration and preview`
- **Commit 3**: Wave 3 — `feat(dtstyle): integrate font picker with Font dialog`
- **Commit 4**: Wave 4 — `feat(dtstyle): add system-wide font application with PolicyKit support`
- **Commit 5**: Wave 5 — `feat(dtstyle): add font picker documentation and i18n`

---

## Success Criteria

### Verification Commands
```bash
# Build verification (both modes — USE_XFT is set by configure, NOT CFLAGS)
cd cde && ./configure --disable-xft && make clean && make 2>&1 | grep -iE "error|warning"
cd cde && ./configure --enable-xft && make clean && make 2>&1 | grep -iE "error|warning"

# No fontconfig symbols in non-XFT build
nm cde/programs/dtstyle/dtstyle | grep -iE "FcFont|Xft"  # Expected: empty

# Message catalog
gencat cde/programs/dtstyle/dtstyle.cat cde/programs/dtstyle/dtstyle.msg  # Expected: no errors

# Font enumeration library exists
ls cde/lib/DtFont/FontEnum.o  # Expected: exists

# Font picker source exists
ls cde/programs/dtstyle/FontPicker.o  # Expected: exists

# Custom font resources in app-defaults
grep "customSysFont" cde/programs/dtstyle/Dtstyle  # Expected: match

# USE_XFT guards complete
grep -rn "fontconfig\|FcFont\|XftFont" cde/programs/dtstyle/FontPicker.c | grep -v "#ifdef USE_XFT\|#endif"  # Expected: 0 matches outside guards
```

### Final Checklist
- [ ] All "Must Have" present
- [ ] All "Must NOT Have" absent
- [ ] All 20 tasks completed
- [ ] Build succeeds with USE_XFT=0 and USE_XFT=1
- [ ] Font picker enumerates X11 core fonts
- [ ] Font picker enumerates fontconfig fonts (USE_XFT=1)
- [ ] Custom font applies to CDE-wide via _DtAddToResource
- [ ] System-wide apply works with root permission
- [ ] Session save/restore preserves custom font state