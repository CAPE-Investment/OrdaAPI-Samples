# C++ Samples

[OrdaAPI Login Sample](OrdaAPI/01_connection/login/README.md)과 [Header Type별 Query 대표 Sample 5개](OrdaAPI/02_query/README.md)의 Source, .sln 및 .vcxproj를 제공합니다. t1001은 사용자 개발서버 검증 완료, 신규 b8024·d7304·d8311·t3084는 빌드·오프라인 검증 완료 상태입니다. 나머지 개요 항목 50개는 Planned이며 실제 109개 서비스별 계획은 Query 서비스 Index에 있습니다.
Login Sample은 VS2010 v100 Debug/Release Win32 빌드를 확인했으며, 실제 DLL·서버 실행은 미검증입니다.

## 기준 개발 환경

- Windows / Visual Studio 2010 / C++ / WinAPI
- MFC 미사용
- Visual Studio 2010에서 빌드 가능한 문법과 기능 사용
- 최신 Visual Studio 전용 기능 및 최신 C++ 문법에 의존하지 않음
- CMake 미사용
- TODO: 지원 Windows 버전, SDK 버전, x86/x64 및 문자 집합 확정

## 지원 모듈 및 구조

- [OrdaAPI](OrdaAPI/)
- [OrdaAPISvr](OrdaAPISvr/)
- [OrdaSDK](OrdaSDK/)
- [OrdaSDKSvr](OrdaSDKSvr/)

각 모듈 아래에 01_connection, 02_query, 03_realtime, 04_account, 05_order와 개별 Sample을 배치합니다.
예: OrdaSDK/02_query/current-price

모듈별 상세 역할과 API 사용 조건은 TODO입니다. 같은 기능이라도 모듈마다 별도의 독립 Sample로 관리하며, 한 Sample에서 조건문으로 모듈을 선택하지 않습니다.

## Sample 종류

| 유형 | 대상 | 원칙 |
| --- | --- | --- |
| 기본 Win32 Sample | 초기화, 접속, 로그인, 조회, 계좌, 주문 | API 사용법이 드러나는 단순한 구성, 불필요한 UI와 Framework 배제 |
| Win32 GUI Sample | 실시간 데이터, 주문/체결 Event, 접속 상태 및 재접속 Event | 순수 WinAPI의 최소 화면으로 데이터와 상태 확인, MFC 미사용 |

현재 03_realtime의 Sample은 Win32 GUI로 계획합니다.
TODO: 추가 Event Sample의 제공 범위와 폴더 위치 확정
TODO: SDK 이벤트 전달 방식 확인 후 기본 Win32 Sample의 콘솔 또는 최소 창 형태 결정

## 독립성 원칙

각 Sample은 필요한 초기화 → 접속 → 로그인 → API 호출 → 결과 처리 → 종료 흐름을 자체 수행하도록 설계합니다.
다른 Sample을 먼저 실행하거나 다른 Sample의 Source를 복사해야 동작하는 구조는 사용하지 않습니다.
학습 편의성을 위한 초기화 코드 중복을 허용하며 복잡한 공통 Framework를 만들지 않습니다.

## 향후 빌드 및 실행 안내

Visual Studio 2010 Solution / Project 방식으로 개별 Sample을 제공할 예정입니다.
TODO: 개별 .sln / .vcxproj 구성 및 빌드 절차 확정
TODO: 모듈별 헤더·라이브러리·DLL 경로와 실행 설정 방법 추가

빈 Sample 폴더의 .gitkeep은 Git에서 폴더를 유지하기 위한 파일입니다.

전체 목록은 [Sample Index](../../docs/sample-index.md)를 참고하세요.
