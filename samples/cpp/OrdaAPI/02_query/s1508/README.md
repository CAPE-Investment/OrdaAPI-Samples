# 만기월 조회 (s1508)

Visual Studio 2010에서 `OrdaAPI-s1508-ExpirationMonths.sln`을 열고 Debug 또는 Release Win32로 빌드합니다. 실행 작업 디렉터리는 `sdk/bin`입니다.

dummy는 1바이트 공백이며 입력 UI 없이 구조체 전체를 전달합니다. lastym은 만기월, lastnm은 만기월명입니다. 수신 순서를 유지합니다. attr 속성이 없어 별도 속성 바이트가 없습니다.

최초 조회를 제공합니다. 수치 표기는 전문의 문자를 유지합니다. 가격·수량의 단위와 유효한 업무 코드 범위는 실제 서버에서 확인합니다.

로컬 오프라인 검증은 `python tests/run-offline.py`로 실행합니다. 테스트 파일은 기존 Git 제외 규칙을 따릅니다.
