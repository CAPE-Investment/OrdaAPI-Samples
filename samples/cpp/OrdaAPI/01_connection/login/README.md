# OrdaAPI Login Sample

OrdaAPI.dll의 로드 → 초기화 → 연결 → 로그인 요청 → 결과 메시지 수신 → 로그아웃 → 연결 종료 → DLL 해제 흐름을 보여주는 최소 Win32 GUI Sample입니다.

## 개발 환경

- Visual Studio 2010, v100 Toolset, Win32, MultiByte Character Set
- C++ / 순수 WinAPI, MFC·CMake·외부 UI Framework 미사용
- Project와 제품 API 호출은 MultiByte 기준입니다. Sample 자체 한글 UI는 명시적인 Unicode WinAPI 함수로 표시하며, 제품 응답 원문은 기존 ANSI 경로로 표시합니다.
- `main.cpp`는 VS2010에서 한글을 인식하도록 UTF-8 BOM으로 저장합니다. README는 GitHub용 UTF-8입니다.

## 필요한 Header와 DLL

공통 `sdk/include`의 `ordalib.h`, `errcode.h`를 참조합니다. 두 Header는 제품 원본 그대로이며 Sample마다 복사하지 않습니다.
`OrdaAPI` 클래스는 지정한 Runtime 폴더에서 LoadLibrary / GetProcAddress를 사용합니다. Import Library 링크는 사용하지 않습니다.

공통 `sdk/bin`에는 제품 `Output/Bin`의 파일과 하위 Directory 전체를 유지합니다. Sample EXE는 `bin/Debug`, `bin/Release`에 따로 출력합니다.

## 빌드

1. OrdaAPI-Samples Repository의 `sdk/include`, `sdk/bin` 구성을 준비합니다. 제품 Source Repository가 옆에 있을 필요는 없습니다.
2. `OrdaAPI-Login.sln`을 Visual Studio 2010으로 엽니다.
3. Debug / Win32 또는 Release / Win32를 Clean 후 Build합니다.

기본 Include는 `$(ProjectDir)..\..\..\..\..\..\sdk\include`이며, 외부 `ORDAAPI_INCLUDE_DIR` 지정값이 있으면 우선합니다. 별도 SDK 배치에서는 환경 변수 또는 MSBuild 속성으로 변경할 수 있으나 최종 고객 배포 계약을 확정하는 의미는 아닙니다.

## 공통 Runtime으로 실행

Visual Studio의 LocalDebuggerWorkingDirectory는 두 구성 모두 `$(ProjectDir)..\..\..\..\..\..\sdk\bin`입니다. F5 실행 시 이 폴더를 Working Directory로 사용합니다.
Sample은 GetCurrentDirectory로 이 경로를 구해 OrdaAPI::Load에 전달하며 이후에도 같은 Working Directory를 유지합니다. 이는 DLL뿐 아니라 Runtime의 설정·인증·하위 파일의 상대경로를 위한 것입니다.

IDE 밖에서는 Working Directory를 Repository의 `sdk/bin`으로 지정하고 Sample 출력 EXE를 실행하세요. EXE를 더블 클릭하여 실행할 때의 Working Directory는 보장하지 않습니다.
Windows 전역 PATH 변경, Sample별 Runtime 복사, sdk/bin으로의 Sample EXE 출력은 하지 않습니다.

Runtime을 복사한 것은 GitHub 외부 공개 승인이 아닙니다. `Cert/test-key.der`, `Cert/test-cert.der`, `xc_conf.txt`, `sys/system.ini`, PDB 및 제3자 DLL의 공개·재배포 여부를 별도 확인해야 합니다. 원본 Runtime은 임의 수정·제외하지 않았습니다.

## 실행

1. 공통 sdk/bin Runtime을 준비하고 Visual Studio에서 F5로 실행합니다.
2. 개발서버 또는 운영서버를 선택하고 서버·포트·로그인 ID·로그인 비밀번호·공동인증 비밀번호를 확인합니다. 모든 입력값은 직접 변경할 수 있습니다.
3. `로그인`을 눌러 연결과 로그인 요청을 수행합니다.
4. 접속 상태, 로그인 상태, 결과 코드, 결과 메시지를 확인합니다.
5. 창의 닫기 버튼으로 종료합니다. 새 시도는 프로그램을 다시 실행합니다. 이 Sample은 실행당 한 번의 연결·로그인 시도를 다룹니다.

두 비밀번호 입력창은 Password Edit 스타일로 가려 표시하며 입력한 고객 인증정보를 저장하거나 로그로 출력하지 않습니다. 요청 후 입력창과 Sample의 비밀번호 임시 버퍼를 비웁니다. 개발서버의 고객 테스트용 기본값은 Source 상수로 제공합니다.
현재 제품 내부 저장 공간에 맞춰 ID·두 비밀번호 입력은 각 15바이트 이하로 제한합니다. 이는 공식 계정 정책의 최대 길이를 정의하는 것이 아닙니다.

현재 DLL은 로그인 비밀번호와 공동인증 비밀번호가 모두 비어 있지 않으면 ID 로그인, 그 외에는 Cloud 인증 경로를 선택합니다. Cloud 경로에서는 SDK가 인증 UI를 표시할 수 있습니다. 사용할 인증 방식과 조건은 제품 문서를 따르세요.
연결·인증 함수에는 내부 대기가 있으므로 호출 중 화면이 잠시 응답하지 않을 수 있습니다.

## 접속 환경

기본 선택은 개발서버입니다. 서버 선택을 변경하면 아래 기본값으로 다시 설정되므로 직접 수정한 입력값은 대체됩니다.

| 항목 | 개발서버 | 운영서버 |
| --- | --- | --- |
| 서버 | api-dev.capefn.com | api.capefn.com |
| 포트 | 8080 | 8080 |
| 로그인 ID | APITEST | 고객 직접 입력 |
| 로그인 비밀번호 | 개발서버 테스트 기본값 자동 입력 | 빈 값으로 초기화 |
| 공동인증 비밀번호 | 개발서버 테스트 기본값 자동 입력 | 빈 값으로 초기화 |

### 개발서버 접속정보 안내

본 Sample에는 고객 개발·테스트용 기본 접속정보가 설정되어 있습니다. 로그인 ID, 로그인 비밀번호, 공동인증 비밀번호와 인증서 정보는 변경될 수 있습니다.
변경 시 당사 **OrdaAPI 관련 자료실**에서 최신 접속정보와 필요한 인증서 정보를 제공합니다. 기본값으로 로그인이 되지 않으면 자료실에서 최신 정보를 확인하세요. 자료실 URL은 TODO입니다.

### 운영서버 안내

운영서버에서는 **당사 OrdaAPI 사용등록을 완료한 계좌의 접속정보**를 사용해야 합니다. 운영서버 선택 시 테스트 ID와 두 비밀번호를 모두 비우며 사용자가 직접 입력해야 합니다. 운영서버 인증정보는 Source에 포함하지 않습니다.
다시 개발서버를 선택하면 개발서버 기본 접속정보가 자동 입력됩니다.

## Login 반환값과 결과 메시지

`OrdaAPI::Login()`의 양수 반환은 요청 단계의 처리 결과이며 로그인 성공 표시가 아닙니다. 0 이하이면 단계와 반환값을 표시합니다. Sample은 반환값에 임의의 오류 의미를 부여하지 않습니다.

최종 결과는 Connect에 전달한 `API_MESSAGE_BASE + WM_LOGIN`으로 받습니다. `WM_LOGIN` 값은 SDK Header를 사용합니다.

- wParam: 결과 코드 문자열 포인터
- lParam: 결과 설명 문자열 포인터
- WndProc에서 Sample 버퍼로 즉시 복사한 뒤 별도 UI 메시지로 결과를 표시합니다.
- DLL 포인터를 보관하거나 해제하지 않으며 `ReleaseMessageData()`를 호출하지 않습니다.
- 현재 제품의 성공 코드 `"0000"`과 `IsLogin() == 1`을 함께 확인하여 성공으로 표시합니다. 그 외 결과는 수신한 코드·설명을 그대로 표시합니다.

결과 메시지는 Login 호출이 반환되기 전에도 전달될 수 있습니다. WndProc의 로그인 통지 처리에서는 종료나 DLL 해제를 하지 않습니다.

## 종료

사용자가 창을 닫으면 로그인 상태를 확인하고 필요한 경우 Logout → Disconnect → `OrdaAPI` 객체 삭제(FreeLibrary) → 창 파괴 순서로 처리합니다.
Logout export는 void이고 동적 로딩 클래스의 반환값은 호출 상태입니다. 서버 로그아웃 완료 여부를 별도로 보장하는 값으로 표시하지 않습니다.
API 호출 도중 닫기 요청을 받으면 호출이 돌아온 뒤 정리합니다.

## 검증 범위

Source 및 Project 작성 완료. VS2010 v100(컴파일러 16.00.30319.01)으로 Debug/Release Win32 빌드 성공을 확인했습니다. 전체 상태는 [Sample Index](../../../../../docs/sample-index.md)를 참고하세요.
기존 sdk/bin Working Directory와 Runtime 참조 방식을 유지했습니다. 이번 한글 UI와 서버 전환은 Source 및 Debug/Release 빌드로 확인했으며 실행 UI는 미검증입니다. 실제 서버 로그인은 수행하지 않았습니다. 이전 Runtime 점검은 [Runtime 검증](../../../../../docs/runtime-review.md)을 참고하세요. 운영 인증정보는 Sample Source에 삽입하지 않습니다.
