# Sample Index

C++의 모듈별 Sample 개요 목록입니다. OrdaAPI login과 Query 대표 5개가 Implemented이며, 나머지 50개 개요 항목은 Planned입니다. 전체 109개 서비스의 Header Type과 계획은 [Query 서비스 Index](../samples/cpp/OrdaAPI/02_query/README.md)에 있습니다.
Implemented는 Source·Project 및 빌드 완료를 의미합니다. login과 t1001은 사용자 개발서버 검증 완료이며, 신규 b8024·d7304·d8311·t3084는 Debug/Release 빌드·오프라인 검증 완료, 실제 서버 미검증입니다.
API/TR은 공식 명세 확인 후 기입합니다. 아래 목록은 계획이며 실제 모듈별 기능 지원을 확정하는 명세가 아닙니다.

## OrdaAPI

| 모듈 | 기능 | Sample | 유형 | API/TR | 설명 | 상태 |
| --- | --- | --- | --- | --- | --- | --- |
| OrdaAPI | Connection | [login](../samples/cpp/OrdaAPI/01_connection/login/) | Win32 GUI | OrdaAPI_Initialize / OrdaAPI_Connect / OrdaAPI_IsConnected / OrdaAPI_Login / OrdaAPI_IsLogin / OrdaAPI_Logout / OrdaAPI_Disconnect | 초기화, 접속, 로그인 결과 수신 및 종료 | Implemented |
| OrdaAPI | Query | [t1001-current-price](../samples/cpp/OrdaAPI/02_query/t1001-current-price/) | Win32 GUI | t1001 / OrdaAPI_RequestData | Header A, 주식현재가, 사용자 개발서버 검증 완료 | Implemented |
| OrdaAPI | Query | [b8024-credit-limit](../samples/cpp/OrdaAPI/02_query/b8024-credit-limit/) | Win32 GUI | b8024 / OrdaAPI_RequestData | Header B, 고정+반복 출력, 서버 미검증 | Implemented |
| OrdaAPI | Query | [d7304-business-days](../samples/cpp/OrdaAPI/02_query/d7304-business-days/) | Win32 GUI | d7304 / OrdaAPI_RequestData | Header C, 영업일정보 고정 출력, 서버 미검증 | Implemented |
| OrdaAPI | Query | [d8311-stock-balance](../samples/cpp/OrdaAPI/02_query/d8311-stock-balance/) | Win32 GUI | d8311 / OrdaAPI_RequestData | Header D, CTS+반복 잔고, 서버 미검증 | Implemented |
| OrdaAPI | Query | [t3084-futures-master](../samples/cpp/OrdaAPI/02_query/t3084-futures-master/) | Win32 GUI | t3084 / OrdaAPI_RequestData | Header A, 블록별 반복, 서버 미검증 | Implemented |
| OrdaAPI | Query | [quote](../samples/cpp/OrdaAPI/02_query/quote/) | Win32 | TODO | 호가 조회 | Planned |
| OrdaAPI | Query | [execution](../samples/cpp/OrdaAPI/02_query/execution/) | Win32 | TODO | 시장 체결 데이터 조회 | Planned |
| OrdaAPI | Realtime | [current-price](../samples/cpp/OrdaAPI/03_realtime/current-price/) | Win32 GUI | TODO | 실시간 현재가 수신 및 해제 | Planned |
| OrdaAPI | Realtime | [quote](../samples/cpp/OrdaAPI/03_realtime/quote/) | Win32 GUI | TODO | 실시간 호가 수신 및 해제 | Planned |
| OrdaAPI | Realtime | [execution](../samples/cpp/OrdaAPI/03_realtime/execution/) | Win32 GUI | TODO | 실시간 시장 체결 데이터 수신 및 해제 | Planned |
| OrdaAPI | Account | [balance](../samples/cpp/OrdaAPI/04_account/balance/) | Win32 | TODO | 계좌 잔고 조회 | Planned |
| OrdaAPI | Account | [open-orders](../samples/cpp/OrdaAPI/04_account/open-orders/) | Win32 | TODO | 계좌 미체결 주문 조회 | Planned |
| OrdaAPI | Account | [executions](../samples/cpp/OrdaAPI/04_account/executions/) | Win32 | TODO | 계좌 주문 체결내역 조회 | Planned |
| OrdaAPI | Order | [new-order](../samples/cpp/OrdaAPI/05_order/new-order/) | Win32 | TODO | 신규 주문 흐름 | Planned |
| OrdaAPI | Order | [amend-order](../samples/cpp/OrdaAPI/05_order/amend-order/) | Win32 | TODO | 정정 주문 흐름 | Planned |
| OrdaAPI | Order | [cancel-order](../samples/cpp/OrdaAPI/05_order/cancel-order/) | Win32 | TODO | 취소 주문 흐름 | Planned |

## OrdaAPISvr

| 모듈 | 기능 | Sample | 유형 | API/TR | 설명 | 상태 |
| --- | --- | --- | --- | --- | --- | --- |
| OrdaAPISvr | Connection | [login](../samples/cpp/OrdaAPISvr/01_connection/login/) | Win32 | TODO | 초기화, 접속, 로그인 및 종료 | Planned |
| OrdaAPISvr | Query | [current-price](../samples/cpp/OrdaAPISvr/02_query/current-price/) | Win32 | TODO | 현재가 조회 | Planned |
| OrdaAPISvr | Query | [quote](../samples/cpp/OrdaAPISvr/02_query/quote/) | Win32 | TODO | 호가 조회 | Planned |
| OrdaAPISvr | Query | [execution](../samples/cpp/OrdaAPISvr/02_query/execution/) | Win32 | TODO | 시장 체결 데이터 조회 | Planned |
| OrdaAPISvr | Realtime | [current-price](../samples/cpp/OrdaAPISvr/03_realtime/current-price/) | Win32 GUI | TODO | 실시간 현재가 수신 및 해제 | Planned |
| OrdaAPISvr | Realtime | [quote](../samples/cpp/OrdaAPISvr/03_realtime/quote/) | Win32 GUI | TODO | 실시간 호가 수신 및 해제 | Planned |
| OrdaAPISvr | Realtime | [execution](../samples/cpp/OrdaAPISvr/03_realtime/execution/) | Win32 GUI | TODO | 실시간 시장 체결 데이터 수신 및 해제 | Planned |
| OrdaAPISvr | Account | [balance](../samples/cpp/OrdaAPISvr/04_account/balance/) | Win32 | TODO | 계좌 잔고 조회 | Planned |
| OrdaAPISvr | Account | [open-orders](../samples/cpp/OrdaAPISvr/04_account/open-orders/) | Win32 | TODO | 계좌 미체결 주문 조회 | Planned |
| OrdaAPISvr | Account | [executions](../samples/cpp/OrdaAPISvr/04_account/executions/) | Win32 | TODO | 계좌 주문 체결내역 조회 | Planned |
| OrdaAPISvr | Order | [new-order](../samples/cpp/OrdaAPISvr/05_order/new-order/) | Win32 | TODO | 신규 주문 흐름 | Planned |
| OrdaAPISvr | Order | [amend-order](../samples/cpp/OrdaAPISvr/05_order/amend-order/) | Win32 | TODO | 정정 주문 흐름 | Planned |
| OrdaAPISvr | Order | [cancel-order](../samples/cpp/OrdaAPISvr/05_order/cancel-order/) | Win32 | TODO | 취소 주문 흐름 | Planned |

## OrdaSDK

| 모듈 | 기능 | Sample | 유형 | API/TR | 설명 | 상태 |
| --- | --- | --- | --- | --- | --- | --- |
| OrdaSDK | Connection | [login](../samples/cpp/OrdaSDK/01_connection/login/) | Win32 | TODO | 초기화, 접속, 로그인 및 종료 | Planned |
| OrdaSDK | Query | [current-price](../samples/cpp/OrdaSDK/02_query/current-price/) | Win32 | TODO | 현재가 조회 | Planned |
| OrdaSDK | Query | [quote](../samples/cpp/OrdaSDK/02_query/quote/) | Win32 | TODO | 호가 조회 | Planned |
| OrdaSDK | Query | [execution](../samples/cpp/OrdaSDK/02_query/execution/) | Win32 | TODO | 시장 체결 데이터 조회 | Planned |
| OrdaSDK | Realtime | [current-price](../samples/cpp/OrdaSDK/03_realtime/current-price/) | Win32 GUI | TODO | 실시간 현재가 수신 및 해제 | Planned |
| OrdaSDK | Realtime | [quote](../samples/cpp/OrdaSDK/03_realtime/quote/) | Win32 GUI | TODO | 실시간 호가 수신 및 해제 | Planned |
| OrdaSDK | Realtime | [execution](../samples/cpp/OrdaSDK/03_realtime/execution/) | Win32 GUI | TODO | 실시간 시장 체결 데이터 수신 및 해제 | Planned |
| OrdaSDK | Account | [balance](../samples/cpp/OrdaSDK/04_account/balance/) | Win32 | TODO | 계좌 잔고 조회 | Planned |
| OrdaSDK | Account | [open-orders](../samples/cpp/OrdaSDK/04_account/open-orders/) | Win32 | TODO | 계좌 미체결 주문 조회 | Planned |
| OrdaSDK | Account | [executions](../samples/cpp/OrdaSDK/04_account/executions/) | Win32 | TODO | 계좌 주문 체결내역 조회 | Planned |
| OrdaSDK | Order | [new-order](../samples/cpp/OrdaSDK/05_order/new-order/) | Win32 | TODO | 신규 주문 흐름 | Planned |
| OrdaSDK | Order | [amend-order](../samples/cpp/OrdaSDK/05_order/amend-order/) | Win32 | TODO | 정정 주문 흐름 | Planned |
| OrdaSDK | Order | [cancel-order](../samples/cpp/OrdaSDK/05_order/cancel-order/) | Win32 | TODO | 취소 주문 흐름 | Planned |

## OrdaSDKSvr

| 모듈 | 기능 | Sample | 유형 | API/TR | 설명 | 상태 |
| --- | --- | --- | --- | --- | --- | --- |
| OrdaSDKSvr | Connection | [login](../samples/cpp/OrdaSDKSvr/01_connection/login/) | Win32 | TODO | 초기화, 접속, 로그인 및 종료 | Planned |
| OrdaSDKSvr | Query | [current-price](../samples/cpp/OrdaSDKSvr/02_query/current-price/) | Win32 | TODO | 현재가 조회 | Planned |
| OrdaSDKSvr | Query | [quote](../samples/cpp/OrdaSDKSvr/02_query/quote/) | Win32 | TODO | 호가 조회 | Planned |
| OrdaSDKSvr | Query | [execution](../samples/cpp/OrdaSDKSvr/02_query/execution/) | Win32 | TODO | 시장 체결 데이터 조회 | Planned |
| OrdaSDKSvr | Realtime | [current-price](../samples/cpp/OrdaSDKSvr/03_realtime/current-price/) | Win32 GUI | TODO | 실시간 현재가 수신 및 해제 | Planned |
| OrdaSDKSvr | Realtime | [quote](../samples/cpp/OrdaSDKSvr/03_realtime/quote/) | Win32 GUI | TODO | 실시간 호가 수신 및 해제 | Planned |
| OrdaSDKSvr | Realtime | [execution](../samples/cpp/OrdaSDKSvr/03_realtime/execution/) | Win32 GUI | TODO | 실시간 시장 체결 데이터 수신 및 해제 | Planned |
| OrdaSDKSvr | Account | [balance](../samples/cpp/OrdaSDKSvr/04_account/balance/) | Win32 | TODO | 계좌 잔고 조회 | Planned |
| OrdaSDKSvr | Account | [open-orders](../samples/cpp/OrdaSDKSvr/04_account/open-orders/) | Win32 | TODO | 계좌 미체결 주문 조회 | Planned |
| OrdaSDKSvr | Account | [executions](../samples/cpp/OrdaSDKSvr/04_account/executions/) | Win32 | TODO | 계좌 주문 체결내역 조회 | Planned |
| OrdaSDKSvr | Order | [new-order](../samples/cpp/OrdaSDKSvr/05_order/new-order/) | Win32 | TODO | 신규 주문 흐름 | Planned |
| OrdaSDKSvr | Order | [amend-order](../samples/cpp/OrdaSDKSvr/05_order/amend-order/) | Win32 | TODO | 정정 주문 흐름 | Planned |
| OrdaSDKSvr | Order | [cancel-order](../samples/cpp/OrdaSDKSvr/05_order/cancel-order/) | Win32 | TODO | 취소 주문 흐름 | Planned |

TODO: 주문/체결 Event、접속 상태 Event 및 재접속 Event 등 추가 Sample 범위와 위치 확정
