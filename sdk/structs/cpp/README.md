# OrdaAPI 3.0 C++ 서비스 구조체

이 폴더는 C++ Sample에서 사용할 서비스별 데이터 구조체 Header를 관리합니다. 현재는 폴더와 관리 규칙만 제공하며 실제 구조체 Header는 사용자가 직접 작성하여 추가합니다.

## 파일명과 서비스 구분

서비스 코드별로 하나의 Header 파일을 사용하는 것을 기본 원칙으로 하며 파일명은 `<서비스코드>.h`로 지정합니다. 예: `t1001.h`, `t3084.h`, `b8024.h`, `d7304.h`, `d8311.h`.

- 5자리 서비스 코드: 조회 서비스 (예: `t1001`)
- 3자리 서비스 코드: 실시간 서비스

## XML 및 Sample과의 관계

저장소 루트를 기준으로 동일한 서비스 코드가 다음 파일과 Sample을 연결합니다.

- XML 정의: `sdk/bin/sys/<서비스코드>.xml`
- C++ 구조체: `sdk/structs/cpp/<서비스코드>.h`
- 조회 Sample: `samples/cpp/OrdaAPI/02_query/`
- 실시간 Sample: `samples/cpp/OrdaAPI/03_realtime/`

예를 들어 `sdk/bin/sys/t1001.xml`, `sdk/structs/cpp/t1001.h`, `samples/cpp/OrdaAPI/02_query/t1001-current-price/`가 동일 서비스를 나타냅니다. 이 Header 경로는 향후 사용자가 파일을 추가할 위치입니다.

XML 기반 자동 생성, 임의 구조체 선언, 필드명 및 자료형 추측을 수행하지 않습니다. 필드 길이, Packing 및 Alignment를 임의로 변경하지 않습니다.

## Visual Studio 2010 Include 설정

프로젝트 속성의 `C/C++ > 일반 > 추가 포함 디렉터리`에 기존 `sdk/include` 경로와 상속 설정을 유지하면서 `sdk/structs/cpp`의 상대경로를 추가합니다. 현재 Query 프로젝트는 Debug/Release Win32에 공통으로 다음 설정을 사용합니다.

```text
$(ORDAAPI_INCLUDE_DIR);$(ProjectDir)..\..\..\..\..\..\sdk\structs\cpp;%(AdditionalIncludeDirectories)
```

`ORDAAPI_INCLUDE_DIR`의 기존 기본값은 `$(ProjectDir)..\..\..\..\..\..\sdk\include`입니다. 새 프로젝트는 해당 프로젝트 위치에 맞게 상대경로를 계산하며 절대경로를 하드코딩하지 않습니다.

사용자가 `t1001.h`를 추가한 후 해당 구조체를 사용하는 Sample에서 다음과 같이 포함할 수 있습니다.

```cpp
#include "t1001.h"
```

현재 Sample Source에는 이 Include를 추가하지 않으며 Request Buffer 구성과 Response Parsing도 그대로 유지합니다. 구조체를 추가하는 것만으로 기존 Sample이 구조체를 사용하도록 전환되지는 않습니다.

Visual Studio 2010, Platform Toolset `v100`, Win32, MultiByte, WinAPI 및 No MFC 환경을 유지합니다. 구조체 추가 또는 변경 후에는 해당 서비스 Sample의 Debug/Release Win32 Build와 실제 서비스 동작을 검증해야 합니다. 구조체 기반 Request/Response 전환은 별도 작업으로 진행합니다.
