# FNG 재무제표 (s2506)

Visual Studio 2010에서 [OrdaAPI-s2506-FinancialStatements.sln](OrdaAPI-s2506-FinancialStatements.sln)을 열어 Debug 또는 Release Win32로 빌드합니다. 실행 작업 디렉터리는 sdk/bin입니다. 로그인과 SDK DLL 로딩에는 기존 Common을 사용합니다.

## 전문 규격

Header Type A, block 1, attr 1. 입력은 s2506InBlock입니다.

| 출력 구조체 | occurs | 건수·길이 |
|---|---|---|
| s2506OutBlock | 미지정 | 구조체 1개 크기 |
| s2506OutBlock1 | 1 | 수신 길이 ÷ 구조체 크기 |

BlockName에 따라 수신 블록을 구분합니다. 반복 ListView에는 수신 순서대로 최대 200건을 표시하고 실제 수신 건수와 표시 건수를 따로 표시합니다.

## 사용 방법과 확인 사항

FNG 종목코드 gicode는 최대 7바이트입니다. 입력 문자열에 접두사를 붙이거나 6자리 코드로 변환하지 않습니다. gs_ym은 회계년월 6바이트이며, 입력한 값은 YYYYMM과 월 범위로 검사합니다. XML에 gs_ym 기본값과 필수 여부가 명시되지 않아 비워 두면 공백 6바이트를 전송합니다. gubun에도 XML 기본값이 없어 사용자가 선택합니다.

| 코드 | codes.xml의 S2506 설명 |
|---|---|
| 0 | 요약대차대조표 |
| 1 | 요약손익계산서 |
| 2 | 현금흐름표 |

고정 출력 gsym1~5를 반복 출력 data1~5의 대응 열 제목으로 사용합니다. 고정 출력이 뒤에 도착하면 이미 표시한 행에 해당하는 열 제목을 갱신합니다. 재무항목 gubunnm과 각 기간의 원문 수치를 표시하며 음수, 공백, 0, 소수점 및 특수값은 문자열로 유지합니다.

실제 FNG 종목코드 형식, 빈 회계년월의 서버 처리, 금액 단위와 소수 자릿수는 서버에서 확인합니다.

전문에 본문 연속키가 정의되어 있지 않아 최초 조회만 제공합니다.

## 오프라인 검증

이 폴더에서 Python 3으로 tests/run-offline.py를 실행하면 XML과 SDK 구조체의 필드 선언을 비교하고 v100 Debug·Release 빌드와 격리된 fake DLL 테스트를 실행합니다. tests는 기존 Git 제외 규칙의 적용을 받습니다.
