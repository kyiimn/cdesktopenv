# XDG → CDE 애플리케이션 매니저 변환 도구 (`dtxdg2appmgr`)

## TL;DR

> **Quick Summary**: GLib 기반 C 프로그램 `dtxdg2appmgr`을 추가하여, 시스템의 XDG `.desktop` 파일을 XDG Icon Theme Spec에 따라 스캔하고, CDE Application Manager 형식(.dt 액션, appmanager 그룹 엔트리, XPM 아이콘)으로 변환하여 `/var/dt/appconfig/appmanager/$DTUSERSESSION/` 에 등록.
>
> **Deliverables**:
> - `cde/programs/dtxdg2appmgr/` 새 C 프로그램 (GLib 사용)
> - `.desktop` 파서, XDG 카테고리 매퍼, XPM 아이콘 변환기, mtime 캐시
> - `configure.ac`에 `HAVE_GLIB` 매크로 + `PKG_CHECK_MODULES([GLIB])` 추가
> - `Xsession.src`에 `dtxdg2appmgr` 자동 실행 라인 추가
> - `dt.dt`의 `ReloadApps` 액션에 `dtxdg2appmgr -r` 호출 추가
> - `.dt`, `.pm` 파일 생성, 아이콘 심볼릭 링크/복사
>
> **Estimated Effort**: Medium
> **Parallel Execution**: YES - 3 waves
> **Critical Path**: Task 1 → Task 4 → Task 9

---

## Context

### Original Request
Linux 시스템에 설치된 XDG(.desktop) 애플리케이션 정보를 CDE Application Manager 구조로 변환하여 표시하는 기능을 추가.

### Interview Summary

**확정된 사용자 결정**:
- **변환 범위**: 전체 시스템 스캔 (XDG 표준 경로 모두)
- **구현 방식**: 런타임 변환 도구 (Autotools 통합 X, 독립 스크립트 X)
- **카테고리 매핑**: 원본 카테고리 유지 (XDG Categories를 그대로 보존)
- **그룹명 접두사**: **접두사 없음 + 모든 XDG 그룹에 `_XDG` 접미사 항상 적용** — 카테고리 자체가 기본 그룹명, 충돌 여부와 무관하게 접미사 추가. 결과: CDE 내장(원본) ↔ XDG 변환(접미사) 명확히 구분
- **아이콘 처리**: PNG/SVG → XPM 변환
- **구현 언어**: **C + GLib** (`GDesktopAppInfo`, `GKeyFile`, `GAppInfo`)
- **이름 충돌**: 카테고리별 그룹화 (같은 그룹 내 충돌 시 파일명 해시 접미사)
- **캐시 전략**: mtime 기반 (변경된 것만 재변환)
- **Exec 파싱**: XDG Exec 표준 완전 준수 (%f, %F, %u, %U, %d, %D, 인용부호 처리)
- **등록 위치**: `/var/dt/appconfig/appmanager/$DTUSERSESSION/<Category>_XDG` (모든 XDG 그룹에 `_XDG` 접미사, CDE 내장과 명확히 구분)
- **i18n**: XDG 다국어 필드 완전 지원 (Name[ko], Comment[ko] 등 → CDE NLS 매크로)
- **필터링**: 표준 XDG 필터링 (Type=Application, NoDisplay=false, Hidden=false, Terminal=false)
- **아이콘 크기**: 32x32 고정

**Research Findings**:
- `dtappgather`는 `cde/programs/dtsearchpath/dtappg/`에 위치, `Options.h/.C` 클래스로 CLI 옵션 관리
- Xsession에서 `dtstart_appgather="$DT_BINPATH/dtappgather &"`로 백그라운드 실행 (line 301)
- `ReloadApps` 액션이 `dtappgather -r` 호출 (dt.dt.src line 325)
- `cde/programs/types/dt.dt.src`가 모든 .dt 파일을 빌드
- CDE는 `libXpm`을 링크 (`configure.ac:621`)
- `cstring`, `DirIterator`, `CDEEnvironment` 클래스 계층이 `dtsearchpath/`에 존재
- 시스템에 사용 가능한 변환 도구: `convert` (ImageMagick), `pngtopnm`, `ppmtoxpm` (netpbm)
- 그룹명은 CDE의 기존 `dtappman.dt` `DATA_CRITERIA` 패턴(`*/appmanager/*/<Group>`)과 호환 (단, CDE 기존 그룹과 충돌 시 `_XDG` 접미사로 회피)
- **🔍 핵심 발견 1: 기존 reference 구현 `cde/contrib/desktop2dt/desktop2dt` (Isaac Dunham, 2013, MIT 라이선스)** — 154줄 셸 스크립트. `find_convert()`, `process_desktop()`, `create_appentry()` 함수가 우리 C/GLib 도구의 알고리즘 기반. **셸 아웃이 아닌 알고리즘 포팅**으로 처리
- **🔍 핵심 발견 2: CDE는 PNG를 전혀 지원하지 않음** — `cde/lib/DtHelp/Graphics.c:419-456`의 image registry는 PM/BM/GIF/JPEG/TIFF/XWD만 등록. `cde/lib/DtHelp/il/libil.la` (CDE 자체 이미징 라이브러리)도 PNG 미지원. 따라서 **ImageMagick `convert` 외부 호출이 필수**
- **🔍 핵심 발견 3: CDE의 `.pm` 파일은 `.xpm` 파일의 rename일 뿐** — `cde/programs/dticon/fileIO.c:319-330` 주석 참조. CDE 표준은 `.t.pm` (16x16), `.m.pm` (32x32), `.l.pm` (48x48) **3개 크기 모두 필요** (lookup 시 size suffix 자동 strip, `cde/programs/dtstyle/Backdrop.c:817-822`)
- **🔍 핵심 발견 4: SVG 처리는 ImageMagick의 librsvg delegate로 자동** — IM 7+는 `convert input.svg output.xpm` 직접 처리. 별도 librsvg 의존성 불필요
- **🔍 핵심 발견 5: 기존 `desktop2dt`의 `Categories=` prefix 제거 로직** (line 106-109) — `X-`, `GTK`, `Motif`, `GNOME`, `Qt` 같은 비-XDG 표준 prefix를 제거해야 함. CDE는 `=`로 시작하는 category만 인식

### Metis Review
**Identified Gaps (addressed)**:
- **Gap 0 (사용자 결정)**: 카테고리 매핑에서 `XDG_` 접두사 제거, 대신 **모든 XDG 그룹에 `_XDG` 접미사 항상 적용** — 단순한 규칙(조건 분기 없음), CDE 내장 그룹과 절대 충돌 회피, 그룹명만 봐도 XDG 변환 결과 식별 가능. CDE ↔ XDG 명확히 구분
- **Gap 1**: `glib-2.0` 의존성을 `configure.ac`에 어떻게 추가할 것인가 → `PKG_CHECK_MODULES` 사용, `AM_CONDITIONAL HAVE_GLIB2`로 분기
- **Gap 2**: SVG 아이콘 처리 → librsvg 의존성 회피, ImageMagick 7+의 rsvg delegate로 자동 처리 (외부 `convert` 사용)
- **Gap 3**: 캐시 디렉토리 권한 (`/var/dt/appconfig/cache/xdg/`) → dtappgather와 동일하게 root 권한 가정
- **Gap 4**: 기존 `dt.dt`의 NLS 매크로 (`%|nls-...-|`)와 XDG i18n 통합 → `%.dt` 파일은 `%|nls-...-|` 매크로 형식 사용, `_common.dt.tmsg` 머지 단계에서 번역
- **Gap 5**: XPM 변환의 알파 채널 손실 → `-background none`으로 1-bit mask, 16/32/48 quantize로 품질 충분
- **Gap 6**: CDE 기본 그룹(`Office`, `Internet`, `Education`, `Graphics`, `System`, `Game` 등)과 XDG 카테고리 이름 충돌 → **Gap 0의 `_XDG` 접미사 규칙으로 완전 해결** (조건부 분기 불필요)
- **Gap 7**: 기존 `contrib/desktop2dt/desktop2dt` (2013, Isaac Dunham, MIT) 셸 스크립트가 정확히 같은 작업을 수행 → 알고리즘 포팅 (셸 아웃 X, 독립 구현 X)으로 결정, license 호환 확인됨

---

## Work Objectives

### Core Objective
XDG FreeDesktop 스펙을 따르는 `.desktop` 파일을 자동으로 발견하여 CDE Application Manager의 .dt 액션 정의, appmanager 그룹 스텁, XPM 아이콘으로 변환하는 런타임 도구 `dtxdg2appmgr`를 추가.

### Concrete Deliverables
1. `cde/programs/dtxdg2appmgr/dtxdg2appmgr.c` - 메인 C 프로그램
2. `cde/programs/dtxdg2appmgr/options.c`, `options.h` - CLI 옵션
3. `cde/programs/dtxdg2appmgr/desktop_parser.c`, `.h` - GKeyFile 기반 .desktop 파서
4. `cde/programs/dtxdg2appmgr/exec_parser.c`, `.h` - XDG Exec 라인 필드 코드 처리
5. `cde/programs/dtxdg2appmgr/icon_resolver.c`, `.h` - XDG Icon Theme Spec 검색
6. `cde/programs/dtxdg2appmgr/xpm_converter.c`, `.h` - PNG → XPM 변환
7. `cde/programs/dtxdg2appmgr/dt_writer.c`, `.h` - .dt 파일 생성
8. `cde/programs/dtxdg2appmgr/cache.c`, `.h` - mtime 기반 캐시
9. `cde/programs/dtxdg2appmgr/Makefile.am`
10. `configure.ac` 수정 (GLib 검사)
11. `Xsession.src` 수정 (자동 실행)
12. `dt.dt.src` 수정 (ReloadApps에 dtxdg2appmgr -r 추가)

### Definition of Done
- `./configure && make` 성공
- `dtxdg2appmgr` 빌드, `make install` 후 `$CDE_INSTALLATION_TOP/bin/`에 설치
- 샘플 시스템에서 실행 시 `/var/dt/appconfig/appmanager/$DTUSERSESSION/Network_XDG/firefox` 같은 그룹/스텁 생성 (모든 XDG 그룹에 `_XDG` 접미사)
- `dtfile`의 Application Manager 뷰에서 XDG 앱이 정상 표시·실행

### Must Have
- GLib `GDesktopAppInfo`/`GKeyFile`를 사용한 표준 .desktop 파싱
- `XDG_DATA_DIRS` + `$HOME/.local/share` 경로 자동 스캔
- `Type=Application` + `NoDisplay=false` + `Hidden=false` 필터
- mtime 기반 증분 업데이트 (캐시: `$HOME/.dt/xdg-cache.db` 또는 `/var/dt/appconfig/xdg-cache.db`)
- **3개 크기 XPM 아이콘 생성**: `.t.pm` (16x16), `.m.pm` (32x32), `.l.pm` (48x48) — CDE 표준 컨벤션 (기존 `desktop2dt` 스크립트와 동일, `cde/programs/dtstyle/Backdrop.c:817-822` 참조)
- ImageMagick `convert` 명령으로 PNG/SVG → XPM 변환 (외부 프로세스 호출, CDE의 자체 이미지 라이브러리는 PNG 미지원이므로 필수)
- `Exec` 라인 필드 코드 완전 처리 (%f, %F, %u, %U, %d, %D, 환경 변수, 인용부호)
- 다국어 `Name[xx]`, `Comment[xx]` 처리 (현재 `$LANG`에 맞는 값 사용)
- 카테고리 → 그룹명 변환 (sanitize 후 **항상 `_XDG` 접미사** 추가, CDE 충돌 자동 회피)
- `/var/dt/appconfig/appmanager/$DTUSERSESSION/<SanitizedCategory>_XDG/<AppName>` 구조로 출력
- dtappgather와 동일한 권한 모델 (root가 실행, 일반 사용자도 본인 디렉토리에 가능)
- **기존 `contrib/desktop2dt/desktop2dt` 셸 스크립트(2013, Isaac Dunham, MIT)의 알고리즘 포팅** — `find_convert()`, `process_desktop()`, `create_appentry()` 로직을 C/GLib로 재구현 (셸 아웃 X, 독립 구현 X)

### Must NOT Have (Guardrails)
- 기존 CDE .dt 파일을 변경하거나 덮어쓰지 않음
- `/usr/dt/appconfig/appmanager/` 시스템 영역을 건드리지 않음 (사용자별 `/var/dt/...`만)
- .desktop 파일의 Exec 라인을 `sh -c`로 래핑하지 않음 (XDG 표준 준수)
- SVG 직접 파싱 안 함 (외부 `convert` 도구로 PNG 변환 후 사용)
- glib 외 추가 의존성 추가 안 함 (librsvg, libpng 등 X)
- GNOME/KDE 전용 GSettings, QSettings 등 비표준 메타데이터 사용 안 함

---

## Verification Strategy (MANDATORY)

> **ZERO HUMAN INTERVENTION** - ALL verification is agent-executed.
> CDE는 자동 테스트 인프라가 없으므로 모든 검증은 수동 + 에이전트 실행 시나리오 기반.

### Test Decision
- **Infrastructure exists**: NO (CDE는 자동 테스트 없음, AGENTS.md 명시)
- **Automated tests**: NONE (신규 도구이지만 통합 테스트 우선)
- **Framework**: none
- **Test Strategy**: Agent-Executed QA Scenarios (실제 시스템에 설치된 .desktop 사용)

### QA Policy
모든 Task는 Agent-Executed QA Scenarios를 포함해야 함. evidence는 `.sisyphus/evidence/task-{N}-{slug}.{ext}`에 저장.

- **CLI 도구 검증**: Bash (`./dtxdg2appmgr -v`, `./dtxdg2appmgr -r` 등)
- **파일 시스템 검증**: Bash (`ls`, `cat`, `file` 명령)
- **XPM 검증**: Bash (`file *.xpm` 또는 `head -3 *.xpm`로 XPM 헤더 확인)
- **.dt 검증**: Bash (`grep ACTION *.dt`로 ACTION 블록 확인)
- **dtfile 통합**: tmux 또는 Xvfb + xdotool (UI 검증, 가능시)

---

## Execution Strategy

### Parallel Execution Waves

```
Wave 1 (기반 + 의존성 + 빌드):
├── Task 1: configure.ac에 GLib 검사 추가
├── Task 2: 디렉토리 구조 + Makefile.am 골격
├── Task 3: CLI 옵션 파서 (options.c/.h)
├── Task 4: .desktop GKeyFile 파서 (desktop_parser.c/.h)
├── Task 5: XDG 데이터 경로 스캐너 (디렉토리 재귀 검색)
└── Task 6: Exec 라인 필드 코드 파서 (exec_parser.c/.h)

Wave 2 (변환 엔진 + 아이콘):
├── Task 7: 카테고리 → 그룹명 sanitization 유틸리티
├── Task 8: 아이콘 검색 (XDG Icon Theme Spec)
├── Task 9: XPM 변환기 (ImageMagick convert + netpbm fallback)
├── Task 10: .dt 파일 생성기 (dt_writer.c)
├── Task 11: mtime 캐시 (cache.c)
└── Task 12: main() 오케스트레이션

Wave 3 (통합 + Xsession + 문서):
├── Task 13: Xsession.src에 dtxdg2appgather 호출 추가
├── Task 14: dt.dt.src ReloadApps 액션에 -r 옵션 추가
├── Task 15: 사용자/시스템 .desktop 발견 통합 테스트
└── Task 16: 종합 QA 시나리오 + 문서화
```

### Dependency Matrix

| Task | Depends On | Blocks |
|------|-----------|--------|
| 1 (configure.ac) | - | 2 |
| 2 (Makefile.am) | 1 | 3,4,5,6,7,8,9,10,11,12 |
| 3 (options) | 2 | 12 |
| 4 (desktop_parser) | 2 | 12 |
| 5 (path_scanner) | 2 | 12 |
| 6 (exec_parser) | 2 | 12 |
| 7 (category_mapper) | 2 | 10 |
| 8 (icon_resolver) | 2 | 9 |
| 9 (xpm_converter) | 8 | 12 |
| 10 (dt_writer) | 4,6,7 | 12 |
| 11 (cache) | 2 | 12 |
| 12 (main) | 3,4,5,6,9,10,11 | 13,14 |
| 13 (Xsession) | 12 | - |
| 14 (ReloadApps) | 12 | - |
| 15 (테스트) | 12 | 16 |
| 16 (QA) | 15 | - |

### Agent Dispatch Summary
- **Wave 1**: `quick` (단순 파일, 명확한 인터페이스)
- **Wave 2**: `quick` (변환 로직, 외부 도구 호출)
- **Wave 3**: `unspecified-low` (통합 + 테스트)

---

## TODOs

<!-- Tasks will be appended below by Edit operations -->

- [x] 1. **configure.ac에 GLib 검사 추가**

  **What to do**:
  - `cde/configure.ac`에 `PKG_CHECK_MODULES([GLIB], [glib-2.0 >= 2.40 gio-2.0 >= 2.40], [have_glib=yes], [have_glib=no])` 추가
  - `AM_CONDITIONAL([HAVE_GLIB], [test "x$have_glib" = "xyes"])` 추가
  - `AC_SUBST([GLIB_CFLAGS])` 및 `AC_SUBST([GLIB_LIBS])` 추가
  - `AC_CONFIG_FILES`에 `programs/dtxdg2appmgr/Makefile` 추가 (HAVE_GLIB이 yes일 때만)
  - 도움말 출력에 `--disable-xdg-integration` 옵션 추가 (선택적, GLib 없을 때 빌드 제외)

  **Must NOT do**:
  - 기존 configure.ac의 다른 AC_CHECK_LIB 섹션 손상 금지
  - GLib 외 다른 의존성 (librsvg, libpng) 추가 금지

  **Recommended Agent Profile**:
  - **Category**: `quick`
    - 단순 configure.ac 수정, 명확한 인터페이스
  - **Skills**: `[]`
    - autotools 표준 패턴, 추가 도구 불필요

  **Parallelization**:
  - **Can Run In Parallel**: YES
  - **Parallel Group**: Wave 1
  - **Blocks**: Task 2
  - **Blocked By**: None

  **References**:
  - `cde/configure.ac:621` - `AC_CHECK_LIB(Xpm, XpmLibraryVersion, ...)` 패턴 참조
  - `cde/configure.ac:878, 973-1030` - AC_CONFIG_FILES에 dtxdg2appmgr/Makefile 추가 위치
  - `cde/programs/dtscreen/Makefile.am` - 단일 프로그램 Makefile.am 참조 패턴

  **Acceptance Criteria**:
  - `./autogen.sh` 성공
  - `./configure` 시 `checking for GLIB... yes` 출력
  - `./configure` 시 `checking for GLib... yes` 출력
  - `grep HAVE_GLIB configure` 시 `HAVE_GLIBCONFIG_TRUE` 패턴 확인 (조건부 빌드 활성화)

  **QA Scenarios (MANDATORY)**:
  ```
  Scenario: configure.ac가 GLib를 정확히 감지하는지 확인
    Tool: Bash
    Preconditions: glib-2.0-dev 패키지 설치됨
    Steps:
      1. `cd /home/kyiimn/Projects/cdesktopenv/cde`
      2. `grep -n "PKG_CHECK_MODULES.*GLIB" configure.ac` → 1개 이상 매치
      3. `grep -n "AM_CONDITIONAL.*HAVE_GLIB" configure.ac` → 1개 매치
      4. `grep -n "dtxdg2appmgr" configure.ac` → AC_CONFIG_FILES 항목 확인
    Expected Result: 모든 4단계 성공
    Evidence: .sisyphus/evidence/task-1-configure-glib.txt

  Scenario: GLib 없을 때 빌드 비활성화 확인
    Tool: Bash
    Preconditions: GLib 없는 환경 (또는 `PKG_CHECK_MODULES` 임시 비활성화)
    Steps:
      1. `./configure 2>&1 | grep -i "glib"` → "no" 출력 확인
    Expected Result: 빌드는 진행되지만 dtxdg2appmgr 디렉토리만 건너뜀
    Evidence: .sisyphus/evidence/task-1-configure-no-glib.txt
  ```

  **Commit**: YES
  - Message: `feat(dtxdg2appmgr): add configure.ac GLib detection`
  - Files: `configure.ac`

- [x] 2. **디렉토리 구조 + Makefile.am 골격**

  **What to do**:
  - `cde/programs/dtxdg2appmgr/` 디렉토리 생성
  - `Makefile.am` 작성:
    - `bin_PROGRAMS = dtxdg2appmgr` (조건부: HAVE_GLIB)
    - `dtxdg2appmgr_SOURCES = options.c desktop_parser.c exec_parser.c icon_resolver.c xpm_converter.c dt_writer.c cache.c path_scanner.c category_mapper.c dtxdg2appmgr.c`
    - `dtxdg2appmgr_CPPFLAGS = $(GLIB_CFLAGS) $(EXTRA_INCS)`
    - `dtxdg2appmgr_LDADD = $(GLIB_LIBS) $(EXTRA_LIBS)`
  - 각 소스 파일의 빈 스텁 생성 (함수 시그니처 + TODO 주석)
  - `README.md` 작성 (간단한 도구 설명, 빌드 방법)

  **Must NOT do**:
  - 빈 디렉토리만 있는 상태로 두지 않음 (최소한 .c 스텁 필요)
  - autogen.sh나 automake 매크로 손상 금지

  **Recommended Agent Profile**:
  - **Category**: `quick`
    - 표준 Makefile.am 패턴
  - **Skills**: `[]`

  **Parallelization**:
  - **Can Run In Parallel**: YES
  - **Parallel Group**: Wave 1
  - **Blocks**: Tasks 3,4,5,6,7,8,9,10,11,12
  - **Blocked By**: Task 1

  **References**:
  - `cde/programs/dthello/Makefile.am` - 가장 단순한 단일 프로그램 Makefile.am (참고용)
  - `cde/programs/dtaction/Makefile.am` - 단일 C 프로그램, lib 의존성
  - `cde/programs/dtsearchpath/dtappg/Makefile.am` - C++ 프로그램 Makefile.am

  **Acceptance Criteria**:
  - `cde/programs/dtxdg2appmgr/Makefile.am` 파일 존재
  - `autoreconf -fi` 후 `Makefile.in` 생성 성공
  - 스텁 파일들이 모두 컴파일 통과 (warning만, error 없음)

  **QA Scenarios (MANDATORY)**:
  ```
  Scenario: 디렉토리 구조와 Makefile.am 검증
    Tool: Bash
    Steps:
      1. `ls -la cde/programs/dtxdg2appmgr/` → 10개 이상 .c/.h 파일
      2. `cat cde/programs/dtxdg2appmgr/Makefile.am | grep SOURCES` → dtxdg2appmgr_SOURCES 라인 존재
      3. `cd cde/programs/dtxdg2appmgr && touch *.c *.h && cd ../.. && make -C cde/programs/dtxdg2appmgr 2>&1 | tail` → "Error 0" 출력
    Expected Result: 모든 스텁이 컴파일되어 dtxdg2appmgr 빈 바이너리 생성
    Evidence: .sisyphus/evidence/task-2-build-skeleton.txt
  ```

  **Commit**: YES
  - Message: `feat(dtxdg2appmgr): add directory structure and Makefile.am skeleton`
  - Files: `cde/programs/dtxdg2appmgr/Makefile.am`, `*.c`, `*.h`, `README.md`

- [x] 3. **CLI 옵션 파서 (`options.c`/`.h`)**

  **What to do**:
  - `Options` 구조체 정의:
    - `gboolean verbose` (`-v`)
    - `gboolean retain` (`-r`)
    - `gchar *output_dir` (`-o`, 기본 `/var/dt/appconfig/appmanager/$DTUSERSESSION`)
    - `gchar *icon_output_dir` (`-i`, 기본 `$CDE_INSTALLATION_TOP/appconfig/icons/C`)
    - `gchar *cache_file` (`-c`, 기본 `/var/dt/appconfig/xdg-cache.db`)
    - `gboolean force_rebuild` (`-f`, 캐시 무시)
  - `parse_options(int argc, char **argv, Options *opts)` 함수 구현 (GOption 또는 getopt_long)
  - `print_usage(const char *progname)` 함수
  - `options_free(Options *opts)` 함수 (g_free)
  - `dtappgather`의 `Options.C`와 동일한 인터페이스 패턴 따름

  **Must NOT do**:
  - g_option_context를 사용해도 되지만, 단순한 getopt_long도 허용
  - 국제화(i18n) 처리는 추후 (현재는 영어)

  **Recommended Agent Profile**:
  - **Category**: `quick`
  - **Skills**: `[]`

  **Parallelization**:
  - **Can Run In Parallel**: YES
  - **Parallel Group**: Wave 1
  - **Blocks**: Task 12
  - **Blocked By**: Task 2

  **References**:
  - `cde/programs/dtsearchpath/dtappg/Options.h` - Options 클래스 구조
  - `cde/programs/dtsearchpath/dtappg/Options.C` - 인자 파싱 패턴
  - `cde/programs/dtaction/Main.c` - getopt_long 사용 예

  **Acceptance Criteria**:
  - `dtxdg2appmgr -v` 실행 시 verbose 모드 활성화
  - `dtxdg2appmgr --help` 사용법 출력
  - 잘못된 옵션 시 종료 코드 1

  **QA Scenarios (MANDATORY)**:
  ```
  Scenario: CLI 옵션 파서 동작 확인
    Tool: Bash
    Steps:
      1. `./dtxdg2appmgr --help` → 사용법 + 옵션 설명 출력
      2. `./dtxdg2appmgr -v 2>&1 | grep -i verbose` → verbose 메시지
      3. `./dtxdg2appmgr -o /tmp/test -r -v` → 정상 종료
      4. `./dtxdg2appmgr --invalid-opt 2>&1; echo $?` → 비-zero 종료 코드
    Expected Result: 4단계 모두 정상
    Evidence: .sisyphus/evidence/task-3-cli-options.txt
  ```

  **Commit**: YES (Task 2와 함께)
  - Message: `feat(dtxdg2appmgr): add CLI options parser`
  - Files: `cde/programs/dtxdg2appmgr/options.c`, `options.h`

- [x] 4. **.desktop GKeyFile 파서 (`desktop_parser.c`/`.h`)**

  **What to do**:
  - `DesktopEntry` 구조체 정의:
    - `gchar *id` (파일명, 예: "firefox.desktop")
    - `gchar *path` (전체 경로)
    - `gchar *name` (Name[현재 LANG] 또는 Name)
    - `gchar *generic_name` (GenericName[lang])
    - `gchar *comment` (Comment[lang])
    - `gchar *exec` (Exec 라인 원본)
    - `gchar *icon` (Icon 이름 또는 경로)
    - `gchar **categories` (Categories 세미콜론 분리 배열)
    - `gchar *type` (Type, 기본 "Application")
    - `gchar *terminal` (Terminal, 기본 "false")
    - `gboolean no_display` (NoDisplay, 기본 false)
    - `gboolean hidden` (Hidden, 기본 false)
    - `gchar *startup_wm_class`
    - `time_t mtime` (파일 mtime, 캐시용)
  - `desktop_parse(const gchar *path, const gchar *lang, DesktopEntry *entry)` 함수:
    - GKeyFile로 .desktop 파싱
    - LANG 코드 정규화 (예: "ko_KR.UTF-8" → "ko")
    - 로케일별 키 lookup (Name[ko] 우선, 없으면 Name)
    - Categories를 g_strsplit(";", ...)로 분리
  - `desktop_entry_free(DesktopEntry *entry)` 함수
  - `desktop_should_include(const DesktopEntry *entry)` 함수 (필터링 로직)

  **Must NOT do**:
  - g_desktop_app_info_new()를 사용하지 말 것 (디스크 I/O가 아니라 직접 GKeyFile로 파싱)
  - Type != "Application" 항목 무시
  - NoDisplay=true 또는 Hidden=true 항목 무시

  **Recommended Agent Profile**:
  - **Category**: `quick`
  - **Skills**: `[]`

  **Parallelization**:
  - **Can Run In Parallel**: YES
  - **Parallel Group**: Wave 1
  - **Blocks**: Task 12
  - **Blocked By**: Task 2

  **References**:
  - FreeDesktop Desktop Entry Specification 1.5 (외부 문서)
  - `glib-2.0/gio/gdesktopappinfo.h` - GDesktopAppInfo API (참고용)
  - `cde/programs/types/*.dt` - CDE .dt 파일 형식 (출력 비교용)

  **Acceptance Criteria**:
  - 실제 `/usr/share/applications/firefox.desktop` 파싱 성공
  - Name, Exec, Icon, Categories 필드 정확히 추출
  - Type="Link" 등 비-Application 항목은 `should_include`에서 false 반환

  **QA Scenarios (MANDATORY)**:
  ```
  Scenario: 실제 .desktop 파일 파싱
    Tool: Bash
    Preconditions: `/usr/share/applications/firefox.desktop` 또는 다른 .desktop 존재
    Steps:
      1. `/usr/share/applications/firefox.desktop` 존재 확인
      2. 단위 테스트 바이너리 실행 (또는 디버그 출력): `dtxdg2appmgr -v /usr/share/applications/firefox.desktop 2>&1`
      3. 출력에 "Name=Firefox", "Exec=firefox %u", "Categories=Network" 매치
    Expected Result: 모든 필드 정확히 파싱
    Evidence: .sisyphus/evidence/task-4-desktop-parse.txt

  Scenario: 필터링 검증
    Tool: Bash
    Steps:
      1. 임시 `NoDisplay=true` .desktop 작성
      2. 파싱 시도 → include=false 반환
    Expected Result: NoDisplay=true 항목 제외
    Evidence: .sisyphus/evidence/task-4-filtering.txt
  ```

  **Commit**: YES (Task 2와 함께)
  - Message: `feat(dtxdg2appmgr): add .desktop GKeyFile parser`
  - Files: `cde/programs/dtxdg2appmgr/desktop_parser.c`, `desktop_parser.h`

- [x] 5. **XDG 데이터 경로 스캐너 (`path_scanner.c`/`.h`)**

  **What to do**:
  - `xdg_get_data_dirs(void)` 함수:
    - `XDG_DATA_DIRS` 환경 변수 파싱 (콜론 구분)
    - 기본값 `/usr/local/share:/usr/share`
    - `$HOME/.local/share` 항상 첫 번째로 추가
  - `xdg_scan_applications(gchar **data_dirs, GSList **entries)` 함수:
    - 각 디렉토리의 `applications/` 서브디렉토리 재귀 검색
    - `*.desktop` 파일 찾기 (GDir 또는 readdir)
    - 각 파일에 대해 `DesktopEntry` 구조체 할당, mtime 저장
    - 결과를 `GSList`로 반환
  - `gchar *xdg_get_language(void)` 함수:
    - `LANG` 환경 변수 파싱
    - `ko_KR.UTF-8` → `ko` 정규화

  **Must NOT do**:
  - Network 파일시스템 (NFS) 마운트는 timeout 설정 권장 (선택적)
  - 숨김 파일 (`.`) 무시

  **Recommended Agent Profile**:
  - **Category**: `quick`
  - **Skills**: `[]`

  **Parallelization**:
  - **Can Run In Parallel**: YES
  - **Parallel Group**: Wave 1
  - **Blocks**: Task 12
  - **Blocked By**: Task 2

  **References**:
  - XDG Base Directory Specification (외부 문서)
  - `cde/programs/dtsearchpath/dtsp/AppSearchPath.C` - 유사한 검색 경로 스캐너 패턴
  - `cde/programs/dtsearchpath/dtappg/dtappgather.C` - `GatherAppsFromASearchElement` 패턴

  **Acceptance Criteria**:
  - 시스템의 모든 `*.desktop` 파일 발견
  - `$XDG_DATA_DIRS` 환경 변수 우선순위 정확히 처리
  - `$HOME/.local/share/applications` 항상 포함

  **QA Scenarios (MANDATORY)**:
  ```
  Scenario: XDG 데이터 경로 스캔
    Tool: Bash
    Preconditions: 시스템에 여러 .desktop 파일 존재
    Steps:
      1. `find /usr/share/applications -name "*.desktop" | wc -l` → 100개 이상
      2. `dtxdg2appmgr --dry-run 2>&1 | wc -l` → 비슷한 개수
      3. 임시 디렉토리에 테스트 .desktop 작성 → 스캔에 포함됨 확인
    Expected Result: 시스템의 .desktop 파일들이 모두 발견됨
    Evidence: .sisyphus/evidence/task-5-path-scanner.txt
  ```

  **Commit**: YES (Task 2와 함께)
  - Message: `feat(dtxdg2appmgr): add XDG data path scanner`
  - Files: `cde/programs/dtxdg2appmgr/path_scanner.c`, `path_scanner.h`

- [x] 6. **Exec 라인 필드 코드 파서 (`exec_parser.c`/`.h`)**

  **What to do**:
  - `exec_parse(const gchar *exec_line, const gchar *file_uri, ExecContext *ctx)` 함수:
    - XDG Exec 라인 토큰화 (공백, 인용부호 처리)
    - 필드 코드 치환:
      - `%f` → 단일 파일 경로
      - `%F` → 다중 파일 경로
      - `%u` → 단일 URL
      - `%U` → 다중 URL
      - `%d` → 단일 디렉토리
      - `%D` → 다중 디렉토리
      - `%i` → `--wmclass StartupWMClass` (있을 때)
      - `%c` → translated Name
      - `%k` → .desktop 파일 경로
      - `%%` → `%` (이스케이프)
    - 환경 변수 확장 (`$VAR`, `${VAR}`)
  - `exec_to_cde_string(const gchar *exec_line, const gchar *name, const gchar *desktop_path, GString *out)` 함수:
    - 파싱된 결과를 CDE .dt의 `EXEC_STRING` 형식으로 변환
    - `%Arg_1%` 같은 CDE 인자 매크로 사용 (또는 그냥 평문)

  **Must NOT do**:
  - 단순 `strtok` 사용 금지 (인용부호 처리)
  - 필드 코드를 무시하고 그대로 사용 금지

  **Recommended Agent Profile**:
  - **Category**: `quick`
  - **Skills**: `[]`

  **Parallelization**:
  - **Can Run In Parallel**: YES
  - **Parallel Group**: Wave 1
  - **Blocks**: Task 10
  - **Blocked By**: Task 2

  **References**:
  - FreeDesktop Desktop Entry Specification 1.5 - "Exec" 키
  - `cde/programs/types/dt.dt.src` - CDE EXEC_STRING 형식 예시

  **Acceptance Criteria**:
  - `Exec=firefox %u` → `firefox %Arg_1%` 같은 CDE 형식으로 변환
  - `Exec=env WINEPREFIX="/path" wine %f` → 인용부호 보존
  - `Exec=myapp --icon=%i` → `%i` 확장

  **QA Scenarios (MANDATORY)**:
  ```
  Scenario: Exec 라인 파싱 정확성
    Tool: Bash
    Steps:
      1. 단위 테스트: `Exec=firefox %u` → "firefox %Arg_1%"
      2. 단위 테스트: `Exec=gimp-2.10 %F` → "gimp-2.10 %Arg_1%"
      3. 단위 테스트: `Exec=code --new-window %F` → "code --new-window %Arg_1%"
      4. 단위 테스트: `Exec=env VAR=value %U` → "env VAR=value %Arg_1%"
    Expected Result: 모든 케이스에서 필드 코드 정확히 치환
    Evidence: .sisyphus/evidence/task-6-exec-parser.txt
  ```

  **Commit**: YES (Task 2와 함께)
  - Message: `feat(dtxdg2appmgr): add XDG Exec line parser`
  - Files: `cde/programs/dtxdg2appmgr/exec_parser.c`, `exec_parser.h`

- [x] 7. **카테고리 → 그룹명 sanitization 유틸리티 (`category_mapper.c`/`.h`)**

  **What to do**:
  - `gchar *category_to_group(const gchar *category)` 함수:
    - 입력: "Office", "Network", "AudioVideo" 등
    - 출력: **"Office_XDG", "Network_XDG", "AudioVideo_XDG"** (모든 XDG 그룹에 `_XDG` 접미사 항상 적용)
    - 정규화 규칙:
      - 알파벳/숫자가 아닌 문자는 `_`로 치환
      - 연속 `_`는 단일 `_`로
      - 앞뒤 `_` 제거
      - 빈 문자열이면 `"Other_XDG"`
      - **정규화 후 항상 `_XDG` 접미사 추가**
  - `gchar *get_primary_category(gchar **categories)` 함수:
    - 첫 번째 카테고리 반환 (보통 가장 구체적인 분류)
    - "Office;Spreadsheet;WordProcessing" → "Office"
  - `gboolean is_collision(const gchar *name, const gchar *group, GHashTable *seen)` 함수:
    - 같은 그룹 내 같은 이름 충돌 감지
    - 충돌 시 `name-HASH8` 형식으로 변경 (예: "firefox-a3f2b1c8")
  - `gchar *sanitize_action_name(const gchar *name)` 함수:
    - CDE .dt ACTION 이름 형식 (영숫자+언더스코어) 검증
    - 공백, 하이픈 등을 언더스코어로 치환
  - **이점**:
    - CDE 기본 그룹(`Office`, `Internet`, `Education` 등)과 절대 충돌 없음
    - PATH_PATTERN `*/appmanager/*/Office_XDG` ↔ CDE의 `*/appmanager/*/Office` 명확히 구분
    - 그룹명만 봐도 XDG 변환 결과임을 즉시 식별 가능
    - 단순한 규칙 (조건 분기 없음)

  **Must NOT do**:
  - **XDG_ 접두사 사용 금지** (사용자 결정)
  - **CDE 기존 그룹과 동일한 이름 사용 금지** (모든 XDG 그룹에 `_XDG` 접미사 필수)

  **Recommended Agent Profile**:
  - **Category**: `quick`
  - **Skills**: `[]`

  **Parallelization**:
  - **Can Run In Parallel**: YES
  - **Parallel Group**: Wave 1
  - **Blocks**: Task 10
  - **Blocked By**: Task 2

  **References**:
  - XDG Menu Specification - Main Categories (Network, Office, AudioVideo 등 12개)
  - `cde/programs/types/dtappman.dt` - CDE 기본 그룹 5개 (Desktop_Apps, Desktop_Tools, Information, System_Admin)
  - `cde/programs/types/{Office,Internet,Education,Graphics,System,Games,Media_Tools,TeX}.dt` - CDE 확장 그룹 8개
  - XDG `Office` ↔ CDE `Office` 충돌 케이스

  **Acceptance Criteria**:
  - "Office" → "Office_XDG"
  - "Network" → "Network_XDG"
  - "AudioVideo" → "AudioVideo_XDG"
  - "AudioVideo;Audio;Video" → 첫 번째 "AudioVideo" → "AudioVideo_XDG"
  - "Development;IDE" → "Development_XDG"
  - "Game" → "Game_XDG" (CDE `Games`와도 충돌 회피)
  - "" → "Other_XDG"
  - 같은 그룹 내 "firefox" 중복 → "firefox-a3f2b1c8" (해시)

  **QA Scenarios (MANDATORY)**:
  ```
  Scenario: 모든 XDG 그룹에 _XDG 접미사 적용 확인
    Tool: Bash
    Steps:
      1. 단위 테스트: "Office" → "Office_XDG"
      2. 단위 테스트: "Network" → "Network_XDG"
      3. 단위 테스트: "AudioVideo" → "AudioVideo_XDG"
      4. 단위 테스트: "" → "Other_XDG"
      5. 단위 테스트: "Office Suite;Document" → "Office_Suite_XDG"
      6. 단위 테스트: "Game" → "Game_XDG" (CDE `Games` 충돌 회피)
    Expected Result: 모든 입력에 `_XDG` 접미사 적용
    Evidence: .sisyphus/evidence/task-7-category-mapper.txt

  Scenario: CDE 기존 그룹과 절대 충돌하지 않음
    Tool: Bash
    Steps:
      1. CDE 내장 그룹 13개 (`Desktop_Apps`, `Desktop_Tools`, `Information`, `System_Admin`, `Office`, `Internet`, `Education`, `Graphics`, `System`, `Games`, `Media_Tools`, `TeX`) 각각에 대해 매핑 시도
      2. 모두 `<name>_XDG`로 변환되어 원본과 다름
    Expected Result: 13개 모두 CDE 원본과 다른 그룹명
    Evidence: .sisyphus/evidence/task-7-no-collision.txt
  ```

  **Commit**: YES (Task 2와 함께)
  - Message: `feat(dtxdg2appmgr): add category to group mapper (always _XDG suffix)`
  - Files: `cde/programs/dtxdg2appmgr/category_mapper.c`, `category_mapper.h`

- [x] 8. **아이콘 검색 (XDG Icon Theme Spec) (`icon_resolver.c`/`.h`)**

  **What to do**:
  - **기존 `contrib/desktop2dt/desktop2dt`의 `find_convert()` 알고리즘 포팅** (line 48-74):
    - 절대 경로 (`/path/to/icon.png`) → 그대로 사용
    - 상대 경로 → `find` 명령으로 표준 XDG 위치 검색
  - 검색 디렉토리 (우선순위 순):
    1. `$HOME/.local/share/icons` (XDG 사용자)
    2. `$XDG_DATA_DIRS` 의 `<dir>/icons` (기본 `/usr/local/share:/usr/share`)
    3. `$HOME/.icons` (deprecated, 하위 호환)
    4. `/usr/share/pixmaps` (unthemed fallback)
  - `IconResolution` 구조체:
    - `gchar *absolute_path` (찾은 아이콘의 절대 경로)
    - `gchar *format` (확장자: `png`, `svg`, `xpm`, `jpg`)
  - `gboolean icon_resolve(const gchar *icon_name, IconResolution *result)` 함수:
    - 1단계: 절대 경로 → 그대로 반환
    - 2단계: 정확한 파일명 (`<basedir>/<icon_name>`) 검색
    - 3단계: 확장자 추가 검색 (`<icon>.png`, `<icon>.svg`, `<icon>.xpm`, `<icon>.jpg`, `<icon>.jpeg`)
    - 4단계: 패턴 매칭 (`<icon>-32.xpm`, `<icon>_*x*.xpm` — desktop2dt와 동일)
  - 확장자 fallback: `.png` → `.svg` → `.xpm` → `.jpg`/`.jpeg` 순서
  - `void icon_resolution_free(IconResolution *res)` 함수

  **Must NOT do**:
  - SVG를 직접 파싱하지 말 것 (이름만 매칭하고 변환은 Task 9)
  - 네트워크 아이콘 (`http://`) 다운로드 안 함
  - `index.theme` 파싱 (단순화, desktop2dt와 동일하게 `find` 기반)

  **Recommended Agent Profile**:
  - **Category**: `quick`
  - **Skills**: `[]`

  **Parallelization**:
  - **Can Run In Parallel**: YES
  - **Parallel Group**: Wave 2
  - **Blocks**: Task 9
  - **Blocked By**: Task 2

  **References**:
  - **`cde/contrib/desktop2dt/desktop2dt:48-74`** — `find_convert()` 원본 알고리즘 (Isaac Dunham, MIT)
  - XDG Icon Theme Specification (외부 문서)
  - `/usr/share/icons/hicolor/index.theme` - 표준 인덱스
  - `cde/programs/icons/` - CDE 기존 아이콘 형식 (`.pm`)

  **Acceptance Criteria**:
  - `firefox` → `/usr/share/icons/hicolor/32x32/apps/firefox.png` (또는 다른 경로) 찾기 성공
  - 절대 경로 (`/path/to/icon.png`) → 그대로 사용
  - 다양한 확장자 (`firefox.png`, `firefox.svg`, `firefox-32.xpm` 등) 모두 검색
  - 존재하지 않는 아이콘 → NULL 반환 (에러 없음)

  **QA Scenarios (MANDATORY)**:
  ```
  Scenario: 표준 XDG 아이콘 검색
    Tool: Bash
    Preconditions: 시스템에 표준 아이콘 설치됨
    Steps:
      1. `find /usr/share/icons -name "firefox*" | head` → 경로 발견
      2. `dtxdg2appmgr --resolve-icon firefox 2>&1` → 발견된 경로 출력
      3. 절대 경로 테스트: `dtxdg2appmgr --resolve-icon /usr/share/pixmaps/firefox.png` → 그대로 반환
    Expected Result: 표준 + 절대 경로 모두 정확히 처리
    Evidence: .sisyphus/evidence/task-8-icon-resolve.txt

  Scenario: desktop2dt와 동일한 검색 결과 보장 (동등성 테스트)
    Tool: Bash
    Preconditions: desktop2dt 셸 스크립트가 시스템에 있음
    Steps:
      1. `desktop2dt /usr/share/applications/firefox.desktop` (백그라운드)
      2. `dtxdg2appmgr --resolve-icon firefox` (백그라운드)
      3. 두 출력의 ICON basename이 동일한지 비교
    Expected Result: 동일한 아이콘 basename (예: `firefox`)
    Evidence: .sisyphus/evidence/task-8-parity-test.txt
  ```

  **Commit**: YES
  - Message: `feat(dtxdg2appmgr): add XDG icon resolver (ported from desktop2dt)`
  - Files: `cde/programs/dtxdg2appmgr/icon_resolver.c`, `icon_resolver.h`

- [x] 9. **XPM 변환기 (`xpm_converter.c`/`.h`)**

  **What to do**:
  - **3개 CDE 크기 동시 생성: `.t.pm` (16x16), `.m.pm` (32x32), `.l.pm` (48x48)** — CDE 표준 컨벤션
  - **`contrib/desktop2dt/desktop2dt:67-72`의 ImageMagick 호출 패턴 포팅**:
    ```c
    // 원본 셸 스크립트:
    // convert -resize 48x48 "$ICON" "$NEWICON.l.xpm" && mv "$NEWICON.l.xpm" "$ICONDIR$NEWICON.l.pm"
    // convert -resize 32x32 "$ICON" "$NEWICON.m.xpm" && mv "$NEWICON.m.xpm" "$ICONDIR$NEWICON.m.pm"
    // convert -resize 16x16 "$ICON" "$NEWICON.t.xpm" && mv "$NEWICON.t.xpm" "$ICONDIR$NEWICON.t.pm"
    ```
  - `IconSizes` 구조체:
    - `gint t_size` = 16
    - `gint m_size` = 32
    - `gint l_size` = 48
  - `gboolean convert_to_xpm_set(const gchar *src_path, const gchar *base_name, const gchar *output_dir, GError **error)` 함수:
    - 3개 크기에 대해 순차적으로 `convert` 호출
    - 1단계: ImageMagick `convert`: `convert -background none -resize <size>x<size> <src> <dst>` (.xpm 임시 파일)
    - 2단계: rename `.xpm` → `.pm` (CDE 컨벤션)
    - 3단계: 실패 시 netpbm 파이프라인: `pngtopnm | ppmtopgm | pgmtoppm | ppmtoxpm` (PNG 전용)
    - 4단계: 모든 도구 실패 시 에러 반환
  - `gboolean check_tool_available(const gchar *tool_name)` 헬퍼:
    - `which` 명령으로 도구 존재 확인
  - `gchar *generate_pm_basename(const gchar *app_name)` 함수:
    - `xdg-<app>` 형식 (예: `xdg-firefox`)
    - 접미사 (`.t/.m/.l`)는 호출자가 추가
  - **SVG 처리**: `convert`는 ImageMagick 7+에 내장된 librsvg delegate로 SVG 자동 처리

  **Must NOT do**:
  - 직접 libpng, libXpm 코드를 작성하지 말 것 (외부 도구 사용)
  - 알파 채널 완벽 보존 시도 (`-background none`으로 1-bit mask로 변환)
  - 16/32/48 외 다른 크기 생성 (CDE는 이 3개만 사용)

  **Recommended Agent Profile**:
  - **Category**: `quick`
  - **Skills**: `[]`

  **Parallelization**:
  - **Can Run In Parallel**: YES
  - **Parallel Group**: Wave 2
  - **Blocks**: Task 12
  - **Blocked By**: Task 8

  **References**:
  - **`cde/contrib/desktop2dt/desktop2dt:67-72`** — ImageMagick 호출 패턴
  - **`cde/programs/dtstyle/Backdrop.c:817-822`** — CDE .pm 컨벤션 (size suffix stripped at lookup)
  - `cde/programs/dticon/fileIO.c:319-419` — XPM/XBM 변환 패턴
  - `man convert` (ImageMagick)
  - `man ppmtoxpm` (netpbm)
  - `cde/lib/DtHelp/Graphics.c:419-456` — CDE 이미지 레지스트리 (PNG 미포함 확인)

  **Acceptance Criteria**:
  - PNG 입력 → 3개 크기 XPM 파일 생성 (16x16, 32x32, 48x48)
  - 생성된 각 XPM 파일이 `file` 명령으로 "XPM image" 인식
  - 모든 .xpm 파일이 .pm으로 rename됨
  - ImageMagick 없을 때 netpbm으로 fallback
  - 3개 크기 중 하나라도 실패 시 partial cleanup (이미 생성된 것들 삭제)

  **QA Scenarios (MANDATORY)**:
  ```
  Scenario: PNG → 3개 크기 XPM 변환 정확성
    Tool: Bash
    Steps:
      1. 테스트 PNG 준비: `convert -size 64x64 xc:blue /tmp/test.png`
      2. 변환: `dtxdg2appmgr --convert-icon-set /tmp/test.png /tmp/test.xdg 2>&1`
      3. 검증:
         - `file /tmp/test.xdg.t.pm` → "XPM image"
         - `file /tmp/test.xdg.m.pm` → "XPM image"
         - `file /tmp/test.xdg.l.pm` → "XPM image"
         - `head -1 /tmp/test.xdg.m.pm` → "/* XPM */" 헤더
         - `identify /tmp/test.xdg.t.pm` (IM 있을 때) → "16x16"
         - `identify /tmp/test.xdg.m.pm` → "32x32"
         - `identify /tmp/test.xdg.l.pm` → "48x48"
    Expected Result: 3개 크기 모두 정확히 생성
    Evidence: .sisyphus/evidence/task-9-xpm-convert-3sizes.txt

  Scenario: SVG → XPM 변환 (ImageMagick rsvg delegate)
    Tool: Bash
    Preconditions: ImageMagick with librsvg 설치됨
    Steps:
      1. 간단한 SVG 작성: `cat > /tmp/test.svg <<EOF
         <svg xmlns="http://www.w3.org/2000/svg" width="32" height="32">
           <circle cx="16" cy="16" r="15" fill="green"/>
         </svg>
         EOF`
      2. 변환: `dtxdg2appmgr --convert-icon-set /tmp/test.svg /tmp/test.xdg`
      3. 검증: `file /tmp/test.xdg.m.pm` → "XPM image"
    Expected Result: SVG가 3개 크기 XPM으로 변환됨
    Evidence: .sisyphus/evidence/task-9-svg-convert.txt
  ```

  **Commit**: YES
  - Message: `feat(dtxdg2appmgr): add 3-size XPM converter (ported from desktop2dt)`
  - Files: `cde/programs/dtxdg2appmgr/xpm_converter.c`, `xpm_converter.h`

- [x] 10. **.dt 파일 생성기 (`dt_writer.c`/`.h`)**

  **What to do**:
  - `gboolean write_dt_file(const DesktopEntry *entry, const gchar *action_name, const gchar *icon_basename, const gchar *dt_output_dir, GError **error)` 함수:
    - 다음 형식으로 .dt 파일 작성:
      ```
      ACTION <sanitized_name>
      {
              LABEL           <Name>
              ICON            <icon_basename>
              TYPE            COMMAND
              WINDOW_TYPE     NO_STDIO
              EXEC_STRING     <parsed_exec>
              DESCRIPTION     <Comment>
      }
      ```
    - 다국어 처리: `Name[ko]`, `Comment[ko]` 등이 있으면 `%|nls-...-|` 매크로 사용
  - `gboolean write_appmanager_stub(const gchar *group_dir, const gchar *action_name, const gchar *stub_template_path)` 함수:
    - 기존 `action` 템플릿 파일을 `<group_dir>/<action_name>`에 복사
    - 실행 권한 (0755) 부여
  - `gboolean write_group_dt(const gchar *group_name, const gchar *dt_output_dir)` 함수:
    - 그룹의 `DATA_ATTRIBUTES`/`DATA_CRITERIA`를 정의하는 .dt 작성:
      ```
      DATA_ATTRIBUTES <GroupName>Appgroup
      {
              ACTIONS         OpenInPlace,OpenNewView
              LABEL           <GroupName>
              ICON            Dtapps
              DESCRIPTION     <GroupName> Applications (XDG)
      }
      DATA_CRITERIA <GroupName>AppgroupCriteria1
      {
              DATA_ATTRIBUTES_NAME    <GroupName>Appgroup
              LABEL                   <GroupName>
              MODE                    d
              PATH_PATTERN            */appmanager/*/<GroupName>
      }
      ACTION Open
      {
              ARG_TYPE        <GroupName>Appgroup
              TYPE            MAP
              MAP_ACTION      OpenAppGroup
      }
      ```

  **Must NOT do**:
  - 기존 CDE .dt 파일 덮어쓰지 말 것
  - 시스템 영역 (`/usr/dt/`)에 쓰지 말 것 (사용자 영역 `/var/dt/`, `$HOME/.dt/`)

  **Recommended Agent Profile**:
  - **Category**: `quick`
  - **Skills**: `[]`

  **Parallelization**:
  - **Can Run In Parallel**: YES
  - **Parallel Group**: Wave 2
  - **Blocks**: Task 12
  - **Blocked By**: Tasks 4, 6, 7

  **References**:
  - `cde/programs/types/dtappman.dt` - CDE .dt 형식 표준
  - `cde/programs/types/Games.dt` - 간단한 그룹 .dt 예시
  - `cde/programs/types/chromium.dt` - 단순 ACTION .dt 예시
  - `cde/programs/types/dt.dt.src` - NLS 매크로 사용 예시

  **Acceptance Criteria**:
  - 생성된 .dt 파일이 CDE에서 인식 (dtdbcache 성공)
  - 그룹 .dt가 `dtappman.dt`의 `OpenAppGroup`과 호환
  - 스텁 파일에 실행 권한 있음

  **QA Scenarios (MANDATORY)**:
  ```
  Scenario: .dt 파일 형식 정확성
    Tool: Bash
    Steps:
      1. 임시 .desktop 작성
      2. dtxdg2appmgr 실행
      3. `cat /var/dt/appconfig/appmanager/$DTUSERSESSION/Network_XDG/firefox.dt`
      4. 내용에 "ACTION firefox", "LABEL Firefox", "TYPE COMMAND" 포함 확인
      5. `dtdbcache -init` 재실행 성공
    Expected Result: 유효한 .dt 파일 + dtdbcache 성공
    Evidence: .sisyphus/evidence/task-10-dt-writer.txt
  ```

  **Commit**: YES
  - Message: `feat(dtxdg2appmgr): add .dt file and appmanager stub writer`
  - Files: `cde/programs/dtxdg2appmgr/dt_writer.c`, `dt_writer.h`

- [x] 11. **mtime 기반 캐시 (`cache.c`/`.h`)**

  **What to do**:
  - `CacheEntry` 구조체:
    - `gchar *desktop_path` (절대 경로)
    - `gchar *group_name` (생성된 XDG 그룹)
    - `time_t mtime` (.desktop mtime)
    - `gchar *stub_path` (생성된 스텁 파일)
  - `GHashTable *cache_load(const gchar *cache_file)` 함수:
    - INI 형식 캐시 파일 로드 (GKeyFile)
    - 키: `desktop_path`, 값: `CacheEntry`
  - `gboolean cache_save(GHashTable *cache, const gchar *cache_file)` 함수:
    - 모든 CacheEntry를 INI로 저장
    - 권한 0644
  - `gboolean cache_is_valid(const CacheEntry *entry, const gchar *desktop_path)` 함수:
    - 현재 .desktop mtime과 캐시된 mtime 비교
    - 같으면 valid (재변환 불필요)
  - `void cache_entry_free(CacheEntry *entry)` 함수

  **Must NOT do**:
  - 캐시 파일을 SQLite 같은 무거운 의존성으로 저장하지 말 것 (단순 INI)
  - 캐시 파일이 손상되어도 빌드 실패 없이 무시

  **Recommended Agent Profile**:
  - **Category**: `quick`
  - **Skills**: `[]`

  **Parallelization**:
  - **Can Run In Parallel**: YES
  - **Parallel Group**: Wave 2
  - **Blocks**: Task 12
  - **Blocked By**: Task 2

  **References**:
  - `cde/programs/dtsearchpath/dtappg/dtappgather.C` - 캐시 없음 (매번 전체) - 우리 개선점
  - 일반적인 mtime 비교 패턴

  **Acceptance Criteria**:
  - 첫 실행: 모든 .desktop 변환, 캐시 생성
  - 두 번째 실행: 변경된 것만 재변환 (성능 개선)
  - `--force` 옵션: 캐시 무시, 전체 재변환

  **QA Scenarios (MANDATORY)**:
  ```
  Scenario: mtime 기반 캐시 동작
    Tool: Bash
    Steps:
      1. 첫 실행: `time dtxdg2appmgr` → 시간 T1
      2. 즉시 재실행: `time dtxdg2appmgr` → 시간 T2 (T2 << T1)
      3. `touch /usr/share/applications/firefox.desktop` (가장 최근)
      4. 재실행: `time dtxdg2appmgr` → 시간 T3 (firefox만 재변환, T3 < T1)
    Expected Result: 캐시 효과로 두 번째 실행이 빠름
    Evidence: .sisyphus/evidence/task-11-cache.txt
  ```

  **Commit**: YES
  - Message: `feat(dtxdg2appmgr): add mtime-based cache`
  - Files: `cde/programs/dtxdg2appmgr/cache.c`, `cache.h`

- [x] 12. **main() 오케스트레이션 (`dtxdg2appmgr.c`)**

  **What to do**:
  - main() 함수 구현 (**`contrib/desktop2dt/desktop2dt`의 메인 루프 패턴 포팅**):
    1. `parse_options(argc, argv, &opts)`
    2. `xdg_get_data_dirs()` → `data_dirs`
    3. `xdg_scan_applications(data_dirs, &entries)` → 모든 DesktopEntry
    4. `cache_load(opts.cache_file)` → `cache`
    5. 각 entry에 대해:
       - `desktop_should_include()` → false면 skip
       - `cache_is_valid()` → true면 skip
       - `get_primary_category()` → group_name
       - `category_to_group()` → `<group>_XDG` (모든 XDG 그룹에 `_XDG` 접미사)
       - `is_collision()` → action_name 결정
       - `icon_resolve()` → IconResolution
       - `convert_to_xpm_set()` → `.t.pm`/`.m.pm`/`.l.pm` 3개 (output_dir에 저장)
       - `exec_to_cde_string()` → EXEC_STRING
       - `write_dt_file()` → .dt (`$HOME/.dt/types/<app>.dt`)
       - `write_appmanager_stub()` → `/var/dt/appconfig/appmanager/$DTUSERS_SESSION/<group>_XDG/<action_name>` 스텁
       - `cache_update()` → 캐시 갱신
    6. `write_group_dt()` → 그룹 .dt
    7. `cache_save()`
  - **출력 경로** (모든 XDG 그룹에 `_XDG` 접미사):
    - `.dt` 파일: `$HOME/.dt/types/<sanitized_name>.dt` (또는 `/var/dt/appconfig/...`)
    - `.pm` 파일: `$HOME/.dt/icons/xdg-<name>.{t,m,l}.pm` (또는 시스템 영역)
    - appmanager 스텁: `/var/dt/appconfig/appmanager/$DTUSERSESSION/<group>_XDG/<action_name>`
    - 예: `Network_XDG/firefox`, `AudioVideo_XDG/vlc`, `Office_XDG/libreoffice-writer`
  - 에러 처리: GError 사용, 종료 코드 적절히 설정
  - 신호 처리: SIGINT 시 부분 상태로 종료하지 말고 정리

  **Must NOT do**:
  - main()에 모든 로직을 넣지 말 것 (각 모듈 함수 호출만)
  - stdout에 진행 상황 출력 시 `\r` 사용 (line-buffered 출력 유지)
  - `Categories=` 의 `X-;` 접두사나 `GTK;`, `Motif;`, `GNOME;`, `Qt;` 같은 비주류 카테고리 prefix 제거 로직 추가 (desktop2dt:106-109)

  **Recommended Agent Profile**:
  - **Category**: `unspecified-low`
  - **Skills**: `[]`

  **Parallelization**:
  - **Can Run In Parallel**: NO (모든 모듈 통합)
  - **Parallel Group**: Wave 2
  - **Blocks**: Tasks 13, 14, 15
  - **Blocked By**: Tasks 3, 4, 5, 6, 8, 9, 10, 11

  **References**:
  - **`cde/contrib/desktop2dt/desktop2dt:118-153`** — main 루프 패턴
  - **`cde/contrib/desktop2dt/desktop2dt:106-109`** — Categories prefix 제거 로직
  - `cde/programs/dtsearchpath/dtappg/dtappgather.C:302-320` - dtappgather main() 패턴
  - `cde/programs/dtaction/Main.c` - 또 다른 main() 예시

  **Acceptance Criteria**:
  - 전체 파이프라인이 한 번에 동작
  - 에러 발생 시 적절한 메시지 + 종료 코드
  - verbose 모드에서 진행 상황 출력
  - **desktop2dt 셸 스크립트와 동일한 입력에 대해 동일한 출력** (동등성 테스트)

  **QA Scenarios (MANDATORY)**:
  ```
  Scenario: 전체 파이프라인 통합 동작
    Tool: Bash
    Preconditions: 시스템에 .desktop 파일 + ImageMagick 설치
    Steps:
      1. `dtxdg2appmgr -v 2>&1 | tee /tmp/run.log` → 모든 단계 출력
      2. `/var/dt/appconfig/appmanager/$DTUSERSESSION/` 에 `_XDG` 접미사 그룹 디렉토리 생성 확인 (예: `Network_XDG/`, `AudioVideo_XDG/`, `Office_XDG/` 등)
      3. `$HOME/.dt/icons/` 에 `xdg-*.{t,m,l}.pm` 3개씩 생성 확인
      4. `$HOME/.dt/types/` 에 `*.dt` 파일 생성 확인
      5. `dtdbcache -init` 실행 후 `dtfile` 실행 시 XDG 그룹 표시
    Expected Result: end-to-end 동작 (모든 XDG 그룹에 `_XDG` 접미사)
    Evidence: .sisyphus/evidence/task-12-main-integration.txt

  Scenario: desktop2dt와의 동등성 검증
    Tool: Bash
    Preconditions: 두 도구 모두 빌드됨
    Steps:
      1. 테스트 .desktop 파일 3개 작성
      2. `desktop2dt test1.desktop test2.desktop test3.desktop` 실행 → .dt 파일들
      3. `dtxdg2appmgr --single-file test1.desktop` 등 실행 → .dt 파일들
      4. 두 출력 diff 비교 (서식 차이만 허용)
    Expected Result: 의미상 동일한 .dt 출력
    Evidence: .sisyphus/evidence/task-12-parity-desktop2dt.txt
  ```

  **Commit**: YES
  - Message: `feat(dtxdg2appmgr): add main orchestrator (with desktop2dt parity)`
  - Files: `cde/programs/dtxdg2appmgr/dtxdg2appmgr.c`

- [x] 13. **Xsession.src에 자동 실행 추가**

  **What to do**:
  - `cde/programs/dtlogin/config/Xsession.src` 수정:
    - line 301 근처에 `dtstart_xdg2appmgr="$DT_BINPATH/dtxdg2appmgr &"` 추가
    - 기존 `dtstart_appgather` 호출 직후에 위치
    - HASH 주석으로 의도 설명 추가
  - `configure.ac` 또는 Makefile.am에서 `DT_BINPATH`가 정의되어 있는지 확인

  **Must NOT do**:
  - 다른 dtstart_* 라인을 변경하지 말 것
  - 조건부 실행 추가하지 말 것 (항상 실행, 단 GLib 빌드 실패 시 누락)

  **Recommended Agent Profile**:
  - **Category**: `quick`
  - **Skills**: `[]`

  **Parallelization**:
  - **Can Run In Parallel**: YES
  - **Parallel Group**: Wave 3
  - **Blocks**: -
  - **Blocked By**: Task 12

  **References**:
  - `cde/programs/dtlogin/config/Xsession.src:301` - 기존 dtstart_appgather 라인
  - `cde/programs/dtlogin/config/Xsession.src:290-310` - 전체 dtstart_* 패턴

  **Acceptance Criteria**:
  - Xsession에서 `dtxdg2appmgr` 자동 실행
  - 로그인 후 `/var/dt/appconfig/appmanager/$DTUSERSESSION/`에 XDG 그룹 존재

  **QA Scenarios (MANDATORY)**:
  ```
  Scenario: Xsession 통합
    Tool: Bash
    Steps:
      1. `grep dtxdg2appmgr cde/programs/dtlogin/config/Xsession.src` → 매치 발견
      2. `grep "dtstart_xdg2appmgr" cde/programs/dtlogin/config/Xsession.src` → 라인 존재
    Expected Result: 자동 실행 라인 추가됨
    Evidence: .sisyphus/evidence/task-13-xsession.txt
  ```

  **Commit**: YES
  - Message: `feat(dtxdg2appmgr): integrate with Xsession for automatic execution`
  - Files: `cde/programs/dtlogin/config/Xsession.src`

- [x] 14. **dt.dt.src의 ReloadApps 액션에 `-r` 추가**

  **What to do**:
  - `cde/programs/types/dt.dt.src` line 318-328의 `ReloadApps` ACTION 수정:
    - EXEC_STRING에 `/usr/dt/bin/dtxdg2appmgr -r` 라인 추가
    - `dtappgather -r` 다음에 위치
  - NLS 매크로 번호 유지 (새 번호 추가 시 충돌 주의)

  **Must NOT do**:
  - 기존 ACTION을 변경하지 말 것 (추가만)
  - 다른 ACTION에 영향 주지 말 것

  **Recommended Agent Profile**:
  - **Category**: `quick`
  - **Skills**: `[]`

  **Parallelization**:
  - **Can Run In Parallel**: YES
  - **Parallel Group**: Wave 3
  - **Blocks**: -
  - **Blocked By**: Task 12

  **References**:
  - `cde/programs/types/dt.dt.src:318-328` - ReloadApps 정의
  - `cde/programs/types/dt.dt.src` - NLS 매크로 번호 컨벤션 (2020번대)

  **Acceptance Criteria**:
  - ReloadApps 액션 실행 시 dtxdg2appmgr도 호출됨
  - dt.dt 컴파일 성공 (tradcpp + gencat)

  **QA Scenarios (MANDATORY)**:
  ```
  Scenario: ReloadApps 액션에 dtxdg2appmgr 포함
    Tool: Bash
    Steps:
      1. `grep -A 10 "ACTION ReloadApps" cde/programs/types/dt.dt.src` → dtxdg2appmgr 라인 포함
      2. `cd cde/programs && make dt.dt 2>&1` → 컴파일 성공
      3. `grep dtxdg2appmgr programs/localized/C/types/dt.dt` → 빌드된 결과에도 포함
    Expected Result: 빌드 + 내용 모두 성공
    Evidence: .sisyphus/evidence/task-14-reloadapps.txt
  ```

  **Commit**: YES
  - Message: `feat(dtxdg2appmgr): add to ReloadApps action`
  - Files: `cde/programs/types/dt.dt.src`

- [x] 15. **사용자/시스템 .desktop 발견 통합 테스트**

  **What to do**:
  - 테스트 시나리오:
    1. 시스템 .desktop (e.g., `/usr/share/applications/firefox.desktop`) 변환 확인
    2. 사용자 .desktop (`$HOME/.local/share/applications/custom.desktop`) 변환 확인
    3. 시스템 우선순위 (system > user) 검증
  - 임시 .desktop 파일 작성하여 시나리오 재현
  - 각 카테고리 (Network, Office, Graphics, Games, AudioVideo, Development) 최소 1개씩 검증

  **Must NOT do**:
  - 실제 시스템의 .desktop 파일을 수정/삭제하지 말 것 (읽기만)
  - 테스트 .desktop은 `/tmp/` 또는 `$HOME/.local/share/applications/`에 작성

  **Recommended Agent Profile**:
  - **Category**: `unspecified-low`
  - **Skills**: `[]`

  **Parallelization**:
  - **Can Run In Parallel**: YES
  - **Parallel Group**: Wave 3
  - **Blocks**: Task 16
  - **Blocked By**: Task 12

  **References**:
  - 실제 시스템의 `/usr/share/applications/*.desktop` 파일들
  - FreeDesktop 테스트 .desktop 예제

  **Acceptance Criteria**:
  - 시스템 + 사용자 .desktop 모두 발견
  - 다양한 카테고리에서 XDG 그룹 생성
  - 시스템 우선순위 정확 (같은 ID면 시스템 우선)

  **QA Scenarios (MANDATORY)**:
  ```
  Scenario: 시스템/사용자 .desktop 통합
    Tool: Bash
    Steps:
      1. `mkdir -p $HOME/.local/share/applications`
      2. `cat > $HOME/.local/share/applications/test-app.desktop <<EOF
         [Desktop Entry]
         Type=Application
         Name=TestApp
         Exec=/usr/bin/true
         Icon=firefox
         Categories=Network;WebBrowser
         EOF`
      3. `dtxdg2appmgr -v 2>&1 | grep test-app` → 발견 로그
      4. `ls /var/dt/appconfig/appmanager/$DTUSERSESSION/Network_XDG/` → test-app 스텁 존재
      5. `dtaction test-app` → /usr/bin/true 실행 (또는 실행 안 됨 확인)
    Expected Result: 사용자 .desktop이 발견되어 변환됨
    Evidence: .sisyphus/evidence/task-15-integration.txt
  ```

  **Commit**: NO (테스트만, 코드 변경 없음)

- [x] 16. **종합 QA 시나리오 + 문서화**

  **What to do**:
  - README.md 보강:
    - 사용법, 옵션 설명
    - 제한사항 (단일 사용자 실행 가정, root 권한, XPM 변환)
    - 문제 해결 (FAQ)
  - `cde/programs/dtxdg2appmgr/dtxdg2appmgr.1.sgm` (man page 소스) 작성
  - `cde/programs/dtxdg2appmgr/dtxdg2appmgr.1` 빌드 파일 추가 (Makefile.am)
  - `cde/doc/en_US.UTF-8/man/Makefile.am`에 등록

  **Must NOT do**:
  - 기존 man page 형식 (SGML) 손상 금지

  **Recommended Agent Profile**:
  - **Category**: `writing`
  - **Skills**: `[]`

  **Parallelization**:
  - **Can Run In Parallel**: YES
  - **Parallel Group**: Wave 3
  - **Blocks**: -
  - **Blocked By**: Task 15

  **References**:
  - `cde/programs/dtappgather` man page (`cde/doc/en_US.UTF-8/guides/man/man1_dt/appgathe.sgm`) - 형식 참조
  - `cde/programs/dtaction/dtaction.1` (있다면) - 또 다른 man page 예시

  **Acceptance Criteria**:
  - `man dtxdg2appmgr` 실행 시 사용법 표시
  - README.md에 명확한 설치/사용 설명

  **QA Scenarios (MANDATORY)**:
  ```
  Scenario: 문서 정확성
    Tool: Bash
    Steps:
      1. `man -l cde/programs/dtxdg2appmgr/dtxdg2appmgr.1` → 사용법 표시
      2. `cat cde/programs/dtxdg2appmgr/README.md` → 명확한 설명
    Expected Result: 문서 가독성 OK
    Evidence: .sisyphus/evidence/task-16-docs.txt
  ```

  **Commit**: YES
  - Message: `docs(dtxdg2appmgr): add README and man page`
  - Files: `cde/programs/dtxdg2appmgr/README.md`, `dtxdg2appmgr.1.sgm`, `Makefile.am`

---

## Final Verification Wave (MANDATORY)

- [x] F1. **Plan Compliance Audit** — `oracle`
  - 모든 Must Have 충족 확인
  - Must NOT Have 위반 없음 확인
  - XDG 호환 .desktop 처리 검증
- [x] F2. **Code Quality Review** — `unspecified-high`
  - `-Wall -Wextra` 경고 0개
  - GLib 메모리 누수 검사 (`valgrind --leak-check=full`)
  - `gcc -std=c11 -pedantic` 통과
- [x] F3. **Real Manual QA** — `unspecified-high`
  - 실제 Linux 시스템에서 실행
  - 적어도 3개 카테고리(Network, Office, Graphics)의 .desktop 변환 확인
  - dtfile에서 XDG 그룹 표시 확인
- [x] F4. **Scope Fidelity Check** — `deep`
  - 모든 Task의 "What to do"와 실제 구현 일치
  - Cross-task contamination 없음
  - 기존 CDE .dt 파일 미변경 확인

---

## Commit Strategy
- **Commit 1**: `feat(dtxdg2appmgr): add configure.ac GLib detection and Makefile.am skeleton`
- **Commit 2**: `feat(dtxdg2appmgr): add .desktop parser, exec parser, and path scanner`
- **Commit 3**: `feat(dtxdg2appmgr): add category mapper, icon resolver, and XPM converter`
- **Commit 4**: `feat(dtxdg2appmgr): add .dt writer, mtime cache, and main orchestrator`
- **Commit 5**: `feat(dtxdg2appmgr): integrate with Xsession and ReloadApps action`
- **Commit 6**: `docs(dtxdg2appmgr): add README and man page`

---

## Success Criteria

### Verification Commands
```bash
# 빌드 확인
./autogen.sh && ./configure && make 2>&1 | tail -5

# 설치 확인
sudo make install

# 실행 확인
$CDE_INSTALLATION_TOP/bin/dtxdg2appmgr -v

# 출력 확인 (모든 XDG 그룹에 _XDG 접미사)
ls -la /var/dt/appconfig/appmanager/$DTUSERSESSION/{Network_XDG,AudioVideo_XDG,Office_XDG,Development_XDG,Game_XDG}/ 2>/dev/null
file /var/dt/appconfig/appmanager/$DTUSERSESSION/Network_XDG/firefox

# .dt 확인
ls -la $HOME/.dt/types/*.dt
cat $HOME/.dt/types/firefox.dt

# 아이콘 확인
file $HOME/.dt/icons/xdg-firefox.{t,m,l}.pm
head -1 $HOME/.dt/icons/xdg-firefox.m.pm

# mtime 캐시 확인
ls -la /var/dt/appconfig/xdg-cache.db

# CDE 내장과 XDG 변환 그룹이 충돌하지 않는지 확인
# (CDE 내장: Office, Internet, Education, Graphics, System, Games, Media_Tools, TeX, Desktop_Apps, ...)
# (XDG 변환: Office_XDG, Network_XDG, AudioVideo_XDG, Game_XDG, ...)
# 두 집합이 disjoint 해야 함
ls /var/dt/appconfig/appmanager/$DTUSERSESSION/ | grep -v _XDG   # CDE 내장만
ls /var/dt/appconfig/appmanager/$DTUSERSESSION/ | grep _XDG       # XDG 변환만
```

### Final Checklist
- [ ] 모든 Must Have 충족
- [ ] 모든 Must NOT Have 부재
- [ ] GLib 의존성 configure.ac에서 자동 감지
- [ ] `make install` 후 모든 파일 정상 설치
- [ ] 실제 .desktop 파일이 정상 변환
- [ ] dtfile에서 XDG 그룹 표시 확인
- [ ] ReloadApps 액션으로 재실행 가능
- [ ] 캐시가 mtime 기반으로 동작
