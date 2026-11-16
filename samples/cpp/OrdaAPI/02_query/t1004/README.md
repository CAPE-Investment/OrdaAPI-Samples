# t1004 — 주식현재가호가조회

종목과 거래소를 선택해 현재가 및 매수·매도 10단계 호가를 조회하는 독립 Win32 GUI Sample입니다. [main.cpp](OrdaAPI-t1004-StockOrderBook/main.cpp)에 요청과 응답 처리가 있습니다. 주문이나 실시간 자동갱신은 수행하지 않습니다.

## 개발 환경과 빌드

- Windows / Visual Studio 2010 / Platform Toolset v100 / Debug 또는 Release / Win32(x86)
- MultiByte / C++03 / 순수 WinAPI. MFC와 추가 공통 DLL은 사용하지 않습니다.
- `OrdaAPI-t1004-StockOrderBook.sln`을 열어 빌드합니다. 기본 출력은 이 Sample의 `bin/Debug`, `bin/Release`, 중간 파일은 `obj/<구성>`입니다.
- SDK 공개 Header는 저장소의 `sdk/include/ordalib.h`, `errcode.h`, 전문 Header는 `sdk/structs/cpp/t1004.h`입니다. 공개 Header 경로는 `ORDAAPI_INCLUDE_DIR`로 재정의할 수 있습니다.
- 기존 `common/OrdaSampleCommon.cpp`를 프로젝트에 함께 컴파일합니다. 공통 코드는 수정하지 않았습니다.
- C++ 소스는 UTF-8 BOM입니다. 화면은 Unicode WinAPI, SDK 호출과 서버 문자열은 MultiByte를 사용합니다.

## SDK 준비와 실행

저장소의 `sdk/bin`을 작업 폴더로 사용합니다. 프로젝트에 디버거 작업 폴더가 설정되어 있으며, IDE 밖에서도 같은 폴더를 지정해야 합니다. `OrdaAPI.dll`, `OrdaAPISvr.dll`, `OrdaSDK.dll`, `OrdaSDKSvr.dll` 및 공급된 통신·인증 의존 DLL과 `sys` 리소스를 포함한 정상 SDK Runtime을 준비하십시오. DLL을 Sample에 복사하거나 전역 PATH를 변경할 필요는 없습니다.

SDK는 기존 `OrdaAPI` 래퍼의 `LoadLibrary` / `GetProcAddress` 방식으로 호출합니다. SDK import library를 링크하지 않습니다.

## 로그인과 조회

1. 왼쪽에서 개발/운영 서버, 접속정보와 로그인 정보를 확인하고 **로그인**합니다. 로그인·공동인증 비밀번호는 마스킹됩니다.
2. 비동기 `WM_LOGIN` 통지와 실제 SDK 로그인 상태가 확인된 후에만 조회할 수 있습니다. 로그인 함수의 즉시 반환만으로 성공 처리하지 않습니다.
3. 주식 종목코드와 거래소를 입력하고 **조회**를 누릅니다.
4. 오른쪽 요약과 10단계 표를 확인합니다. 요청 완료 전에는 조회 버튼과 조건 입력을 잠가 중복 전송을 막습니다.
5. 실패한 로그인은 상태 메시지를 확인한 후 프로그램을 종료하고 다시 실행하십시오. 별도 재로그인·연속조회 기능은 없습니다.

| 입력 필드 | XML 데이터 길이 | 화면 및 전송 |
| --- | ---: | --- |
| `shcode` | 6바이트 | 영문·숫자 6자리. 빈 값·길이 오류·기타 문자는 차단 |
| `exchange` | 1바이트 | 한국거래소 KRX `1`, 넥스트레이드 NXT `2`, 통합 `3` |

거래소 기본값은 `t1004.xml`의 `default="3"` 및 기존 t1001 정책을 따른 **통합(3)**입니다. 코드 목록은 `sdk/bin/sys/codes.xml`의 `EXCHANGE`를 확인했습니다. 종목 기본값은 없습니다.

`t1004InBlock`을 속성 바이트까지 공백 초기화하고 실제 `shcode`, `exchange` 멤버에 값을 설정합니다. 전송용 고정 필드에 종료 NUL을 추가하지 않습니다.

## 출력

| 화면 | 실제 출력 멤버 | XML 데이터 길이 |
| --- | --- | ---: |
| 종목명 | `hname` | 20 |
| 현재가 | `price` | 8 |
| 호가 시간 | `hotime` (XML 명칭: 수신시간) | 8 |
| 거래량 | `volume` | 12 |
| 매도호가 / 매수호가 1~10 | `offerho1`~`offerho10` / `bidho1`~`bidho10` | 각 8 |
| 매도잔량 / 매수잔량 1~10 | `offerrem1`~`offerrem10` / `bidrem1`~`bidrem10` | 각 12 |
| 매도 / 매수 총잔량 | `offer` / `bid` | 각 12 |

위 필드는 모두 고정 길이 `char` 배열입니다. `attr="1"`이므로 Header에 선언된 각 필드의 속성 바이트도 존재합니다. 표시할 때 공백·끝 NUL 패딩만 제거하고 값의 선행 0, 부호, 소수점은 보존합니다. 내부 제어문자는 표시가 잘리지 않도록 `?`로 치환합니다. 가격·잔량을 부동소수점으로 변환하거나 반올림하지 않습니다. XML·Header에 없는 가격 배율이나 단위 변환을 가정하지 않습니다.

## 요청·비동기 응답·해제

`WinMain` → Common 초기화 → 연결/비동기 로그인 → `BuildInput` → `RequestQuote` → `CaptureNotification` / `ProcessNotification` → 종료 순서입니다.

```cpp
Common::Api()->RequestData(window, NAME_t1004, &input, sizeof(input));
```

4인자 최초 조회입니다. 즉시 반환 0~255는 요청 ID이며 조회 성공 결과가 아닙니다. 응답의 요청 ID·서비스 코드 `t1004`·블록명 `t1004OutBlock`·데이터 포인터·정확한 길이 `sizeof(t1004OutBlock)`를 확인한 후 지역 구조체에 복사해 읽습니다. 현재 Header 기준 입력은 9바이트, 출력은 속성 바이트 포함 951바이트입니다. Header 오류 플래그와 서버 메시지도 처리합니다.

API 호출 중 재진입한 통지는 빌린 버퍼가 유효할 때 복사하고 요청 ID를 받은 뒤 순서대로 처리합니다. `MESSAGE_DATA` / `SYSTEM_ERROR_DATA`는 메시지를 복사한 뒤 `ReleaseMessageData()`로 해제합니다. 제품 소스의 `nIsSystemError==1` 메시지 해제 시 요청도 해제하는 동작을 반영하여 중복 해제를 피합니다. 형식 오류는 이 SDK 값과 별도로 처리합니다.

일반 요청은 `RELEASE_DATA`·타임아웃·종료에서 `ReleaseRequestData()`로 해제합니다. 종료 시 대기 메시지 정리 → 활성 요청 해제 → Logout → Disconnect → DLL 해제 → 창 파괴 순서입니다. 호출 중 종료는 호출이 끝난 후 처리합니다.

## 오류와 주의 사항

- 초기화·로그인·요청 실패, 서버 오류, 다른 서비스/블록, 부족하거나 과도한 응답 길이를 상태 영역에 표시합니다.
- 응답이 없거나 타임아웃이어도 자동 재조회하지 않습니다. 조건과 상태를 확인하고 사용자가 다시 조회합니다.
- 표는 마지막 요청의 일회성 결과입니다. 실시간 호가로 간주하지 마십시오.
- Sample은 로그인 비밀번호를 출력하지 않습니다. SDK 자체 로그 정책은 별도로 확인하십시오.
- 기존 Sample·공통 코드·SDK·XML·상위 문서·`.gitignore`는 변경하지 않았습니다.

## 테스트 결과

2026-11-16 기준 VS2010 v100 **Debug Win32 / Release Win32 빌드 성공**. 실제 서버 연결·로그인·조회는 수행하지 않았습니다.

저장소 루트에서 다음 명령으로 오프라인 검사를 재현할 수 있습니다. Python 3 및 VS2010 v100/.NET 4 MSBuild가 필요합니다.

```text
python samples/cpp/OrdaAPI/02_query/t1004/tests/run-offline.py
```

XML/Header 데이터 필드·속성 선언, 거래소 기본값과 선택값, 고정 입력·NUL 비전송, 40개 호가 표 셀과 요약 값, 초기화/비동기 로그인 실패 및 완료 대기, 요청 실패·타임아웃·오류 플래그·다른 ID/서비스/블록·응답 길이·NULL 데이터·비정상 메시지, 재진입/대기 중 중복 클릭, 메시지/요청 해제, 호출 중/대기 중/게시 통지 대기 중 종료 검사를 통과했습니다.

`tests/`는 개발 전용이며 고객 실행에 필요하지 않습니다. 가짜 DLL과 테스트 프로젝트·산출물은 새 임시 폴더에만 생성하고 그 폴더에서 실행합니다. 실제 `sdk/bin` DLL은 로드하거나 교체하지 않습니다. 고객 배포본에서는 `tests/`를 제외하십시오. 실제 서버의 지원 종목·거래소별 호가·가격 단위·서버 오류 메시지·시점별 응답은 별도 검증이 필요합니다.
