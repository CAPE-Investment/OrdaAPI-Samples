# S3T — 주식 실시간 시세

[main.cpp](main.cpp)에서 등록·종목 전환·수신·해제 순서를 확인합니다. 기존 단일 메인 화면과 같은 창의 재로그인 기능을 유지합니다.

## 빌드와 실행

- Visual Studio 2010 / v100 / Win32 / MultiByte / C++03 / 순수 WinAPI입니다.
- `OrdaAPI-s3t.sln`을 열고 Debug 또는 Release / Win32로 빌드합니다.
- SDK Header·구조체와 기존 Common Helper를 참조합니다. 별도 MFC 의존성은 없습니다.
- 디버거와 직접 실행의 작업 폴더는 저장소 `sdk/bin`으로 지정합니다. SDK DLL을 복사하지 않습니다.
- 소스는 UTF-8 BOM으로 저장합니다.

## 사용 순서와 입력

1. 접속정보와 비밀번호를 입력하고 로그인합니다. 로그인 결과는 최종 통지와 세션 상태로 확인합니다.
2. 영문·숫자 6자리 종목코드를 입력하고 **요청**을 누릅니다. 자동 로그인·등록은 하지 않습니다.
3. 현재 등록 종목과 실시간 데이터를 확인합니다. 같은 종목을 다시 요청하면 추가 등록하지 않습니다.
4. 다른 종목을 요청하면 이전 종목 해제 성공 후 새 종목을 등록합니다. 기존 해제 실패 시 새 등록은 하지 않습니다.
5. **다시 로그인**은 기존 작업과 등록을 정리하고 같은 프로세스·윈도우·컨트롤에서 세션을 다시 초기화합니다. 서버·ID는 유지하고 비밀번호는 다시 입력합니다.
6. 종료 시 종목별 해제와 윈도우 전체 해제 뒤 로그아웃·연결 해제·DLL·스레드 정리를 수행합니다.

| 입력 | 구조체 길이 | API 전달 |
| --- | ---: | --- |
| shcode | InBlock의 char[6] | 키 목록용 char[7]에 종료 NUL을 추가하고 nUnitLen=6 전달 |

공통 Helper의 미리 설정된 비밀번호는 컨트롤 생성·서버 선택·재로그인 시 지웁니다. 비밀번호가 비어 있으면 SDK의 클라우드 인증이 시작될 수 있습니다.

## 코드 읽는 순서와 API

- `WinMain` / `WindowProc`: Common 초기화·로그인과 화면 메시지를 처리합니다.
- 요청 처리: 입력 검증 후 작업을 게시하고 진행 중 중복 요청을 막습니다.
- `ApiWorker`: 한 작업 스레드에서 UnadviseRealData(old) → AdviseRealData(new)를 순서대로 호출합니다.
- 실시간 수신: 서비스·본문 길이·현재 등록 종목을 확인하고 지역 OutBlock에 복사한 뒤 화면을 갱신합니다.
- 종료·재로그인: 작업 완료 후 UnadviseRealData → UnadviseWindow → Common Cleanup → 작업 스레드 종료 순서를 지킵니다.

```cpp
api->AdviseRealData(context->window, NAME_S3T, job->key, sizeof(InBlock));
api->UnadviseRealData(context->window, NAME_S3T, job->oldKey, sizeof(InBlock));
api->UnadviseWindow(context->window);
```

API 반환값 1은 로컬 호출 성공이며 서버 등록 ACK나 실제 데이터 수신을 보장하지 않습니다. 등록·해제 API를 수신 콜백 안에서 호출하지 않습니다. UI는 작업 스레드의 동기 DLL 통지를 계속 처리하며 완료 전 API 포인터나 작업 메모리를 해제하지 않습니다.

## 수신과 오류 처리

`API_BASE + WM_RECEIVE_REAL_DATA`의 BC_ITEM_INFO에서 서비스 S3T, 본문 223바이트, 키 길이 6과 현재 등록 키를 확인합니다. 본문을 복사한 뒤 OutBlock.shcode도 비교합니다. 전환 중·이전 종목·해제 뒤 늦게 도착한 통지는 무시합니다.

BC_ITEM_INFO와 그 버퍼는 통지 중에만 유효합니다. 포인터를 보관하거나 직접 해제하지 않으며 고정 길이 문자열에 strlen을 적용하지 않습니다. shcode, chetime, price, sign, change, drate, cgubun, cvolume, volume, value, cpower, offerho, bidho, status를 표시합니다.

기존 종목 해제 실패 시 이전 등록 상태를 유지하지만 수신 지속이나 SDK 내부 상태까지 보장하지는 않습니다. 신규 등록 실패 시 이전 화면 표시를 비우고 UnadviseWindow로 잔여 상태를 정리합니다. 자동 재등록·보상 등록은 하지 않습니다. 정리가 완료되지 않으면 윈도우와 DLL을 유지하여 버퍼·스레드 수명을 보호합니다.

재로그인은 Cleanup과 작업 스레드 종료 후 ResetAfterCleanup → Initialize를 수행합니다. 새 프로세스를 만들지 않으며 이전 세션 통지가 새 로그인 상태에 섞이지 않도록 합니다.

실제 서버 테스트는 이번 가독성 정리에서 수행하지 않았습니다.
