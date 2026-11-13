# t3084 — 선물마스터조회(t3084)

[main.cpp](OrdaAPI-t3084-FuturesMaster/main.cpp)에서 입력 구성, 요청 API, 응답 처리와 종료 순서를 확인할 수 있습니다.

## 빌드와 실행

- Visual Studio 2010 / v100 / Win32 / MultiByte / C++03 / 순수 WinAPI입니다.
- `OrdaAPI-t3084-FuturesMaster.sln`을 열고 Debug 또는 Release / Win32로 빌드합니다.
- SDK Header는 `sdk/include`, 전문 구조체는 `sdk/structs/cpp`를 참조합니다. `ORDAAPI_INCLUDE_DIR` 재정의를 지원합니다.
- 디버거 작업 폴더는 저장소의 `sdk/bin`입니다. IDE 밖에서 실행할 때도 이 폴더를 작업 폴더로 지정합니다. SDK DLL을 Sample마다 복사하지 않습니다.
- 소스는 UTF-8 BOM으로 저장합니다. 화면은 Unicode WinAPI, 제품 API와 서버 원문은 MultiByte 경로를 사용합니다.

## 사용 순서

1. 접속정보를 확인하고 로그인합니다. Helper가 로그인 최종 통지와 세션 상태를 확인합니다.
2. 단축코드를 입력한 뒤 처음 조회를 누릅니다. XML 초기값은 공백 8바이트입니다.
3. 응답과 서버 메시지를 확인합니다. 로그인만으로 조회하지 않습니다.
4. XML에 출력 커서를 다음 입력으로 전달하는 정의가 없으므로 처음 조회만 요청합니다.
5. 창을 닫으면 진행 요청과 세션을 정리한 뒤 DLL을 해제합니다.

조회 Sample은 기존 한 번의 로그인 시도 방식을 유지합니다. 로그인 실패 후 다시 시도하려면 프로그램을 재실행합니다.

## 입력 항목

| 입력 필드 | 설명 | 바이트 | UI / 초기값 |
| --- | --- | ---: | --- |
| `focode` | 단축코드 | 8 | Edit / 공백 8개 |

XML에 정의된 기본값만 적용합니다. ComboBox의 설명 대신 실제 코드값을 전송하며 고정 길이 필드는 공백으로 채우고 종료 NUL을 전송하지 않습니다. Edit 입력 제한과 전송 전 바이트 길이 검사를 유지합니다.

## 코드 읽는 순서와 API

- `WinMain` → `Common::Initialize`: 공통 Helper의 Load/Initialize를 호출합니다.
- `WindowProc` → `Common::HandleMessage`: 로그인 버튼의 Connect/Login과 최종 로그인 통지를 처리합니다.
- `RequestService`: 입력 전문을 구성하고 `Common::Api()->RequestData`를 직접 호출합니다.
- `ReceiveService`: 응답의 요청 ID·서비스·블록·길이를 검증하고 실제 구조체 멤버를 표시합니다.
- `WM_CLOSE`: 진행 요청 해제 → Helper Cleanup의 Logout/Disconnect/DLL 해제 → 창 파괴 순서입니다.

```cpp
Common::Api()->RequestData(window, SERVICE_CODE, &input, sizeof(input), NULL);
```

반환값 0~255는 요청 ID이며 최종 조회 성공이 아닙니다. 한 번에 한 요청만 처리하며 RELEASE_DATA까지 다음 요청을 막습니다. RequestData 반환 전 재진입 통지는 ID가 정해진 뒤 원래 순서로 처리합니다.

## 응답과 메모리

A형 `t3084OutBlock`은 속성 바이트를 포함한 53바이트 반복 레코드입니다. 블록명과 전체 길이의 배수를 검증하고 구조체 단위로 접근합니다. lpData에는 별도 건수 접두부가 없습니다. 반복 결과는 최대 200행까지 표시합니다.

`WM_RECEIVE_DATA`에서 REQUEST_DATA, MESSAGE_DATA, SYSTEM_ERROR_DATA, RELEASE_DATA를 구분합니다. 응답과 lpData는 OrdaAPI 소유이므로 직접 해제하지 않습니다. 서버 메시지는 표시 후 ReleaseMessageData로, 요청 데이터는 RELEASE_DATA·타임아웃·종료 시 ReleaseRequestData로 정리합니다. 길이 검증 없이 전문을 문자열로 출력하거나 DLL 소유 포인터를 보관하지 않습니다.

실제 서버 테스트는 이번 가독성 정리에서 수행하지 않았습니다.
