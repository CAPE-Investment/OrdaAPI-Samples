# d7304 영업일정보 조회 — Header Type C

[XML 정의](../../../../../sdk/bin/sys/d7304.xml)에 대응하는 C 대표 Sample입니다. 단일 입력 블록에 여러 필드를 넣고 고정 길이 단일 출력을 해석합니다. 실제 서버 Login/Query는 자동 실행하지 않았습니다.

## 개발 환경 및 실행

VS2010 / v100 / Win32 / MultiByte / WinAPI, No MFC, No C++11 환경입니다. `OrdaAPI-d7304-BusinessDays.sln`을 열어 Debug/Release Win32로 빌드합니다. 기존 t1001과 같은 [공통 ID Login Helper](../../common/README.md)를 Source로 직접 컴파일하며 Header·DLL은 `sdk/include`, `sdk/bin`을 공유합니다. Debugging Working Directory 및 IDE 밖의 실행 작업 폴더는 `sdk/bin`입니다. 실행 파일은 Sample의 `bin/Debug` 또는 `bin/Release`에 있습니다.

개발서버 `api-dev.capefn.com:8080` 또는 운영서버 `api.capefn.com:8080`을 선택해 로그인하고 오른쪽 입력을 채워 **처음 조회**를 누릅니다. 운영 인증정보는 직접 입력합니다. 개발 기본값과 비밀번호 가림·지우기 정책은 기존 Helper를 사용합니다.

## Header Type과 Request

C는 제품 Source의 TUXIDO Header입니다. Header와 내부 서비스 코드 변환은 OrdaAPI가 처리하므로 Sample에서 wire Header를 구성하지 않습니다. 공개 API는 A/B/D와 같은 `RequestData`이며 처음 요청에는 마지막 인수로 `NULL`을 전달합니다. 같은 C라도 반복 블록·attr 유무는 서비스마다 다릅니다. 이 Sample은 비반복·attr 없는 사례를 검증합니다.

`d7304InBlock`은 **21바이트**입니다. 값은 XML에 기본값이 없으므로 사용자가 입력하며, 공백 패딩과 바이트 길이 제한을 적용합니다.

| Field | 설명 | Type | Length | Offset |
| --- | --- | --- | --- | --- |
| gubun | 구분: 1=N일전, 5=N영업일전 | char | 1 | 0 |
| fromdt | 시작일자 | char | 8 | 1 |
| todt | 종료일자 | char | 8 | 9 |
| cnt | 일수 | char | 4 | 17 |

날짜·일수 조합의 업무상 유효성은 정식 서비스 안내와 실제 서버 결과로 확인하십시오. XML에 없는 입력 기본값은 넣지 않았습니다.

## Response

`d7304OutBlock` 하나이며 **36바이트**, 비반복입니다. 반복 건수 접두부나 필드 사이 attr 바이트를 추가하지 않습니다. `main.cpp`의 `ParseService`가 정확한 길이를 확인한 후 다음 필드를 표시합니다.

최신 사용자 헤더의 `d7304OutBlock`을 사용합니다. 모든 필드 offset/길이와 크기·alignment를 컴파일 검사하고, NULL이 아닌 정확히 36바이트의 본문만 지역 구조체에 memcpy해 각 멤버를 읽습니다. 기존 Request와 UI·오류·메모리 해제 처리는 유지했습니다. 정상 7개와 예외/메타데이터 15개가 기존 결과와 일치했고 Debug/Release 빌드가 통과했습니다. 자세한 내용은 [B/C/D Response 보고서](../../../../../docs/query-bcd-struct-response-report.md)를 참고하십시오.

| Field | Type | Length | Offset |
| --- | --- | --- | --- |
| bizdt | char | 8 | 0 |
| bfbizdt | char | 8 | 8 |
| afbizdt | char | 8 | 16 |
| afbizdt2 | char | 8 | 24 |
| day_cnt | char | 4 | 32 |

## 연속조회 및 메모리

이 XML에는 반복 블록이나 연속 입력/출력 필드가 없습니다. 이 Sample은 최초 요청만 수행하고 다음 조회를 만들지 않습니다. C 타입 내부의 Y/N 연속 신호를 다른 C 서비스에 동일하게 적용할 수 있다는 뜻은 아닙니다. C의 CTS 기반 서비스는 별도 서버 검증이 필요합니다.

`main.cpp`에 입력 구성, RequestData, 응답 메시지 분기와 파싱을 남겼습니다. REQUEST_DATA의 제품 버퍼는 복사해서 표시하고, MESSAGE_DATA/SYSTEM_ERROR_DATA는 원문을 복사한 뒤 `ReleaseMessageData`, RELEASE_DATA/timeout은 `ReleaseRequestData`를 사용합니다. 제품 메모리를 임의 해제하지 않습니다. 요청 ID 0, 조회 중 비활성화, 로그인 전·접속 해제 후 조회 차단을 처리합니다.

빌드·계약 검증 및 사용자 서버 확인 항목은 [완료 보고서](../../../../../docs/query-header-type-samples-report.md)를 참고하십시오.
