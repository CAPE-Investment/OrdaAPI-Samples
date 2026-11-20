# 회원사 리스트 조회 (s2523)

Visual Studio 2010에서 [OrdaAPI-s2523-BrokerList.sln](OrdaAPI-s2523-BrokerList.sln)을 열어 Debug 또는 Release Win32로 빌드합니다. 실행 작업 디렉터리는 sdk/bin입니다. 로그인과 SDK DLL 로딩에는 기존 Common을 사용합니다.

## 전문 규격

Header Type A, block 1, attr 미지정(속성 바이트 없음). 입력은 s2523InBlock입니다.

| 출력 구조체 | occurs | 건수·길이 |
|---|---|---|
| s2523OutBlock1 | 1 | 수신 길이 ÷ 구조체 크기 |

BlockName에 따라 수신 블록을 구분합니다. 반복 ListView에는 수신 순서대로 최대 200건을 표시하고 실제 수신 건수와 표시 건수를 따로 표시합니다.

## 사용 방법과 확인 사항

로그인한 뒤 조회 버튼을 누르면 거래원코드, 거래원명, 구분 원문이 ListView에 표시됩니다. 조회 조건은 없습니다.

입력 dummy에는 XML 기본값인 공백 1바이트를 전송합니다. 첫 출력 블록의 실제 이름은 s2523OutBlock1입니다. 거래원코드의 선행 0을 보존합니다. XML과 codes.xml에 wtradgubun의 값별 설명이 없어 원문을 표시합니다. 구분값의 업무 의미는 서버 자료로 확인합니다.

전문에 본문 연속키가 정의되어 있지 않아 최초 조회만 제공합니다.

## 오프라인 검증

이 폴더에서 Python 3으로 tests/run-offline.py를 실행하면 XML과 SDK 구조체의 필드 선언을 비교하고 v100 Debug·Release 빌드와 격리된 fake DLL 테스트를 실행합니다. tests는 기존 Git 제외 규칙의 적용을 받습니다.
