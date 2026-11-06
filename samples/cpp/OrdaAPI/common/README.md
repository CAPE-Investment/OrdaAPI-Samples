# OrdaAPI Sample 공통 접속/Login Helper

`OrdaSampleCommon.h/.cpp`를 각 Sample Project에서 직접 컴파일합니다. VS2010 v100 / Win32 / MultiByte 기준이며 별도 DLL·Library 빌드는 필요 없습니다. 한 프로세스에 한 Sample 창과 한 번의 ID Login 시도를 지원합니다.

기존 `01_connection/login`의 동작과 코드를 기준으로 DLL Load·Initialize·Connect·Login·WM_LOGIN 문자열 복사·로그인 상태 확인·Logout·Disconnect·DLL 해제를 공통화했습니다. 개발/운영 서버 선택, 기본 개발 접속정보, 비밀번호 가림과 입력 삭제, 공통 상태/결과 UI를 함께 제공합니다. 기존 독립형 Login Sample은 변경하지 않았습니다.

Sample은 `CreateLoginControls` 후 `Initialize`를 호출하고 WndProc에서 `HandleMessage`를 전달합니다. `IsReady`로 서비스 버튼을 제어합니다. 동기 API 호출 전후 `BeginCall/EndCall`을 사용하고 종료 시 `BeginClose`가 TRUE일 때 `Cleanup` 후 창을 파괴합니다. 호출 중 WM_CLOSE는 호출 종료 뒤로 미룹니다. 내부 통지용으로 WM_APP+200을 사용하고 API 메시지 base는 WM_APP+100입니다.

`Api()`가 반환하는 인스턴스는 Helper 소유입니다. Sample은 이를 삭제하지 않습니다. 서비스 코드·입력 구성·RequestData·조회/실시간 메시지·출력 파싱·조회 데이터 수명 및 해제는 각 Sample에서 직접 구현합니다. Helper가 Query 메시지를 소비하거나 결과를 파싱하지 않습니다.

Runtime 작업 폴더는 공통 `sdk/bin`이며 실행 중 유지합니다. Cloud Login은 지원하지 않습니다. 운영서버 인증정보는 사용자가 직접 입력합니다. 첫 사용 예는 [t1001](../02_query/t1001-current-price/README.md)입니다.

같은 프로세스에서 로그인 재시도가 필요한 Sample은 Cleanup 및 API worker 종료 후 UI 스레드에서 ResetAfterCleanup(window)를 호출하고 Initialize를 다시 호출할 수 있습니다. API 인스턴스가 남아 있거나 Helper 호출 중이면 초기화를 거부합니다. 이전 통지·로그인 상태와 비밀번호를 지우고 입력을 다시 활성화하며 서버·ID는 유지합니다. 선택적 기능이므로 기존 Sample의 한 번 로그인 흐름은 유지됩니다. S3T Sample에서 사용합니다.
