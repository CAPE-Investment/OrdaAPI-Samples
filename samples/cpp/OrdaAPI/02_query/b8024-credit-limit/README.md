# b8024 종목별 신용한도 조회 — Header Type B

[XML 정의](../../../../../sdk/bin/sys/b8024.xml)에 대응하는 B 대표 Sample입니다. 신용한도 조회이며 주문 요청을 전송하지 않습니다. 실제 서버 Login/Query는 자동 실행하지 않았습니다.

## 개발 환경 및 실행

VS2010 / v100 / Win32 / MultiByte / WinAPI이며 MFC와 C++11은 사용하지 않습니다. `OrdaAPI-b8024-CreditLimit.sln`을 열어 Debug 또는 Release Win32로 빌드합니다. 기존 t1001과 동일한 [공통 접속/Login Helper](../../common/README.md)를 Source로 직접 컴파일합니다. 별도 Helper DLL/Library는 없습니다.

공통 Header는 `sdk/include`, Runtime과 Debugging Working Directory는 `sdk/bin`입니다. 상대경로 설정을 유지하며 IDE 밖에서도 작업 폴더를 `sdk/bin`으로 지정하십시오. 실행 파일은 이 Sample의 `bin/Debug` 또는 `bin/Release`에 생성됩니다. 자체 한글 UI는 Unicode WinAPI, 제품 원문은 ANSI를 사용합니다.

개발서버 `api-dev.capefn.com:8080` 또는 운영서버 `api.capefn.com:8080`을 선택해 ID Login을 완료한 뒤, 오른쪽의 XML 입력값을 채워 **처음 조회**를 누릅니다. 운영 인증정보는 직접 입력하며 개발 기본값·비밀번호 가림 정책은 기존 Helper와 같습니다. 조회값의 업무상 허용 코드는 정식 서비스 안내에 따라 입력하십시오. XML에 없는 기본값이나 코드 의미를 만들어 넣지 않았습니다.

## Header Type 특징과 Request

B는 제품 Source의 PowerBase/BASE21 Header에 대응합니다. Header 구성·서비스 정보 조회는 OrdaAPI 내부에서 수행하므로 고객이 통신 Header를 직접 붙이지 않습니다. 공개 API는 t1001과 동일한 `RequestData`입니다. 동일 B 서비스라도 각 XML의 블록·필드·반복 여부는 별도 확인해야 합니다.

`main.cpp`의 `INPUT_FIELDS`, `BuildInput`, `RequestService`에서 아래 본문을 공백으로 초기화하고 직접 구성합니다. `attr` 속성이 없어 필드 사이 속성 바이트가 없습니다. NUL은 전송하지 않습니다.

| b8024InBlock Field | Type | Length | Offset |
| --- | --- | --- | --- |
| reccnt | char | 5 | 0 |
| lmtclasscd | char | 2 | 5 |
| expcode | char | 12 | 7 |
| AcntNo | char | 20 | 19 |
| jolmtgb | char | 1 | 39 |
| lmtvaluegb | char | 1 | 40 |

전체 입력은 **41바이트**입니다. `reccnt`와 응답 반복부 앞의 건수는 서로 다른 위치의 값입니다.

## Response와 반복 Block

`ParseService`는 다음 non-block 본문을 명시적으로 처리합니다.

```text
b8024OutBlock 41바이트
반복 건수 5바이트 ASCII
b8024OutBlock1 399바이트 × N
전체 길이 = 46 + 399 × N
```

건수·레코드 폭·전체 길이를 모두 검증합니다. 화면에는 전체 건수와 최대 200행의 `lmtclassnm`, `hname`, `lmtgbnm`, `lmtval`, `loanjanval`, `useablelmtamt`를 표시합니다. 전체 출력 정의는 XML에서 확인하십시오. 입력 Echo 블록의 `reccnt`를 반복 건수로 대신 사용하지 않습니다.

최신 사용자 헤더의 `b8024OutBlock`(고정 경계 41바이트)과 `b8024OutBlock1`(반복 399바이트)을 사용합니다. 두 구조체의 모든 필드 offset/길이와 크기·alignment를 컴파일 검사하고, 검증된 각 표시 레코드를 지역 `b8024OutBlock1`에 memcpy해 이름 있는 멤버를 읽습니다. 고정 Echo 필드는 기존처럼 추가 표시하지 않습니다. Request 구조체, B 연속 플래그/실제 Header key 및 메모리 해제 처리는 유지했습니다. 정상·연속조회 조합 57개와 예외/메타데이터 24개가 기존 결과와 일치했고 Debug/Release 빌드가 통과했습니다. 자세한 내용은 [B/C/D Response 보고서](../../../../../docs/query-bcd-struct-response-report.md)를 참고하십시오.

## 연속조회

B의 응답 `Header.szPNFlag=='Y'`와 실제로 수신한 비어 있지 않은 `Header.szContKey`가 모두 있을 때, 요청 해제가 완료된 후 **다음 조회**를 활성화합니다. 키의 공백을 보존해 `RequestData` 마지막 인수에 전달합니다. 검색 조건을 바꾸면 처음 조회부터 다시 시작해야 합니다. 화면에는 한 페이지씩 표시하며 자동 다음 조회는 하지 않습니다.

제품 Source에는 응답 키가 공개 패킷에 전달되지 않는 경로가 있습니다. 플래그만 있거나 키가 없으면 추측하지 않고 다음 조회를 비활성화합니다. 이 Sample에서 키가 항상 내려온다고 보장하지 않으며, 실제 페이지 전환은 사용자 서버 검증 대상입니다.

## 메시지와 메모리

`main.cpp`에서 REQUEST_DATA·MESSAGE_DATA·SYSTEM_ERROR_DATA·RELEASE_DATA 및 timeout을 처리합니다. 요청 ID 0도 유효합니다. 코드·본문 길이·블록 형태를 확인하고 제품 소유 버퍼를 즉시 복사합니다. 메시지는 `ReleaseMessageData`, 요청은 RELEASE_DATA/timeout에서 `ReleaseRequestData`로 정리하며 `delete/free`하지 않습니다. 오류·연결 해제·진행 중에는 추가 요청을 막습니다. 서버 메시지는 원문으로 표시합니다.

Debug/Release 빌드와 오프라인 계약 검증 결과, 실제 서버에서 확인할 내용은 [완료 보고서](../../../../../docs/query-header-type-samples-report.md)에 있습니다.
