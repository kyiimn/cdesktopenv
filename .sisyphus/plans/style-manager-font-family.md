# Style Manager Font Dialog - Font Family Selection

## TL;DR

> **Quick Summary**: CDE Style Manager의 Font 대화상자에 폰트 패밀리(글꼴 종류) 선택 기능을 추가. 현재 7개로 하드코딩된 "Size" 선택(사실은 미리 묶인 (size+family) 쌍)을 패밀리 선택과 크기 선택의 2D 구조로 재설계. 기존 session 파일과의 호환성을 유지하면서 app-defaults에 패밀리 리소스를 추가하여 관리자가 패밀리 목록을 정의할 수 있게 함.
>
> **Deliverables**:
> - Font 대화상자 좌측에 폰트 패밀리 목록 (XmList) 추가
> - `fontChoice[]` 1D 배열을 `fontChoice[MAX_FAMILIES * MAX_SIZES]` 플랫 1D 배열로 확장 (XtOffset 호환성)
> - app-defaults(Dtstyle)에 `NumFontFamilies`, `FontFamily[0..N]`, `FontFamily[0..N]SystemFont[0..6]`, `FontFamily[0..N]UserFont[0..6]` 리소스 추가
> - 세션 저장/복원에 `*Fonts.familyIndex` 추가 (기존 session 파일과 호환)
> - 메시지 카탈로그에 "Family", "Font Family" 등 신규 메시지 추가
> - **8개** 로케일(C + de_DE/es_ES/fr_FR/it_IT/ja_JP/zh_CN/zh_TW) app-defaults에 패밀리 리소스 추가
>   - 참고: ko_KR, sv_SE는 .tmsg 번역만 존재하고 app-defaults/Dtstyle 파일은 없음 → 업데이트 대상에서 제외
>
> **Estimated Effort**: Medium-Large
> **Parallel Execution**: YES - 4 waves
> **Critical Path**: Wave 1 (data model) → Wave 2 (resource loading) → Wave 3 (UI) → Wave 4 (integration + i18n)

---

## Context

### Original Request
CDE Style Manager → Font 대화상자에 폰트 패밀리(글꼴 종류) 선택 기능이 없음. 현재는 7개로 미리 묶인 (size+family) 쌍에서만 선택 가능. 사용자가 직접 폰트 패밀리를 선택할 수 있는 UI를 추가하고, 기존 session 파일과 호환성을 유지하며, 모든 UI 요소에 빠짐없이 적용.

### Interview Summary

**Key Discussions**:
- **패밀리 목록 정의 방식**: 설정 기반 (app-defaults에 FontFamily[N] 리소스 추가) — 동적 X11/fontconfig 열거는 플랫폼 호환성 문제로 배제
- **XLFD 저장 방식**: 풀세트 별칭 저장 (기존 sysStr/userStr 패턴 유지) — 2D로 인덱싱하되 XLFD 풀 문자열 그대로 보존
- **Session 호환성**: 기존 session 파일과 호환 유지 — `*Fonts.familyIndex` 새 리소스 추가, 없으면 0 기본값
- **UI 레이아웃**: 좌측 패밀리 목록 + 우측 크기 목록 (수평) — 가장 직관적이고 기존 패턴과 일관

**Research Findings**:
- 현재 `fontChoice[10]` 고정 배열 + 7개 `XtResource` 하드코딩 → XtOffset 메커니즘은 컴파일 타임 오프셋 필요 → 2D 배열 직접 매핑 불가
- **해결**: `fontChoice[MAX_FAMILIES * MAX_SIZES]` 플랫 1D + `FONT_INDEX(fam, sz)` 매크로로 가상 2D 인덱싱
- 기존 `SystemFont1..7`/`UserFont1..7` 리소스는 Family 0의 기본값으로 alias (기존 admin 커스터마이즈 보존)
- 8개 로케일의 Dtstyle app-defaults가 각각 다른 폰트 정의 보유 → 모든 로케일 업데이트 필요

### Metis Review (Critical Findings Applied)

**Identified Gaps (addressed)**:

1. **XtOffset 제약 (CRITICAL)**: 2D 배열에 직접 XtOffset 사용 불가. → **해결**: 플랫 1D 배열 `fontChoice[MAX_FAMILIES*MAX_SIZES]` + 인덱스 매크로
2. **8개 로케일 app-defaults**: de/es/fr/it/ja/zh_CN/zh_TW 모두 업데이트 필요 → 명시적 task로 분리
3. **Edge case: numFamilies<=1**: 기존 동작 보존을 위해 패밀리 UI 숨김 → 가드 코드 추가
4. **Edge case: family index out of bounds**: restore 시 클램핑 필수
5. **NLS 메시지 카탈로그**: set 5에 메시지 25, 26 추가
6. **size list repopulation**: 패밀리 변경 시 size list 아이템은 동일("1".."7")하지만 preview 폰트 변경
7. **Cancel 동작**: originalFamilyIndex + originalSizeIndex 별도 추적
8. **session save string에 `*FontFamily` 추가**: dtsession이 저장하는 resource string에 family 인덱스 포함

**Guardrails Applied**:
- 기존 `SystemFont1..7`/`UserFont1..7` 리소스명/의미 변경 금지 (G1)
- `Fontset` 구조체 필드 순서/제거 금지 (XtOffset 의존성) (G2)
- `ButtonCB`의 `fontres` 형식 변경 금지 (다른 CDE 앱이 읽음) — 추가만 가능 (G3)
- `SmNewFontSettings` 프로토콜 변경 금지 (G4) — Task 12에 명시적 검증 추가됨
- `USE_XFT` include guard dance 보존 (G5) — Task 9에 명시적 검증 추가됨
- dtstyle은 fontconfig/Xft 직접 의존 금지 (리소스 기반만) (G6)
- numFamilies<=1일 때 기존 UI와 동일 (패밀리 TitleBox 숨김) (G7)

---

## Work Objectives

### Core Objective
CDE Style Manager Font 대화상자에 폰트 패밀리 선택 UI를 추가하여, 사용자가 미리 정의된 폰트 패밀리 중 하나를 선택하고 크기와 조합하여 적용할 수 있게 한다.

### Concrete Deliverables
- `cde/programs/dtstyle/Font.h`: Fontset 구조체 확장 (familyName, familyLabel 필드 추가)
- `cde/programs/dtstyle/Font.c`: familyList 위젯 추가, 2D 인덱싱, changeSampleFontCB/ButtonCB/saveFonts/restoreFonts 업데이트
- `cde/programs/dtstyle/Main.h`: ApplicationData 확장 (numFamilies, familyNames, familyLabels, MAX_FONT_FAMILIES, MAX_FONT_SIZES 상수)
- `cde/programs/dtstyle/Resource.c`: 새 Xresources 테이블 추가, GetFamilyResources() 함수 추가
- `cde/programs/dtstyle/Resource.h`: 새 함수 선언 추가
- `cde/programs/dtstyle/Dtstyle.src` + `Dtstyle`: NumFontFamilies, FontFamily[N], FontFamily[N]SystemFont[0..6], FontFamily[N]UserFont[0..6] 리소스 추가
- `cde/programs/dtstyle/dtstyle.msg`: 메시지 5-25, 5-26 추가 ("Family", "Font Family")
- `cde/programs/dtstyle/nlsREADME.txt`: 새 리소스 문서화
- `cde/programs/dtstyle/dtstyle.man`: 새 리소스 문서화
- 8개 로케일 `cde/programs/localized/*/app-defaults/Dtstyle` (C + 7개: de_DE, es_ES, fr_FR, it_IT, ja_JP, zh_CN, zh_TW): NumFontFamilies + Family[0]SystemFont/UserFont 리소스 추가
  - 참고: `ko_KR.UTF-8`와 `sv_SE.UTF-8`은 `.tmsg` 번역 파일만 있고 `app-defaults/Dtstyle` 파일은 없으므로 업데이트 대상에서 제외
- 10개 로케일 `cde/programs/localized/*/app-defaults/Dtstyle.tmsg`: 새 메시지 번호 추가 (ko_KR, sv_SE 포함)

### Definition of Done
- [x] `cd cde/programs/dtstyle && make clean && make 2>&1` → 경고 0, 에러 0
- [x] `gencat cde/programs/dtstyle/dtstyle.cat cde/programs/dtstyle/dtstyle.msg` → 성공
- [x] `grep "FontFamily0" cde/programs/localized/C/app-defaults/Dtstyle` → 매치
- [x] `grep "Family" cde/programs/localized/*/app-defaults/Dtstyle` → **8개** 로케일(C + de_DE, es_ES, fr_FR, it_IT, ja_JP, zh_CN, zh_TW)에 매치 (ko_KR, sv_SE은 .tmsg만 존재)
- [x] Family 0 기본값이 기존 SystemFont1..7 / UserFont1..7과 동일 값 유지 (backward compat)
- [x] session save/restore round-trip 검증 (수동)

### Must Have
- 폰트 패밀리 선택 UI (좌측 TitleBox + ScrolledList)
- 패밀리 변경 시 size list preview 폰트 갱신
- OK 버튼으로 `*FontFamily: N` 리소스 저장
- Cancel 버튼으로 패밀리+크기 모두 원복
- 기존 7개 묶음 SystemFont1..7/UserFont1..7 → Family 0에 alias
- **8개** 로케일 app-defaults에 패밀리 리소스 추가 (Family 0은 각 로케일 기본값; ko_KR, sv_SE는 .tmsg만 존재하여 제외)

### Must NOT Have (Guardrails)
- ❌ 동적 폰트 열거 (X11/fontconfig API 호출)
- ❌ 기존 `SystemFont1..7` 리소스명 제거/변경
- ❌ `Fontset` 필드 순서 변경/제거
- ❌ `ButtonCB`의 `fontres` 형식 변경 (추가만 허용)
- ❌ Xft/fontconfig 직접 의존성 추가
- ❌ CJK 폰트 패밀리 별도 정의 (기존 로케일 기본값 사용)
- ❌ 가변 패밀리 크기 (모든 패밀리는 정확히 numFonts개 사이즈)
- ❌ 다른 CDE 앱 (dtwm/dtterm/dtfile) 폰트 처리 변경

---

## Verification Strategy (MANDATORY)

> **ZERO HUMAN INTERVENTION** - ALL verification is agent-executed. No exceptions.

### Test Decision
- **Infrastructure exists**: NO (CDE has no automated test suite for dtstyle)
- **Automated tests**: NONE (will be added as build verification only)
- **QA Policy**: Every task MUST include agent-executed QA scenarios. CDE는 자동화 테스트가 없으므로 **수동 시뮬레이션**으로 대체:
  - **Build verification**: `make` 명령으로 컴파일 성공 확인
  - **Resource verification**: `grep`/`xrdb`로 리소스 로딩 확인
  - **Code review**: 정적 분석 (XtOffset 정확성, 인덱스 범위, NULL 처리)
  - **Manual simulation**: dialog lifecycle을 코드로 시뮬레이션 (콜백 호출 시나리오)

### QA Tool Mapping
- **Build**: `Bash` (`make` 명령)
- **Static analysis**: `Grep` (리소스 사용처 검증), `Bash` (cpp/objdump로 XtOffset 확인)
- **Resource loading**: `Bash` (`xrdb -query` 또는 `XrmDatabase` 직접 호출)
- **Manual interaction simulation**: 코드 리뷰로 dialog lifecycle 검증 (CreateFontDlg → changeSampleFontCB → ButtonCB OK)

---

## Execution Strategy

### Parallel Execution Waves

> Maximize throughput. 4-7 tasks per wave. Critical path: data model → resource loading → UI → integration.

```
Wave 1 (Foundation - data model + types, MAX PARALLEL):
├── Task 1: Add MAX_FONT_FAMILIES / MAX_FONT_SIZES constants to Main.h
├── Task 2: Extend Fontset struct in Font.h (add familyName, familyLabel)
├── Task 3: Extend ApplicationData struct in Main.h (numFamilies, familyNames, familyLabels, fontChoice[1D])
└── Task 4: Update Makefile.am if needed for new files (likely no change)

Wave 2 (Resource system - data loading):
├── Task 5: Add NumFontFamilies, familyNames, familyLabels Xresources to Resource.c
├── Task 6: Add FontFamily[N]SystemFont/UserFont Xresources (with default alias to SystemFont1..7)
├── Task 7: Add GetFamilyResources() function and Resource.h declarations
└── Task 8: Update GetApplicationResources() to call GetFamilyResources()

Wave 3 (UI + Logic - dialog and callbacks):
├── Task 9: Add familyTB/familyList widgets to CreateFontDlg layout
├── Task 10: Add originalFamilyIndex, selectedFamilyIndex tracking (FONT_INDEX macro)
├── Task 11: Update changeSampleFontCB for 2D family+size selection
├── Task 12: Update ButtonCB OK to include *FontFamily: N in fontres
├── Task 13: Update ButtonCB CANCEL to revert both family and size
└── Task 14: Update saveFonts() and restoreFonts() for family index

Wave 4 (i18n + Integration):
├── Task 15: Update dtstyle.msg (add messages 5-25, 5-26)
├── Task 16: Update Dtstyle.src + Dtstyle (C locale app-defaults)
├── Task 17: Update 7 other locale Dtstyle files (de_DE, es_ES, fr_FR, it_IT, ja_JP, ko_KR, sv_SE, zh_CN, zh_TW)
├── Task 18: Update Dtstyle.tmsg files for all locales
├── Task 19: Update nlsREADME.txt and dtstyle.man documentation
└── Task 20: Full build verification + static analysis + manual simulation

Critical Path: Task 1-3 → Task 5-8 → Task 9-14 → Task 15-20
Parallel Speedup: ~60% faster than sequential
Max Concurrent: 7 (Wave 3)
```

### Agent Dispatch Summary

- **Wave 1**: `[quick]` x4 - data model additions, mechanical changes
- **Wave 2**: `[unspecified-high]` x4 - resource system requires careful XtOffset handling
- **Wave 3**: `[unspecified-high]` x6 - UI changes + callback logic with state management
- **Wave 4**: `[unspecified-low]` x3 + `[quick]` x3 - i18n + build verification

---

## TODOs

> Implementation = ONE Task. Every task MUST have: Recommended Agent Profile + QA Scenarios.
> **A task WITHOUT QA Scenarios is INCOMPLETE.**

---

### Wave 1: Foundation - Data Model & Types (PARALLEL)

- [x] 1. Add MAX_FONT_FAMILIES and MAX_FONT_SIZES constants to Main.h

  **What to do**:
  - In `cde/programs/dtstyle/Main.h`, add two `#define` constants:
    ```c
    #define MAX_FONT_FAMILIES  8   /* Max number of font families in Style Manager Font dialog */
    #define MAX_FONT_SIZES     7   /* Max number of font sizes per family (backward compat: was numFonts) */
    #define FONT_INDEX(fam, sz) ((fam) * MAX_FONT_SIZES + (sz))
    #define FONT_FAMILY(idx)    ((idx) / MAX_FONT_SIZES)
    #define FONT_SIZE(idx)      ((idx) % MAX_FONT_SIZES)
    ```
  - Place these defines in the `#define statements` section near line 75 (after `#define DIALOG_MWM_FUNC`)

  **Must NOT do**:
  - Do not change existing #define values
  - Do not add includes (this is a constants-only file)

  **Recommended Agent Profile**:
  - **Category**: `quick` — single-file constants addition
  - **Skills**: `[]`

  **Parallelization**:
  - **Can Run In Parallel**: YES
  - **Parallel Group**: Wave 1
  - **Blocks**: Tasks 2, 3, 5-14 (all code that uses these constants)
  - **Blocked By**: None (can start immediately)

  **References**:
  - `cde/programs/dtstyle/Main.h:75-95` — existing #define block for placement context

  **Acceptance Criteria**:
  - [ ] Constants `MAX_FONT_FAMILIES = 8`, `MAX_FONT_SIZES = 7` defined
  - [ ] `FONT_INDEX`, `FONT_FAMILY`, `FONT_SIZE` macros defined
  - [ ] Constants placed in `#define statements` section (not random location)
  - [ ] No existing defines changed/removed

  **QA Scenarios (MANDATORY)**:

  ```
  Scenario: Constants compile and are accessible
    Tool: Bash (gcc preprocessor)
    Preconditions: dtstyle Makefile available
    Steps:
      1. cd cde/programs/dtstyle
      2. echo '#include "Main.h"' | gcc -E -I. -I../include -I../../include - | grep "MAX_FONT_FAMILIES"
      3. Verify output contains "#define MAX_FONT_FAMILIES 8"
    Expected Result: Preprocessor outputs the define
    Failure Indicators: "not found", empty output
    Evidence: .sisyphus/evidence/task-1-constants-defined.txt

  Scenario: Macros expand correctly
    Tool: Bash (gcc preprocessor)
    Preconditions: Main.h edited
    Steps:
      1. echo '#include "Main.h"\nint test_f1s3 = FONT_INDEX(1, 3);' | gcc -E -I. - | grep "test_f1s3"
      2. Verify test_f1s3 = 1*7+3 = 10
      3. echo '#include "Main.h"\nint test_fam = FONT_FAMILY(15); int test_sz = FONT_SIZE(15);' | gcc -E -I. - | grep -E "test_fam|test_sz"
      4. Verify test_fam = 2, test_sz = 1
    Expected Result: test_f1s3=10, test_fam=2, test_sz=1
    Failure Indicators: wrong values, macro not expanded
    Evidence: .sisyphus/evidence/task-1-macros-expand.txt
  ```

  **Commit**: NO (groups with Task 2-3)
  - Files: `cde/programs/dtstyle/Main.h`

- [x] 2. Extend Fontset struct in Font.h with familyName and familyLabel fields

  **What to do**:
  - In `cde/programs/dtstyle/Font.h`, extend the `Fontset` struct (lines 46-52):
    ```c
    typedef struct {
      XmFontList sysFont;
      XmFontList userFont;
      String     sysStr;
      String     userStr;
      XmString   pointSize;
      /* Added for font family selection feature */
      String     familyName;   /* family identifier (e.g., "system", "user", "serif") */
      String     familyLabel;  /* display label for UI list (e.g., "System", "User", "Serif") */
    } Fontset;
    ```
  - **CRITICAL**: Only ADD fields at the END. Do not reorder existing fields — XtOffset macro in Resource.c depends on exact struct layout.

  **Must NOT do**:
  - Do not reorder existing fields (XtOffset in Resource.c would break)
  - Do not change field types
  - Do not remove existing fields

  **Recommended Agent Profile**:
  - **Category**: `quick` — single-field struct extension
  - **Skills**: `[]`

  **Parallelization**:
  - **Can Run In Parallel**: YES
  - **Parallel Group**: Wave 1
  - **Blocks**: Tasks 3, 5, 6, 7, 9-14
  - **Blocked By**: None (can start immediately, only depends on Task 1 for Main.h include)

  **References**:
  - `cde/programs/dtstyle/Font.h:46-52` — current Fontset definition
  - `cde/programs/dtstyle/Resource.c:89-119` — sysFont_resources uses XtOffset on Fontset fields (proves ordering matters)

  **Acceptance Criteria**:
  - [ ] `familyName` field of type `String` added
  - [ ] `familyLabel` field of type `String` added
  - [ ] Both fields added AT THE END of struct (after pointSize)
  - [ ] Existing 5 fields unchanged in order and type

  **QA Scenarios (MANDATORY)**:

  ```
  Scenario: Struct field offsets preserved
    Tool: Bash (gcc struct size)
    Preconditions: Font.h modified
    Steps:
      1. cat > /tmp/font_struct_test.c <<'EOF'
         #include <stddef.h>
         #include <Xm/Xm.h>
         typedef struct {
           XmFontList sysFont;
           XmFontList userFont;
           String     sysStr;
           String     userStr;
           XmString   pointSize;
           String     familyName;
           String     familyLabel;
         } Fontset;
         int main() {
           return (offsetof(Fontset, sysFont) == 0 &&
                   offsetof(Fontset, userFont) > 0 &&
                   offsetof(Fontset, sysStr) > 0 &&
                   offsetof(Fontset, userFont) > 0 &&
                   offsetof(Fontset, userStr) > 0 &&
                   offsetof(Fontset, pointSize) > 0 &&
                   offsetof(Fontset, familyName) > 0 &&
                   offsetof(Fontset, familyLabel) > 0) ? 0 : 1;
         }
         EOF
      2. gcc -I/usr/include -I/usr/include/X11 /tmp/font_struct_test.c -o /tmp/font_struct_test -lXm -lXt
      3. /tmp/font_struct_test; echo "exit=$?"
    Expected Result: exit=0
    Failure Indicators: exit=1, struct fields have same offset, struct too small
    Evidence: .sisyphus/evidence/task-2-struct-offsets.txt

  Scenario: Existing XtOffset usages still resolve
    Tool: Grep
    Preconditions: Font.h modified
    Steps:
      1. grep -n "XtOffset.*fontChoice\[" cde/programs/dtstyle/Resource.c
      2. For each match, verify the field is one of: sysFont, userFont, sysStr, userStr
    Expected Result: All XtOffset references use only existing 4 fields (no familyName/familyLabel XtOffset)
    Failure Indicators: XtOffset references familyName/familyLabel directly
    Evidence: .sisyphus/evidence/task-2-xtoffset-check.txt
  ```

  **Commit**: NO (groups with Task 1, 3)
  - Files: `cde/programs/dtstyle/Font.h`

- [x] 3. Extend ApplicationData struct in Main.h for 2D font selection

  **What to do**:
  - In `cde/programs/dtstyle/Main.h` (lines 140-157), replace `Fontset fontChoice[10]` with:
    ```c
    /* Font family + size 2D structure flattened to 1D for XtOffset compatibility */
    Fontset    fontChoice[MAX_FONT_FAMILIES * MAX_FONT_SIZES];
    #define fontChoiceIdx(fam, sz) fontChoice[FONT_INDEX(fam, sz)]
    ```
  - Add new fields to `ApplicationData`:
    ```c
    /* Font family selection (new feature) */
    int        numFamilies;        /* Number of font families (default 2: system, user) */
    int        maxFontSizes;       /* Alias for numFonts for clarity (default 7) */
    String     familyNames[MAX_FONT_FAMILIES];    /* Family identifiers from Xresources */
    String     familyLabels[MAX_FONT_FAMILIES];   /* Display labels for UI */
    ```
  - Add new widget pointers to `Style` struct (around line 195):
    ```c
    /* Font family selection widgets (new) */
    Widget     familyTB,           /* TitleBox containing family list */
               familyList;         /* ScrolledList of font families */
    ```

  **Must NOT do**:
  - Do not change the `numFonts` field (preserve backwards compat semantics)
  - Do not change `systemFont`/`userFont` field types
  - Do not reorder existing fields in ApplicationData (XtOffset depends on it)

  **Recommended Agent Profile**:
  - **Category**: `unspecified-high` — data structure redesign with XtOffset constraints
  - **Skills**: `[]`

  **Parallelization**:
  - **Can Run In Parallel**: YES
  - **Parallel Group**: Wave 1
  - **Blocks**: Tasks 5-14
  - **Blocked By**: Task 1 (needs MAX_FONT_FAMILIES constants)

  **References**:
  - `cde/programs/dtstyle/Main.h:140-157` — current ApplicationData definition
  - `cde/programs/dtstyle/Resource.c:217-271` — `resources[]` table uses XtOffset on ApplicationData
  - `cde/programs/dtstyle/Resource.c:89-119` — `sysFont_resources[]` uses XtOffset on fontChoice[i] (1D indexing must be preserved)

  **Acceptance Criteria**:
  - [ ] `fontChoice[10]` replaced with `fontChoice[MAX_FONT_FAMILIES * MAX_FONT_SIZES]`
  - [ ] `numFamilies` field added (type int)
  - [ ] `familyNames[MAX_FONT_FAMILIES]` field added (type String array)
  - [ ] `familyLabels[MAX_FONT_FAMILIES]` field added (type String array)
  - [ ] Existing fields unchanged in order and type
  - [ ] `Style` struct extended with `familyTB` and `familyList` widget pointers

  **QA Scenarios (MANDATORY)**:

  ```
  Scenario: Compile check after struct changes
    Tool: Bash (make)
    Preconditions: Tasks 1-3 completed
    Steps:
      1. cd cde/programs/dtstyle
      2. make dtstyle-Font.o 2>&1 | head -20
    Expected Result: Compiles without "redefinition" or "conflicting types" errors
    Failure Indicators: errors about Fontset, fontChoice, ApplicationData redeclaration
    Evidence: .sisyphus/evidence/task-3-compile-check.log

  Scenario: XtOffset for fontChoice[i] still computes correct byte offset
    Tool: Bash (cpp + grep)
    Preconditions: Main.h and Resource.c exist
    Steps:
      1. grep "XtOffset.*fontChoice\[" cde/programs/dtstyle/Resource.c
      2. Each match should resolve to ApplicationDataPtr + offsetof(fontChoice[0]) + i*sizeof(Fontset)
      3. For i=0..6, verify all 28 XtOffset usages still valid (4 fields × 7 sizes)
    Expected Result: All XtOffset usages still resolve, no out-of-bounds (i < 7 = within MAX_FONT_SIZES)
    Failure Indicators: XtOffset uses i >= 7, references removed fields
    Evidence: .sisyphus/evidence/task-3-xtoffset-valid.txt

  Scenario: New fields don't break existing struct
    Tool: Bash (gcc sizeof)
    Preconditions: Main.h modified
    Steps:
      1. cat > /tmp/appdata_sizeof.c <<'EOF'
         #include <Xm/Xm.h>
         #include "Font.h"
         typedef struct {
           int numFonts;
           XmFontList userFont, systemFont;
           String userFontStr, systemFontStr;
           Fontset fontChoice[56]; /* MAX_FONT_FAMILIES*MAX_FONT_SIZES */
           String familyNames[8], familyLabels[8];
           int numFamilies;
           /* ... rest of fields ... */
         } ApplicationData;
         int main() { return sizeof(ApplicationData) > 0 ? 0 : 1; }
         EOF
      2. gcc -I/usr/include -I/usr/include/X11 /tmp/appdata_sizeof.c -o /tmp/test 2>&1
      3. echo "exit=$?"
    Expected Result: Compiles, exit=0
    Failure Indicators: errors about struct size, undefined fields
    Evidence: .sisyphus/evidence/task-3-struct-size.txt
  ```

  **Commit**: YES
  - Message: `feat(dtstyle): add MAX_FONT_FAMILIES, extend Fontset and ApplicationData structs`
  - Files: `cde/programs/dtstyle/Font.h`, `cde/programs/dtstyle/Main.h`
  - Pre-commit: `cd cde/programs/dtstyle && make dtstyle-Font.o dtstyle-Resource.o`

---

- [x] 4. Update Makefile.am if needed (likely no change)

  **What to do**:
  - Check `cde/programs/dtstyle/Makefile.am` for any references to Font.c or Resource.c
  - If they exist in `_OBJECTS` or similar list, no change needed (existing files are already included)
  - If new `.c` files were created, add them; but this plan does not add new files

  **Must NOT do**:
  - Do not modify build flags, library dependencies, or include paths
  - Do not add new files (all changes are to existing files)

  **Recommended Agent Profile**:
  - **Category**: `quick` — verification + no-change-or-trivial change
  - **Skills**: `[]`

  **Parallelization**:
  - **Can Run In Parallel**: YES
  - **Parallel Group**: Wave 1
  - **Blocks**: Wave 4 Task 20 (build verification)
  - **Blocked By**: None

  **References**:
  - `cde/programs/dtstyle/Makefile.am` — current Makefile

  **Acceptance Criteria**:
  - [ ] Confirmed no Makefile.am changes needed (all target files already in build)

  **QA Scenarios (MANDATORY)**:

  ```
  Scenario: Makefile includes all modified files
    Tool: Grep
    Preconditions: Makefile.am exists
    Steps:
      1. grep -E "Font\.c|Resource\.c|Font\.h|Main\.h" cde/programs/dtstyle/Makefile.am
      2. Verify Font.c and Resource.c are in source list (or implicit via glob)
      3. Verify Makefile.am does not explicitly list .h files in compilation (they're auto-included)
    Expected Result: Font.c and Resource.c in source list, no .h references needed
    Failure Indicators: Font.c or Resource.c missing from source list
    Evidence: .sisyphus/evidence/task-4-makefile-check.txt
  ```

  **Commit**: NO

### Wave 2: Resource System (PARALLEL)

- [x] 5. Add NumFontFamilies, familyNames, familyLabels Xresources to Resource.c

  **What to do**:
  - In `cde/programs/dtstyle/Resource.c`, add new XtResource tables (place after the existing `userStr_resources[]` table, before the main `resources[]` table at line 217):
    ```c
    XtResource numFamilies_resources[] = {
      {"numFontFamilies", "NumFontFamilies", XmRInt, sizeof (int),
          XtOffset(ApplicationDataPtr, numFamilies), XmRImmediate, (caddr_t) 2
      },
      {"maxFontSizes", "MaxFontSizes", XmRInt, sizeof (int),
          XtOffset(ApplicationDataPtr, maxFontSizes), XmRImmediate, (caddr_t) 7
      },
    };

    XtResource familyNames_resources[] = {
      {"fontFamily0", "FontFamily0", XmRString, sizeof (String),
          XtOffset(ApplicationDataPtr, familyNames[0]), XmRString, "system" },
      {"fontFamily1", "FontFamily1", XmRString, sizeof (String),
          XtOffset(ApplicationDataPtr, familyNames[1]), XmRString, "user" },
      /* Family 2-7: defaults must be non-NULL for safety */
      {"fontFamily2", "FontFamily2", XmRString, sizeof (String),
          XtOffset(ApplicationDataPtr, familyNames[2]), XmRString, "serif" },
      {"fontFamily3", "FontFamily3", XmRString, sizeof (String),
          XtOffset(ApplicationDataPtr, familyNames[3]), XmRString, "monospace" },
      {"fontFamily4", "FontFamily4", XmRString, sizeof (String),
          XtOffset(ApplicationDataPtr, familyNames[4]), XmRString, "sansserif" },
      {"fontFamily5", "FontFamily5", XmRString, sizeof (String),
          XtOffset(ApplicationDataPtr, familyNames[5]), XmRString, "cursive" },
      {"fontFamily6", "FontFamily6", XmRString, sizeof (String),
          XtOffset(ApplicationDataPtr, familyNames[6]), XmRString, "fantasy" },
      {"fontFamily7", "FontFamily7", XmRString, sizeof (String),
          XtOffset(ApplicationDataPtr, familyNames[7]), XmRString, "" },
    };

    XtResource familyLabels_resources[] = {
      {"fontFamily0Label", "FontFamily0Label", XmRString, sizeof (String),
          XtOffset(ApplicationDataPtr, familyLabels[0]), XmRString, "System" },
      {"fontFamily1Label", "FontFamily1Label", XmRString, sizeof (String),
          XtOffset(ApplicationDataPtr, familyLabels[1]), XmRString, "User" },
      {"fontFamily2Label", "FontFamily2Label", XmRString, sizeof (String),
          XtOffset(ApplicationDataPtr, familyLabels[2]), XmRString, "Serif" },
      {"fontFamily3Label", "FontFamily3Label", XmRString, sizeof (String),
          XtOffset(ApplicationDataPtr, familyLabels[3]), XmRString, "Monospace" },
      {"fontFamily4Label", "FontFamily4Label", XmRString, sizeof (String),
          XtOffset(ApplicationDataPtr, familyLabels[4]), XmRString, "Sans Serif" },
      {"fontFamily5Label", "FontFamily5Label", XmRString, sizeof (String),
          XtOffset(ApplicationDataPtr, familyLabels[5]), XmRString, "Cursive" },
      {"fontFamily6Label", "FontFamily6Label", XmRString, sizeof (String),
          XtOffset(ApplicationDataPtr, familyLabels[6]), XmRString, "Fantasy" },
      {"fontFamily7Label", "FontFamily7Label", XmRString, sizeof (String),
          XtOffset(ApplicationDataPtr, familyLabels[7]), XmRString, "" },
    };
    ```
  - **CRITICAL**: All defaults MUST be non-NULL. Empty string "" is acceptable for unused slots.

  **Must NOT do**:
  - Do not change existing XtResource tables (sysFont_resources, userFont_resources, sysStr_resources, userStr_resources, resources)
  - Do not use 2D array indexing in XtOffset (Xt requires compile-time offsets)

  **Recommended Agent Profile**:
  - **Category**: `unspecified-high` — careful Xt resource table construction
  - **Skills**: `[]`

  **Parallelization**:
  - **Can Run In Parallel**: YES
  - **Parallel Group**: Wave 2
  - **Blocks**: Task 8 (GetApplicationResources call)
  - **Blocked By**: Task 3 (needs ApplicationData fields)

  **References**:
  - `cde/programs/dtstyle/Resource.c:89-119` — sysFont_resources[] template
  - `cde/programs/dtstyle/Resource.c:217-271` — resources[] template
  - `cde/programs/dtstyle/Main.h:140-157` — ApplicationData field offsets

  **Acceptance Criteria**:
  - [ ] `numFamilies_resources[]` table with 2 entries (numFontFamilies, maxFontSizes)
  - [ ] `familyNames_resources[]` table with 8 entries (fontFamily0..7)
  - [ ] `familyLabels_resources[]` table with 8 entries (fontFamily0Label..7)
  - [ ] All defaults non-NULL
  - [ ] XtOffset usage correct (single-dimension field access)
  - [ ] Existing tables unchanged

  **QA Scenarios (MANDATORY)**:

  ```
  Scenario: XtOffset for new family resources
    Tool: Bash (cpp + grep)
    Preconditions: Resource.c modified
    Steps:
      1. grep -A 3 "numFamilies_resources\[\]\|familyNames_resources\[\]\|familyLabels_resources\[\]" cde/programs/dtstyle/Resource.c | grep XtOffset
      2. Verify each XtOffset is: ApplicationDataPtr, familyNames[0..7], familyLabels[0..7], numFamilies, maxFontSizes
    Expected Result: 18 XtOffset references, all single-dimension, all point to existing fields
    Failure Indicators: XtOffset with 2D indexing, references to non-existent fields
    Evidence: .sisyphus/evidence/task-5-xtoffset-tables.txt

  Scenario: Compile after adding resource tables
    Tool: Bash (make)
    Preconditions: Resource.c modified
    Steps:
      1. cd cde/programs/dtstyle
      2. make dtstyle-Resource.o 2>&1 | tee /tmp/build-task5.log
      3. grep -c "error\|warning" /tmp/build-task5.log
    Expected Result: 0 errors, 0 warnings
    Failure Indicators: errors, warnings
    Evidence: .sisyphus/evidence/task-5-build.log
  ```

  **Commit**: NO (groups with Task 6-8)

- [x] 6. Add FontFamily[N]SystemFont/UserFont Xresources (Family 0 alias to SystemFont1..7)

  **What to do**:
  - In `cde/programs/dtstyle/Resource.c`, add two new XtResource tables for Family 0 (the default that maps to existing SystemFont1..7 / UserFont1..7):
    ```c
    /* FontFamily0SystemFont[0..6] - alias to SystemFont1..7 for backward compat */
    XtResource family0SysStr_resources[] = {
      {"fontFamily0SystemFont0", "FontFamily0SystemFont0", XmRString, sizeof (String),
          XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(0,0)].sysStr), XmRString,
          "-dt-interface system-medium-r-normal-xxs*-*-*-*-*-*-*-*-*:" },
      /* ... 7 entries for sizes 0..6 with default = SystemFont1..7 ... */
    };

    XtResource family0SysFont_resources[] = {
      /* Similar for XmFontList type, 7 entries */
    };

    /* Repeat for Family 1 (User): FontFamily1SystemFont0..6 + UserFont0..6 */
    /* ... */
    ```
  - **Family 0 defaults MUST match the existing SystemFont1..7 / UserFont1..7 defaults** exactly.
  - Optionally, add stubs for Family 2..7 with safe defaults (or rely on null-check in code).

  **Must NOT do**:
  - Do not change the existing sysFont_resources[]/userFont_resources[] defaults
  - Do not change sysStr_resources[]/userStr_resources[] defaults
  - Do not use 2D array indexing incorrectly (must use FONT_INDEX macro)

  **Recommended Agent Profile**:
  - **Category**: `unspecified-high` — careful resource default matching for backward compat
  - **Skills**: `[]`

  **Parallelization**:
  - **Can Run In Parallel**: YES
  - **Parallel Group**: Wave 2
  - **Blocks**: Task 8
  - **Blocked By**: Task 3

  **References**:
  - `cde/programs/dtstyle/Resource.c:89-119` — sysFont_resources[] (defaults to copy)
  - `cde/programs/dtstyle/Resource.c:121-151` — userFont_resources[] (defaults to copy)
  - `cde/programs/dtstyle/Resource.c:153-183` — sysStr_resources[] (defaults to copy)
  - `cde/programs/dtstyle/Resource.c:185-215` — userStr_resources[] (defaults to copy)

  **Acceptance Criteria**:
  - [ ] `family0SysStr_resources[]` with 7 entries (FONT_INDEX(0, 0..6))
  - [ ] `family0SysFont_resources[]` with 7 entries (XmFontList type)
  - [ ] `family0UserStr_resources[]` with 7 entries
  - [ ] `family0UserFont_resources[]` with 7 entries
  - [ ] Defaults exactly match SystemFont1..7 / UserFont1..7
  - [ ] Use FONT_INDEX macro for all offsets

  **QA Scenarios (MANDATORY)**:

  ```
  Scenario: Defaults match existing SystemFont/UserFont defaults
    Tool: Bash (diff)
    Preconditions: Resource.c modified
    Steps:
      1. Extract defaults from family0SysStr_resources and sysStr_resources
      2. For each entry i=0..6, verify family0SysStr_resources[i].default_addr == sysStr_resources[i].default_addr
    Expected Result: 7/7 match
    Failure Indicators: any mismatch in default values
    Evidence: .sisyphus/evidence/task-6-defaults-match.txt

  Scenario: XtOffset uses FONT_INDEX macro
    Tool: Grep
    Preconditions: Resource.c modified
    Steps:
      1. grep "XtOffset.*fontChoice\[FONT_INDEX" cde/programs/dtstyle/Resource.c
      2. Count expected: 7 (sysStr) + 7 (sysFont) + 7 (userStr) + 7 (userFont) = 28
    Expected Result: 28 XtOffset usages with FONT_INDEX macro
    Failure Indicators: literal index (e.g., fontChoice[0]) instead of FONT_INDEX
    Evidence: .sisyphus/evidence/task-6-fontindex-usage.txt
  ```

  **Commit**: NO (groups with Task 5, 7, 8)

- [x] 7. Add GetFamilyResources() function and Resource.h declarations

  **What to do**:
  - In `cde/programs/dtstyle/Resource.c`, add new functions (place after `GetFontStrResources()` at line 309):
    ```c
    void GetFamilyNamesResources(void) {
      XtGetApplicationResources(style.shell, &style.xrdb, numFamilies_resources,
                                XtNumber(numFamilies_resources), NULL, 0);
      XtGetApplicationResources(style.shell, &style.xrdb, familyNames_resources,
                                XtNumber(familyNames_resources), NULL, 0);
      XtGetApplicationResources(style.shell, &style.xrdb, familyLabels_resources,
                                XtNumber(familyLabels_resources), NULL, 0);
    }

    void GetFamily0FontResources(void) {
      XtGetApplicationResources(style.shell, &style.xrdb, family0SysStr_resources,
                                XtNumber(family0SysStr_resources), NULL, 0);
      XtGetApplicationResources(style.shell, &style.xrdb, family0SysFont_resources,
                                XtNumber(family0SysFont_resources), NULL, 0);
      XtGetApplicationResources(style.shell, &style.xrdb, family0UserStr_resources,
                                XtNumber(family0UserStr_resources), NULL, 0);
      XtGetApplicationResources(style.shell, &style.xrdb, family0UserFont_resources,
                                XtNumber(family0UserFont_resources), NULL, 0);
    }
    ```
  - In `cde/programs/dtstyle/Resource.h`, add function declarations (after line 49):
    ```c
    extern void GetFamilyNamesResources(void);
    extern void GetFamily0FontResources(void);
    ```

  **Must NOT do**:
  - Do not change existing function signatures
  - Do not call XtGetApplicationResources more than once per resource group

  **Recommended Agent Profile**:
  - **Category**: `quick` — follow existing pattern
  - **Skills**: `[]`

  **Parallelization**:
  - **Can Run In Parallel**: YES
  - **Parallel Group**: Wave 2
  - **Blocks**: Task 8
  - **Blocked By**: Task 5, 6 (tables must exist)

  **References**:
  - `cde/programs/dtstyle/Resource.c:309-322` — GetFontStrResources() template
  - `cde/programs/dtstyle/Resource.c:280-301` — GetSysFontResource() pattern (uses single entry)
  - `cde/programs/dtstyle/Resource.h:44-55` — existing function declarations

  **Acceptance Criteria**:
  - [ ] `GetFamilyNamesResources()` calls XtGetApplicationResources for 3 tables
  - [ ] `GetFamily0FontResources()` calls XtGetApplicationResources for 4 tables
  - [ ] Function declarations added to Resource.h
  - [ ] Both functions use `style.shell` and `&style.xrdb` consistently

  **QA Scenarios (MANDATORY)**:

  ```
  Scenario: Functions compile without warnings
    Tool: Bash (make)
    Preconditions: Resource.c and Resource.h modified
    Steps:
      1. cd cde/programs/dtstyle
      2. make dtstyle-Resource.o 2>&1 | tee /tmp/build-task7.log
    Expected Result: 0 errors, 0 warnings
    Failure Indicators: implicit declaration, unused variable
    Evidence: .sisyphus/evidence/task-7-build.log

  Scenario: Function symbols exported
    Tool: Bash (nm)
    Preconditions: Object file built
    Steps:
      1. nm cde/programs/dtstyle/dtstyle-Resource.o | grep -E "GetFamilyNames|GetFamily0"
    Expected Result: 2 T symbols (GetFamilyNamesResources, GetFamily0FontResources)
    Failure Indicators: no symbols, U (undefined) instead of T (defined)
    Evidence: .sisyphus/evidence/task-7-symbols.txt
  ```

  **Commit**: NO (groups with Task 5, 6, 8)

- [x] 8. Update GetApplicationResources() to call new family resource loaders

  **What to do**:
  - In `cde/programs/dtstyle/Resource.c`, update `GetApplicationResources()` function (lines 332-339):
    ```c
    void GetApplicationResources(void) {
      XtGetApplicationResources(style.shell, &style.xrdb, resources,
                                XtNumber(resources), NULL, 0);
      GetFontStrResources();
      GetFamilyNamesResources();  /* NEW */
      GetFamily0FontResources();  /* NEW */
    }
    ```
  - **CRITICAL**: `GetFamily0FontResources()` is called AFTER `GetFontStrResources()` to ensure Family 0 resources override the legacy resources if both are set.

  **Must NOT do**:
  - Do not change the order of existing function calls
  - Do not call XtGetApplicationResources directly here (use the new functions)

  **Recommended Agent Profile**:
  - **Category**: `quick` — 2-line addition
  - **Skills**: `[]`

  **Parallelization**:
  - **Can Run In Parallel**: YES
  - **Parallel Group**: Wave 2
  - **Blocks**: Wave 3 (all UI tasks that read style.xrdb.numFamilies etc.)
  - **Blocked By**: Tasks 5, 6, 7

  **References**:
  - `cde/programs/dtstyle/Resource.c:332-339` — current GetApplicationResources

  **Acceptance Criteria**:
  - [ ] `GetFamilyNamesResources()` called in GetApplicationResources
  - [ ] `GetFamily0FontResources()` called after GetFontStrResources
  - [ ] Order: resources → GetFontStrResources → GetFamilyNamesResources → GetFamily0FontResources

  **QA Scenarios (MANDATORY)**:

  ```
  Scenario: All resource loaders called at startup
    Tool: Bash (make + nm)
    Preconditions: All Wave 2 tasks completed
    Steps:
      1. cd cde/programs/dtstyle
      2. make 2>&1 | tail -5
      3. nm dtstyle-Resource.o | grep -E "GetApplication|GetFamily|GetFont"
    Expected Result: GetApplicationResources references GetFamilyNamesResources and GetFamily0FontResources
    Failure Indicators: missing references
    Evidence: .sisyphus/evidence/task-8-resource-call.log
  ```

  **Commit**: YES
  - Message: `feat(dtstyle): add font family Xresources and GetFamilyResources() loader`
  - Files: `cde/programs/dtstyle/Resource.c`, `cde/programs/dtstyle/Resource.h`
  - Pre-commit: `cd cde/programs/dtstyle && make`

---

- [x] 9. Add familyTB/familyList widgets to CreateFontDlg layout (horizontal layout)

  **What to do**:
  - In `cde/programs/dtstyle/Font.c`, modify `CreateFontDlg()` (lines 172-429):
    1. Add new widget variables to the function:
       ```c
       Widget           familyTB;
       Widget           familyList;
       XmStringTable    familyItems;
       ```
    2. After the `fontpictLabel` widget is created (line 293), add family TitleBox:
       ```c
       /* Create Family TitleBox - placed top-left, below the icon */
       n = 0;
       string = CMPSTR(FAMILY);  /* GetMessage catalog 5-25 */
       XtSetArg(args[n], XmNtitleString, string);  n++;
       XtSetArg(args[n], XmNtopAttachment,      XmATTACH_WIDGET);     n++;
       XtSetArg(args[n], XmNtopWidget,          font.fontpictLabel);  n++;
       XtSetArg(args[n], XmNtopOffset,          style.verticalSpacing+5);  n++;
       XtSetArg(args[n], XmNleftAttachment,     XmATTACH_FORM);       n++;
       XtSetArg(args[n], XmNleftOffset,         style.horizontalSpacing);  n++;
       XtSetArg(args[n], XmNbottomAttachment,   XmATTACH_FORM);       n++;
       widget_list[count++] = font.familyTB =
           _DtCreateTitleBox(font.fontWkarea, "familyTB", args, n);
       XmStringFree(string);
       ```
    3. Populate family list from `style.xrdb.familyLabels[]`:
       ```c
       familyItems = (XmString *) XtMalloc(sizeof(XmString) * style.xrdb.numFamilies);
       for (n=0; n<style.xrdb.numFamilies; n++) {
         familyItems[n] = CMPSTR(style.xrdb.familyLabels[n] ? style.xrdb.familyLabels[n] : "");
       }
       n = 0;
       XtSetArg(args[n], XmNselectionPolicy, XmBROWSE_SELECT); n++;
       XtSetArg(args[n], XmNautomaticSelection, True); n++;
       XtSetArg(args[n], XmNvisibleItemCount, MIN(7, style.xrdb.numFamilies)); n++;
       XtSetArg(args[n], XmNitemCount, style.xrdb.numFamilies); n++;
       XtSetArg(args[n], XmNitems, familyItems); n++;
       if (font.selectedFontIndex >= 0) {
         XtSetArg(args[n], XmNselectedItems, &familyItems[FONT_FAMILY(font.selectedFontIndex)]); n++;
         XtSetArg(args[n], XmNselectedItemCount, 1); n++;
       }
       font.familyList = XmCreateScrolledList(font.familyTB, "familyList", args, n);
       XtAddCallback(font.familyList, XmNbrowseSelectionCallback, changeFamilyCB, NULL);
       XtFree((char *)familyItems);
       ```
    4. **CRITICAL: When `style.xrdb.numFamilies <= 1`, hide the familyTB**:
       ```c
       if (style.xrdb.numFamilies <= 1) {
         XtUnmanageChild(font.familyTB);  /* Hide family UI for backward compat */
       }
       ```
    5. **Modify the sizeTB attachment** to be on the RIGHT of familyTB (not left of form):
       Change lines 303-304 from:
       ```c
       XtSetArg(args[n], XmNleftAttachment,     XmATTACH_FORM);       n++;
       XtSetArg(args[n], XmNleftOffset,         style.horizontalSpacing);  n++;
       ```
       To:
       ```c
       XtSetArg(args[n], XmNleftAttachment,     XmATTACH_WIDGET);     n++;
       XtSetArg(args[n], XmNleftWidget,         font.familyTB);       n++;
       XtSetArg(args[n], XmNleftOffset,         style.horizontalSpacing);  n++;
       ```
    6. Update `Widget fontpictLabel` to also attach to familyTB top (so it spans both):
       ```c
       XtSetArg(args[n], XmNrightAttachment, XmATTACH_WIDGET);  n++;
       XtSetArg(args[n], XmNrightWidget,     font.familyTB);   n++;
       ```
    7. Add `#include <sys/param.h>` for MIN macro, or use a local MIN macro:
       ```c
       #ifndef MIN
       #define MIN(a,b) ((a) < (b) ? (a) : (b))
       #endif
       ```

  **Must NOT do**:
  - Do not change the existing sizeTB layout when numFamilies <= 1
  - Do not change previewTB attachment (must still be on right of sizeTB, bottom)
  - Do not create a new dialog shell (only add widgets to existing fontDialog)
  - **G5 Enforcement**: Do not remove, modify, or reorder the USE_XFT include-guard dance at the top of Font.c. The pattern at lines 46-65 (`#ifdef USE_XFT` → `#define _CDE_SAVED_USE_XFT 1` → `#undef USE_XFT` → includes → restore) must be preserved exactly. Do not add new includes inside the guard dance. Do not add includes after the dance that depend on Xft/XftWrapper/fontconfig (e.g., `<X11/Xft/Xft.h>`, `<fontconfig/fontconfig.h>`).

  **Recommended Agent Profile**:
  - **Category**: `unspecified-high` — Motif widget hierarchy construction
  - **Skills**: `[]`

  **Parallelization**:
  - **Can Run In Parallel**: YES
  - **Parallel Group**: Wave 3
  - **Blocks**: Tasks 10, 11, 12
  - **Blocked By**: Tasks 1-3 (data model), 5-8 (resource loading)

  **References**:
  - `cde/programs/dtstyle/Font.c:172-429` — current CreateFontDlg implementation
  - `cde/programs/dtstyle/Font.c:277-308` — fontWkarea, fontpictLabel, sizeTB creation (template)
  - `cde/programs/dtstyle/Font.c:321-340` — sizeList creation (template)
  - `cde/programs/dtstyle/Main.h:76-78` — MAX_ARGS, CMPSTR definitions

  **Acceptance Criteria**:
  - [ ] `familyTB` TitleBox widget created with name "familyTB"
  - [ ] `familyList` ScrolledList widget created with name "familyList"
  - [ ] Family list items populated from `style.xrdb.familyLabels[]`
  - [ ] `changeFamilyCB` callback registered on familyList
  - [ ] `sizeTB` left attachment changed to familyTB
  - [ ] `familyTB` is hidden when `numFamilies <= 1` (backward compat)
  - [ ] MIN macro defined or included
  - [ ] All widgets managed in correct order
  - [ ] **G5**: USE_XFT include-guard dance at Font.c lines 46-65 preserved verbatim
  - [ ] **G5**: No new Xft/XftWrapper/fontconfig includes added anywhere in Font.c
  - [ ] **G5**: Build with `-DUSE_XFT=1` and `-DUSE_XFT=0` both succeed

  **QA Scenarios (MANDATORY)**:

  ```
  Scenario: Widget creation compiles
    Tool: Bash (make)
    Preconditions: Font.c modified
    Steps:
      1. cd cde/programs/dtstyle
      2. make dtstyle-Font.o 2>&1 | tee /tmp/build-task9.log
      3. grep -c "error\|warning" /tmp/build-task9.log
    Expected Result: 0 errors, 0 warnings
    Failure Indicators: errors, warnings about widgets, undeclared functions
    Evidence: .sisyphus/evidence/task-9-build.log

  Scenario: Widget creation flow matches Motif pattern
    Tool: Grep
    Preconditions: Font.c modified
    Steps:
      1. grep -n "CreateFontDlg\|_DtCreateTitleBox\|XmCreateScrolledList" cde/programs/dtstyle/Font.c
      2. Verify familyTB uses _DtCreateTitleBox
      3. Verify familyList uses XmCreateScrolledList
      4. Verify both are added to widget_list and managed
    Expected Result: All Motif widget creation APIs used correctly
    Failure Indicators: wrong widget type, missing management
    Evidence: .sisyphus/evidence/task-9-widget-pattern.txt

  Scenario: Backward compat hide when numFamilies <= 1
    Tool: Grep
    Preconditions: Font.c modified
    Steps:
      1. grep -B 1 -A 3 "numFamilies <= 1" cde/programs/dtstyle/Font.c
      2. Verify XtUnmanageChild(font.familyTB) is called
    Expected Result: Hide logic present
    Failure Indicators: missing hide logic
    Evidence: .sisyphus/evidence/task-9-hide-logic.txt

  Scenario: [G5] USE_XFT include-guard dance preserved
    Tool: Bash (git diff + grep)
    Preconditions: Task 9 implementation complete
    Steps:
      1. git diff cde/programs/dtstyle/Font.c | head -80
      2. Verify lines 46-65 of Font.c (the guard dance) appear unchanged in diff context
      3. grep -n "USE_XFT\|_CDE_SAVED_USE_XFT" cde/programs/dtstyle/Font.c
      4. Verify the 4 occurrences of USE_XFT / _CDE_SAVED_USE_XFT are present:
         - Line ~46: #ifdef USE_XFT
         - Line ~48: #define _CDE_SAVED_USE_XFT 1
         - Line ~62: #ifdef _CDE_SAVED_USE_XFT
         - Line ~64: #undef _CDE_SAVED_USE_XFT
      5. Verify NO new includes between these markers
    Expected Result: Guard dance intact, no Xft/fontconfig includes added
    Failure Indicators: dance broken, new Xft includes, #include reordered
    Evidence: .sisyphus/evidence/task-9-g5-include-guard.txt

  Scenario: [G5] Build succeeds with both USE_XFT=0 and USE_XFT=1
    Tool: Bash (make with both flags)
    Preconditions: All Wave 3 code complete
    Steps:
      1. cd cde/programs/dtstyle
      2. make clean
      3. CFLAGS_EXTRA="-DUSE_XFT=0" make dtstyle-Font.o 2>&1 | tee /tmp/build-no-xft.log
      4. grep -cE "error|warning" /tmp/build-no-xft.log
      5. make clean
      6. CFLAGS_EXTRA="-DUSE_XFT=1" make dtstyle-Font.o 2>&1 | tee /tmp/build-with-xft.log
      7. grep -cE "error|warning" /tmp/build-with-xft.log
    Expected Result: 0 errors, 0 warnings in both builds
    Failure Indicators: any error or warning, or build success only with one flag
    Evidence: .sisyphus/evidence/task-9-g5-build-both-modes.log

  Scenario: [G5] No Xft/fontconfig symbols linked into dtstyle
    Tool: Bash (nm)
    Preconditions: Full build complete
    Steps:
      1. cd cde/programs/dtstyle
      2. nm dtstyle 2>/dev/null | grep -iE "Xft|fontconfig|Fc[A-Z]"
      3. Verify output is empty (no Xft/fontconfig symbols in final binary)
    Expected Result: Empty (no Xft/fontconfig symbols)
    Failure Indicators: XftInit, FcInit, FcFontList symbols present
    Evidence: .sisyphus/evidence/task-9-g5-no-xft-deps.txt
  ```

  **Commit**: NO (groups with Tasks 10-14)

- [x] 10. Add originalFamilyIndex, selectedFamilyIndex tracking with FONT_INDEX macro

  **What to do**:
  - In `cde/programs/dtstyle/Font.c`, update the `FontData` struct (lines 103-116):
    ```c
    typedef struct {
        Widget fontWkarea;
        Widget fontpictLabel;
        Widget previewTB;
        Widget previewForm;
        Widget systemLabel;
        Widget userText;
        Widget familyTB;          /* NEW */
        Widget familyList;        /* NEW */
        Widget sizeTB;
        Widget sizeList;
        /* composite index = (family * MAX_FONT_SIZES + size) */
        int    originalFontIndex;
        int    selectedFontIndex;
        String selectedFontStr;
        Boolean userTextChanged;
    } FontData;
    ```
  - In `CreateFontDlg()` (around line 192), update font.selectedFontStr logic to search all families:
    ```c
    font.selectedFontStr = style.xrdb.systemFontStr;
    font.selectedFontIndex = -1;
    /* Search through all families × sizes for matching font */
    for (fam=0; fam<style.xrdb.numFamilies; fam++) {
      for (sz=0; sz<style.xrdb.numFonts; sz++) {
        if (strcmp(font.selectedFontStr, fontChoiceIdx(fam, sz).sysStr) == 0) {
          font.selectedFontIndex = FONT_INDEX(fam, sz);
          if (!fontChoiceIdx(fam, sz).userFont)
            GetUserFontResource(FONT_INDEX(fam, sz));
          if (!fontChoiceIdx(fam, sz).sysFont)
            GetSysFontResource(FONT_INDEX(fam, sz));
          break;
        }
      }
      if (font.selectedFontIndex >= 0) break;
    }
    ```
  - Update the `changeFamilyCB` callback registration (added in Task 9) to also need the family dimension

  **Must NOT do**:
  - Do not change the type of existing FontData fields
  - Do not reorder existing FontData fields
  - Do not change changeSampleFontCB semantics (this task adds changeFamilyCB)

  **Recommended Agent Profile**:
  - **Category**: `unspecified-high` — state machine with index encoding
  - **Skills**: `[]`

  **Parallelization**:
  - **Can Run In Parallel**: YES
  - **Parallel Group**: Wave 3
  - **Blocks**: Task 11, 12, 13
  - **Blocked By**: Tasks 1-3, 9

  **References**:
  - `cde/programs/dtstyle/Font.c:103-116` — current FontData struct
  - `cde/programs/dtstyle/Font.c:192-228` — selectedFontIndex initialization
  - `cde/programs/dtstyle/Main.h:76-78` — FONT_INDEX, FONT_FAMILY, FONT_SIZE macros (from Task 1)

  **Acceptance Criteria**:
  - [ ] `familyTB` and `familyList` added to FontData
  - [ ] 2D search loop (family × size) for initial font match
  - [ ] Uses `fontChoiceIdx(fam, sz)` macro for 2D access
  - [ ] Uses `FONT_INDEX(fam, sz)` for index computation
  - [ ] No break needed in outer loop after match

  **QA Scenarios (MANDATORY)**:

  ```
  Scenario: 2D search compiles and works correctly
    Tool: Bash (make + static analysis)
    Preconditions: Font.c modified
    Steps:
      1. cd cde/programs/dtstyle
      2. make dtstyle-Font.o 2>&1 | grep -E "error|warning"
      3. Verify no out-of-bounds access (fam < numFamilies, sz < numFonts)
    Expected Result: 0 errors, 0 warnings
    Failure Indicators: errors, warnings
    Evidence: .sisyphus/evidence/task-10-build.log

  Scenario: FontData struct additions don't break existing access
    Tool: Bash (nm + objdump)
    Preconditions: Object file built
    Steps:
      1. objdump -d dtstyle-Font.o | grep -c "font\."
      2. Verify all expected FontData members are accessed (fontWkarea, fontpictLabel, familyTB, familyList, sizeList, etc.)
    Expected Result: All members referenced
    Failure Indicators: missing member references
    Evidence: .sisyphus/evidence/task-10-struct-access.txt
  ```

  **Commit**: NO (groups with Tasks 9, 11-14)

- [x] 11. Implement changeFamilyCB and update changeSampleFontCB for 2D selection

  **What to do**:
  - In `cde/programs/dtstyle/Font.c`, add new static function (place after `changeSampleFontCB` declaration, before its definition):
    ```c
    static void
    changeFamilyCB(Widget w, XtPointer client_data, XtPointer call_data)
    {
      XmListCallbackStruct *cb = (XmListCallbackStruct *) call_data;
      int fam, sz, idx;
      int hourGlassOn;
      Arg args[MAX_ARGS];
      int n;

      fam = cb->item_position - 1;
      sz = FONT_SIZE(font.selectedFontIndex >= 0 ? font.selectedFontIndex : 0);
      idx = FONT_INDEX(fam, sz);

      font.selectedFontIndex = idx;
      font.selectedFontStr = fontChoiceIdx(fam, sz).sysStr;

      hourGlassOn = !fontChoiceIdx(fam, sz).userFont ||
                    !fontChoiceIdx(fam, sz).sysFont;
      if (hourGlassOn) _DtTurnOnHourGlass(style.fontDialog);
      if (!fontChoiceIdx(fam, sz).userFont) GetUserFontResource(idx);
      if (!fontChoiceIdx(fam, sz).sysFont)  GetSysFontResource(idx);
      if (hourGlassOn) _DtTurnOffHourGlass(style.fontDialog);

      /* Update preview */
      n = 0;
      XtSetArg(args[n], XmNfontList, fontChoiceIdx(fam, sz).sysFont); n++;
      XtSetArg(args[n], XmNlabelString, CMPSTR(SYSTEM_MSG)); n++;
      XtSetValues(font.systemLabel, args, n);

      n = 0;
      if (!font.userTextChanged) XtSetArg(args[n], XmNvalue, USER_MSG);
      n++;
      XtSetArg(args[n], XmNfontList, fontChoiceIdx(fam, sz).userFont); n++;
      XtSetValues(font.userText, args, n);
      XmTextShowPosition(font.userText, 0);
    }
    ```
  - Update `changeSampleFontCB` (lines 618-670) to use FONT_INDEX-aware logic:
    ```c
    static void changeSampleFontCB(...) {
      int pos = cb->item_position - 1;
      int fam = (font.selectedFontIndex >= 0) ? FONT_FAMILY(font.selectedFontIndex) : 0;
      int idx = FONT_INDEX(fam, pos);
      /* ... use fontChoiceIdx(fam, pos) instead of fontChoice[pos] ... */
    }
    ```

  **Must NOT do**:
  - Do not change the signature of changeSampleFontCB
  - Do not duplicate logic between changeFamilyCB and changeSampleFontCB (call shared function if possible)
  - **G5 Reference**: Same as Task 9 — USE_XFT include-guard dance preserved, no Xft/fontconfig includes added

  **Recommended Agent Profile**:
  - **Category**: `unspecified-high` — callback implementation with state coordination
  - **Skills**: `[]`

  **Parallelization**:
  - **Can Run In Parallel**: YES
  - **Parallel Group**: Wave 3
  - **Blocks**: Task 12
  - **Blocked By**: Tasks 9, 10

  **References**:
  - `cde/programs/dtstyle/Font.c:618-670` — current changeSampleFontCB
  - `cde/programs/dtstyle/Font.c:136-143` — static function declarations

  **Acceptance Criteria**:
  - [ ] `changeFamilyCB` function added with correct signature
  - [ ] `changeFamilyCB` updates `font.selectedFontIndex` correctly
  - [ ] `changeFamilyCB` updates preview systemLabel and userText
  - [ ] `changeSampleFontCB` uses FONT_FAMILY/FONT_SIZE to preserve current family
  - [ ] Both callbacks compile without warnings

  **QA Scenarios (MANDATORY)**:

  ```
  Scenario: changeFamilyCB compiles and links
    Tool: Bash (make)
    Preconditions: Font.c modified
    Steps:
      1. cd cde/programs/dtstyle
      2. make dtstyle-Font.o 2>&1 | tee /tmp/build-task11.log
      3. nm dtstyle-Font.o | grep -E "changeFamilyCB|changeSampleFontCB"
    Expected Result: Both functions exist as T symbols
    Failure Indicators: missing symbols, undefined references
    Evidence: .sisyphus/evidence/task-11-build.log

  Scenario: changeFamilyCB uses fontChoiceIdx macro
    Tool: Grep
    Preconditions: Font.c modified
    Steps:
      1. grep -A 30 "^changeFamilyCB" cde/programs/dtstyle/Font.c
      2. Verify body uses fontChoiceIdx(fam, sz) for access
      3. Verify no direct fontChoice[fam*MAX_FONT_SIZES+sz] literal access
    Expected Result: All 2D accesses use macro
    Failure Indicators: literal index access
    Evidence: .sisyphus/evidence/task-11-fontchoiceidx.txt
  ```

  **Commit**: NO (groups with Tasks 9, 10, 12-14)

- [x] 12. Update ButtonCB OK to include *FontFamily: N in fontres

  **What to do**:
  - In `cde/programs/dtstyle/Font.c`, modify the `ButtonCB()` OK case (lines 475-570):
    1. Add new variable declaration at top of OK case:
       ```c
       int selectedFamily = FONT_FAMILY(font.selectedFontIndex);
       ```
    2. In the `sprintf(fontres, ...)` (around line 546), add a new resource line:
       ```c
       sprintf(fontres,
           "*systemFont: %s\n*userFont: %s\n*FontList: %s\n*buttonFontList: %s\n"
           "*labelFontList: %s\n*textFontList: %s\n*XmText*FontList: %s\n"
           "*XmTextField*FontList: %s\n*DtEditor*textFontList: %s\n"
           "*Font: %s\n*FontSet: %s\n*FontFamily: %d\n",
           fontChoiceIdx(selectedFamily, FONT_SIZE(font.selectedFontIndex)).sysStr,
           fontChoiceIdx(selectedFamily, FONT_SIZE(font.selectedFontIndex)).userStr,
           /* ... rest unchanged ... */,
           selectedFamily);  /* NEW: family index for session restore */
       ```
    3. The `*FontFamily` resource will be read by `restoreFonts()` (Task 14) to restore family selection on next login.

  **Must NOT do**:
  - Do not change the format of any existing resource line in `fontres` (other apps depend on exact format)
  - Do not change the order of existing lines
  - Do not exceed 8192-byte `fontres` buffer (current usage ~1KB, new line adds ~20 bytes)
  - **G4 Enforcement**: Do not modify the `SmNewFontSettings()` call signature, the `_DtAddToResource()` call, or the `Protocol.c` file. The `fontres` string is opaque to dtsession — only its content changes (additive), not the protocol.
  - **G5 Reference**: Same as Task 9 — USE_XFT include-guard dance preserved

  **Recommended Agent Profile**:
  - **Category**: `unspecified-high` — resource string format with backward compat
  - **Skills**: `[]`

  **Parallelization**:
  - **Can Run In Parallel**: YES
  - **Parallel Group**: Wave 3
  - **Blocks**: Task 13 (related)
  - **Blocked By**: Tasks 9-11

  **References**:
  - `cde/programs/dtstyle/Font.c:475-570` — current OK case
  - `cde/programs/dtstyle/Font.c:546-557` — exact sprintf format
  - `cde/programs/dtstyle/Protocol.c:774-779` — SmNewFontSettings protocol (must not change)

  **Acceptance Criteria**:
  - [ ] `*FontFamily: %d\n` added at the end of fontres format string
  - [ ] `selectedFamily` computed via FONT_FAMILY macro
  - [ ] All other resource lines unchanged
  - [ ] `fontres` buffer does not overflow (current usage < 4096 bytes)
  - [ ] **G4**: `SmNewFontSettings(fontres)` call signature unchanged (still passes the same string)
  - [ ] **G4**: `_DtAddToResource(style.display, fontres)` call unchanged
  - [ ] **G4**: `cde/programs/dtstyle/Protocol.c` NOT modified (diff against HEAD is empty)

  **QA Scenarios (MANDATORY)**:

  ```
  Scenario: fontres format adds FontFamily line
    Tool: Grep
    Preconditions: Font.c modified
    Steps:
      1. grep -A 1 'sprintf(fontres' cde/programs/dtstyle/Font.c
      2. Verify format string ends with "*FontFamily: %d\\n"
      3. Verify selectedFamily is the last argument
    Expected Result: New line present, last argument is selectedFamily
    Failure Indicators: missing *FontFamily line, wrong order
    Evidence: .sisyphus/evidence/task-12-fontres-format.txt

  Scenario: Buffer size check
    Tool: Bash (static analysis)
    Preconditions: Font.c modified
    Steps:
      1. grep "char fontres\[8192\]" cde/programs/dtstyle/Font.c
      2. Calculate max output: 11 resource lines × ~200 chars = ~2200 bytes < 8192
    Expected Result: Buffer safe (well below 8192)
    Failure Indicators: buffer overflow risk
    Evidence: .sisyphus/evidence/task-12-buffer-safe.txt

  Scenario: [G4] SmNewFontSettings protocol not modified
    Tool: Bash (git diff + grep)
    Preconditions: Task 12 implementation complete
    Steps:
      1. git diff cde/programs/dtstyle/Protocol.c | head
      2. Verify diff is empty (no changes to Protocol.c)
      3. grep -n "SmNewFontSettings" cde/programs/dtstyle/Font.c
      4. Verify the call signature is still: SmNewFontSettings(fontres);  (single arg, no struct changes)
      5. grep -n "_DtAddToResource" cde/programs/dtstyle/Font.c
      6. Verify still calls _DtAddToResource(style.display, fontres);  (unchanged)
    Expected Result: Protocol.c untouched, call signatures identical to before
    Failure Indicators: Protocol.c modified, signature changed, new args added
    Evidence: .sisyphus/evidence/task-12-g4-protocol.txt

  Scenario: [G4] fontres string remains valid X resource format
    Tool: Bash (xrdb parse test)
    Preconditions: Build complete
    Steps:
      1. cat > /tmp/test_fontres.txt <<'EOF'
         *systemFont: -dt-interface system-medium-r-normal-m*-*-*-*-*-*-*-*-*:
         *userFont: -dt-interface user-medium-r-normal-m*-*-*-*-*-*-*-*-*:
         *FontList: -dt-interface system-medium-r-normal-m*-*-*-*-*-*-*-*-*:
         *buttonFontList: -dt-interface system-medium-r-normal-m*-*-*-*-*-*-*-*-*:
         *labelFontList: -dt-interface system-medium-r-normal-m*-*-*-*-*-*-*-*-*:
         *textFontList: -dt-interface user-medium-r-normal-m*-*-*-*-*-*-*-*-*:
         *XmText*FontList: -dt-interface user-medium-r-normal-m*-*-*-*-*-*-*-*-*:
         *XmTextField*FontList: -dt-interface user-medium-r-normal-m*-*-*-*-*-*-*-*-*:
         *DtEditor*textFontList: -dt-interface user-medium-r-normal-m*-*-*-*-*-*-*-*-*:
         *Font: -dt-interface user-medium-r-normal-m*-*-*-*-*-*-*-*-*
         *FontSet: -dt-interface user-medium-r-normal-m*-*-*-*-*-*-*-*-*
         *FontFamily: 1
         EOF
      2. xrdb -merge /tmp/test_fontres.txt && echo "xrdb OK" || echo "xrdb FAIL"
    Expected Result: xrdb parses the resource string without errors
    Failure Indicators: xrdb syntax error, unrecognized resource format
    Evidence: .sisyphus/evidence/task-12-g4-xrdb-valid.txt
  ```

  **Commit**: NO (groups with Tasks 9-11, 13, 14)

- [x] 13. Update ButtonCB CANCEL to revert both family and size

  **What to do**:
  - In `cde/programs/dtstyle/Font.c`, modify the `ButtonCB()` CANCEL case (lines 577-602):
    1. Add separate tracking of original family and size:
       ```c
       int originalFamily = FONT_FAMILY(font.originalFontIndex);
       int originalSize = FONT_SIZE(font.originalFontIndex);
       ```
    2. In the cancel handler, restore both family list and size list:
       ```c
       case CANCEL_BUTTON:
         XtUnmanageChild(style.fontDialog);
         if (font.originalFontIndex >= 0) {
           /* Revert size list selection */
           XmListSelectPos(font.sizeList, originalSize + 1, True);
           /* Revert family list selection (if family UI is visible) */
           if (style.xrdb.numFamilies > 1) {
             XmListSelectPos(font.familyList, originalFamily + 1, True);
           }
         } else {
           /* ... existing reset logic ... */
         }
         break;
       ```

  **Must NOT do**:
  - Do not change the OK button logic
  - Do not change the existing reset behavior when originalFontIndex < 0
  - **G5 Reference**: Same as Task 9 — USE_XFT include-guard dance preserved

  **Recommended Agent Profile**:
  - **Category**: `quick` — 5-line modification
  - **Skills**: `[]`

  **Parallelization**:
  - **Can Run In Parallel**: YES
  - **Parallel Group**: Wave 3
  - **Blocks**: None
  - **Blocked By**: Task 11

  **References**:
  - `cde/programs/dtstyle/Font.c:577-602` — current CANCEL case

  **Acceptance Criteria**:
  - [ ] `originalFamily` and `originalSize` computed via FONT_FAMILY/FONT_SIZE
  - [ ] size list reverts to originalSize + 1 (XmListSelectPos uses 1-based)
  - [ ] family list reverts to originalFamily + 1 (only when numFamilies > 1)
  - [ ] Existing reset behavior for originalFontIndex < 0 preserved

  **QA Scenarios (MANDATORY)**:

  ```
  Scenario: CANCEL logic compiles
    Tool: Bash (make)
    Preconditions: Font.c modified
    Steps:
      1. cd cde/programs/dtstyle
      2. make dtstyle-Font.o 2>&1 | grep -E "error|warning"
    Expected Result: 0 errors, 0 warnings
    Failure Indicators: errors
    Evidence: .sisyphus/evidence/task-13-build.log

  Scenario: Both family and size reverted
    Tool: Grep
    Preconditions: Font.c modified
    Steps:
      1. grep -A 20 "case CANCEL_BUTTON" cde/programs/dtstyle/Font.c
      2. Verify both XmListSelectPos calls (sizeList and familyList)
    Expected Result: 2 XmListSelectPos calls
    Failure Indicators: missing family list revert
    Evidence: .sisyphus/evidence/task-13-cancel-revert.txt
  ```

  **Commit**: NO (groups with Tasks 9-12, 14)

- [x] 14. Update saveFonts() and restoreFonts() for family index

  **What to do**:
  - In `cde/programs/dtstyle/Font.c`, modify `restoreFonts()` (lines 694-726):
    1. Add reading of familyIndex resource (after x, y reads, before ismapped):
       ```c
       int restoredFamily = 0;  /* default to first family */
       xrm_name[1] = XrmStringToQuark("familyIndex");
       if (XrmQGetResource(db, xrm_name, xrm_name, &rep_type, &value)) {
         restoredFamily = atoi((char *)value.addr);
         /* Clamp to valid range */
         if (restoredFamily < 0) restoredFamily = 0;
         if (restoredFamily >= style.xrdb.numFamilies) 
           restoredFamily = style.xrdb.numFamilies - 1;
         if (restoredFamily < 0) restoredFamily = 0;  /* safety for numFamilies==0 */
       }
       style.xrdb.restoredFamilyIndex = restoredFamily;  /* temporary storage */
       ```
    2. Modify `saveFonts()` (lines 740-775) to also save familyIndex:
       ```c
       int curFamily = (font.selectedFontIndex >= 0) ? 
                       FONT_FAMILY(font.selectedFontIndex) : 0;
       snprintf(bufr, sizeof(style.tmpBigStr), "*Fonts.familyIndex: %d\n", curFamily);
       WRITE_STR2FD(fd, bufr);
       ```
  3. **In `CreateFontDlg()`**, use the restored family index when computing initial selection (modify the search loop from Task 10 to prefer the restored family)
  4. Add temporary field to ApplicationData or Style: `int restoredFamilyIndex`

  **Must NOT do**:
  - Do not change the existing ismapped, x, y save/restore logic
  - Do not break the WRITING of *Fonts.familyIndex when numFamilies changes between sessions (clamp logic)
  - **G5 Reference**: Same as Task 9 — USE_XFT include-guard dance preserved

  **Recommended Agent Profile**:
  - **Category**: `unspecified-high` — session persistence with edge cases
  - **Skills**: `[]`

  **Parallelization**:
  - **Can Run In Parallel**: YES
  - **Parallel Group**: Wave 3
  - **Blocks**: None
  - **Blocked By**: Tasks 1-3, 11-13

  **References**:
  - `cde/programs/dtstyle/Font.c:694-726` — current restoreFonts
  - `cde/programs/dtstyle/Font.c:740-775` — current saveFonts
  - `cde/programs/dtstyle/Main.h:159-206` — Style struct (may need new field)

  **Acceptance Criteria**:
  - [ ] `restoreFonts` reads `*Fonts.familyIndex` and clamps to [0, numFamilies-1]
  - [ ] `saveFonts` writes `*Fonts.familyIndex: N\n`
  - [ ] `CreateFontDlg` uses restored family index for initial selection
  - [ ] Edge case: numFamilies=0 → restoredFamily=0
  - [ ] Edge case: family index >= numFamilies → clamped

  **QA Scenarios (MANDATORY)**:

  ```
  Scenario: Session save/restore round-trip
    Tool: Bash (make + static analysis)
    Preconditions: Font.c modified
    Steps:
      1. cd cde/programs/dtstyle
      2. make 2>&1 | grep -E "error|warning"
      3. Verify saveFonts writes *Fonts.familyIndex
      4. Verify restoreFonts reads *Fonts.familyIndex
    Expected Result: 0 errors, 0 warnings
    Failure Indicators: missing resource read/write
    Evidence: .sisyphus/evidence/task-14-build.log

  Scenario: Clamping logic
    Tool: Grep
    Preconditions: Font.c modified
    Steps:
      1. grep -B 1 -A 5 "restoredFamily" cde/programs/dtstyle/Font.c
      2. Verify: lower bound clamp to 0, upper bound clamp to numFamilies-1
    Expected Result: Clamping present
    Failure Indicators: missing clamp, wrong direction
    Evidence: .sisyphus/evidence/task-14-clamping.txt
  ```

  **Commit**: YES
  - Message: `feat(dtstyle): add family selection UI to Font dialog`
  - Files: `cde/programs/dtstyle/Font.c`
  - Pre-commit: `cd cde/programs/dtstyle && make`

---

- [x] 15. Update dtstyle.msg (add messages 5-25, 5-26)

  **What to do**:
  - In `cde/programs/dtstyle/dtstyle.msg`, add 2 new messages to set 5 (after the existing 5-22, 5-23, 5-24 entries around line 226):
    ```
    $ _DtMessage 25 is the label of the titlebox containing the list of font families
    25 Family

    $ _DtMessage 26 is a label used for accessibility/tooltip of the font family list
    26 Font Family
    ```
  - The message numbers (25, 26) must be unique within set 5. Verify no conflicts with existing entries.

  **Must NOT do**:
  - Do not change existing message numbers (1-24 in set 5)
  - Do not modify the message format
  - Do not use special characters that would break gencat

  **Recommended Agent Profile**:
  - **Category**: `quick` — message catalog append
  - **Skills**: `[]`

  **Parallelization**:
  - **Can Run In Parallel**: YES
  - **Parallel Group**: Wave 4
  - **Blocks**: None
  - **Blocked By**: None (independent of code changes, but logically follows UI work)

  **References**:
  - `cde/programs/dtstyle/dtstyle.msg:215-226` — existing set 5 entries
  - `cde/programs/dtstyle/Font.c:88-96` — GETMESSAGE calls in Font.c

  **Acceptance Criteria**:
  - [ ] Message 5-25 ("Family") added
  - [ ] Message 5-26 ("Font Family") added
  - [ ] Both messages in set 5
  - [ ] gencat succeeds: `gencat dtstyle.cat dtstyle.msg`

  **QA Scenarios (MANDATORY)**:

  ```
  Scenario: Message catalog compiles
    Tool: Bash (gencat)
    Preconditions: dtstyle.msg modified
    Steps:
      1. cd cde/programs/dtstyle
      2. gencat dtstyle.cat dtstyle.msg 2>&1 | tee /tmp/gencat.log
    Expected Result: No errors, dtstyle.cat updated
    Failure Indicators: gencat errors, invalid syntax
    Evidence: .sisyphus/evidence/task-15-gencat.log

  Scenario: Message numbers unique
    Tool: Bash (grep)
    Preconditions: dtstyle.msg modified
    Steps:
      1. grep -E "^[0-9]+ " cde/programs/dtstyle/dtstyle.msg | awk '{print $1}' | sort | uniq -d
    Expected Result: Empty (no duplicates)
    Failure Indicators: duplicate message numbers
    Evidence: .sisyphus/evidence/task-15-unique.txt
  ```

  **Commit**: NO (groups with Task 16-19)

- [x] 16. Update Dtstyle.src + Dtstyle (C locale app-defaults)

  **What to do**:
  - In `cde/programs/dtstyle/Dtstyle.src`, add new resource lines in the "Font Dialog" section (after line 50, before the "Audio Dialog" section):
    ```
    !### NEW: Font Family Selection
    !# Number of font families available in the Style Manager Font dialog
    Dtstyle*NumFontFamilies: %|nls-7-2^NumFontFamilies|

    !# Family 0: System (alias to legacy SystemFont/UserFont 1-7)
    Dtstyle*FontFamily0: %|nls-8-system^FontFamily0|
    Dtstyle*FontFamily0Label: %|nls-9-System^FontFamily0Label|
    Dtstyle*FontFamily0SystemFont0: -dt-interface system-medium-r-normal-xxs*-*-*-*-*-*-*-*-*:
    Dtstyle*FontFamily0SystemFont1: -dt-interface system-medium-r-normal-xs*-*-*-*-*-*-*-*-*:
    Dtstyle*FontFamily0SystemFont2: -dt-interface system-medium-r-normal-s*-*-*-*-*-*-*-*-*:
    Dtstyle*FontFamily0SystemFont3: -dt-interface system-medium-r-normal-m*-*-*-*-*-*-*-*-*:
    Dtstyle*FontFamily0SystemFont4: -dt-interface system-medium-r-normal-l*-*-*-*-*-*-*-*-*:
    Dtstyle*FontFamily0SystemFont5: -dt-interface system-medium-r-normal-xl*-*-*-*-*-*-*-*-*:
    Dtstyle*FontFamily0SystemFont6: -dt-interface system-medium-r-normal-xxl*-*-*-*-*-*-*-*-*:
    Dtstyle*FontFamily0UserFont0: -dt-interface user-medium-r-normal-xxs*-*-*-*-*-*-*-*-*:
    Dtstyle*FontFamily0UserFont1: -dt-interface user-medium-r-normal-xs*-*-*-*-*-*-*-*-*:
    Dtstyle*FontFamily0UserFont2: -dt-interface user-medium-r-normal-s*-*-*-*-*-*-*-*-*:
    Dtstyle*FontFamily0UserFont3: -dt-interface user-medium-r-normal-m*-*-*-*-*-*-*-*-*:
    Dtstyle*FontFamily0UserFont4: -dt-interface user-medium-r-normal-l*-*-*-*-*-*-*-*-*:
    Dtstyle*FontFamily0UserFont5: -dt-interface user-medium-r-normal-xl*-*-*-*-*-*-*-*-*:
    Dtstyle*FontFamily0UserFont6: -dt-interface user-medium-r-normal-xxl*-*-*-*-*-*-*-*-*:

    !# Family 1: User (different family, same 7 sizes)
    Dtstyle*FontFamily1: %|nls-10-user^FontFamily1|
    Dtstyle*FontFamily1Label: %|nls-11-User^FontFamily1Label|
    Dtstyle*FontFamily1SystemFont0: -adobe-courier-medium-r-normal--*-*-*-*-*-*-*-*-*:
    Dtstyle*FontFamily1SystemFont1: -adobe-courier-medium-r-normal--*-*-*-*-*-*-*-*-*:
    ... (7 entries for SystemFont and 7 for UserFont, similar pattern)
    ```
  - **CRITICAL**: The first family's SystemFont0-6 / UserFont0-6 must have IDENTICAL values to the existing `SystemFont1-7` / `UserFont1-7` to maintain backward compatibility. If user has old app-defaults with only `SystemFont1-7`, the defaults in Resource.c will fill in Family 0 with the same values.
  - Also update the built `cde/programs/dtstyle/Dtstyle` file with the same content (this is the tradcpp output).

  **Must NOT do**:
  - Do not change existing `Dtstyle*SystemFont1-7` / `UserFont1-7` resource lines
  - Do not change the order of existing sections
  - Do not use 8+ families (MAX_FONT_FAMILIES=8 means families 0-7, but only define 2 in defaults)

  **Recommended Agent Profile**:
  - **Category**: `unspecified-high` — resource file with NLS markers
  - **Skills**: `[]`

  **Parallelization**:
  - **Can Run In Parallel**: YES
  - **Parallel Group**: Wave 4
  - **Blocks**: Task 20 (build verification)
  - **Blocked By**: Task 15 (needs nls message numbers)

  **References**:
  - `cde/programs/dtstyle/Dtstyle.src:46-50` — current Font Dialog section
  - `cde/programs/dtstyle/Dtstyle:41-43` — built app-defaults
  - `cde/programs/localized/C/app-defaults/Dtstyle:41-55` — actual resource values (use these for defaults)

  **Acceptance Criteria**:
  - [ ] `NumFontFamilies: 2` resource added
  - [ ] `FontFamily0SystemFont0-6` defined with exact match to SystemFont1-7 defaults
  - [ ] `FontFamily0UserFont0-6` defined with exact match to UserFont1-7 defaults
  - [ ] `FontFamily1` System/User fonts defined (7 sizes each)
  - [ ] FontFamily0 = system, FontFamily1 = user (or similar sensible default)
  - [ ] nls markers `%|nls-N-|` used for translatable strings

  **QA Scenarios (MANDATORY)**:

  ```
  Scenario: Family 0 defaults match legacy SystemFont1-7
    Tool: Bash (diff)
    Preconditions: Dtstyle.src modified
    Steps:
      1. Extract SystemFont1-7 default values from Dtstyle.src (lines 47-50)
      2. Extract FontFamily0SystemFont0-6 default values from new lines
      3. diff them (after normalizing index labels)
    Expected Result: Values are identical
    Failure Indicators: any mismatch
    Evidence: .sisyphus/evidence/task-16-family0-match.txt

  Scenario: All nls markers have valid message numbers
    Tool: Bash (grep)
    Preconditions: Dtstyle.src modified
    Steps:
      1. grep -oE "%\|nls-[0-9]+-" cde/programs/dtstyle/Dtstyle.src
      2. Verify each number is in 1-26 range (or however many messages exist in dtstyle.msg set 1)
    Expected Result: All markers in valid range
    Failure Indicators: markers with numbers > available messages
    Evidence: .sisyphus/evidence/task-16-nls-markers.txt
  ```

  **Commit**: NO (groups with Task 17-19)

- [x] 17. Update 7 other locale Dtstyle files (de_DE, es_ES, fr_FR, it_IT, ja_JP, zh_CN, zh_TW) — ko_KR and sv_SE are translation-only (no .app-defaults file)

  **What to do**:
  - For each of the 7 locale directories that have an actual `app-defaults/Dtstyle` file, update it:
    ```bash
    LOCALES=(de_DE.UTF-8 es_ES.UTF-8 fr_FR.UTF-8 it_IT.UTF-8 
             ja_JP.UTF-8 zh_CN.UTF-8 zh_TW.UTF-8)
    # Note: ko_KR.UTF-8 and sv_SE.UTF-8 only have .tmsg files, NO app-defaults/Dtstyle
    ```
  - Verify each locale file exists BEFORE attempting to update it (do not assume):
    ```bash
    for loc in "${LOCALES[@]}"; do
      test -f "cde/programs/localized/$loc/app-defaults/Dtstyle" || { echo "MISSING: $loc"; exit 1; }
    done
    ```
  - For each locale, add the same NumFontFamilies, FontFamily0SystemFont/UserFont, FontFamily1SystemFont/UserFont resources as in C locale (Task 16), but use the locale's own font resources if applicable.
  - **CRITICAL for CJK locales (ja_JP, zh_CN, zh_TW)**: These locales have locale-specific font requirements. The Family 0 defaults should use the same fonts as the existing `SystemFont1-7` / `UserFont1-7` in those locale files.
  - **Strategy**: For each locale, copy the existing SystemFont1-7 / UserFont1-7 values as Family 0 defaults. For Family 1, use a fallback or locale-specific alternative.

  **Must NOT do**:
  - Do not change existing `SystemFont1-7` / `UserFont1-7` values in any locale
  - Do not translate the resource values (they are font names, not user-facing strings)
  - Do not attempt to update ko_KR or sv_SE (they don't have app-defaults files)
  - Do not create new locale files; only modify existing ones

  **Recommended Agent Profile**:
  - **Category**: `unspecified-low` — mechanical file updates with locale-specific defaults
  - **Skills**: `[]`

  **Parallelization**:
  - **Can Run In Parallel**: YES (per-locale can run in parallel)
  - **Parallel Group**: Wave 4
  - **Blocks**: Task 20
  - **Blocked By**: Task 16 (template from C locale)

  **References**:
  - `cde/programs/localized/C/app-defaults/Dtstyle:41-55` — C locale template
  - `cde/programs/localized/de_DE.UTF-8/app-defaults/Dtstyle:41-55` — German locale
  - `cde/programs/localized/ja_JP.UTF-8/app-defaults/Dtstyle:41-55` — Japanese locale
  - (similar for other 5 locales: es_ES, fr_FR, it_IT, zh_CN, zh_TW)

  **Acceptance Criteria**:
  - [ ] All 7 locale Dtstyle files updated (de_DE, es_ES, fr_FR, it_IT, ja_JP, zh_CN, zh_TW)
  - [ ] Each file has NumFontFamilies: 2 (or appropriate value)
  - [ ] Each file has FontFamily0SystemFont0-6 matching locale's SystemFont1-7
  - [ ] Each file has FontFamily0UserFont0-6 matching locale's UserFont1-7
  - [ ] Each file has FontFamily1 entries (Family 1 may be a fallback)
  - [ ] Pre-check confirms all 7 files exist before editing

  **QA Scenarios (MANDATORY)**:

  ```
  Scenario: All 8 Dtstyle files have new family resources (C + 7 locales)
    Tool: Bash (find + grep)
    Preconditions: All locale files updated
    Steps:
      1. for loc in C de_DE.UTF-8 es_ES.UTF-8 fr_FR.UTF-8 it_IT.UTF-8 ja_JP.UTF-8 zh_CN.UTF-8 zh_TW.UTF-8; do
           grep -l "NumFontFamilies" cde/programs/localized/$loc/app-defaults/Dtstyle
         done
      2. Count results - should be 8 (C + 7 others)
    Expected Result: 8 files contain NumFontFamilies
    Failure Indicators: missing locale files, count != 8
    Evidence: .sisyphus/evidence/task-17-locale-coverage.txt

  Scenario: Pre-check verifies all 7 locale files exist
    Tool: Bash (test -f)
    Preconditions: Before editing
    Steps:
      1. for loc in de_DE.UTF-8 es_ES.UTF-8 fr_FR.UTF-8 it_IT.UTF-8 ja_JP.UTF-8 zh_CN.UTF-8 zh_TW.UTF-8; do
           test -f "cde/programs/localized/$loc/app-defaults/Dtstyle" && echo "OK $loc" || echo "MISSING $loc"
         done
    Expected Result: All 7 "OK"
    Failure Indicators: any "MISSING"
    Evidence: .sisyphus/evidence/task-17-precheck.txt

  Scenario: Family 0 matches SystemFont1-7 per locale
    Tool: Bash (script)
    Preconditions: All locale files updated
    Steps:
      1. For each of the 8 locales, extract SystemFont1 default and FontFamily0SystemFont0 default
      2. Verify they are identical strings
    Expected Result: 8/8 match
    Failure Indicators: any mismatch
    Evidence: .sisyphus/evidence/task-17-per-locale-match.txt
  ```

  **Commit**: YES
  - Message: `feat(dtstyle): add font family resources to all 8 locale app-defaults`
  - Files: 7 locale Dtstyle files
  - Pre-commit: `cd cde/programs/localized && for f in C de_DE.UTF-8 es_ES.UTF-8 fr_FR.UTF-8 it_IT.UTF-8 ja_JP.UTF-8 zh_CN.UTF-8 zh_TW.UTF-8; do echo "== $f =="; grep -c "FontFamily" "cde/programs/localized/$f/app-defaults/Dtstyle"; done`

- [x] 18. Update Dtstyle.tmsg files for all locales (if applicable)

  **What to do**:
  - The `.tmsg` files in `cde/programs/localized/*/app-defaults/Dtstyle.tmsg` are translation catalogs. If the new family resource values need translation (e.g., "System" / "User" labels), update them. If the labels are technical font names (like "system", "user"), they may not need translation.
  - **Initial scope**: Do NOT add new translations. The defaults in Dtstyle (C locale) and other locales already provide English labels. Translations can be added in a follow-up.
  - **Documentation update only**: Add a note to each `.tmsg` file (or README) explaining that the new FontFamily resources do not require new translations initially.

  **Must NOT do**:
  - Do not break existing translations in .tmsg files
  - Do not add new translatable strings without proper message numbering

  **Recommended Agent Profile**:
  - **Category**: `unspecified-low` — documentation-only updates
  - **Skills**: `[]`

  **Parallelization**:
  - **Can Run In Parallel**: YES
  - **Parallel Group**: Wave 4
  - **Blocks**: None
  - **Blocked By**: None

  **References**:
  - `cde/programs/localized/C/app-defaults/Dtstyle.tmsg` — C locale tmsg
  - `cde/programs/localized/ja_JP.UTF-8/app-defaults/Dtstyle.tmsg` — Japanese tmsg

  **Acceptance Criteria**:
  - [ ] All 10 locale .tmsg files compile without errors
  - [ ] No existing translations removed
  - [ ] Note added to each .tmsg (or README) about family resources

  **QA Scenarios (MANDATORY)**:

  ```
  Scenario: tmsg files unchanged or note added
    Tool: Bash (find + diff)
    Preconditions: Wave 4 partially done
    Steps:
      1. for loc in C de_DE.UTF-8 es_ES.UTF-8 fr_FR.UTF-8 it_IT.UTF-8 ja_JP.UTF-8 ko_KR.UTF-8 sv_SE.UTF-8 zh_CN.UTF-8 zh_TW.UTF-8; do
           diff -q cde/programs/localized/$loc/app-defaults/Dtstyle.tmsg /dev/null
         done
    Expected Result: Either all files unchanged OR notes added consistently
    Failure Indicators: tmsg files corrupted, translations lost
    Evidence: .sisyphus/evidence/task-18-tmsg-check.txt
  ```

  **Commit**: NO

- [x] 19. Update nlsREADME.txt and dtstyle.man documentation

  **What to do**:
  - In `cde/programs/dtstyle/nlsREADME.txt` (around lines 51-102 which document font resources), add documentation for the new resources:
    - `NumFontFamilies`
    - `FontFamily0` through `FontFamily7`
    - `FontFamily0Label` through `FontFamily7Label`
    - `FontFamily[N]SystemFont[0-6]`
    - `FontFamily[N]UserFont[0-6]`
  - In `cde/programs/dtstyle/dtstyle.man`, update the resources section (around line 270 where `numFonts` is documented) with the new resources and their semantics.

  **Must NOT do**:
  - Do not change existing documentation
  - Do not use technical jargon without explanation

  **Recommended Agent Profile**:
  - **Category**: `writing` — documentation update
  - **Skills**: `[]`

  **Parallelization**:
  - **Can Run In Parallel**: YES
  - **Parallel Group**: Wave 4
  - **Blocks**: None
  - **Blocked By**: None

  **References**:
  - `cde/programs/dtstyle/nlsREADME.txt:51-102` — font resource documentation
  - `cde/programs/dtstyle/dtstyle.man:270` — numFonts documentation

  **Acceptance Criteria**:
  - [ ] nlsREADME.txt documents NumFontFamilies and FontFamily[N] resources
  - [ ] dtstyle.man documents new resources
  - [ ] No existing documentation removed

  **QA Scenarios (MANDATORY)**:

  ```
  Scenario: Documentation contains new resource names
    Tool: Bash (grep)
    Preconditions: Docs updated
    Steps:
      1. grep -E "NumFontFamilies|FontFamily[0-9]" cde/programs/dtstyle/nlsREADME.txt
      2. grep -E "NumFontFamilies|FontFamily[0-9]" cde/programs/dtstyle/dtstyle.man
    Expected Result: Both files mention new resources
    Failure Indicators: missing documentation
    Evidence: .sisyphus/evidence/task-19-docs.txt
  ```

  **Commit**: NO

- [x] 20. Full build verification + static analysis + manual simulation

  **What to do**:
  - **Build verification**:
    ```bash
    cd cde/programs/dtstyle && make clean && make 2>&1 | tee /tmp/full-build.log
    grep -cE "error|warning" /tmp/full-build.log
    ```
  - **Static analysis**:
    - All XtOffset references compile and link
    - All `fontChoiceIdx(fam, sz)` calls are within bounds
    - All `XtCreate*` calls have matching `XtManageChild`/`XtUnmanageChild`
    - All XmStringFree calls for allocated XmStrings
  - **Manual simulation** (code review):
    - Trace CreateFontDlg → changeFamilyCB → ButtonCB OK → saveFonts path
    - Verify: numFamilies=0 handled gracefully (default to 1)
    - Verify: family index out of bounds clamped in restoreFonts
    - Verify: Cancel button reverts both family and size

  **Must NOT do**:
  - Do not skip any verification step

  **Recommended Agent Profile**:
  - **Category**: `unspecified-high` — full build + static analysis + code review
  - **Skills**: `[]`

  **Parallelization**:
  - **Can Run In Parallel**: NO (this is the final integration test)
  - **Parallel Group**: Wave 4
  - **Blocks**: Final Verification Wave
  - **Blocked By**: Tasks 1-19

  **References**:
  - All tasks 1-19 outputs

  **Acceptance Criteria**:
  - [ ] `make clean && make` succeeds with 0 errors, 0 warnings
  - [ ] `gencat` succeeds for all 10 locale message catalogs
  - [ ] All XtOffset references resolve
  - [ ] All widget hierarchies have correct parent/child
  - [ ] All malloc/XtMalloc have matching free/XtFree
  - [ ] All 10 locale Dtstyle files contain NumFontFamilies

  **QA Scenarios (MANDATORY)**:

  ```
  Scenario: Full build with zero warnings
    Tool: Bash (make)
    Preconditions: All previous tasks complete
    Steps:
      1. cd cde/programs/dtstyle
      2. make clean
      3. make 2>&1 | tee /tmp/full-build.log
      4. grep -cE "error|warning" /tmp/full-build.log
    Expected Result: 0 (zero)
    Failure Indicators: any error or warning
    Evidence: .sisyphus/evidence/task-20-build.log

  Scenario: All locale message catalogs compile
    Tool: Bash (find + gencat)
    Preconditions: All .msg and .tmsg files updated
    Steps:
      1. find cde/programs -name "*.msg" -path "*dtstyle*" -exec gencat {}.cat {} \;
      2. find cde/programs/localized -name "*.tmsg" -path "*Dtstyle*" -exec sh -c '...gencat or lint...' \;
    Expected Result: All gencat succeed
    Failure Indicators: gencat errors
    Evidence: .sisyphus/evidence/task-20-msg-catalogs.log

  Scenario: Static analysis - no XtOffset errors
    Tool: Bash (cpp + nm)
    Preconditions: Build complete
    Steps:
      1. nm dtstyle-Resource.o | grep -E "GetSysFontResource|GetUserFontResource"
      2. objdump -d dtstyle-Resource.o | grep -c "XtOffset"
      3. Verify all references are valid
    Expected Result: All XtOffset references resolve
    Failure Indicators: undefined references
    Evidence: .sisyphus/evidence/task-20-static-analysis.txt

  Scenario: Manual simulation - dialog lifecycle
    Tool: Code review (read + analyze)
    Preconditions: Code complete
    Steps:
      1. Read Font.c lines 172-429 (CreateFontDlg)
      2. Read Font.c lines 619-690 (changeFamilyCB + changeSampleFontCB)
      3. Read Font.c lines 475-611 (ButtonCB OK + CANCEL)
      4. Read Font.c lines 694-775 (saveFonts + restoreFonts)
      5. Trace: dialog opens → user clicks Family 1 → preview updates → user clicks OK → resources written → session saved
    Expected Result: All state transitions are correct
    Failure Indicators: state machine bugs, null pointer dereferences
    Evidence: .sisyphus/evidence/task-20-lifecycle-review.md
  ```

  **Commit**: NO (will be committed in final verification wave)

---

## Final Verification Wave (MANDATORY)

- [x] F1. **Plan Compliance Audit** — `oracle`
  Verify all 20 tasks completed. Verify no Must-NOT-Have violations. Check evidence files exist.

- [x] F2. **Code Quality Review** — `unspecified-high`
  Run `make clean && make` in dtstyle. Check for warnings, unused variables, XtOffset correctness via objdump.

- [x] F3. **Manual Simulation Review** — `unspecified-high`
  Trace through dialog lifecycle: CreateFontDlg → changeSampleFontCB → ButtonCB OK → saveFonts. Verify state machine.

- [x] F4. **Scope Fidelity Check** — `deep`
  For each task verify: 1) implementation matches spec, 2) no cross-task contamination, 3) no unaccounted changes.

---

## Commit Strategy

- **Commit 1**: Wave 1 - "feat(dtstyle): add MAX_FONT_FAMILIES constant, extend Fontset and ApplicationData structs"
- **Commit 2**: Wave 2 - "feat(dtstyle): add font family Xresources and GetFamilyResources()"
- **Commit 3**: Wave 3 - "feat(dtstyle): add family selection UI to Font dialog"
- **Commit 4**: Wave 4 - "feat(dtstyle): add family resources to all locale app-defaults + i18n"

---

## Success Criteria

### Verification Commands
```bash
# Build verification
cd cde/programs/dtstyle && make clean && make 2>&1 | tee /tmp/build.log
grep -i "error\|warning" /tmp/build.log  # Expected: empty

# Message catalog
gencat dtstyle.cat dtstyle.msg  # Expected: no errors

# Locale coverage (8 files: C + 7 with app-defaults)
grep -l "NumFontFamilies" cde/programs/localized/*/app-defaults/Dtstyle  # Expected: 8 files

# Backwards compat: Family 0 == SystemFont1..7
grep "FontFamily0SystemFont1:" cde/programs/localized/C/app-defaults/Dtstyle
grep "SystemFont1:" cde/programs/localized/C/app-defaults/Dtstyle
# Expected: both present, with matching values (Family0SystemFont1 = SystemFont1)
```

### Final Checklist
- [x] All "Must Have" present
- [x] All "Must NOT Have" absent
- [x] All 20 tasks completed
- [x] Build succeeds with zero warnings
- [x] All 8 locales (C + 7) have NumFontFamilies + Family0* resources
- [x] Session save/restore works (family index round-trip)
- [x] Cancel button reverts both family and size

---

## Appendix A: File Reference Map

| File | Role | Lines Changed (est) |
|------|------|---------------------|
| `cde/programs/dtstyle/Font.h` | Fontset struct | +10 |
| `cde/programs/dtstyle/Font.c` | Main UI | +180, -50 |
| `cde/programs/dtstyle/Main.h` | ApplicationData struct | +30, -5 |
| `cde/programs/dtstyle/Resource.c` | Xresources | +150, -20 |
| `cde/programs/dtstyle/Resource.h` | Function decls | +5 |
| `cde/programs/dtstyle/Dtstyle.src` | C locale app-defaults source | +20 |
| `cde/programs/dtstyle/Dtstyle` | C locale app-defaults built | +20 |
| `cde/programs/dtstyle/dtstyle.msg` | Message catalog source | +8 |
| `cde/programs/dtstyle/nlsREADME.txt` | Doc | +15 |
| `cde/programs/dtstyle/dtstyle.man` | Doc | +20 |
| **7x locale Dtstyle** (de_DE, es_ES, fr_FR, it_IT, ja_JP, zh_CN, zh_TW) | Locale app-defaults | +15 each (~105) |
| **10x locale Dtstyle.tmsg** (C + 7 above + ko_KR + sv_SE) | Locale tmsg | +10 each (~100) |
| **Total** | | **~650 lines added/changed across ~22 files** |

> **Locale file inventory note**: 8 `app-defaults/Dtstyle` files exist (C + 7). 10 `Dtstyle.tmsg` files exist (C + 7 + ko_KR + sv_SE, both translation-only). Task 17 updates the 7 non-C app-defaults files; Task 18 handles all 10 tmsg files (or, as decided, leaves them unchanged with documentation note).
