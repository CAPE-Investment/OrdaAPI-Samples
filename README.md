# OrdaAPI 3.0 Samples

OrdaAPI 3.0 고객 개발자를 위한 공식 Sample Code Repository입니다.
[OrdaAPI Login Sample](samples/cpp/OrdaAPI/01_connection/login/README.md)과 [Query 대표 Sample 5개](samples/cpp/OrdaAPI/02_query/README.md)의 Source와 Visual Studio 2010 Project를 제공합니다. t1001은 사용자가 개발서버 검증을 완료했으며 신규 Header Type 대표 4개는 빌드·오프라인 검증 완료 상태입니다. 개요 목록의 나머지 50개 항목은 Planned이며, 전체 109개 XML 서비스별 계획은 Query 서비스 Index에서 확인할 수 있습니다.

## Repository 목적

고객이 원하는 기능을 쉽게 찾고, 개별 Sample 하나만으로 API 사용 흐름을 이해할 수 있도록 합니다.
Sample은 최대한 단순하게 작성하고 가능한 한 독립적으로 빌드·실행하도록 설계합니다.

## 개발 환경 및 언어

- Windows / Visual Studio 2010 / C++ / WinAPI
- MFC 미사용, 최신 Visual Studio 전용 기능 및 최신 C++ 문법에 의존하지 않음
- 향후 Visual Studio Solution / Project 방식으로 제공하며 현재 CMake는 사용하지 않음
- 현재 작성 대상 언어: [C++](samples/cpp/README.md)
- 향후 추가 예정: C#, Python, Delphi
- TODO: 지원 Windows 버전, SDK 버전, x86/x64 및 문자 집합 확정

향후 언어를 추가할 때도 같은 분류 원칙을 따릅니다. 해당 언어에서 지원하지 않거나 권장하지 않는 모듈의 빈 폴더는 만들지 않습니다.

## 모듈

| 모듈 | Sample 위치 | 상세 역할 |
| --- | --- | --- |
| OrdaAPI | [OrdaAPI](samples/cpp/OrdaAPI/) | TODO: 공식 역할 및 사용 조건 확인 |
| OrdaAPISvr | [OrdaAPISvr](samples/cpp/OrdaAPISvr/) | TODO: 공식 역할 및 사용 조건 확인 |
| OrdaSDK | [OrdaSDK](samples/cpp/OrdaSDK/) | TODO: 공식 역할 및 사용 조건 확인 |
| OrdaSDKSvr | [OrdaSDKSvr](samples/cpp/OrdaSDKSvr/) | TODO: 공식 역할 및 사용 조건 확인 |

동일한 기능도 모듈마다 별도의 Sample로 관리합니다. 한 Sample에서 조건문으로 모듈을 선택하는 방식은 사용하지 않습니다.

## Repository 탐색 방법

언어 → 모듈 → 기능 → Sample 순서로 탐색합니다.

예: samples/cpp/OrdaSDK/02_query/current-price

| 기능 폴더 | 내용 |
| --- | --- |
| 01_connection | 접속 및 로그인 |
| 02_query | 현재가, 호가, 시장 체결 조회 |
| 03_realtime | 현재가, 호가, 시장 체결 실시간 수신 |
| 04_account | 잔고, 미체결, 계좌 체결내역 |
| 05_order | 신규, 정정, 취소 주문 |

## 공통 SDK / Runtime

- `sdk/include`: Sample Compile에 필요한 공개 Header입니다. 현재 `ordalib.h`, `errcode.h`를 제품 원본 그대로 포함합니다.
- `sdk/bin`: 제품 `Output/Bin`의 모든 파일과 하위 Directory 구조를 유지하는 공통 Runtime입니다.
- Sample 실행 파일은 각 Sample의 `bin/Debug`, `bin/Release`에 출력하며 Runtime과 분리합니다.
- Visual Studio에서 Login Sample을 실행하면 `sdk/bin`을 Working Directory로 사용합니다. 다른 C++ Sample에도 같은 구조를 적용할 예정입니다.
- Runtime은 Sample별로 복사하지 않으며 Windows 전역 PATH를 변경하지 않습니다.

Runtime 복사는 외부 공개 승인을 의미하지 않습니다. 인증 관련 파일, 설정 및 디버그 심볼 등은 GitHub 공개 전에 별도 검토가 필요합니다. 현재 전체 Runtime 원본은 임의 제외하거나 수정하지 않았습니다. [Runtime 검증 및 공개 전 확인](docs/runtime-review.md)을 참고하세요.

## 안내 문서

- [Getting Started](docs/getting-started.md)
- [Sample Index](docs/sample-index.md)
- [문제 해결](docs/troubleshooting.md)

공식 문서 URL: TODO

고객 지원 채널: TODO

## 공개 정보 관리

실제 계좌번호, 사용자 ID, 비밀번호, 인증정보, Token, 사내 IP, 내부 서버 주소, 내부 시스템 정보, 개인정보, 직원 정보, 개발자 PC의 절대 경로 및 운영 환경 접속 정보를 포함하지 않습니다.
문서, Sample, 로그, 화면 캡처 및 이슈에도 동일하게 적용하며 필요한 값은 Placeholder 또는 명확한 예제 값으로 표시합니다.
실제 설정 및 인증정보는 저장소 밖에서 관리하는 것을 원칙으로 합니다.

.gitignore는 이미 추적 중인 파일이나 Git 이력을 정리하지 않습니다. 공개 전 파일과 이력에 민감정보가 없는지 확인하며, 인증정보 노출 시 폐기·재발급 및 이력 정리를 수행합니다.

TODO: 공개 시 Sample 이용 조건 및 SDK 재배포 가능 범위 확정
