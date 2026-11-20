# 주식선물 현재가 조회 (s3010)

Visual Studio 2010에서 [OrdaAPI-s3010-StockFuturesQuote.sln](OrdaAPI-s3010-StockFuturesQuote.sln)을 열고 Debug 또는 Release Win32로 빌드합니다. 실행 작업 디렉터리는 sdk/bin입니다. 기존 Common이 SDK DLL 로딩과 접속·로그인을 담당합니다.

Header Type A, block 1, attr 1입니다. 입력은 s3010InBlock입니다. SDK의 4인자 최초 조회 호출을 사용합니다.

| 출력 | occurs | 처리 |
|---|---|---|
| s3010OutBlock | 미지정 | 고정 구조체 1개 |

shcode 입력은 8바이트 선물 단축코드입니다. 영문 대문자 및 숫자 8자를 입력하며 선행 0을 유지합니다. 출력에 별도로 포함되는 shcode는 6바이트입니다. 응답 코드의 업무상 대응 관계와 유효 종목은 서버에서 확인하십시오.

시세·호가 집계 표에 이름, 현재가, 전일대비, 등락률, 거래량, 총 잔량·건수, 수신 시간, 응답 코드를 표시합니다. 별도의 호가 표는 매도·매수 호가, 잔량, 건수를 1~10단계별로 배치합니다. 전체 74필드를 표시합니다. s3009는 focode 8바이트를 받으며 시고저·미결제약정·기초자산 정보를 포함하고 기초자산 호가는 두 필드입니다. s3010에는 본문 연속키가 없습니다.

모든 수신값은 실제 SDK 구조체에서 고정 폭 문자열로 읽으며 단위를 변환하지 않습니다.

오프라인 테스트는 Python 3으로 tests/run-offline.py를 실행합니다. 버전 100 도구로 Debug·Release를 빌드하고 격리된 fake DLL과 WinAPI 메시지 테스트를 실행합니다. tests 디렉터리에는 기존 Git 제외 규칙이 적용됩니다.
