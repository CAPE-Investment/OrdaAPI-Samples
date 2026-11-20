# 관심종목현재가 조회 (s1032)

요청 프로젝트명은 `OrdaAPI-s1032-WatchlistQuotes`입니다. 입력 조립 계약이 확인될 때까지 실행 프로젝트와 요청 전문 조립을 보류합니다.

SDK XML과 헤더의 필드 순서, 길이 및 속성 바이트를 대조했습니다. A형, block=1, attr=1입니다. `s1032InBlock`은 nrec 4바이트와 속성 바이트로 총 5바이트입니다. `s1032InBlock1`은 smktgb(2), gubun(2), shcode(16), exchange(1)와 각 속성 바이트로 총 25바이트입니다. XML에는 두 번째 입력 블록의 occurs 선언이 없습니다. `s1032OutBlock`은 occurs=1이며 756바이트입니다.

smktgb의 XML 기본값은 KR이며 코드표 참조가 없습니다. gubun은 S1032의 01 코스피 및 02 코스닥이며 기본값 선언이 없습니다. exchange는 EXCHANGE를 참조하고 XML 기본값은 3입니다.

제품의 `CSDKWnd::Request`는 첫 번째 입력 블록만 직렬화합니다. 저수준 `OrdaAPI_RequestData`와 `CComGate::RequestData` 경로는 호출자가 제공한 본문과 길이를 전달합니다. 이 경로에서 s1032 전용 반복 개수나 두 번째 입력 블록 조립 규칙을 찾을 수 없었습니다. 5 + 25 × N은 두 SDK 구조체를 연달아 놓았을 때의 길이입니다. 실제 요청 길이, nrec 패딩, 별도 반복 카운트나 헤더의 유무 및 서버의 최대 종목 수를 확인하려면 정상 다종목 요청 전문 또는 서비스 사양이 필요합니다.
