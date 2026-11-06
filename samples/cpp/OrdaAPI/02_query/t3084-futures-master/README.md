# t3084 선물마스터 조회 — Header Type A / 반복 Block

[XML 정의](../../../../../sdk/bin/sys/t3084.xml)에 대응하는 A의 반복 출력 대표 Sample입니다. 실제 서버 검증이 끝난 t1001은 같은 A의 단일 출력 기준으로 그대로 유지합니다. 이 Sample의 실제 서버 Login/Query는 자동 수행하지 않았습니다.

## 개발 환경 및 실행

VS2010 / v100 / Win32 / MultiByte / WinAPI, No MFC, No C++11 환경입니다. `OrdaAPI-t3084-FuturesMaster.sln`을 열어 Debug/Release Win32로 빌드합니다. [공통 접속/Login Helper](../../common/README.md)를 Source로 직접 컴파일하고 `sdk/include`, `sdk/bin`을 공유합니다. Debugging Working Directory 및 IDE 밖의 작업 폴더는 `sdk/bin`입니다. 실행 파일은 Sample의 `bin/Debug` 또는 `bin/Release`입니다.

개발서버 `api-dev.capefn.com:8080` 또는 운영서버 `api.capefn.com:8080`을 선택해 ID Login 후 오른쪽의 단축코드를 확인하고 **처음 조회**를 누릅니다. 운영 로그인 인증정보는 직접 입력합니다. 개발 기본값, 한글 UI, 비밀번호 가림·지우기는 기존 Helper를 사용합니다. 자동 조회는 없습니다.

## Header Type과 Request

A의 Header는 OrdaAPI 내부에서 구성됩니다. t1001과 같은 공개 `RequestData` API를 사용하지만 출력은 단일 레코드가 아니라 반복 블록입니다. Header Type이 같아도 응답 길이와 반복 건수 계산을 t1001에서 그대로 가져오면 안 됩니다.

`t3084InBlock`은 `focode char[8] @0`와 속성 공간 1바이트로 구성된 **9바이트** 본문입니다. XML 기본값은 공백 8개이며 UI에서도 그대로 사용합니다. 입력값 의미와 결과 범위는 실제 서비스 안내 및 서버 결과로 확인하십시오.

## Response와 반복 건수

`block=1`, `attr=1`인 `t3084OutBlock`을 확인한 후, 본문 길이가 53바이트의 배수인지 검사합니다. OrdaAPI가 wire Header와 블록명을 이미 분리했으므로 본문에 이를 다시 더하지 않습니다. **5자리 건수 접두부도 없습니다.**

```text
레코드 수 = nDataLength / 53
각 레코드 = shcode[8] + 속성 1 + expcode[12] + 속성 1 + hname[30] + 속성 1
```

| Field | Type | Length | Offset |
| --- | --- | --- | --- |
| shcode | char | 8 | 0 |
| expcode | char | 12 | 9 |
| hname | char | 30 | 22 |

전체 건수와 최대 200행을 표시합니다. 길이가 맞지 않거나 다른 블록이면 결과로 해석하지 않습니다. 빈 반복 블록은 0행으로 처리합니다.

응답은 사용자 제공 `t3084OutBlock` 구조체로 처리합니다. XML과 일치하는 크기·alignment·세 필드의 offset/길이/속성 위치를 컴파일 시 검사합니다. 음수 길이·불완전 레코드·양수 길이의 NULL 본문을 먼저 거부한 뒤 `nDataLength / sizeof(t3084OutBlock)`으로 건수를 구합니다. 각 표시 레코드를 지역 구조체에 `memcpy`하고 `output.shcode`, `output.expcode`, `output.hname`을 실제 배열 길이로 읽습니다. Header나 블록명은 구조체에 포함하지 않으며 DLL 소유 포인터를 보관하지 않습니다. 기존 공백/NUL 처리·표시 순서·200행 제한을 유지합니다.

## 연속조회 및 Memory

XML에 명시적인 출력 cursor→입력 cursor 계약이 없으므로 `shcode`를 `focode`의 다음 키로 임의 대입하지 않습니다. 이 Sample은 최초 요청만 수행합니다. 연속 신호가 있더라도 키 매핑이 확인되지 않았다는 메시지를 표시하며 자동 추가 조회를 하지 않습니다.

Query 본문 작성, 공개 API 호출, 응답 메시지, 반복 레코드 파싱은 모두 `main.cpp`에서 확인할 수 있습니다. REQUEST_DATA를 복사해 표시하고 제품 메모리를 임의 해제하지 않습니다. MSG_PACKET은 `ReleaseMessageData`, 요청은 RELEASE_DATA/timeout에서 `ReleaseRequestData`를 사용합니다. 요청 ID 0 및 호출 중 메시지 재진입도 처리합니다.

빌드·오프라인 검증과 남은 서버 검증 항목은 [완료 보고서](../../../../../docs/query-header-type-samples-report.md)를 참고하십시오.

Response 구조체 전환 후 42개 정상 조합(0/1/3/200/201/1000건 × 7개 데이터 패턴)과 19개 예외/메타데이터 사례를 기존 파서와 비교했습니다. 원본 필드 바이트·문자열·표시 함수에 전달되는 결과와 오류 처리가 일치했고, Debug/Release Win32 Clean→Build와 Runtime Load/Initialize 검사가 통과했습니다. 실제 서버 Login/Query와 GUI 렌더링 검증은 실행하지 않았습니다. 상세 결과는 [t3084 Response 전환 보고서](../../../../../docs/t3084-struct-response-report.md)에 기록했습니다.
