# 주식선물투자자 시간별 동향 (s2106)

Visual Studio 2010에서 [OrdaAPI-s2106-StockFuturesInvestorTrends.sln](OrdaAPI-s2106-StockFuturesInvestorTrends.sln)을 열어 Debug 또는 Release Win32로 빌드합니다. 실행 작업 디렉터리는 sdk/bin입니다. 로그인과 SDK DLL 로딩에는 기존 Common을 사용합니다.

## 전문 규격

Header Type A, block 1, attr 1. 입력은 s2106InBlock입니다.

| 출력 구조체 | occurs | 건수·길이 |
|---|---|---|
| s2106OutBlock | 미지정 | 구조체 1개 크기 |
| s2106OutBlock1 | 1 | 수신 길이 ÷ 구조체 크기 |

BlockName에 따라 수신 블록을 구분합니다. 반복 ListView에는 수신 순서대로 최대 200건을 표시하고 실제 수신 건수와 표시 건수를 따로 표시합니다.

## 사용 방법과 확인 사항

s2105의 기초자산코드를 최대 10바이트로 입력합니다. 합계 표는 투자자별 매도·매수·순매수를 표시하고 시간별 표는 일자, 시간 및 투자자별 값을 표시합니다. 수치는 원문 문자열로 표시합니다.

| 입력 | 선택값 | XML 기본값 |
|---|---|---|
| flag | 1 금액, 2 수량 | 1 |
| unit | 1 백만원, 2 억원 | 1 |
| jnilflag | 1 당일, 2 전일 | 1 |

입력과 고정 출력의 cont는 각각 20바이트입니다. 최초 입력은 공백이고 응답 cont는 합계 옆에 표시합니다. SDK 공개 API의 다섯 번째 인자 pszNextKey는 제품의 Type A 헤더에서 연속 플래그 szCont를 설정할 때 포인터의 유무에 사용됩니다. 값 자체는 Type A 헤더에 복사되지 않습니다. XML과 제품 클라이언트에서 응답 szPNFlag의 의미, s2106 본문 cont의 재전달 조건, 그리고 종료 조건을 확인할 수 없어 최초 조회만 제공합니다.

서버에서는 s2105에서 받은 eitem으로 조회해 금액 단위 전환, 수량 단위, 상세 필드의 매매 기준 및 연속조회 계약을 확인합니다.

## 오프라인 검증

이 폴더에서 Python 3으로 tests/run-offline.py를 실행하면 XML과 SDK 구조체의 필드 선언을 비교하고 v100 Debug·Release 빌드와 격리된 fake DLL 테스트를 실행합니다. tests는 기존 Git 제외 규칙의 적용을 받습니다.
