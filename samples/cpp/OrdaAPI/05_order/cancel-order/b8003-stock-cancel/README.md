# b8003 — 현금취소주문

[main.cpp](OrdaAPI-b8003-StockCancel/main.cpp)에서 입력 구성, 요청 API, 응답 처리와 종료 순서를 확인할 수 있습니다.

## 빌드와 실행

- Visual Studio 2010 / v100 / Win32 / MultiByte / C++03 / 순수 WinAPI입니다.
- `OrdaAPI-b8003-StockCancel.sln`을 열고 Debug 또는 Release / Win32로 빌드합니다.
- SDK Header는 `sdk/include`, 전문 구조체는 `sdk/structs/cpp`를 참조합니다. `ORDAAPI_INCLUDE_DIR` 재정의를 지원합니다.
- 디버거 작업 폴더는 저장소의 `sdk/bin`입니다. IDE 밖에서 실행할 때도 이 폴더를 작업 폴더로 지정합니다. SDK DLL을 Sample마다 복사하지 않습니다.
- 소스는 UTF-8 BOM으로 저장합니다. 화면은 Unicode WinAPI, 제품 API와 서버 원문은 MultiByte 경로를 사용합니다.

## 사용 순서와 주문 안전성

**실제 서버에서 주문 전송을 확인하면 실제 주문이 발생할 수 있습니다.**

1. 로그인 후 스크롤 입력 영역에서 주문 조건과 실제 코드값을 확인합니다.
2. **주문 전송**을 누르고 서비스명·주요 입력값 확인 창에서 승인해야 요청합니다.
3. 비동기 응답의 실제 주문번호와 서버 메시지를 확인합니다. 요청 접수나 응답 수신은 체결 완료가 아닙니다.
4. 다음 전송은 이전 주문의 실제 결과를 확인한 뒤 별도 **새 주문 준비** 확인을 거쳐야 합니다. 자동 주문·자동 재시도는 없습니다.
5. 로그인 실패 시 **다시 로그인**으로 같은 화면에서 세션을 정리하고 초기화합니다. 서버·ID는 유지하고 비밀번호는 지웁니다. 진행 중 주문이 있으면 재로그인을 막습니다.
6. 종료 시 요청 해제 → Logout → Disconnect → DLL 해제 → 창 파괴 순서로 정리합니다.

타임아웃·세션 단절·잘못되거나 없는 응답은 최종 결과 미확인입니다. 이를 주문 실패나 미접수로 단정하여 재주문하지 마십시오.

## 입력 항목

| 입력 필드 | 설명 | 바이트 | UI / 초기값 |
| --- | --- | ---: | --- |
| `reccnt` | 레코드갯수 | 5 | Edit / `00001` |
| `orgordno` | 원주문번호 | 10 | Edit / 기본값 없음 |
| `accno` | 계좌번호 | 20 | Edit / 기본값 없음 |
| `passwd` | 계좌비밀번호 | 8 | 비밀번호 Edit / 기본값 없음 |
| `expcode` | 종목번호 | 12 | Edit / 기본값 없음 |
| `qty` | 주문수량 | 16 | Edit / 기본값 없음; 숫자 필수, 앞쪽 0 채움 |
| `tongsingb` | 통신매체코드 | 2 | Edit / `11` |
| `groupid` | 그룹ID | 20 | Edit / 공백 20개 |
| `stragb` | 전략코드 | 6 | Edit / 공백 6개 |
| `ordernum` | 주문회차 | 10 | Edit / `0000000000` |
| `portnum` | 포트폴리오번호 | 10 | Edit / `0000000000` |
| `basketnum` | 바스켓번호 | 10 | Edit / `0000000000` |
| `tranchnum` | 트렌치번호 | 10 | Edit / `0000000000` |
| `itemnum` | 아이템번호 | 10 | Edit / `0000000000` |
| `TrgtOrdMktCode` | 목표주문시장코드 | 2 | Edit / 공백 2개 |
| `SorChldOrdYn` | SOR자주문여부 | 1 | Edit / 공백 1개 |
| `SorTrxMthd` | SOR처리방법 | 40 | Edit / 공백 40개 |
| `PbArea` | PowerBASE내부영역 | 50 | 읽기 전용 Edit / 공백 50개 |

원주문번호(`orgordno`)는 앞뒤 ASCII 공백을 제거한 뒤 1~10자리 숫자 문자열인지 검증합니다. 빈 입력·문자·음수·10자리 초과 입력은 차단합니다. 요청 필드 10바이트를 왼쪽 Zero Padding하며 종료 NUL은 넣지 않습니다. 예를 들어 `12345`와 `00012345`는 모두 `0000012345`로 전송합니다. 화면 입력값은 유지합니다.

`qty`는 앞뒤 ASCII 공백을 제거한 뒤 비어 있지 않은 숫자인지 검증합니다. 필드 길이를 초과하거나 음수·문자·소수점이 있으면 요청을 막습니다. `0`은 허용하며 실제 멤버 길이에 맞춰 왼쪽을 `0`으로 채웁니다. 가격 단위 변환은 하지 않습니다. 나머지 필드는 기존 공백 패딩과 XML 기본값을 유지하고 종료 NUL을 전송하지 않습니다.

## 코드 읽는 순서와 요청 API

- `WinMain` / `WindowProc`: Common Helper로 초기화·연결·로그인합니다.
- `BuildInput`: UI 값을 실제 구조체 멤버에 설정하고 코드값·길이·Zero Padding을 처리합니다.
- `RequestOrder`: 사용자 확인과 중복 차단 후 아래 요청을 직접 호출하고 ID를 보관합니다.
- `CaptureNotification` / `ProcessNotification`: 통지 내용을 유효한 동안 복사하고 ID를 확인한 뒤 유형별로 처리합니다.
- `Receiveb8003`: 서비스·블록·정확한 길이 확인 뒤 두 고정 출력 블록을 구조체 포인터로 읽습니다.

```cpp
Common::Api()->RequestData(window, NAME_b8003, &input, sizeof(b8003InBlock));
```

입력은 `b8003InBlock`을 0x20으로 초기화한 본문입니다. API_MFCDlg의 실제 구조체 설정·RequestData 패턴을 따르며 Assign 함수나 별도 전문 객체를 만들지 않습니다. 통신 헤더는 SDK가 구성합니다. 즉시 반환값 0~255는 요청 ID/호출 접수이며 최종 주문 결과는 비동기 응답으로 확인합니다.

## 응답과 메모리

Header B이며 출력은 `b8003OutBlock` 242바이트, `b8003OutBlock1` 232바이트가 XML 순서로 연결됩니다. 반복 Block·커서·attr 선언이 없으며 reccnt만으로 반복을 추정하지 않습니다. 실제 OrdNo0~OrdNo4를 표시하고 주문번호를 생성하지 않습니다.

WM_RECEIVE_DATA의 REQUEST_DATA / MESSAGE_DATA / SYSTEM_ERROR_DATA / RELEASE_DATA와 타임아웃을 처리합니다. 재진입 통지는 버퍼를 복사해 순서대로 처리하고 구조체 포인터를 함수 밖에 보관하지 않습니다. 메시지는 ReleaseMessageData로 해제하며 시스템 오류의 경우 이 호출이 요청도 해제한다는 제품 동작을 반영합니다. 그 외 요청은 RELEASE_DATA·타임아웃·종료에서 ReleaseRequestData로 정리합니다.

## 민감 정보와 테스트 범위

Sample의 비밀번호 입력은 마스킹하고 응답 echo의 passwd 및 알려진 계좌비밀번호는 로그에서 가립니다. SDK 자체에는 g_Cfg.bLog가 활성화되면 전송 전문을 기록하는 기존 경로가 있습니다. Sample의 마스킹이 SDK 내부 로그까지 비노출을 보장하지 않으므로 실제 사용 전 SDK 로깅 정책을 확인하십시오. SDK Runtime과 system.ini는 수정하지 않았습니다.

실제 서버 접속·주문 테스트는 수행하지 않았습니다.
