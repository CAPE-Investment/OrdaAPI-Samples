# t1001 주식현재가 조회

서비스 코드 `t1001`, 서비스명 `주식현재가조회`, [XML 정의](../../../../../sdk/bin/sys/t1001.xml)에 대응하는 최소 Win32 GUI Sample입니다. 접속/Login은 [공통 Helper](../../common/OrdaSampleCommon.h)를 사용하며, `main.cpp`에 입력 버퍼 작성 → RequestData → 응답 메시지 → 출력 필드 해석 → 결과 표시가 직접 들어 있습니다. 화면은 전체 134개 출력 중 주요 6개 필드만 표시합니다.

## 개발 환경과 빌드

- Visual Studio 2010 / v100 / Win32 / MultiByte / 순수 WinAPI. MFC 및 C++11 문법을 사용하지 않습니다.
- `OrdaAPI-t1001-CurrentPrice.sln`을 열어 Debug 또는 Release / Win32로 빌드합니다.
- Project는 `../../../common/OrdaSampleCommon.cpp`를 직접 컴파일합니다. 별도 Helper DLL·Library·Solution은 없습니다.
- 공통 Header는 `sdk/include`를 상대경로로 참조합니다. `ORDAAPI_INCLUDE_DIR` 재정의도 기존 Login Sample과 동일하게 지원합니다.
- 공통 Runtime은 `sdk/bin`입니다. Debugging Working Directory를 해당 폴더로 유지하며 Header·DLL을 Sample마다 복사하지 않습니다.
- Source는 VS2010 한글 리터럴 처리를 위해 UTF-8 BOM으로 저장합니다. 자체 UI는 Unicode WinAPI, 제품 호출과 서버 원문은 기존 ANSI 경로를 사용합니다.
- 결과 EXE는 Sample 폴더의 `bin/Debug`, `bin/Release`, 중간 산출물은 `obj`에 출력합니다.

IDE 밖에서 실행할 때도 작업 폴더를 저장소의 `sdk/bin`으로 지정하고 위 EXE를 실행하십시오. EXE 더블 클릭 시 작업 폴더는 보장되지 않습니다.

## 접속 환경

현재 ID Login만 지원합니다. 기존 독립형 Login Sample의 연결·로그인 흐름과 한 번의 로그인 시도 정책을 유지합니다. 재접속 또는 로그인 재시도는 프로그램을 다시 실행합니다.

| 항목 | 개발서버 | 운영서버 |
| --- | --- | --- |
| 서버 | api-dev.capefn.com | api.capefn.com |
| 포트 | 8080 | 8080 |
| 로그인 ID | APITEST | 사용등록 계좌 정보 직접 입력 |
| 로그인 비밀번호 | 1q2w3e4r!! | 직접 입력 |
| 공동인증 비밀번호 | 1q2w3e4r!! | 직접 입력 |

운영서버를 선택하면 인증 입력값을 비웁니다. 비밀번호 입력은 가려지며 로그인 호출 후 입력창과 임시 버퍼를 비웁니다. 개발서버 접속정보·인증서가 변경되면 당사 OrdaAPI 관련 자료실을 확인하십시오.

## 실행 순서

1. 공통 Runtime 작업 폴더로 Sample을 실행합니다.
2. 개발서버 또는 운영서버를 선택하고 접속정보를 확인합니다.
3. 로그인 버튼을 누르고 로그인 성공을 확인합니다.
4. 종목코드 6자리와 거래소를 선택합니다. 종목코드는 자동 입력하지 않습니다.
5. 조회 버튼을 눌러 `t1001`을 요청합니다.
6. 요청 종목코드와 현재가 결과를 확인합니다.
7. 창을 닫으면 진행 요청 정리, Logout, Disconnect, DLL 해제를 수행합니다.

로그인 성공 전·접속 해제 후·조회 진행 중에는 조회 버튼이 비활성화됩니다. 조회는 자동 실행하지 않습니다. 요청 ID는 0도 유효하며, 한 번에 한 요청만 진행합니다. 응답 후에도 RELEASE_DATA를 받을 때까지 다음 요청을 막습니다.

## 주요 Public API와 공통 Helper

`OrdaAPI::Load / Initialize / Connect / Login / IsConnected / IsLogin / Logout / Disconnect`는 공통 Helper가 담당합니다. `WM_LOGIN`의 빌린 문자열을 통지 중 복사하고, 게시된 내부 메시지에서 최종 코드 `0000`과 `IsLogin()` 상태를 확인합니다. 동기 API 호출 중 종료는 호출이 끝난 뒤로 미룹니다. 기존 `01_connection/login` Source는 변경하지 않았습니다.

조회는 `main.cpp`에서 다음 Public API를 직접 호출합니다.

```cpp
int result = Common::Api()->RequestData(window, T1001_CODE, input, sizeof(input), NULL);
```

반환값 0~255는 요청 접수 및 요청 ID이며 조회 결과 성공을 뜻하지 않습니다. 오류는 음수입니다. 본문은 제품 내부에서 복사되므로 지역 입력 버퍼를 사용할 수 있습니다. 제품이 통신 헤더를 생성하므로 Sample은 XML 입력 본문만 전달합니다. 공통 Helper에는 Query 요청·출력 파싱·조회 메모리 해제 로직을 넣지 않았습니다.

## 실제 Input 구조

`SERVICE code="t1001" attr="1" block="1" headtype="A"`, 입력 블록 `t1001InBlock`. 모든 타입은 `char`입니다.

| 필드 | 설명 | 길이 | 0 기반 Offset | UI/기본값 |
| --- | --- | --- | --- | --- |
| shcode | 단축코드 | 6 | 0 | 종목코드 입력 |
| 속성 바이트 | attr=1 | 1 | 6 | 공백 |
| exchange | 거래소구분 | 1 | 7 | KRX=1 / NXT=2 / 통합=3, XML 기본값 3 |
| 속성 바이트 | attr=1 | 1 | 8 | 공백 |

전체 요청 길이는 **9바이트**이며 NUL 종료 문자를 추가 전송하지 않습니다. `t1001InBlock input`을 `memset(&input, ' ', sizeof(input))`으로 초기화하고 `input.shcode`, `input.exchange`를 설정합니다. 속성 공간과 공백 초기화는 제품 `OrdaSDK/ServiceIO.cpp`의 offset 계산 및 `ServiceIO.h::CreateMemory` 구현을 확인했습니다. 거래소 값은 `codes.xml`의 EXCHANGE 정의를 사용합니다.

## 실제 Output 구조와 화면 표시

출력 블록은 `t1001OutBlock` 하나이며 **비반복**, 총 **134필드 / 속성 바이트 포함 1,184바이트**입니다. 서비스 코드·요청 ID·블록명을 확인하고 응답 길이가 정확히 일치할 때 표시합니다. 화면에 나타나는 요청 종목코드는 보관한 입력값이며 XML Output 필드가 아닙니다.

응답의 `Header`와 `szBlockName`은 공개 API의 별도 멤버이며 `lpData`가 출력 블록 본문입니다. 길이와 오류 플래그 검증을 통과한 뒤 `memcpy(&output, response->lpData, sizeof(output))`로 지역 `t1001OutBlock`에 복사하고 `output.hname`, `output.price` 등 실제 멤버를 읽습니다. 필드 배열의 실제 길이를 사용해 기존 공백 제거와 숫자 원문 표시를 유지하며 DLL 소유 포인터를 보관하지 않습니다.

| 화면 | XML Field | Type | 길이 | 0 기반 Offset |
| --- | --- | --- | --- | --- |
| 종목명 | hname | char | 20 | 0 |
| 현재가 | price | char | 8 | 21 |
| 전일대비 구분 | sign | char | 1 | 30 |
| 전일대비 | change | char | 8 | 32 |
| 등락률 | diff | char | 6 | 41 |
| 누적거래량 | volume | char | 12 | 48 |

가격·비율·구분 값은 원문 문자값으로 표시합니다. `sign` 의미를 임의 매핑하거나 가격의 부호·단위를 추측하지 않습니다. 전체 Field 정의는 [t1001.xml](../../../../../sdk/bin/sys/t1001.xml)과 [분석 보고서](../../../../../docs/query-xml-structure-analysis.md)를 참고하십시오. Source에는 사용한 필드만 명시했습니다.

## 메시지와 메모리 소유권

- `nBaseID + WM_RECEIVE_DATA`의 `wParam=REQUEST_DATA`: `lParam`은 `RP_DATA_INFO*`. 요청 ID, `Header.szTrCode`, `szBlockName`, `nDataLength` 확인 후 `lpData`를 해석합니다.
- 고정폭 필드는 NUL 종료를 가정하지 않고 지역 버퍼로 복사·패딩 제거 후 화면에 표시합니다. OrdaAPI 포인터를 보관하거나 `delete/free`하지 않습니다.
- `MESSAGE_DATA / SYSTEM_ERROR_DATA`: `MSG_PACKET*`의 코드·원문 메시지를 복사해 표시한 뒤 `ReleaseMessageData(lParam)`를 호출합니다. 다른 요청의 메시지도 메모리는 해제합니다.
- `RELEASE_DATA`: `lParam`의 활성 요청 ID로 `ReleaseRequestData`를 호출합니다. 이때 제품이 RP_DATA_INFO 및 본문 메모리를 해제하므로 이전 포인터를 사용하지 않습니다.
- `nBaseID + WM_TIMEOUT_DATA`: `lParam` 요청 ID를 확인하고 `ReleaseRequestData`로 정리합니다. 종료 시 남아 있는 요청도 정리합니다.
- RequestData 내부에서 메시지가 먼저 전달될 경우 요청 ID 반환까지 통지를 순서대로 보관하고, 반환 후 같은 순서로 처리합니다. RELEASE 전에 데이터를 복사합니다.

연속조회 관련 API 인수 `pszNextKey`, 응답 `Header.szPNFlag / szContKey`가 있으나 t1001은 연속 입력 필드가 없는 단일 현재가 조회입니다. 이 Sample은 `NULL`로 최초 요청만 수행하며 다음 페이지를 자동 요청하지 않습니다.

## 검증 범위

Response 구조체 전환 후 134개 필드 전체의 XML 대비 offset/길이/속성 위치와 sizeof·alignment를 VS2010 x86 컴파일러로 검사했습니다. 기존/변경 응답 함수를 동일한 합성 데이터로 비교해 7개 정상 조합의 원본 필드 바이트·문자열·6개 표시값과 21개 예외 패킷의 처리 결과가 일치했습니다. 표시값은 SetWindowTextA 호출을 가로채 비교한 것으로 실제 GUI 렌더링 검증과 구분합니다. 보호 페이지 경계의 전체 본문과 잘못된 길이에서 접근 불가능한 본문을 사용한 검사도 통과했습니다. Debug/Release Win32 Clean→Build 및 Runtime Load/Initialize 콘솔 검사를 수행했으며, 상세 결과는 [Response 전환 보고서](../../../../../docs/t1001-struct-response-report.md)를 참고하십시오.

VS2010 v100 Debug/Release Win32에서 각각 Clean→Build에 성공했습니다. XML로부터 별도로 계산한 입력 9바이트·출력 1,184바이트·주요 필드 offset과 Source가 일치함을 확인했습니다. 오프라인 모의 DLL과 합성 응답으로 요청 ID 0, 입력 본문, 응답 ID·길이 검사, 응답/해제 순서 보존, 화면 복사, 서버 원문 메시지, 해제·타임아웃, 로그아웃 후 조회 비활성화 및 개발/운영 선택 정책을 확인했습니다.

실제 `sdk/bin`을 작업 폴더로 사용한 별도 콘솔 검사에서 Runtime Load=1, Initialize=1을 확인했습니다. UI 도구의 기본 작업 폴더로 실행한 창에서는 DLL 로드 오류(-1002)가 표시됐으며, 접근성 정보로 한글 UI·기본 선택·조회 비활성화는 확인했습니다. 화면 캡처 타임아웃과 클릭 geometry 오류로 실제 GUI 로그인 검증은 완료하지 못했습니다.

실제 서버 로그인 및 t1001 조회는 이번 자동 검증에서 수행하지 않았습니다. 사용자가 위 실행 순서로 실제 종목명 한글, 거래소별 값, 응답 블록/길이, 반복 조회, 오류·접속 해제·종료 동작을 최종 확인하십시오. 보호 대상인 sdk 전체와 기존 Login 폴더는 파일 해시 비교에서 변경·신규 파일이 없었습니다.
