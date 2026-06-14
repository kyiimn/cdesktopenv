# Draft: XDG → CDE Application Manager 변환 도구

## 사용자 요구사항 (확정)
- **변환 범위**: 전체 시스템 스캔 (XDG 표준 경로 모두)
- **구현 방식**: 런타임 변환 도구 (Autotools 통합 X, 독립 스크립트 X)
  - dtappgather와 유사한 영구 도구
  - 런타임에 XDG 앱을 발견하여 /var/dt/appconfig/appmanager에 동적 추가
- **카테고리 매핑**: 원본 카테고리 유지 (XDG Categories를 그대로 보존)
- **아이콘 처리**: PNG/SVG → XPM 변환

## 핵심 설계 결정
1. **도구 위치**: `cde/programs/dtxdg2appmgr/` (신규 디렉토리)
2. **언어**: C++ (기존 dtappgather와 일관성)
3. **실행 시점**:
   - 사용자 로그인 시 (Xsession에서)
   - 수동 실행 (ReloadApps 액션 등)
4. **출력 위치**: `/var/dt/appconfig/appmanager/$DTUSERSESSION/XDG_<Category>/`
5. **메타데이터**:
   - 원본 .desktop 경로 → $HOME/.dt/xdg-cache/ 에 매핑 캐시
   - 변경 감지 (mtime 체크) → 증분 업데이트

## 조사 필요 항목
- [ ] dtappgather 정확한 인터페이스 (Options, AppManagerDirectory)
- [ ] 기존 CDE ACTION/DATA_ATTRIBUTES .dt 문법 상세
- [ ] XDG .desktop Entry Specification (Version 1.5)
- [ ] CDE에서 사용하는 아이콘 형식 (XPM? .pm? .bm?)
- [ ] PNG → XPM 변환 라이브러리 (ImageMagick? xpm-converter?)
- [ ] 기존 Xpm 라이브러리 사용 가능 여부
- [ ] 중복 이름 처리 방식 (예: 같은 Name을 가진 두 앱)
- [ ] NoDisplay=true, Hidden=true .desktop 필터링
- [ ] i18n 처리 (Name[ko], Comment[ko] 등)

## 핵심 알고리즘 (의사 코드)
```
main():
    xdg_paths = getenv("XDG_DATA_DIRS").split(":") + [getenv("HOME") + "/.local/share"]
    for each path in xdg_paths:
        for each .desktop file in path/applications/:
            desktop = parse_desktop_file(path)
            if desktop.NoDisplay or desktop.Hidden: continue
            if not desktop.Type == "Application": continue
            if not desktop.Exec: continue
            
            category = desktop.Categories[0]  // 첫 번째 카테고리 사용
            group_name = "XDG_" + category.replace(";", "_")
            
            // .dt 파일 생성
            dt_content = generate_dt(desktop)
            write_to("/var/dt/appconfig/appmanager/$DTUSERSESSION/" + group_name + "/.dt/" + desktop.Name + ".dt")
            
            // 액션 스텁 생성 (기존 패턴)
            create_stub(group_name + "/" + desktop.Name, ACTION_PLACEHOLDER)
            
            // 아이콘 변환
            xpm_path = convert_icon(desktop.Icon)
            copy_to("/usr/dt/appconfig/icons/C/" + xpm_path)
```

## TODO 리스트 (계획용)
- Wave 1: 기반 구조
  - Task 1: 새 디렉토리 + Makefile.am + configure.ac 등록
  - Task 2: .desktop 파서 (INI 형식)
  - Task 3: XDG 검색 경로 스캐너
- Wave 2: 변환 엔진
  - Task 4: .dt 파일 생성기
  - Task 5: 액션 스텁 생성기
  - Task 6: 카테고리 → 그룹명 매퍼
- Wave 3: 아이콘 처리
  - Task 7: 아이콘 검색 (XDG Icon Theme Spec)
  - Task 8: PNG → XPM 변환기
- Wave 4: 통합
  - Task 9: Xsession 통합 (dtlogin)
  - Task 10: ReloadApps 액션 추가
  - Task 11: 캐시/증분 업데이트

## 기술적 위험 요소
1. **XPM 변환 품질**: PNG의 알파 채널/안티앨리어싱이 손실될 수 있음
2. **이름 충돌**: 다른 카테고리에 같은 앱 이름이 있을 수 있음
3. **Exec 라인 파싱**: %U, %F 같은 필드 코드 처리 필요
4. **의존성 추적**: XDG 앱이 다른 앱(예: Java)에 의존하는 경우
5. **권한**: /var/dt 쓰기 권한 필요 (dtappgather와 동일)
