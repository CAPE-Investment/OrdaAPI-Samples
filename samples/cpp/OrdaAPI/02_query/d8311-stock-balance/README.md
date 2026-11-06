# d8311 주식잔고 조회 — Header Type D

[XML 정의](../../../../../sdk/bin/sys/d8311.xml)에 대응하는 D 대표 Sample입니다. 속성 바이트가 있는 입력, 연속 종목번호 블록, 반복 잔고 블록과 조건부 수동 다음 조회를 보여줍니다. 실제 서버 Login/Query는 자동 수행하지 않았습니다.

## 개발 환경 및 실행

VS2010 / v100 / Win32 / MultiByte / WinAPI이며 MFC와 C++11은 사용하지 않습니다. `OrdaAPI-d8311-StockBalance.sln`을 열어 Debug/Release Win32로 빌드합니다. [공통 ID Login Helper](../../common/README.md)는 Source로 직접 컴파일합니다. 공통 Header·Runtime은 `sdk/include`, `sdk/bin`이며 Debugging Working Directory와 IDE 밖의 작업 폴더도 `sdk/bin`입니다. 실행 파일은 Sample의 `bin/Debug`, `bin/Release`에 생성됩니다.

개발서버 `api-dev.capefn.com:8080` 또는 운영서버 `api.capefn.com:8080`을 선택해 로그인한 뒤 오른쪽 계좌 입력을 채웁니다. 운영 로그인 정보와 조회 계좌 정보는 직접 입력합니다. **계좌 비밀번호는 Login 비밀번호와 다른 XML 입력값**입니다. 비밀번호는 가려 표시하고 각 조회 후 지우므로 다음 조회 때 다시 입력하십시오.

## Header Type과 Request

D는 제품 Source의 TUXIDO & PowerBase Header입니다. 제품이 Header를 구성하므로 Sample에서 wire Header를 추가하지 않습니다. 공개 API는 기존 `RequestData`이며 서비스 본문과 연속 키만 인수로 전달합니다. 동일 D 서비스도 필드 폭·attr·반복 구조가 다르므로 이 offset을 다른 서비스에 복사하면 안 됩니다.

`d8311InBlock`은 `attr=1`이며 각 필드 뒤에 속성 공간 1바이트를 포함해 **49바이트**입니다.

| Field | Type | Length | Offset | UI 처리 |
| --- | --- | --- | --- | --- |
| accno | char | 11 | 0 | 계좌번호 직접 입력 |
| passwd | char | 8 | 12 | 비밀번호 가림, 요청 후 지움 |
| expcode | char | 12 | 21 | 종목번호 직접 입력 |
| cts_expcode | char | 12 | 34 | 처음은 XML 기본 공백, 다음은 응답값; 읽기 전용 표시 |
| exchange | char | 1 | 47 | XML 기본값 3; 1=KRX, 2=NXT, 3=통합 |

속성 공간은 공백으로 초기화하고 NUL을 추가 전송하지 않습니다. 계좌·종목 값의 업무상 허용 범위는 정식 안내에 따라 입력하십시오. 처음 조회는 항상 빈 CTS를 사용합니다.

## Response와 반복 Block

`main.cpp`에서 다음 non-block 본문을 검증합니다.

```text
d8311OutBlock: cts_expcode[12] + 속성 공간 = 13바이트
반복 건수: 5바이트 ASCII
d8311OutBlock1: 속성 공간 포함 150바이트 × N
전체 길이 = 18 + 150 × N
```

화면에는 연속 종목번호, 전체 건수와 최대 200행의 `expcode`, `janqty`, `mdposqt`, `pamt`, `price`, `sunikrt`를 표시합니다. 건수·레코드 폭·정확한 전체 길이를 검증하고 각 필드를 복사합니다. 전체 10개 반복 필드 정의는 XML에서 확인하십시오.

최신 사용자 헤더의 `d8311OutBlock`(13바이트)과 `d8311OutBlock1`(150바이트)을 사용합니다. 모든 필드 offset/길이·속성 위치와 크기·alignment를 컴파일 검사합니다. 전체 길이·반복 건수 검증 후 고정 블록과 각 표시 레코드를 지역 구조체에 memcpy하고, `fixed.cts_expcode`와 반복 멤버에서 값을 읽습니다. 본문 커서 12바이트와 Header key 18바이트는 각각 보존합니다. 기존 Request와 연속조회 조건·UI·메모리 해제 처리는 유지했습니다. 정상·연속조회 조합 57개와 예외/메타데이터 24개가 기존 결과와 일치했고 Debug/Release 빌드가 통과했습니다. 자세한 내용은 [B/C/D Response 보고서](../../../../../docs/query-bcd-struct-response-report.md)를 참고하십시오.

## 연속조회

다음 조건이 모두 충족될 때만 요청 해제 후 **다음 조회**를 활성화합니다.

- D의 공개 응답 연속 플래그가 `Header.szPNFlag=='1'`.
- 실제 수신한 `Header.szContKey`가 비어 있지 않음.
- `d8311OutBlock.cts_expcode`가 비어 있지 않음.
- 오류가 없고 검색 조건이 이전 페이지와 같음.

다음 요청에서는 XML cursor를 입력 offset 34에 넣고 Header key는 공백을 보존해 `RequestData` 마지막 인수로 전달합니다. 두 값은 별개이며 서로 대체하지 않습니다. 계좌 비밀번호는 다시 입력하되 조건 비교용 저장 버퍼에는 보관하지 않습니다. 페이지는 수동으로 요청하며 현재 페이지를 표시합니다.

제품 Source에는 공개 패킷에 연속 키가 복사되지 않는 경로도 있습니다. 그런 경우 빈 문자열 같은 대체 키를 만들어 요청하지 않고 다음 조회를 비활성화합니다. 실제 D 응답 플래그와 페이지 전환은 사용자 서버 확인이 필요합니다.

## 메시지·메모리

Query 요청·응답·반복 파싱은 모두 이 Sample의 `main.cpp`에 있습니다. 제품 소유 RP_DATA_INFO/lpData는 표시 전에 복사하고 `delete/free`하지 않습니다. MSG_PACKET은 복사 후 `ReleaseMessageData`, 요청은 RELEASE_DATA/timeout/종료에서 `ReleaseRequestData`로 정리합니다. 요청 ID 0, 호출 중 재진입, 조회 중 중복 요청·로그인 전 요청·접속 해제 후 요청을 처리합니다. 서버 메시지는 원문을 표시합니다.

검증 결과와 남은 실제 서버 확인 항목은 [완료 보고서](../../../../../docs/query-header-type-samples-report.md)에 있습니다.
