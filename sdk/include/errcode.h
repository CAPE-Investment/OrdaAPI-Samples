/*******************************************************************************
    author   : Hoya. Kim
    compiler : Microsoft Visual C++ 2010
    copyright: Copyright 2021 
    remarks  : Orda-API Error Code
    date     : 2026.08.06
             







*******************************************************************************/

#pragma once


namespace ErrCode
{
    enum Enum
    {
        // SYSTEM
        AlreadyLoaded       = 2,
        Success             = 1,
        Failed              = 0,
        ModuleNotLoaded     = -1001,
        ModuleLoadFailed    = -1002,
        FuncNotFound        = -1003,
        FuncLoadFailed      = -1004,
        AlreadyConnected    = -1101,
        ModuleNotInit       = -1102,
        CreateEventFailed   = -1103,
        CreateSocketFailed  = -1104,
        NewVersionRequired  = -1105,

        NotEnoughtMemory    = -1201,                // 버퍼의 메모리가 충분하지 않습니다.


        TRNotFound          = -2001,                // TR 정보를 찾을 수 없습니다.
        LoginRequired       = -2002,                // 로그인이 필요합니다.
        AccNotFound         = -2003,                // 계좌가 존재하지 않습니다.
        UnauthDeleAcc       = -2004,                // 승인된 위임계좌가 아닙니다.
        NotEnoughRqID       = -2005,                // 사용가능한 Request ID가 없습니다.
        InvalidHeadType     = -2006,                // 유효하지 않은 헤더 타입입니다.
        DataSendFailed      = -2007,                // 데이터 전송에 실패했습니다.

        LoginCancel         = -3001,                // 사용자 로그인 취소
        LoginReject         = -3002,                // 접속 거부
        LoginError          = -3003,                // 접속 오류
        NotExistID          = -3004,                // 일치하는 아이디가 존재하지 않습니다.

        CertExpire          = -3101,                // 인증서 만료/정지/폐지

        CertSuspend         = -3102,                // 인증서 정지
        CertRevoke          = -3103,                // 인증서 폐지
        CertVerifyFail      = -3104,                // 인증서 검증오류
        CertAuthService     = -3105,                // 인증기관 서비스오류

        CertNotReg          = -3106,                // 인증서 미등록
        CertNotExist        = -3107,                // 인증서 없음.
        CertWrongPwd        = -3108,                // 인증서 비밀번호 오류
        CertWrongPwd5       = -3109,                // 인증서 비밀번호 오류 5회
        CertNotUse          = -3110,                // 공동인증 절차 수행할 수 없음.
        CertInitFail        = -3111,                // 공인인증 모듈 초기화 실패
        CertCancel          = -3112,                // 공인인증 취소
        CertFail            = -3113,                // 공인인증 실패
        CertPubKeyFail      = -3114,                // 공개키 추출 실패
    
        NotExistAccList     = -3201,                // 계좌리스트가 없습니다.
        NotExistAccNo       = -3202,                // 해당하는 계좌가 없습니다.

        SvcAlreadyReg       = -4001,                // 이미 등록된 서비스코드입니다.
    };
}



//#define ERROR_SOCKET_CREATE_FAIL                -1            // 소켓생성 실패
//#define ERROR_CONNECT_FAIL                      -2            // 서버연결 실패
//#define ERROR_WRONG_ADDRESS                -3            // 서버주소가 틀렸음
//#define ERROR_CONNECT_TIMEOUT            -4            // 연결시간 초과
//#define ERROR_ALREADY_CONNECT            -5            // 이미 서버에 연결중입니다.
//#define ERROR_CANT_USE_TR                -6            // 해당 TR은 사용할 수 없습니다.
//#define ERROR_NEED_LOGIN                    -7            // 로그인이 필요합니다.
//#define ERROR_CANT_USE_SISEONLY            -8            // 시세전용에서는 사용이 불가능합니다.
//#define ERROR_CANT_USE_ACCOUNTLIST        -9            // 계좌리스트에서는 사용이 불가능합니다.
//#define ERROR_FAULT_PACKET_SIZE            -10            // Packet의 크기가 잘못되었습니다.
//#define ERROR_DIFF_DATA_SIZE                -11            // Data 크기가 다릅니다.
//#define ERROR_NOT_EXIST_ACCOUNT            -12            // 계좌가 존재하지 않습니다.
//#define ERROR_NOT_ENOUGH_RQID            -13            // Request ID 부족
//#define ERROR_NOT_CREATE_SOCKET            -14            // 소켓이 생성되지 않았습니다.
//#define ERROR_ENCRYPT_CREATE_FAIL        -15            // 암호화 생성에 실패했습니다.
//#define ERROR_DATA_SEND_FAIL                -16            // 데이터 전송에 실패했습니다.
//#define ERROR_FAIL_ENCRYPT_RTN            -17            // 암호화(RTN)처리에 실패했습니다.
//#define ERROR_CERT_NOT_EXIST_FILE        -18            // 공인인증 파일이 없습니다.
//#define ERROR_CERT_NOT_EXIST_FUNC        -19            // 공인인증 Function이 없습니다.
//#define ERROR_NOT_ENOUGH_MEMORY            -20            // 메모리가 충분하지 않다.
//#define ERROR_LIMIT_TR                    -21            // TR의 사용제한.
//#define ERROR_NOT_USE_FUNCTION            -22            // 해당 TR은 해당 함수를 이용할 수 없습니다.
//#define ERROR_NOT_FIND_TR_INFO            -23            // TR에 대한 정보를 찾을 수 없습니다.
//#define ERROR_ACCOUNT_POS                -24            // 계좌위치가 지정되지 않았습니다.
//#define ERROR_DELEGATED_AUTH             -25         // 인증된 위임계좌가 아닙니다.
