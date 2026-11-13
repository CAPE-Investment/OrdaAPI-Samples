# 현물주문 Sample

기존 분류에 따라 네 독립 프로젝트를 제공합니다.

| 서비스 | 기능 | 안내 |
| --- | --- | --- |
| b8000 | 현물매수주문 | [StockBuy](new-order/b8000-stock-buy/README.md) |
| b8001 | 현물매도주문 | [StockSell](new-order/b8001-stock-sell/README.md) |
| b8002 | 현금정정주문 | [StockAmend](amend-order/b8002-stock-amend/README.md) |
| b8003 | 현금취소주문 | [StockCancel](cancel-order/b8003-stock-cancel/README.md) |

각 안내문에 빌드·실행·입력 필드·API·응답·메모리 해제를 정리했습니다. VS2010 v100 / Win32 / MultiByte / C++03 / 순수 WinAPI를 사용하고 공통 Helper와 SDK Runtime은 그대로 참조합니다.

요청은 실제 구조체를 0x20으로 초기화하고 `RequestData(window, NAME_b800x, &input, sizeof(b800xInBlock))`로 전달합니다. 수량은 숫자 검증 후 왼쪽을 0으로 채웁니다. 가격이 있는 b8000~b8002는 정수 입력을 최대 10자리로 제한하고 13바이트 필드에 정수부 Zero Padding과 `.00`을 적용합니다. 같은 세 서비스의 CndiPrc도 동일 형식이며, 조건가격의 빈 입력과 XML 기본값은 `0000000000.00`으로 전송합니다. UI 기본값은 유지합니다. b8003에는 주문가격·조건가격 필드가 없습니다. 나머지는 공백 패딩입니다. 응답은 ID·서비스·정확한 길이 검증 후 두 고정 블록을 구조체로 접근합니다. 별도 Assign 객체나 반복 블록은 없습니다.

**실제 서버에서는 주문이 발생할 수 있습니다.** 주문 버튼과 확인 창을 통과해야 전송하며 다음 전송에는 별도 새 주문 준비가 필요합니다. 자동 재주문하지 않고 타임아웃은 결과 미확인으로 처리합니다.

Sample은 비밀번호를 마스킹하지만 SDK 자체는 g_Cfg.bLog가 켜지면 전송 전문을 기록할 수 있습니다. 실제 사용 전 SDK 로깅 정책을 확인하십시오. SDK Runtime과 system.ini는 수정하지 않습니다.

이번 가독성 정리는 실행 동작을 변경하지 않았으며 실제 서버 접속·주문 테스트는 수행하지 않았습니다.
