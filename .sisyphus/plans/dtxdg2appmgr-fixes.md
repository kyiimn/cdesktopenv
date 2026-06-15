# dtxdg2appmgr 보안 및 품질 수정 계획

## TL;DR

> **Quick Summary**: Metis + Oracle 두 전문가 감사를 통해 발견된 **2개 CRITICAL + 11개 MAJOR 버그**를 모두 수정합니다. 가장 시급한 것은 xpm_converter.c의 **셸 인젝션 취약점**(악성 .desktop 파일이 임의 코드 실행 가능)과 Xsession.src의 **자동 실행 누락**입니다.
>
> **Deliverables**:
> - 셸 인젝션 취약점 제거 (xpm_converter.c: g_spawn_sync with argv 사용)
> - Xsession.src에 StartFirst 호출 추가
> - 11개 MAJOR 버그 모두 수정
> - 0-warning 깨끗한 빌드
> - 수정 후 통합 테스트 통과
>
> **Estimated Effort**: Medium
> **Parallel Execution**: YES - 3 waves
> **Critical Path**: Shell injection fix → Xsession → Logic bugs → Verification

---

## Context

### 발견된 문제 (감사 결과)

#### CRITICAL (즉시 수정 필요)

1. **A-CRITICAL-1 / B1**: `xpm_converter.c:62,96` — `g_spawn_command_line_sync`로 raw `src_path`를 `/bin/sh -c`에 전달. 악성 `.desktop` 파일이 `Icon=/tmp/x;rm -rf ~;echo .png`로 임의 셸 명령 실행 가능. **CVSS 8.6 (RCE)**

2. **C1**: `Xsession.src:303` — `dtstart_xdg2appmgr` 변수를 설정했지만 `StartFirst dtstart_xdg2appmgr` 호출이 없음. **자동 실행 자체가 안 됨.**

#### MAJOR (반드시 수정)

3. **B2**: `dtxdg2appmgr.c:350-368` — 스텁 쓰기 실패 시에도 캐시가 업데이트되어, 다음 실행에서 항목이 건너뛰어짐. **사용자에게 "분명 등록됐는데 보이지 않는" 앱 발생**

4. **C2**: `dtxdg2appmgr.c:114` — `get_appmanager_dir`가 `opts->output_dir`를 사용하지만, CDE appmanager는 `/var/dt/appconfig/appmanager/$DTUSERSESSION/`을 봄. **스텁이 잘못된 경로에 쓰여짐**

5. **C3**: `desktop_parser.c:184-200` — `Terminal=true` 필터가 누락되어 터미널 앱이 터미널 없이 실행됨

6. **C5**: `path_scanner.c:100` — `g_dir_open`이 비재귀적. 일부 배포판의 `applications/kde/`, `applications/gnome/` 같은 하위 디렉토리 누락

7. **C7 / D-MAJOR-1**: `dt_writer.c:208-213` — 그룹 .dt에 `ACTION Print` 누락. `OPEN_*_APPGROUP` 블록도 누락. CDE 표준 위반

8. **C9**: `exec_parser.c` — `%i`를 `--icon <name>`로 변환. GTK-특화. CDE는 .dt의 ICON 필드 사용. 비표준 변환

9. **A1**: `dtxdg2appmgr.c:259` — `g_strjoinv(";", ...)` 결과가 누수됨

10. **C8**: `dtxdg2appmgr.c:283` — `Name=` 없는 .desktop은 매번 실패하지만 캐시되지 않아 매 실행마다 재시도

11. **A-MAJOR-6**: `dtxdg2appmgr.c:57-64` — `signal()` (SysV) 사용, `SIGTERM` 미처리 → 종료 시 캐시 미저장

12. **C-MAJOR-2**: `desktop_parser.c:103` — `G_KEY_FILE_NONE`이 너무 엄격. 실제 .desktop 파일들이 무시됨 (trailing whitespace, BOM 등)

### Inherited Wisdom (이전 세션)

- 모든 모듈은 빌드 성공 (commit `7b90c6df2`)
- 79개 앱, 15개 그룹, 51개 아이콘 생성 확인
- 모든 XDG 그룹에 `_XDG` 접미사 (이미 검증됨)
- `dtxdg2appmgr_` 접두사 일관성 사용
- LGPL v2 라이선스 헤더 모든 파일
- 빌드 명령어: `cd cde/programs/dtxdg2appmgr && make clean && make`

---

## Work Objectives

### Core Objective
Metis + Oracle 전문가 감사를 통해 발견된 **모든 CRITICAL 및 MAJOR 보안/논리 버그 수정**하여, 실제 CDE 환경에서 안전하고 안정적으로 동작하는 도구로 완성.

### Concrete Deliverables
1. `cde/programs/dtxdg2appmgr/xpm_converter.c` — 셸 인젝션 제거
2. `cde/programs/dtlogin/config/Xsession.src` — StartFirst 호출 추가
3. `cde/programs/dtxdg2appmgr/dtxdg2appmgr.c` — 메모리 누수, 캐시 로직, 신호 처리 수정
4. `cde/programs/dtxdg2appmgr/desktop_parser.c` — G_KEY_FILE 플래그, Terminal 필터
5. `cde/programs/dtxdg2appmgr/path_scanner.c` — 재귀 스캔
6. `cde/programs/dtxdg2appmgr/dt_writer.c` — Print 액션, OPEN_X_APPGROUP 블록
7. `cde/programs/dtxdg2appmgr/exec_parser.c` — %i 변환 제거

### Definition of Done
- [ ] 모든 CRITICAL/MAJOR 버그 0개 잔존
- [ ] `make clean && make` 0-warning
- [ ] 통합 테스트 (dry-run + real run) 모두 통과
- [ ] 악성 .desktop 파일 (셸 인젝션 시도) 안전하게 거부됨
- [ ] `Xsession.src`에 `StartFirst dtstart_xdg2appmgr` 존재

### Must Have
- 모든 CRITICAL 보안 취약점 제거
- 모든 MAJOR 로직 버그 수정
- CDE 표준 .dt 형식 준수 (Print 액션, OPEN_X_APPGROUP)
- 재귀 디렉토리 스캔
- SIGTERM/SIGPIPE 적절 처리

### Must NOT Have (Guardrails)
- 기존에 작동하던 기능 회귀 금지
- .desktop 파싱의 spec 호환성 손상 금지
- CDE 표준 형식에서 벗어나는 변경 금지

---

## Verification Strategy (MANDATORY)

### Test Decision
- **Infrastructure exists**: NO (CDE 자동 테스트 인프라 없음)
- **Automated tests**: NONE
- **Test Strategy**: Agent-Executed QA Scenarios (수동 + 에이전트 실행)

### QA Policy
- **Shell injection test**: 악성 .desktop (Icon=`/tmp/x;rm -rf ~`) 생성 → 안전하게 거부되는지
- **Xsession test**: `StartFirst dtstart_xdg2appmgr` 라인 존재 확인
- **Cache failure test**: stub 디렉토리 read-only 설정 → 캐시 미업데이트 확인
- **Recursive scan test**: `applications/kde/test.desktop` 생성 → 발견 확인
- **Print action test**: 생성된 group .dt에 `ACTION Print` 존재 확인
- **OPEN_X_APPGROUP test**: group .dt에 `OPEN_X_APPGROUP` 존재 확인
- **Terminal filter test**: `Terminal=true` .desktop이 스킵되는지
- **End-to-end test**: 79+ 앱, 15+ 그룹, 0개 이중 _XDG

---

## Execution Strategy

### Parallel Execution Waves

```
Wave 1 (CRITICAL - 보안/통합):
├── Task 1: xpm_converter.c 셸 인젝션 제거 (g_spawn_sync with argv)
└── Task 2: Xsession.src에 StartFirst 추가

Wave 2 (MAJOR - 핵심 로직):
├── Task 3: dtxdg2appmgr.c - 캐시 실패 시 continue + 메모리 누수 수정
├── Task 4: dtxdg2appmgr.c - sigaction + SIGTERM/SIGPIPE 처리
└── Task 5: desktop_parser.c - G_KEY_FILE_KEEP 플래그 + Terminal 필터

Wave 3 (MAJOR - 검색/출력):
├── Task 6: path_scanner.c - 재귀 디렉토리 스캔
├── Task 7: dt_writer.c - Print 액션 + OPEN_X_APPGROUP 블록
└── Task 8: exec_parser.c - %i 변환 제거

Wave FINAL (검증):
├── Task 9: 통합 테스트 - 셸 인젝션 방어
├── Task 10: 통합 테스트 - 모든 기능
└── Task 11: 빌드 + 0-warning 확인
```

### Dependency Matrix

- **1 (Shell injection)**: -
- **2 (Xsession)**: -
- **3 (main loop)**: -
- **4 (signals)**: -
- **5 (parser)**: -
- **6 (recursive scan)**: -
- **7 (dt writer)**: -
- **8 (exec parser)**: -
- **9-11 (verification)**: 1-8

---

## TODOs

- [x] 1. **xpm_converter.c 셸 인젝션 제거 (CRITICAL)**

  **What to do**:
  - `convert_with_imagemagick` 함수를 `g_spawn_sync` + argv 배열로 변경
  - argv: `["convert", "-background", "none", "-resize", "WxH", src, dst]`
  - `g_spawn_command_line_sync` 제거 (셸 우회)
  - `convert_with_netpbm`은 파이프라인이 필요하므로 `g_shell_quote`로 escape

  **Must NOT do**:
  - 기존 fallback 동작 변경 (ImageMagick → netpbm)
  - g_spawn_* 에러 처리 패턴 변경

  **Recommended Agent Profile**:
  - **Category**: `quick`
  - **Skills**: `[]`

  **References**:
  - GLib docs: g_spawn_sync with argv array
  - `g_shell_quote` for paths with shell metacharacters

  **Acceptance Criteria**:
  - `convert -background none -resize WxH src dst` argv 형식 정확
  - 빌드 0-warning
  - 기존 3-size XPM 생성 동작 유지

  **QA Scenarios (MANDATORY)**:
  ```
  Scenario: 악성 Icon= 필드로 셸 인젝션 시도
    Tool: Bash
    Preconditions: 테스트 사용자 디렉토리 ($HOME/.local/share/applications/)
    Steps:
      1. `cat > /tmp/evil.desktop <<EOF
         [Desktop Entry]
         Type=Application
         Name=Evil
         Icon=/tmp/x;touch /tmp/PWNED;echo .png
         Categories=Network
         EOF`
      2. `dtxdg2appmgr --dry-run -v 2>&1 | grep -c "PWNED"` → 0
      3. `ls /tmp/PWNED 2>&1` → "No such file or directory"
    Expected Result: PWNED 파일 생성 안 됨
    Evidence: .sisyphus/evidence/wave1-shell-injection.txt
  ```

- [x] 2. **Xsession.src StartFirst 호출 추가 (CRITICAL)**

  **What to do**:
  - `cde/programs/dtlogin/config/Xsession.src` line 505 근처에 추가:
    `StartFirst dtstart_xdg2appmgr`
  - 기존 `StartFirst dtstart_appgather` 패턴 따름

  **Must NOT do**:
  - 다른 dtstart_* StartFirst 라인 변경 금지

  **Recommended Agent Profile**:
  - **Category**: `quick`

  **References**:
  - `Xsession.src:505` 기존 `StartFirst dtstart_appgather` 패턴

  **Acceptance Criteria**:
  - `grep "StartFirst dtstart_xdg2appmgr" Xsession.src` → 1개 매치

  **QA Scenarios**:
  ```
  Scenario: StartFirst 호출 존재 확인
    Tool: Bash
    Steps:
      1. `grep "StartFirst dtstart_xdg2appmgr" cde/programs/dtlogin/config/Xsession.src` → 1 match
    Expected Result: 매치 발견
    Evidence: .sisyphus/evidence/wave1-xsession-startfirst.txt
  ```

- [x] 3. **dtxdg2appmgr.c 캐시 실패 + 메모리 누수 수정 (MAJOR)**

  **What to do**:
  - Line 350-368: `ensure_dir` 실패 또는 `write_appmanager_stub` 실패 시 `continue` 추가
  - Line 259: `g_strjoinv` 결과를 `gchar *joined`에 저장 후 `g_free`
  - Line 283-286: 중복 sanitize 제거
  - Line 274: `entry->id` (예: "firefox.desktop")의 `.desktop` 접미사 제거 후 sanitize

  **Must NOT do**:
  - 정상 경로 동작 변경 금지

  **Recommended Agent Profile**:
  - **Category**: `unspecified-high`

  **Acceptance Criteria**:
  - read-only 그룹 디렉토리에서 실행 시 해당 항목이 다음 실행에서도 재시도됨
  - `g_strjoinv` 누수 제거 (valgrind clean)
  - `Name=` 없는 .desktop이 `firefox_desktop` 형태로 fallback 액션명 생성

  **QA Scenarios**:
  ```
  Scenario: read-only 디렉토리에서 스텁 실패 후 재시도
    Tool: Bash
    Preconditions: 출력 디렉토리가 read-only
    Steps:
      1. `dtxdg2appmgr -o /root/foo 2>&1; echo $?` → exit 1
      2. `cat /var/dt/xdg-cache.db | wc -l` → 0 lines (캐시 갱신 안 됨)
    Expected Result: 실패한 항목 캐시 미저장
    Evidence: .sisyphus/evidence/wave3-cache-on-failure.txt
  ```

- [x] 4. **신호 처리 sigaction + SIGTERM/SIGPIPE (MAJOR)**

  **What to do**:
  - `signal()` → `sigaction()` 교체
  - `SIGTERM`, `SIGHUP` 핸들러 추가 (캐시 저장 후 종료)
  - `SIGPIPE` 무시 (xpm_converter 출력 파이프 끊김 대비)

  **Must NOT do**:
  - SIGKILL은 잡을 수 없으므로 처리 불필요

  **Recommended Agent Profile**:
  - **Category**: `unspecified-high`

  **References**:
  - POSIX sigaction(2) man page
  - CDE 기존 sigaction 사용 패턴 (예: dtsession)

  **Acceptance Criteria**:
  - `kill -TERM $pid` 시 캐시 저장 후 종료
  - 빌드 0-warning

- [x] 5. **desktop_parser.c G_KEY_FILE 플래그 + Terminal 필터 (MAJOR)**

  **What to do**:
  - Line 103: `G_KEY_FILE_NONE` → `G_KEY_FILE_KEEP_COMMENTS | G_KEY_FILE_KEEP_TRANSLATIONS`
  - `desktop_should_include`에 Terminal=true 필터 추가

  **Must NOT do**:
  - 기존 필터 (Type, NoDisplay, Hidden) 변경 금지

  **Recommended Agent Profile**:
  - **Category**: `quick`

  **Acceptance Criteria**:
  - `Terminal=true` .desktop이 변환 결과에 없음
  - trailing whitespace 있는 .desktop 파싱 성공

  **QA Scenarios**:
  ```
  Scenario: Terminal=true 필터 동작
    Tool: Bash
    Preconditions: 임시 .desktop
    Steps:
      1. `cat > /tmp/terminal.desktop <<EOF
         [Desktop Entry]
         Type=Application
         Name=TerminalApp
         Exec=vim
         Terminal=true
         Categories=Utility
         EOF`
      2. `dtxdg2appmgr --dry-run -v 2>&1 | grep terminal` → "SKIP (filtered)"
    Expected Result: Terminal=true 항목 제외
    Evidence: .sisyphus/evidence/wave2-terminal-filter.txt
  ```

- [x] 6. **path_scanner.c 재귀 디렉토리 스캔 (MAJOR)**

  **What to do**:
  - `g_dir_open` + `g_dir_read_name` 비재귀 루프를 재귀 함수로 교체
  - symlink loop 방지 (visited path set)
  - subdir (kde/, gnome/ 등) 진입

  **Must NOT do**:
  - symlink follow 금지 (보안)
  - 깊이 제한 무한 (stack overflow)

  **Recommended Agent Profile**:
  - **Category**: `medium`

  **References**:
  - XDG Base Directory Spec
  - `GFileEnumerator` docs

  **Acceptance Criteria**:
  - `applications/kde/test.desktop` 발견
  - `applications/gnome/test.desktop` 발견
  - symlink loop 무한 루프 없음

  **QA Scenarios**:
  ```
  Scenario: 하위 디렉토리 .desktop 발견
    Tool: Bash
    Steps:
      1. `mkdir -p $HOME/.local/share/applications/kde`
      2. `cat > $HOME/.local/share/applications/kde/test.desktop <<EOF
         [Desktop Entry]
         Type=Application
         Name=KDETest
         Categories=Utility
         EOF`
      3. `dtxdg2appmgr --dry-run -v 2>&1 | grep KDETest` → 발견 로그
    Expected Result: 하위 디렉토리 .desktop 발견
    Evidence: .sisyphus/evidence/wave2-recursive-scan.txt
  ```

- [x] 7. **dt_writer.c Print 액션 + OPEN_X_APPGROUP 블록 (MAJOR)**

  **What to do**:
  - `write_group_dt` 함수에 `ACTION Print` 추가 (PrintAppGroup 매핑)
  - `DATA_ATTRIBUTES OPEN_X_APPGROUP` 블록 추가 (CDE 표준 형식)
  - `ICON` 필드를 per-group 아이콘으로 변경 (CDE 표준: TeXGroup 등)
  - `DATA_ATTRIBUTES` 이름을 `*Appgroup` 접미사로 변경

  **Must NOT do**:
  - 기존 PrintAppGroup, OpenAppGroup 매크로 변경 금지 (CDE 시스템)
  - PATH_PATTERN 변경 금지 (앱매니저 매칭 위해)

  **Recommended Agent Profile**:
  - **Category**: `unspecified-high`

  **References**:
  - `cde/programs/types/TeX.dt` - 표준 그룹 .dt 예시
  - `cde/programs/types/dtappman.dt.src` - 표준 appgroup 형식

  **Acceptance Criteria**:
  - 생성된 group .dt에 `ACTION Print` 존재
  - `DATA_ATTRIBUTES OPEN_X_APPGROUP` 블록 존재
  - CDE dtfile에서 그룹 우클릭 시 "Print" 메뉴 표시

  **QA Scenarios**:
  ```
  Scenario: Print 액션 생성 확인
    Tool: Bash
    Steps:
      1. `dtxdg2appmgr -o /tmp/test`
      2. `grep -A 3 "ACTION Print" /tmp/test/Network_XDG.dt` → Print 블록 매치
      3. `grep "OPEN_X_APPGROUP" /tmp/test/Network_XDG.dt` → 매치
    Expected Result: Print + OPEN 블록 모두 존재
    Evidence: .sisyphus/evidence/wave3-print-action.txt
  ```

- [x] 8. **exec_parser.c %i 변환 제거 (MAJOR)**

  **What to do**:
  - `%i` (StartupWMClass → --icon) 변환 로직 제거
  - XDG 표준: `%i`는 `--icon <name>`을 launch 시 추가 (GTK 컨벤션)
  - CDE는 .dt의 ICON 필드를 직접 사용하므로 변환 불필요
  - 단순히 `%i`를 빈 문자열로 대체하거나 무시

  **Must NOT do**:
  - 다른 field code (%f, %F, %u, %U, %c, %k, %%) 처리 변경 금지

  **Recommended Agent Profile**:
  - **Category**: `quick`

  **Acceptance Criteria**:
  - `Exec=firefox %i` → `firefox %Arg_1%` (또는 `firefox` 단독)
  - GTK 앱과 비-GTK 앱 모두 동일하게 처리

- [x] 9. **통합 테스트: 셸 인젝션 방어**

  **Tool**: Bash
  **Preconditions**: 빌드 완료, 테스트 .desktop 작성
  **Steps**:
    1. 악성 .desktop 작성 (`Icon=/tmp/x;touch /tmp/PWNED;echo .png`)
    2. `dtxdg2appmgr --dry-run -v` 실행
    3. `/tmp/PWNED` 파일이 생성되지 않았는지 확인
  **Expected Result**: PWNED 파일 없음
  **Evidence**: `.sisyphus/evidence/wave-final-shell-injection-defense.txt`

- [x] 10. **통합 테스트: 모든 기능**

  **Tool**: Bash
  **Steps**:
    1. `make clean && make` → 0-warning
    2. `dtxdg2appmgr -v -o /tmp/final -i /tmp/final-icons -c /tmp/final-cache` → 79+ apps
    3. `find /tmp/final -name "*_XDG_XDG*"` → 0개 (이중 _XDG 없음)
    4. 모든 .dt 파일에 `ACTION Print` 포함
    5. `OPEN_X_APPGROUP` 블록 포함
  **Expected Result**: 모든 기능 정상
  **Evidence**: `.sisyphus/evidence/wave-final-integration.txt`

- [x] 11. **빌드 + 0-warning 확인**

  **Tool**: Bash
  **Steps**:
    1. `cd cde/programs/dtxdg2appmgr && make clean && make 2>&1 | grep -ci warning` → 0
    2. `ldd dtxdg2appmgr` → glib-2.0 링크 확인
    3. `nm dtxdg2appmgr | grep -c dtxdg2appmgr_` → 10+ 심볼
  **Expected Result**: 깨끗한 빌드
  **Evidence**: `.sisyphus/evidence/wave-final-build-clean.txt`

---

## Final Verification Wave (MANDATORY)

- [x] F1. **Plan Compliance Audit** — 모든 11개 TODO 완료 확인
- [x] F2. **Code Quality Review** — 0-warning, 0-shell-injection, 0-cache-bug
- [x] F3. **Real Manual QA** — 악성 .desktop 방어, 79+ apps 처리, _XDG suffix 100%
- [x] F4. **Scope Fidelity Check** — 기존 CDE .dt 파일 미변경

---

## Commit Strategy

- **Commit 1**: `fix(dtxdg2appmgr): critical - shell injection in xpm_converter`
- **Commit 2**: `fix(dtxdg2appmgr): critical - Xsession StartFirst invocation`
- **Commit 3**: `fix(dtxdg2appmgr): cache not updated on stub-write failure, memory leak fixes`
- **Commit 4**: `fix(dtxdg2appmgr): sigaction + SIGTERM/SIGPIPE handling`
- **Commit 5**: `fix(dtxdg2appmgr): G_KEY_FILE_KEEP flags + Terminal filter`
- **Commit 6**: `fix(dtxdg2appmgr): recursive directory scan in path_scanner`
- **Commit 7**: `fix(dtxdg2appmgr): CDE-conformant group .dt with Print action + OPEN_X_APPGROUP`
- **Commit 8**: `fix(dtxdg2appmgr): remove GTK-specific %i transformation`

---

## Success Criteria

### Verification Commands
```bash
# CRITICAL #1: Shell injection defense
cat > /tmp/evil.desktop <<EOF
[Desktop Entry]
Type=Application
Name=Evil
Icon=/tmp/x;touch /tmp/PWNED;echo .png
Categories=Network
EOF
HOME=/tmp/fakehome dtxdg2appmgr --dry-run -v 2>&1
ls /tmp/PWNED 2>&1  # should NOT exist

# CRITICAL #2: Xsession integration
grep "StartFirst dtstart_xdg2appmgr" cde/programs/dtlogin/config/Xsession.src

# MAJOR #3: Cache failure handling
chmod 000 /tmp/test-stub-dir
dtxdg2appmgr -o /tmp/test-stub-dir 2>&1
chmod 755 /tmp/test-stub-dir
cat /var/dt/xdg-cache.db | wc -l  # should NOT have the failed entry

# MAJOR #5: Terminal filter
cat > /tmp/term.desktop <<EOF
[Desktop Entry]
Type=Application
Name=TermApp
Exec=vim
Terminal=true
EOF
dtxdg2appmgr --dry-run -v 2>&1 | grep TermApp  # should say "SKIP (filtered)"

# MAJOR #6: Recursive scan
mkdir -p $HOME/.local/share/applications/kde
cat > $HOME/.local/share/applications/kde/test.desktop <<EOF
[Desktop Entry]
Type=Application
Name=KDETest
Categories=Utility
EOF
dtxdg2appmgr --dry-run -v 2>&1 | grep KDETest  # should be found

# MAJOR #7: Print action
dtxdg2appmgr -o /tmp/test
grep "ACTION Print" /tmp/test/Network_XDG.dt  # should match
grep "OPEN_X_APPGROUP" /tmp/test/Network_XDG.dt  # should match

# End-to-end
make clean && make  # 0 warnings
dtxdg2appmgr -v -o /tmp/final
find /tmp/final -name "*_XDG_XDG*"  # should be empty
```

### Final Checklist
- [x] 0개 셸 인젝션 취약점
- [x] 0개 캐시-실패 정합성 버그
- [x] 0개 빌드 경고
- [x] 100% `_XDG` 접미사 적용
- [x] Xsession 자동 실행
- [x] 재귀 스캔
- [x] Terminal 필터
- [x] Print 액션 + OPEN_X_APPGROUP
