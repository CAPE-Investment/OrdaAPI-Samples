# s3001 — 선물/옵션 현재가 호가 조회

선물·옵션 종목의 현재가와 매수·매도 5단계 호가·잔량·건수를 조회하는 독립 Win32 GUI Sample입니다. [main.cpp](OrdaAPI-s3001-FuturesOptionsOrderBook/main.cpp)에 요청과 응답 처리가 있습니다. 주문이나 실시간 자동갱신은 수행하지 않습니다.

## 개발 환경과 빌드

- Windows / Visual Studio 2010 / Platform Toolset v100 / Debug 또는 Release / Win32(x86)
- MultiByte / C++03 / 순수 WinAPI. MFC와 추가 공통 DLL은 사용하지 않습니다.
- `OrdaAPI-s3001-FuturesOptionsOrderBook.sln`을 열어 빌드합니다. 기본 출력은 이 Sample의 `bin/Debug`, `bin/Release`, 중간 파일은 `obj/<구성>`입니다.
- 공개 Header는 `sdk/include/ordalib.h`, `errcode.h`, 전문 Header는 `sdk/structs/cpp/s3001.h`를 참조합니다. 공개 Header 경로는 `ORDAAPI_INCLUDE_DIR`로 재정의할 수 있습니다.
- 기존 `common/OrdaSampleCommon.cpp`를 함께 컴파일하며 공통 코드는 변경하지 않았습니다.
- C++ 소스는 UTF-8 BOM입니다. 화면은 Unicode WinAPI, SDK 호출과 서버 문자열은 MultiByte를 사용합니다.

## SDK 준비와 실행

저장소의 `sdk/bin`을 작업 폴더로 사용합니다. 프로젝트의 디버거 작업 폴더도 이 위치입니다. IDE 밖에서도 같은 작업 폴더를 지정하십시오. `OrdaAPI.dll`, `OrdaAPISvr.dll`, `OrdaSDK.dll`, `OrdaSDKSvr.dll` 및 공급된 통신·인증 의존 DLL, `sys` 리소스를 포함한 정상 SDK Runtime이 필요합니다. Sample별 DLL 복사나 전역 PATH 변경은 필요하지 않습니다.

기존 `OrdaAPI` 래퍼가 `LoadLibrary` / `GetProcAddress`로 DLL을 사용하므로 SDK import library는 링크하지 않습니다.

## 로그인과 조회

1. 왼쪽에서 개발/운영 서버와 접속정보, 로그인 ID·비밀번호·공동인증 비밀번호를 확인하고 **로그인**합니다. 비밀번호 입력은 마스킹됩니다.
2. 비동기 `WM_LOGIN` 결과와 SDK 로그인 상태를 확인한 후에만 조회할 수 있습니다. 즉시 반환만으로 로그인 성공을 판단하지 않습니다.
3. 선물·옵션 종목코드를 입력하고 **조회**합니다.
4. 오른쪽에서 현재가·호가 시간·거래량·총잔량/총건수와 5단계 표를 확인합니다. 요청 중에는 입력과 조회 버튼을 잠가 중복 전송을 막습니다.
5. 로그인 실패 시 상태 메시지를 확인하고 종료 후 다시 실행하십시오. 별도 재로그인·연속조회 기능은 없습니다.

| 입력 필드 | XML 데이터 길이 | 처리 |
| --- | ---: | --- |
| `shcode` | 8바이트 | 영문·숫자 8자리. 기본값 없음. 빈 값·길이 오류·기타 문자 차단 |

주식의 6자리 코드와 구분합니다. 실제 서버에서 유효한 선물·옵션 단축코드를 사용하십시오. `s3001InBlock`을 속성 바이트까지 공백 초기화하고 실제 `shcode` 멤버에 8바이트를 복사합니다. 고정 필드에 종료 NUL을 넣지 않습니다. 이 서비스 입력에는 거래소 선택 필드나 `coderef`가 없습니다.

## 출력

| 화면 | 실제 출력 멤버 | XML 데이터 길이 |
| --- | --- | ---: |
| 종목명 | `hname` | 20 |
| 현재가 | `price` | 10 |
| 호가 시간 | `time` (XML 명칭: 수신시간) | 6 |
| 거래량 | `volume` | 12 |
| 매도호가 / 매수호가 1~5 | `offerho1`~`offerho5` / `bidho1`~`bidho5` | 각 10 |
| 매도잔량 / 매수잔량 1~5 | `offerrem1`~`offerrem5` / `bidrem1`~`bidrem5` | 각 8 |
| 매도건수 / 매수건수 1~5 | `dcnt1`~`dcnt5` / `scnt1`~`scnt5` | 각 8 |
| 매도 / 매수 총잔량 | `dvol` / `svol` | 각 8 |
| 매도 / 매수 총건수 | `toffernum` / `tbidnum` | 각 8 |

모두 고정 길이 `char` 배열이며 각 필드의 속성 바이트는 SDK Header 선언을 따릅니다. 표시용 복사본에서 공백·끝 NUL 패딩만 제거하고 선행 0·부호·소수점을 보존합니다. 내부 제어문자는 표시가 잘리지 않도록 `?`로 치환합니다. 주식과 같은 가격 배율이라고 가정하지 않으며, 부동소수점 변환·반올림·단위 변환을 하지 않습니다. XML·Header에 없는 상품별 단위와 호가단위는 실제 서비스 규격으로 확인해야 합니다.

## 요청·비동기 응답·해제

`WinMain` → Common 초기화 → 연결/비동기 로그인 → `BuildInput` → `RequestQuote` → `CaptureNotification` / `ProcessNotification` → 종료 순서입니다.

```cpp
Common::Api()->RequestData(window, NAME_s3001, &input, sizeof(input));
```

4인자 최초 조회이며 즉시 반환 0~255는 요청 ID입니다. 최종 조회 결과는 비동기 통지로 확인합니다. 요청 ID·서비스 `s3001`·블록명 `s3001OutBlock`·데이터 포인터·정확한 `sizeof(s3001OutBlock)` 길이를 검사한 후 지역 구조체에 복사합니다. 현재 Header 기준 입력 9바이트, 출력은 속성 바이트 포함 425바이트입니다. Header 오류 플래그와 메시지·시스템 오류도 처리합니다.

호출 중 재진입한 통지는 빌린 버퍼가 유효할 때 복사하고 요청 ID를 확보한 뒤 순서대로 처리합니다. 메시지는 복사 후 `ReleaseMessageData()`로 해제합니다. 제품 소스의 `nIsSystemError==1` 메시지 해제 시 요청도 해제하는 동작을 반영하여 이중 해제를 피합니다. 메시지 형식 오류는 원본 SDK 시스템 오류 값과 구분합니다.

일반 요청은 `RELEASE_DATA`·타임아웃·종료에서 `ReleaseRequestData()`로 해제합니다. 종료 시 대기 메시지 정리 → 활성 요청 해제 → Logout → Disconnect → DLL 해제 → 창 파괴 순서이며, 호출 중 종료는 호출 완료 후 처리합니다.

## 오류와 주의 사항

- 초기화·로그인·요청 실패와 서버 오류를 상태 영역에 표시합니다. 잘못된 서비스·블록·응답 길이·데이터 포인터는 무조건 역참조하지 않습니다.
- 타임아웃이나 무자료 응답에 자동 재조회하지 않습니다. 사용자가 상태와 종목코드를 확인하고 다시 조회합니다.
- 표는 일회성 조회 결과입니다. 실시간 호가로 간주하지 마십시오.
- 로그인 비밀번호는 Sample 결과에 출력하지 않습니다. SDK 자체 로그 정책은 별도로 확인하십시오.
- 기존 Sample·공통 코드·SDK·XML·상위 문서·`.gitignore`는 변경하지 않았습니다.

## 테스트 결과

2026-11-16 기준 VS2010 v100 **Debug Win32 / Release Win32 빌드 성공**. 실제 서버 연결·로그인·조회는 수행하지 않았습니다.

Python 3와 VS2010 v100/.NET 4 MSBuild 환경에서 저장소 루트를 기준으로 실행합니다.

```text
python samples/cpp/OrdaAPI/02_query/s3001/tests/run-offline.py
```

XML/Header 데이터 필드·속성 선언, 8자리 고정 입력·NUL 비전송, 30개 호가/잔량/건수 셀과 요약 값, 가격 원문 보존, 초기화 실패·로그인 실패·비동기 로그인 완료 대기, 요청 실패·타임아웃·서버 오류·다른 ID/서비스/블록·응답 길이·NULL 데이터·비정상 메시지, 재진입/대기 중 중복 요청 방지, 메시지/요청 해제, 호출 중/대기 중/게시 통지 대기 중 종료 검사를 통과했습니다.

`tests/`는 개발 전용이며 고객 실행에 필요하지 않습니다. 가짜 DLL·테스트 프로젝트·산출물은 새 임시 폴더에만 생성하고 그 폴더에서 실행합니다. 실제 `sdk/bin` DLL을 로드하거나 교체하지 않습니다. 고객 배포본에서는 `tests/`를 제외하십시오. 실제 선물·옵션 종목별 조회, 가격 단위·소수점 의미·호가 시간·건수 및 서버 오류 응답은 별도 검증이 필요합니다.
