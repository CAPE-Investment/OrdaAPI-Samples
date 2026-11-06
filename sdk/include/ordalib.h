#pragma once


#include <stdio.h>
#include <tchar.h>
#include "errcode.h"


// 거래상품코드
#define PROD_STK                    0x01    // 주식
#define PROD_DRV                    0x02    // 지수선물옵션
#define PROD_STKDRV                 0x04    // 주식선물옵션
#define PROD_CMDDRV                 0x08    // 상품선물


// Orda-API 이벤트 정의
// Connect(.., UINT nBaseID) 함수의 nBaseID + 이벤트코드로 메시지 수신.
#define WM_DISCONNECT               1       // Disconnect 되었을때
#define WM_RECEIVE_DATA             2       // 조회성 TR에 대한 Data를 받았을때
#define WM_RECEIVE_REAL_DATA        3       // 실시간 TR에 대한 Data를 받았을때
#define WM_LOGIN                    4       // Login 되었을때
#define WM_LOGOUT                   5       // Logout 되었을때
#define WM_TIMEOUT_DATA             6       // RequestData로 요청한 데이터가 Timeout 이 발생했을때


// Data Receive Flag
#define	REQUEST_DATA                1
#define	MESSAGE_DATA                2
#define	SYSTEM_ERROR_DATA           3
#define	RELEASE_DATA                4

// Orda-SDK 윈도우 메시지 수신을 위한 문자열, 아래의 nID 값으로 메시지 수신 통지.
// UINT nID = RegisterWindowMessage(ORDASDK_RECEIVEDATA)
//#define ORDASDK_RECEIVEDATA         _T("ORDA_SDK_RECEIVEDATA")


#pragma pack(push, 1)

#define MAX_PACKET_SIZE             (1024 * 5)


#define MAX_BC_KEY_LENGTH           32
#define MAX_BC_CODE_LENGTH          3


typedef struct _RP_HEADER
{
    _RP_HEADER()
    {
        memset(this, 0, sizeof(_RP_HEADER));
    }
    char szTrCode       [  6];  // AP Code
    char szRetFlag      [  1];  // Error 유무
                                // '1'~'9':Error  
                                // ' ','0' :OK
                                // 'T' : TUXIDO에서 조회(D Type 에만 해당)
                                // 'K' : KOSCOM에서 조회(D Type 에만 해당)
    char szPNFlag       [  1];  // 이전.이후 플래그
                                // '0':before/next disable
                                // '1':next enable
                                // '2':before enable
                                // '3':before/next enable
    char szMsgCode      [  5];
    char szMsgTxt       [ 81];
    char szContKey      [ 19];
    char szFuncCode     [  5];
    char szUserData     [ 31];  // 사용자 데이터
    char szEtkMedia     [  2];  // 당사매체
    char szCommMedia    [  3];  // 통신매체
    char szFiller       [ 30];   
} RP_HEADER, *LPRP_HEADER;
#define RP_HEADER_SIZE      sizeof(RP_HEADER)

typedef struct _RP_DATA_INFO
{
    _RP_DATA_INFO()
    {
        memset(this, 0, sizeof(_RP_DATA_INFO));
    }

    int             nConnectID;
    int             nRqID;
    int             nDataLength;
    int             nTotalDataBufferSize;
    int             nElapsedTime;
    int             nDataMode;
    RP_HEADER       Header;
    unsigned char   szBlockName[17];
    unsigned char  *lpData;
} RP_DATA_INFO, *LPRP_DATA_INFO;
#define RP_DATA_INFO_SIZE   sizeof(RP_DATA_INFO)

typedef struct _BC_ITEM_INFO
{
    _BC_ITEM_INFO()
    {
        memset(this, 0, sizeof(_BC_ITEM_INFO));
    }

    int             nIndexKey;
    int             nKeyLength;
    int             nDataLength;
    LPARAM          lParam;             // BC_KEY_INFO 의 lParam 값 반환용
    char            szBcCode[MAX_BC_CODE_LENGTH + 1];
    char            szKeyData[MAX_BC_KEY_LENGTH + 1];
    char            szRegKey[MAX_BC_KEY_LENGTH + 1];
    char            szBcData[MAX_PACKET_SIZE + 1];
}BC_ITEM_INFO, *LPBC_ITEM_INFO;
#define BC_ITEM_INFO_SIZE   sizeof(BC_ITEM_INFO)


#define MSG_CODE_LENGTH                 5
typedef struct _MSG_PACKET         // system error or message structure 
{
    int             nConnectID;
    int             nRqID;
    int             nMsgLength;
    int             nIsSystemError;
    unsigned char   szCode[MSG_CODE_LENGTH + 1];
    unsigned char  *lpszMessageData;
} MSG_PACKET, *LPMSG_PACKET;


#pragma pack(pop)


/*******************************************************************************

    OrdaAPI class


*******************************************************************************/

class OrdaAPI
{
public:

    OrdaAPI() { memset(this, 0x00, sizeof(OrdaAPI)); };
    ~OrdaAPI() { if (m_hModule) FreeLibrary(m_hModule); };

    inline int Load(TCHAR *pszPath);
    inline int Initialize(void);
    inline int Connect(HWND hWnd, LPCTSTR pszHost, int nPort, UINT nBaseID);
    inline int IsConnected(void);
    inline int Disconnect(void);
    inline int Login(LPCTSTR pszID, LPCTSTR pszPwd, LPCTSTR pszCertPwd);
    inline int Logout(void);
    inline int IsLogin(void);
    inline int RequestData(HWND hWnd, LPCTSTR pszCode, LPVOID pData, int nDataSize, LPCTSTR pszNextKey = NULL);
    inline int ReleaseRequestData(int nRqID);
    inline int ReleaseMessageData(LPARAM lParam);
    inline int AdviseRealData(HWND hWnd, LPCTSTR pszCode, LPCTSTR pData, int nUnitLen);
    inline int UnadviseRealData(HWND hWnd, LPCTSTR pszCode, LPCTSTR pData, int nUnitLen);
    inline int UnadviseWindow(HWND hWnd);
    inline int GetAccountCount(void);
    inline int GetAccountNo(int index, LPTSTR pszAccNo, int nLength);
    inline int GetAccountNoName(int index, LPTSTR pszAccNo, int nNoLen, LPTSTR pszAccName, int nNameLen);
    inline int GetCommMedia(LPTSTR pszBuffer, int nBuffLen);
    inline int GetMedia(LPTSTR pszBuffer, int nBuffLen);
    inline int GetClientIP(LPTSTR pszBuffer, int nBuffLen);
    inline int GetCertSignData(LPTSTR pszBuffer, int nBuffLen);
    inline int GetMngBranchCode(LPCTSTR pszAccNo, LPTSTR pszBuffer, int nBuffLen);
    inline int DelegatedCertify(LPCTSTR pszAccNo, LPCTSTR pszID, LPCTSTR pszPwd);
    inline LPARAM GetSysConfig(UINT nID, LPARAM lParam);
    inline int SetBCKeyParam(LPCTSTR pszCode, LPCTSTR pszKey, LPARAM lParam);
protected:
private:

    typedef int(*PFN_INITIALIZE)();
    typedef int(*PFN_CONNECT)(HWND hWnd, LPCTSTR pszHost, int nPort, UINT nBaseID);
    typedef int(*PFN_ISCONNECTED)();
    typedef int(*PFN_DISCONNECT)();
    typedef int(*PFN_LOGIN)(LPCTSTR pszID, LPCTSTR pszPwd, LPCTSTR pszCertPwd);
    typedef void(*PFN_LOGOUT)();
    typedef int(*PFN_ISLOGIN)();
    typedef int(*PFN_REQUESTDATA)(HWND hWnd, LPCTSTR pszCode, LPVOID pData, int nDataSize, LPCTSTR pszNextKey);
    typedef void(*PFN_RELEASEREQUESTDATA)(int nRqID);
    typedef void(*PFN_RELEASEMESSAGEDATA)(LPARAM lParam);
    typedef int(*PFN_ADVISEREALDATA)(HWND hWnd, LPCTSTR pszCode, LPCTSTR pData, int nUnitLen);
    typedef int(*PFN_UNADVISEREALDATA)(HWND hWnd, LPCTSTR pszCode, LPCTSTR pData, int nUnitLen);
    typedef int(*PFN_UNADVISEWINDOW)(HWND hWnd);
    typedef int(*PFN_GETACCOUNTCOUNT)();
    typedef int(*PFN_GETACCOUNTNO)(int index, LPTSTR pszAccNo, int nLength);
    typedef int(*PFN_GETACCOUNTNONAME)(int index, LPTSTR pszAccNo, int nNoLen, LPTSTR pszAccName, int nNameLen);
    typedef int(*PFN_GETCOMMMEDIA)(LPTSTR pszBuffer, int nBuffLen);
    typedef int(*PFN_GETMEDIA)(LPTSTR pszBuffer, int nBuffLen);
    typedef int(*PFN_GETCLIENTIP)(LPTSTR pszBuffer, int nBuffLen);
    typedef int(*PFN_GETCERTSIGNDATA)(LPTSTR pszBuffer, int nBuffLen);
    typedef int(*PFN_GETMNGBRANCHCODE)(LPCTSTR pszAccNo, LPTSTR pszBuffer, int nBuffLen);
    typedef int(*PFN_DELEGATEDCERTIFY)(LPCTSTR pszAccNo, LPCTSTR pszID, LPCTSTR pszPwd);
    typedef int(*PFN_GETSYSCONFIG)(UINT nID, LPARAM lParam);
    typedef int(*PFN_SETBCKEYPARAM)(LPCTSTR pszCode, LPCTSTR pszKey, LPARAM lParam);

    HMODULE                 m_hModule;
    PFN_INITIALIZE          m_pfnInitialize;
    PFN_CONNECT             m_pfnConnect;
    PFN_ISCONNECTED         m_pfnIsConnected;
    PFN_DISCONNECT          m_pfnDisconnect;
    PFN_LOGIN               m_pfnLogin;
    PFN_LOGOUT              m_pfnLogout;
    PFN_ISLOGIN             m_pfnIsLogin;
    PFN_REQUESTDATA         m_pfnRequestData;
    PFN_RELEASEREQUESTDATA  m_pfnReleaseRequestData;
    PFN_RELEASEMESSAGEDATA  m_pfnReleaseMessageData;
    PFN_ADVISEREALDATA      m_pfnAdviseRealData;
    PFN_UNADVISEREALDATA    m_pfnUnAdviseRealData;
    PFN_UNADVISEWINDOW      m_pfnUnadviseWindow;
    PFN_GETACCOUNTCOUNT     m_pfnGetAccountCount;
    PFN_GETACCOUNTNO        m_pfnGetAccountNo;
    PFN_GETACCOUNTNONAME    m_pfnGetAccountNoName;
    PFN_GETCOMMMEDIA        m_pfnGetCommMedia;
    PFN_GETMEDIA            m_pfnGetMedia;
    PFN_GETCLIENTIP         m_pfnGetClientIP;
    PFN_GETCERTSIGNDATA     m_pfnGetCertSignData;
    PFN_GETMNGBRANCHCODE    m_pfnGetMngBranchCode;
    PFN_DELEGATEDCERTIFY    m_pfnDelegatedCertify;
    PFN_GETSYSCONFIG        m_pfnGetSysConfig;
    PFN_SETBCKEYPARAM       m_pfnSetBCKeyParam;

};


//******************************************************************************
//* Orda-API 모듈 로드.
//* API를 사용하기 위한 외부 Dll 도 함께 로드됩니다.
//* 
//* pszPath     : OrdaAPI.dll 이 존재하는 디렉토리.
//*               실행프로그램과 동일한 위치에 존재할 경우 NULL.
//* Return      : 0 보다 큰 값이면 성공, 아니면 실패.
//* 
//******************************************************************************
int OrdaAPI::Load(TCHAR *pszPath)
{
    const TCHAR *DLL_NAME = _T("OrdaAPI.dll");
    TCHAR buff[MAX_PATH] = { 0, };
    TCHAR szPath[MAX_PATH] = { 0, };

    if (m_hModule != 0)
        return ErrCode::AlreadyLoaded;

    if (pszPath == NULL || lstrlen(pszPath) == 0)
    {
        lstrcpy(szPath, DLL_NAME);

        m_hModule = LoadLibrary(szPath);
    }
    else
    {
        lstrcpy(szPath, pszPath);
        int len = lstrlen(szPath);
        if (szPath[len - 1] == '\\')
            szPath[len - 1] = 0;

        GetCurrentDirectory(MAX_PATH, buff);
        SetCurrentDirectory(szPath);

        lstrcat(szPath, "\\");
        lstrcat(szPath, DLL_NAME);

        m_hModule = LoadLibrary(szPath);

        SetCurrentDirectory(buff);
    }

    return (m_hModule) ? ErrCode::Success : ErrCode::ModuleLoadFailed;
}

//******************************************************************************
//* Orda-API 초기화.
//* 
//* Return      : 0 보다 큰 값이면 성공, 아니면 실패.
//* 
//******************************************************************************
int OrdaAPI::Initialize()
{
    if (m_hModule == 0)
        return ErrCode::ModuleNotLoaded;

    if (m_pfnInitialize == NULL)
    {
        m_pfnInitialize = (PFN_INITIALIZE)GetProcAddress(m_hModule, _T("OrdaAPI_Initialize"));

        if (m_pfnInitialize == NULL)
            return ErrCode::FuncLoadFailed;
    }

    return m_pfnInitialize();
}

//******************************************************************************
//* 서버 접속.
//* 
//* hWnd        : 접속 관련 메시지 수신할 윈도우 핸들
//* pszHost     : 접속할 서버의 도메인(api.capefn.com)
//* nPort       : 접속할 서버의 포트. 8080
//* nBaseID     : 데이터 수신(조회, 실시간, 로그인)을 메시지로 받기 위한 베이스 ID
//*               nBaseID + WM_LOGIN, 
//*               nBaseID + WM_RECEIVE_DATA, 
//*               nBaseID + WM_RECEIVE_REAL_DATA 와 같은 윈도우 메시지로 수신.
//* 
//* Return      : 1 이면 성공, 아니면 실패.
//* 
//******************************************************************************
int OrdaAPI::Connect(HWND hWnd, LPCTSTR pszHost, int nPort, UINT nBaseID)
{
    if (m_hModule == 0)
        return ErrCode::ModuleNotLoaded;

    if (m_pfnConnect == NULL)
    {
        m_pfnConnect = (PFN_CONNECT)GetProcAddress(m_hModule, _T("OrdaAPI_Connect"));

        if (m_pfnConnect == NULL)
            return ErrCode::FuncLoadFailed;
    }

    return m_pfnConnect(hWnd, pszHost, nPort, nBaseID);
}

//******************************************************************************
//* 서버 접속 여부 반환.
//* 
//* Return      : 1 이면 접속상태.
//* 
//******************************************************************************
int OrdaAPI::IsConnected()
{
    if (m_hModule == 0)
        return ErrCode::ModuleNotLoaded;

    if (m_pfnIsConnected == NULL)
    {
        m_pfnIsConnected = (PFN_ISCONNECTED)GetProcAddress(m_hModule, _T("OrdaAPI_IsConnected"));

        if (m_pfnIsConnected == NULL)
            return ErrCode::FuncLoadFailed;
    }

    return m_pfnIsConnected();
}

//******************************************************************************
//* 접속 종료.
//* 
//* Return      : 1 이면 성공.
//* 
//******************************************************************************
int OrdaAPI::Disconnect()
{
    if (m_hModule == 0)
        return ErrCode::ModuleNotLoaded;

    if (m_pfnDisconnect == NULL)
    {
        m_pfnDisconnect = (PFN_DISCONNECT)GetProcAddress(m_hModule, _T("OrdaAPI_Disconnect"));

        if (m_pfnDisconnect == NULL)
            return ErrCode::FuncLoadFailed;
    }

    return m_pfnDisconnect();
}

//******************************************************************************
//* 서버 접속한 상태에서 로그인 시도.
//* 
//* pszID       : HTS 로그인 아이디.
//* pszPwd      : HTS 로그인 비밀번호
//* pszCertPwd  : 공동인증서 비밀번호
//* 
//* Return      : 1 이면 성공, 오류 일 경우 0 이하의 값을 반환. 
//* 
//* Remark      : 이 함수의 반환 값은 해당 함수의 정상적 호출 여부 결과만을 나타내며
//*               실질적인 로그인 성공여부는 Connect() 함수 호출 시 전달된 nBaseID와
//*               WM_LOGIN 을 합산한 값(nBaseID + WM_LOGIN)의 윈도우 메시지로 전달된다.
//*               수신된 윈도우 메시지의 wParam, lParam 은 각각 스트링 값으로 코드와 메시지.
//* 
//******************************************************************************
int OrdaAPI::Login(LPCTSTR pszID, LPCTSTR pszPwd, LPCTSTR pszCertPwd)
{
    if (m_hModule == 0)
        return ErrCode::ModuleNotLoaded;

    if (m_pfnLogin == NULL)
    {
        m_pfnLogin = (PFN_LOGIN)GetProcAddress(m_hModule, _T("OrdaAPI_Login"));

        if (m_pfnLogin == NULL)
            return ErrCode::FuncLoadFailed;
    }

    return m_pfnLogin(pszID, pszPwd, pszCertPwd);
}

//******************************************************************************
//* 로그 아웃 시도.
//* 
//* Return      : 1 이면 성공, 오류 일 경우 0 이하의 값을 반환. 
//* 
//******************************************************************************
int OrdaAPI::Logout()
{
    if (m_hModule == 0)
        return ErrCode::ModuleNotLoaded;

    if (m_pfnLogout == NULL)
    {
        m_pfnLogout = (PFN_LOGOUT)GetProcAddress(m_hModule, _T("OrdaAPI_Logout"));

        if (m_pfnLogout == NULL)
            return ErrCode::FuncLoadFailed;
    }

    m_pfnLogout();
    return ErrCode::Success;
}

//******************************************************************************
//* 로그인 상태 여부 반환.
//* 
//* Return      : 1 이면 로그인 상태.
//* 
//******************************************************************************
int OrdaAPI::IsLogin()
{
    if (m_hModule == 0)
        return ErrCode::ModuleNotLoaded;

    if (m_pfnIsLogin == NULL)
    {
        m_pfnIsLogin = (PFN_ISLOGIN)GetProcAddress(m_hModule, _T("OrdaAPI_IsLogin"));

        if (m_pfnIsLogin == NULL)
            return ErrCode::FuncLoadFailed;
    }

    return m_pfnIsLogin();
}

//******************************************************************************
//* 데이터 조회.
//* 
//* hWnd        : 조회 결과 수신하기 위한 윈도우 핸들.
//* pszCode     : 트랜잭션 코드.
//* pData       : 조회하기 위한 입력 값.
//* nDataSize   : pData 의 사이즈.
//* pszNextKey  : 연속 조회를 위한 키.
//* 
//* Return      : 0 ~ 255 의 값을 반환하면 성공, 아니면 오류.
//* 
//* Remark      : 이 함수의 반환 값은 해당 함수의 정상적 호출 여부 결과만을 나타내며
//*               조회의 결과는 Connect() 함수 호출 시 전달된 nBaseID 를 기준으로 
//*               다음과 같은 윈도우 메시지로 전달된다.
//*               
//*               nBaseID + WM_RECEIVE_DATA: 조회 결과 수신.
//*               nBaseID + WM_TIMEOUT_DATA: 일정 시간 초과 시 수신.
//*               
//*               수신 메시지 처리 방법은 샘플 코드를 참고하시기 바랍니다.
//* 
//******************************************************************************
int OrdaAPI::RequestData(HWND hWnd, LPCTSTR pszCode, LPVOID pData, int nDataSize, LPCTSTR pszNextKey/* = NULL*/)
{
    if (m_hModule == 0)
        return ErrCode::ModuleNotLoaded;

    if (m_pfnRequestData == NULL)
    {
        m_pfnRequestData = (PFN_REQUESTDATA)GetProcAddress(m_hModule, _T("OrdaAPI_RequestData"));

        if (m_pfnRequestData == NULL)
            return ErrCode::FuncLoadFailed;
    }

    return m_pfnRequestData(hWnd, pszCode, pData, nDataSize, pszNextKey);
}

//******************************************************************************
//* 데이터 조회 결과로 잡힌 메모리 해제.
//* 
//* nRqID       : RequestData() 호출의 반환 값.
//* 
//* Return      : 1 을 반환하면 성공, 아니면 오류.
//* 
//* Remark      : nBaseID + WM_RECEIVE_DATA 메시지 수신 시 wParam 으로 다음의 값이 온다.
//*               REQUEST_DATA : 요청한 결과 데이터.
//*               MESSAGE_DATA : 서버에서 송신한 메시지.
//*               SYSTEM_ERROR_DATA : 조회 오류 시 서버에서 송신한 에러 메시지.
//*               RELEASE_DATA : 모든 데이터 전송 완료. Release 요청 메시지.
//* 
//*               RELEASE_DATA 수신 시 필수적으로 호출해야 한다.
//*               동시 조회 가능 건수는 256개이며, RequestData() 호출 시 마다 
//*               동시 조회 가능 건수가 하나씩 줄어든다.
//*               요청 데이터 수신 후 이 함수를 호출하지 않으면 Lock 이 걸린 상태로 
//*               RequestData() 호출 시 -1 값을 반환하게 된다.
//* 
//******************************************************************************
int OrdaAPI::ReleaseRequestData(int nRqID)
{
    if (m_hModule == 0)
        return ErrCode::ModuleNotLoaded;

    if (m_pfnReleaseRequestData == NULL)
    {
        m_pfnReleaseRequestData = (PFN_RELEASEREQUESTDATA)GetProcAddress(m_hModule, _T("OrdaAPI_ReleaseRequestData"));

        if (m_pfnReleaseRequestData == NULL)
            return ErrCode::FuncLoadFailed;
    }

    m_pfnReleaseRequestData(nRqID);
    return ErrCode::Success;
}

//******************************************************************************
//* 데이터 조회 결과로 잡힌 메모리 해제.
//* 
//* lParam      : MSG_PACKET 의 포인터.
//* 
//* Return      : 1 을 반환하면 성공, 아니면 오류.
//* 
//* Remark      : nBaseID + WM_RECEIVE_DATA 메시지 수신 시 wParam 으로 다음의 값이 온다.
//*               REQUEST_DATA : 요청한 결과 데이터.
//*               MESSAGE_DATA : 서버에서 송신한 메시지.
//*               SYSTEM_ERROR_DATA : 조회 오류 시 서버에서 송신한 에러 메시지.
//*               RELEASE_DATA : 모든 데이터 전송 완료. Release 요청 메시지.
//* 
//*               MESSAGE_DATA, SYSTEM_ERROR_DATA 수신 시 필수적으로 호출해야 한다.
//*               이 함수를 호출하지 않으면 Lock 이 걸린 상태로 
//*               RequestData() 호출 시 -1 값을 반환하게 된다.
//* 
//******************************************************************************
int OrdaAPI::ReleaseMessageData(LPARAM lParam)
{
    if (m_hModule == 0)
        return ErrCode::ModuleNotLoaded;

    if (m_pfnReleaseMessageData == NULL)
    {
        m_pfnReleaseMessageData = (PFN_RELEASEMESSAGEDATA)GetProcAddress(m_hModule, _T("OrdaAPI_ReleaseMessageData"));

        if (m_pfnReleaseMessageData == NULL)
            return ErrCode::FuncLoadFailed;
    }

    m_pfnReleaseMessageData(lParam);
    return ErrCode::Success;
}

//******************************************************************************
//* 실시간(자동갱신) 데이터 요청.
//* 
//* hWnd        : 실시간 데이터 수신 할 윈도우 핸들.
//* pszCode     : 실시간 서비스 코드.
//* pData       : 요청에 필요한 입력(키)값. 복수개의 키가 연속될 수 있으며, 
//*               마지막에 NULL 문자가 포함되어야 한다.
//* nUnitLen    : pData의 전체 길이가 아닌, 키 단위의 길이.
//* 
//* Return      : 1 을 반환하면 성공, 아니면 오류.
//* 
//******************************************************************************
int OrdaAPI::AdviseRealData(HWND hWnd, LPCTSTR pszCode, LPCTSTR pData, int nUnitLen)
{
    if (m_hModule == 0)
        return ErrCode::ModuleNotLoaded;

    if (m_pfnAdviseRealData == NULL)
    {
        m_pfnAdviseRealData = (PFN_ADVISEREALDATA)GetProcAddress(m_hModule, _T("OrdaAPI_AdviseRealData"));

        if (m_pfnAdviseRealData == NULL)
            return ErrCode::FuncLoadFailed;
    }

    return m_pfnAdviseRealData(hWnd, pszCode, pData, nUnitLen);
}

//******************************************************************************
//* 실시간(자동갱신) 데이터 해제.
//* 
//* hWnd        : 실시간 데이터 수신 할 윈도우 핸들.
//* pszCode     : 실시간 서비스 코드.
//* pData       : 요청에 필요한 입력(키)값. 복수개의 키가 연속될 수 있으며, 
//*               마지막에 NULL 문자가 포함되어야 한다.
//* nUnitLen    : pData의 전체 길이가 아닌, 키 단위의 길이.
//* 
//* Return      : 1 을 반환하면 성공, 아니면 오류.
//* 
//******************************************************************************
int OrdaAPI::UnadviseRealData(HWND hWnd, LPCTSTR pszCode, LPCTSTR pData, int nUnitLen)
{
    if (m_hModule == 0)
        return ErrCode::ModuleNotLoaded;

    if (m_pfnUnAdviseRealData == NULL)
    {
        m_pfnUnAdviseRealData = (PFN_UNADVISEREALDATA)GetProcAddress(m_hModule, _T("OrdaAPI_UnadviseRealData"));

        if (m_pfnUnAdviseRealData == NULL)
            return ErrCode::FuncLoadFailed;
    }

    return m_pfnUnAdviseRealData(hWnd, pszCode, pData, nUnitLen);
}

//******************************************************************************
//* 특정 윈도우가 요청한 모든 실시간(자동갱신) 데이터 해제. 
//* 
//* hWnd        : 실시간 데이터 수신 할 윈도우 핸들.
//* 
//* Return      : 1 을 반환하면 성공, 아니면 오류.
//* 
//******************************************************************************
int OrdaAPI::UnadviseWindow(HWND hWnd)
{
    if (m_hModule == 0)
        return ErrCode::ModuleNotLoaded;

    if (m_pfnUnadviseWindow == NULL)
    {
        m_pfnUnadviseWindow = (PFN_UNADVISEWINDOW)GetProcAddress(m_hModule, _T("OrdaAPI_UnadviseWindow"));

        if (m_pfnUnadviseWindow == NULL)
            return ErrCode::FuncLoadFailed;
    }

    return m_pfnUnadviseWindow(hWnd);
}

//******************************************************************************
//* API 사용 가능한 계좌 수 반환.
//* 
//******************************************************************************
int OrdaAPI::GetAccountCount(void)
{
    if (m_hModule == 0)
        return ErrCode::ModuleNotLoaded;

    if (m_pfnGetAccountCount == NULL)
    {
        m_pfnGetAccountCount = (PFN_GETACCOUNTCOUNT)GetProcAddress(m_hModule, _T("OrdaAPI_GetAccountCount"));

        if (m_pfnGetAccountCount == NULL)
            return ErrCode::FuncLoadFailed;
    }

    return m_pfnGetAccountCount();
}

//******************************************************************************
//* 특정 인덱스의 계좌번호 취득.
//* 
//* index       : 인덱스.
//* pszAccNo    : 계좌번호를 복사할 버퍼.
//* nLength     : pszAccNo의 길이.
//* 
//* Return      : 1 을 반환하면 성공, 아니면 오류.
//* 
//******************************************************************************
int OrdaAPI::GetAccountNo(int index, LPTSTR pszAccNo, int nLength)
{
    if (m_hModule == 0)
        return ErrCode::ModuleNotLoaded;

    if (m_pfnGetAccountNo == NULL)
    {
        m_pfnGetAccountNo = (PFN_GETACCOUNTNO)GetProcAddress(m_hModule, _T("OrdaAPI_GetAccountNo"));

        if (m_pfnGetAccountNo == NULL)
            return ErrCode::FuncLoadFailed;
    }

    return m_pfnGetAccountNo(index, pszAccNo, nLength);
}

//******************************************************************************
//* 특정 인덱스의 계좌번호와 계좌명 취득.
//* 
//* index       : 인덱스.
//* pszAccNo    : 계좌번호를 복사할 버퍼.
//* nNoLen      : pszAccNo의 길이.
//* pszAccName  : 계좌명을 복사할 버퍼.
//* nNameLen    : pszAccName의 길이.
//* 
//* Return      : 1 을 반환하면 성공, 아니면 오류.
//* 
//******************************************************************************
int OrdaAPI::GetAccountNoName(int index, LPTSTR pszAccNo, int nNoLen, LPTSTR pszAccName, int nNameLen)
{
    if (m_hModule == 0)
        return ErrCode::ModuleNotLoaded;

    if (m_pfnGetAccountNoName == NULL)
    {
        m_pfnGetAccountNoName = (PFN_GETACCOUNTNONAME)GetProcAddress(m_hModule, _T("OrdaAPI_GetAccountNoName"));

        if (m_pfnGetAccountNoName == NULL)
            return ErrCode::FuncLoadFailed;
    }

    return m_pfnGetAccountNoName(index, pszAccNo, nNoLen, pszAccName, nNameLen);
}

//******************************************************************************
//* 통신매체 코드 취득.
//* 
//* pszBuffer   : 문자열 버퍼.
//* nBuffLen    : pszBuffer의 길이.
//* 
//* Return      : 쓰여진 문자열 길이. 
//*               pszBuffer 가 NULL 이거나 nBuffLen 이 0 이면 문자열 버퍼의 최소 길이.
//*               
//******************************************************************************
int OrdaAPI::GetCommMedia(LPTSTR pszBuffer, int nBuffLen)
{
    if (m_hModule == 0)
        return ErrCode::ModuleNotLoaded;

    if (m_pfnGetCommMedia == NULL)
    {
        m_pfnGetCommMedia = (PFN_GETCOMMMEDIA)GetProcAddress(m_hModule, _T("OrdaAPI_GetCommMedia"));

        if (m_pfnGetCommMedia == NULL)
            return ErrCode::FuncLoadFailed;
    }

    return m_pfnGetCommMedia(pszBuffer, nBuffLen);
}

//******************************************************************************
//* 매체 코드 취득.
//* 
//* pszBuffer   : 문자열 버퍼.
//* nBuffLen    : pszBuffer의 길이.
//* 
//* Return      : 쓰여진 문자열 길이. 
//*               pszBuffer 가 NULL 이거나 nBuffLen 이 0 이면 문자열 버퍼의 최소 길이.
//*               
//* 
//******************************************************************************
int OrdaAPI::GetMedia(LPTSTR pszBuffer, int nBuffLen)
{
    if (m_hModule == 0)
        return ErrCode::ModuleNotLoaded;

    if (m_pfnGetMedia == NULL)
    {
        m_pfnGetMedia = (PFN_GETMEDIA)GetProcAddress(m_hModule, _T("OrdaAPI_GetMedia"));

        if (m_pfnGetMedia == NULL)
            return ErrCode::FuncLoadFailed;
    }

    return m_pfnGetMedia(pszBuffer, nBuffLen);
}

//******************************************************************************
//* 로컬(사설) 아이피 취득.
//* 
//* pszBuffer   : 문자열 버퍼.
//* nBuffLen    : pszBuffer의 길이.
//* 
//* Return      : 쓰여진 문자열 길이. 
//*               pszBuffer 가 NULL 이거나 nBuffLen 이 0 이면 문자열 버퍼의 최소 길이.
//*
//*               문자열은 점을 제외한 3자리의 A, B, C, D 클래스로 구성된다.
//*               예) 172.0.0.1 >> 172000000001 
//*
//******************************************************************************
int OrdaAPI::GetClientIP(LPTSTR pszBuffer, int nBuffLen)
{
    if (m_hModule == 0)
        return ErrCode::ModuleNotLoaded;

    if (m_pfnGetClientIP == NULL)
    {
        m_pfnGetClientIP = (PFN_GETCOMMMEDIA)GetProcAddress(m_hModule, _T("OrdaAPI_GetClientIP"));

        if (m_pfnGetClientIP == NULL)
            return ErrCode::FuncLoadFailed;
    }

    return m_pfnGetClientIP(pszBuffer, nBuffLen);
}

//******************************************************************************
//* 공동인증서 사인 데이터 취득.
//* 
//* pszBuffer   : 문자열 버퍼.
//* nBuffLen    : pszBuffer의 길이.
//* 
//* Return      : 복사된 문자열 길이. 
//*               pszBuffer 가 NULL 이거나 nBuffLen 이 0 이면 문자열 버퍼의 최소 길이.
//*
//*               
//*               
//*
//******************************************************************************
int OrdaAPI::GetCertSignData(LPTSTR pszBuffer, int nBuffLen)
{
    if (m_hModule == 0)
        return ErrCode::ModuleNotLoaded;

    if (m_pfnGetCertSignData == NULL)
    {
        m_pfnGetCertSignData = (PFN_GETCERTSIGNDATA)GetProcAddress(m_hModule, _T("OrdaAPI_GetCertSignData"));

        if (m_pfnGetCertSignData == NULL)
            return ErrCode::FuncLoadFailed;
    }

    return m_pfnGetCertSignData(pszBuffer, nBuffLen);
}

//******************************************************************************
//* 입력된 계좌번호의 관리지점 코드 취득
//* 
//* pszAccNo    : 대상 계좌번호.
//* pszBuffer   : 관리지점 코드를 반화하기 위한 문자열 버퍼.
//* nBuffLen    : pszBuffer의 길이.
//* 
//* Return      : 복사된 문자열 길이. 
//*               pszBuffer 가 NULL 이거나 nBuffLen 이 0 이면 문자열 버퍼의 최소 길이.
//*
//*
//******************************************************************************
int OrdaAPI::GetMngBranchCode(LPCTSTR pszAccNo, LPTSTR pszBuffer, int nBuffLen)
{
    if (m_hModule == 0)
        return ErrCode::ModuleNotLoaded;

    if (m_pfnGetMngBranchCode == NULL)
    {
        m_pfnGetMngBranchCode = (PFN_GETMNGBRANCHCODE)GetProcAddress(m_hModule, _T("OrdaAPI_GetMngBranchCode"));

        if (m_pfnGetMngBranchCode == NULL)
            return ErrCode::FuncLoadFailed;
    }

    return m_pfnGetMngBranchCode(pszAccNo, pszBuffer, nBuffLen);
}

//******************************************************************************
//* 위임계좌 인증처리
//* 
//* pszAccNo    : 위임계좌번호.
//* pszID       : 위임계좌의 소유주의 HTS 접속 아이디.
//* pszPwd      : 위임계좌의 소유주의 HTS 접속 비밀번호.
//* 
//* Return      : 0이면 성공, 아니면 비밀번호 재시도 횟수.
//*               0보다 작은 값이면 오류코드.
//*               
//*
//******************************************************************************
int OrdaAPI::DelegatedCertify(LPCTSTR pszAccNo, LPCTSTR pszID, LPCTSTR pszPwd)
{
    if (m_hModule == 0)
        return ErrCode::ModuleNotLoaded;

    if (m_pfnDelegatedCertify == NULL)
    {
        m_pfnDelegatedCertify = (PFN_DELEGATEDCERTIFY)GetProcAddress(m_hModule, _T("OrdaAPI_DelegatedCertify"));

        if (m_pfnDelegatedCertify == NULL)
            return ErrCode::FuncLoadFailed;
    }

    return m_pfnDelegatedCertify(pszAccNo, pszID, pszPwd);
}

//******************************************************************************
//* API 내의 정보를 반환한다. OrdaSDK에서 사용하기 위한 함수.
//* 
//* nID         : 취득하고자하는 정보의 아이디
//* lParam      : 예비.
//* 
//* Return      : nID 에 따라 반환값이 달라진다. 
//*               
//*               
//*
//******************************************************************************
LPARAM OrdaAPI::GetSysConfig(UINT nID, LPARAM lParam)
{
    if (m_hModule == 0)
        return ErrCode::ModuleNotLoaded;

    if (m_pfnGetSysConfig == NULL)
    {
        m_pfnGetSysConfig = (PFN_GETSYSCONFIG)GetProcAddress(m_hModule, _T("OrdaAPI_GetSysConfig"));

        if (m_pfnGetSysConfig == NULL)
            return ErrCode::FuncLoadFailed;
    }

    return m_pfnGetSysConfig(nID, lParam);
}

//******************************************************************************
//* CPushDataDispatcher 의 메모리 내 생성된 BC_KEY_INFO 구조체의 lParam의 값 설정.
//* 
//* pszCode     : 실시간 서비스 코드
//* pszKey      : 키 데이터
//* lParam      : 설정할 값.
//* 
//* Return      : 1 을 반환하면 성공, 아니면 오류.
//*               
//* Remark      : 실시간 데이터 수신 시 전송되는 BC_ITEM_INFO 구조체의 lParam 항목에
//*               여기서 설정한 lParam 값이 전송된다.
//*
//*
//*
//******************************************************************************
int OrdaAPI::SetBCKeyParam(LPCTSTR pszCode, LPCTSTR pszKey, LPARAM lParam)
{
    if (m_hModule == 0)
        return ErrCode::ModuleNotLoaded;

    if (m_pfnSetBCKeyParam == NULL)
    {
        m_pfnSetBCKeyParam = (PFN_SETBCKEYPARAM)GetProcAddress(m_hModule, _T("OrdaAPI_SetBCKeyParam"));

        if (m_pfnSetBCKeyParam == NULL)
            return ErrCode::FuncLoadFailed;
    }

    return m_pfnSetBCKeyParam(pszCode, pszKey, lParam);
}




/*******************************************************************************

    OrdaSDK class


*******************************************************************************/
class OrdaSDK
{
public:

    OrdaSDK() { memset(this, 0x00, sizeof(OrdaSDK)); };
    ~OrdaSDK() { if (m_hModule) FreeLibrary(m_hModule); };

    inline int Load(TCHAR *pszPath);
    inline int Initialize(void);
    inline int SetServerInfo(LPCTSTR pszHost, int nPort, HWND hWnd, UINT nBaseID);
    inline int SetLoginInfo(int type, LPCTSTR pszID, LPCTSTR pszPwd, LPCTSTR pszCert);
    inline int PrepareService(LPCTSTR pstr);
    inline int Start(BYTE nProduct);
    inline int IsConnected(void);
    inline int IsLogin(void);
    inline LPCTSTR GetItemName(LPCTSTR pszCode);
    inline LPCTSTR GetISINCode(LPCTSTR pszCode);
    inline LPCTSTR GetItemCode(LPCTSTR pszISINCode);
    inline LONG_PTR GetService(LPCTSTR pszTRCode);
    inline LPCTSTR GetServiceCode(LONG_PTR key);
    inline int GetRecordCount(LONG_PTR key, LPCTSTR szBlock);
    inline int SetField(LONG_PTR key, LPCTSTR szBlock, LPCTSTR szField, int index, LPCTSTR szValue);
    inline int GetField(LONG_PTR key, LPCTSTR szBlock, LPCTSTR szField, int index, LPTSTR szBuff, int nBuffLen);
    inline int Request(HWND hWnd, LONG_PTR key);
    inline int Advise(HWND hWnd, LONG_PTR key);
    inline int Unadvise(HWND hWnd, LONG_PTR key);
    inline int Release(LONG_PTR key);
    inline int Stop();
    inline int IsBlockMode(LONG_PTR key);
    inline int GetReceiveBlockName(LONG_PTR key, LPTSTR szBuff, int nBuffLen);

protected:
private:

    typedef int(*PFN_INITIALIZE)();
    typedef int(*PFN_SETSERVERINFO)(LPCTSTR pszHost, int nPort, HWND hWnd, UINT nBaseID);
    typedef int(*PFN_SETLOGININFO)(int type, LPCTSTR pszID, LPCTSTR pszPwd, LPCTSTR pszCert);
    typedef int(*PFN_PREPARESERVICE)(LPCTSTR pstr);
    typedef int(*PFN_START)(BYTE nProduct);
    typedef int(*PFN_ISCONNECTED)();
    typedef int(*PFN_ISLOGIN)();
    typedef LPCTSTR(*PFN_GETITEMNAME)(LPCTSTR pszCode);
    typedef LPCTSTR(*PFN_GETISINCODE)(LPCTSTR pszCode);
    typedef LPCTSTR(*PFN_GETITEMCODE)(LPCTSTR pszISINCode);
    typedef LONG_PTR(*PFN_GETSERVICE)(LPCTSTR pszTRCode);
    typedef LPCTSTR(*PFN_GETSERVICECODE)(LONG_PTR key);
    typedef int(*PFN_GETRECORDCOUNT)(LONG_PTR key, LPCTSTR szBlock);
    typedef int(*PFN_SETFIELD)(LONG_PTR key, LPCTSTR szBlock, LPCTSTR szField, int index, LPCTSTR szValue);
    typedef int(*PFN_GETFIELD)(LONG_PTR key, LPCTSTR szBlock, LPCTSTR szField, int index, LPTSTR szBuff, int nBuffLen);
    typedef int(*PFN_REQUEST)(HWND hWnd, LONG_PTR key);
    typedef int(*PFN_ADVISE)(HWND hWnd, LONG_PTR Key);
    typedef int(*PFN_UNADVISE)(HWND hWnd, LONG_PTR Key);
    typedef int(*PFN_RELEASE)(LONG_PTR key);
    typedef int(*PFN_STOP)();
    typedef int(*PFN_ISBLOCKMODE)(LONG_PTR key);
    typedef int (*PFN_GETRECEIVEBLOCKNAME)(LONG_PTR key, LPTSTR szBuff, int nBuffLen);

    HMODULE                 m_hModule;
    PFN_INITIALIZE          m_pfnInitialize;
    PFN_SETSERVERINFO       m_pfnSetServerInfo;
    PFN_SETLOGININFO        m_pfnSetLoginInfo;
    PFN_PREPARESERVICE      m_pfnPrepareService;
    PFN_START               m_pfnStart;
    PFN_ISCONNECTED         m_pfnIsConnected;
    PFN_ISLOGIN             m_pfnIsLogin;
    PFN_GETITEMNAME         m_pfnGetItemName;
    PFN_GETISINCODE         m_pfnGetISINCode;
    PFN_GETITEMCODE         m_pfnGetItemCode;
    PFN_GETSERVICE          m_pfnGetService;
    PFN_GETSERVICECODE      m_pfnGetServiceCode;
    PFN_GETRECORDCOUNT      m_pfnGetRecordCount;
    PFN_SETFIELD            m_pfnSetField;
    PFN_GETFIELD            m_pfnGetField;
    PFN_REQUEST             m_pfnRequest;
    PFN_ADVISE              m_pfnAdvise;
    PFN_UNADVISE            m_pfnUnadvise;
    PFN_RELEASE             m_pfnRelease;
    PFN_STOP                m_pfnStop;
    PFN_ISBLOCKMODE         m_pfnIsBlockMode;
    PFN_GETRECEIVEBLOCKNAME m_pfnGetReceiveBlockName;

};


//******************************************************************************
//* Orda-SDK 모듈 로드.
//* API를 사용하기 위한 외부 Dll 도 함께 로드됩니다.
//* 
//* pszPath     : OrdaSDK.dll 이 존재하는 디렉토리.
//*               실행프로그램과 동일한 위치에 존재할 경우 NULL.
//* 
//* Return      : 0 보다 큰 값이면 성공, 아니면 실패.
//* 
//* Remark      : 
//* 
//******************************************************************************
int OrdaSDK::Load(TCHAR *pszPath)
{
    const TCHAR *DLL_NAME = _T("OrdaSDK.dll");
    TCHAR buff[MAX_PATH] = { 0, };
    TCHAR szPath[MAX_PATH] = { 0, };

    if (m_hModule != 0)
        return ErrCode::AlreadyLoaded;

    if (pszPath == NULL || lstrlen(pszPath) == 0)
    {
        lstrcpy(szPath, DLL_NAME);

        m_hModule = LoadLibrary(szPath);
    }
    else
    {
        lstrcpy(szPath, pszPath);
        int len = lstrlen(szPath);
        if (szPath[len - 1] == '\\')
            szPath[len - 1] = 0;

        GetCurrentDirectory(MAX_PATH, buff);
        SetCurrentDirectory(szPath);

        lstrcat(szPath, "\\");
        lstrcat(szPath, DLL_NAME);

        m_hModule = LoadLibrary(szPath);

        SetCurrentDirectory(buff);
    }

    return (m_hModule) ? ErrCode::Success : ErrCode::ModuleLoadFailed;
}


//******************************************************************************
//* OrdaSDK 초기화.
//* 
//* Return      : 0 보다 큰 값이면 성공, 아니면 실패.
//* 
//******************************************************************************
int OrdaSDK::Initialize()
{
    if (m_hModule == 0)
        return ErrCode::ModuleNotLoaded;

    if (m_pfnInitialize == NULL)
    {
        m_pfnInitialize = (PFN_INITIALIZE)GetProcAddress(m_hModule, _T("OrdaSDK_Initialize"));

        if (m_pfnInitialize == NULL)
            return ErrCode::FuncLoadFailed;
    }

    return m_pfnInitialize();
}

//******************************************************************************
//* 접속할 서버 정보 및 결과 수신을 위한 정보 설정
//* 
//* pszHost     : 접속할 서버의 도메인
//*               운영서버: api.capefn.com
//*               개발서버: api-dev.capefn.com
//* nPort       : 접속할 서버의 포트. 8080
//* hWnd        : 접속 관련 메시지 수신할 윈도우 핸들
//* nBaseID     : 데이터 수신(조회, 실시간, 로그인)을 메시지로 받기 위한 베이스 ID
//*               nBaseID + WM_LOGIN, 
//*               nBaseID + WM_RECEIVE_DATA, 
//*               nBaseID + WM_RECEIVE_REAL_DATA 와 같은 윈도우 메시지로 수신.
//* 
//* Return      : 1 이면 성공, 오류 일 경우 0 이하의 값을 반환. 
//* 
//******************************************************************************
int OrdaSDK::SetServerInfo(LPCTSTR pszHost, int nPort, HWND hWnd, UINT nBaseID)
{
    if (m_hModule == 0)
        return ErrCode::ModuleNotLoaded;

    if (m_pfnSetServerInfo == NULL)
    {
        m_pfnSetServerInfo = (PFN_SETSERVERINFO)GetProcAddress(m_hModule, _T("OrdaSDK_SetServerInfo"));

        if (m_pfnSetServerInfo == NULL)
            return ErrCode::FuncLoadFailed;
    }

    return m_pfnSetServerInfo(pszHost, nPort, hWnd, nBaseID);
}

//******************************************************************************
//* 로그인 정보 설정
//* 
//* type        : 로그인 타입
//*               1이면 아이디 로그인, pszID, pszPwd, pszCert 의 값이 모두 세팅 
//*               되어야 한다.
//*               3이면 클라우드 로그인, pszID 의 값만 참조.
//* pszID       : HTS 로그인 아이디.
//* pszPwd      : HTS 로그인 비밀번호
//* pszCertPwd  : 공동인증서 비밀번호
//* 
//* Return      : 1 이면 성공, 오류 일 경우 0 이하의 값을 반환. 
//* 
//******************************************************************************
int OrdaSDK::SetLoginInfo(int type, LPCTSTR pszID, LPCTSTR pszPwd, LPCTSTR pszCert)
{
    if (m_hModule == 0)
        return ErrCode::ModuleNotLoaded;

    if (m_pfnSetLoginInfo == NULL)
    {
        m_pfnSetLoginInfo = (PFN_SETLOGININFO)GetProcAddress(m_hModule, _T("OrdaSDK_SetLoginInfo"));

        if (m_pfnSetLoginInfo == NULL)
            return ErrCode::FuncLoadFailed;
    }

    return m_pfnSetLoginInfo(type, pszID, pszPwd, pszCert);
}

//******************************************************************************
//* 전문 구조체를 미리 준비하기 위한 서비스의 TR 리스트
//* 
//* pstr        : ',' 를 구분자로 사용한 서비스 리스트
//*               ex)
//*               t1001,t1004,K3_,H1_,
//* 
//* Return      : 1 이면 성공, 오류 일 경우 0 이하의 값을 반환. 
//* 
//* Remark      : 여기서 지정한 서비스들은 OrdaSDK_Start() 에서 해당 정보 수신 후 
//*               메모리를 구축한다. 여기에 지정하지 않은 서비스는 해당 서비스 호출 시 
//*               서비스에 대한 정보 수신 & 메모리 구축이 이루어지므로 최초 호출 시 
//*               지연 및 오류 발생 할 수 있다.
//* 
//******************************************************************************
int OrdaSDK::PrepareService(LPCTSTR pstr)
{
    if (m_hModule == 0)
        return ErrCode::ModuleNotLoaded;

    if (m_pfnPrepareService == NULL)
    {
        m_pfnPrepareService = (PFN_PREPARESERVICE)GetProcAddress(m_hModule, _T("OrdaSDK_PrepareService"));

        if (m_pfnPrepareService == NULL)
            return ErrCode::FuncLoadFailed;
    }

    return m_pfnPrepareService(pstr);
}

//******************************************************************************
//* OrdaSDK 서비스 시작
//* 
//* nProduct    : 거래상품 코드, 각 비트 조합으로 사용 가능.
//*               0x01 : 주식
//*               0x02 : 지수선물옵션
//*               0x04 : 주식선물옵션
//*               0x08 : 상품선물
//*               
//*               설정된 상품의 마스터 코드를 위한 메모리를 자동 구축하므로 메모리가 
//*               과도하게 잡힐 수 있다. 매매하기 위한 상품만 설정하도록 한다.
//*               
//* Return      : 1 이면 성공, 오류 일 경우 0 이하의 값을 반환. 
//* 
//* Remark      : 서비스를 시작하기 위한 일련의 절차를 내부적으로 진행한다.
//*               
//*               이 함수를 호출하기 이전 접속정보와 로그인정보가 설정이 선행되어야 한다.
//*               
//*               아래와 같은 순서로 프로세스가 진행된다.
//*               - 서버 접속
//*               - 로그인 처리
//*               - 전문정보 수신
//*               - 전문정보 메모리 구축
//*               - 종목 마스터 수신
//*                 nProduct에서 설정된 상품의 종목 마스터 수신 처리.
//*               - 종목 마스터 메모리 구축
//*               
//* 
//* 
//******************************************************************************
int OrdaSDK::Start(BYTE nProduct)
{
    if (m_hModule == 0)
        return ErrCode::ModuleNotLoaded;

    if (m_pfnStart == NULL)
    {
        m_pfnStart = (PFN_START)GetProcAddress(m_hModule, _T("OrdaSDK_Start"));

        if (m_pfnStart == NULL)
            return ErrCode::FuncLoadFailed;
    }

    return m_pfnStart(nProduct);
}

//******************************************************************************
//* 서버 접속 여부 반환.
//* 
//* Return      : 1 이면 접속상태.
//* 
//******************************************************************************
int OrdaSDK::IsConnected()
{
    if (m_hModule == 0)
        return ErrCode::ModuleNotLoaded;

    if (m_pfnIsConnected == NULL)
    {
        m_pfnIsConnected = (PFN_ISCONNECTED)GetProcAddress(m_hModule, _T("OrdaSDK_IsConnected"));

        if (m_pfnIsConnected == NULL)
            return ErrCode::FuncLoadFailed;
    }

    return m_pfnIsConnected();
}

//******************************************************************************
//* 로그인 상태 여부 반환.
//* 
//* Return      : 1 이면 로그인 상태.
//* 
//******************************************************************************
int OrdaSDK::IsLogin()
{
    if (m_hModule == 0)
        return ErrCode::ModuleNotLoaded;

    if (m_pfnIsLogin == NULL)
    {
        m_pfnIsLogin = (PFN_ISLOGIN)GetProcAddress(m_hModule, _T("OrdaSDK_IsLogin"));

        if (m_pfnIsLogin == NULL)
            return ErrCode::FuncLoadFailed;
    }

    return m_pfnIsLogin();
}


//******************************************************************************
//* 종목명 반환
//* 
//* pszCode     : 단축코드
//* 
//* Return      : pszCode 에 해당하는 종목명 반환
//* 
//******************************************************************************
LPCTSTR OrdaSDK::GetItemName(LPCTSTR pszCode)
{
    if (m_hModule == 0)
        return NULL;

    if (m_pfnGetItemName == NULL)
    {
        m_pfnGetItemName = (PFN_GETITEMNAME)GetProcAddress(m_hModule, _T("OrdaSDK_GetItemName"));

        if (m_pfnGetItemName == NULL)
            return NULL;
    }

    return m_pfnGetItemName(pszCode);
}

//******************************************************************************
//* 표준 코드 반환
//* 
//* pszCode     : 단축코드
//* 
//* Return      : pszCode 에 해당하는 표준코드 반환
//* 
//******************************************************************************
LPCTSTR OrdaSDK::GetISINCode(LPCTSTR pszCode)
{
    if (m_hModule == 0)
        return NULL;

    if (m_pfnGetISINCode == NULL)
    {
        m_pfnGetISINCode = (PFN_GETISINCODE)GetProcAddress(m_hModule, _T("OrdaSDK_GetISINCode"));

        if (m_pfnGetISINCode == NULL)
            return NULL;
    }

    return m_pfnGetISINCode(pszCode);
}


//******************************************************************************
//* 단축 코드 반환
//* 
//* pszISINCode : 표준코드(ISIN코드)
//* 
//* Return      : pszCode 에 해당하는 단축코드 반환
//* 
//******************************************************************************
LPCTSTR OrdaSDK::GetItemCode(LPCTSTR pszISINCode)
{
    if (m_hModule == 0)
        return NULL;

    if (m_pfnGetItemCode == NULL)
    {
        m_pfnGetItemCode = (PFN_GETITEMCODE)GetProcAddress(m_hModule, _T("OrdaSDK_GetItemCode"));

        if (m_pfnGetItemCode == NULL)
            return NULL;
    }

    return m_pfnGetISINCode(pszISINCode);
}

//******************************************************************************
//* 서비스 코드에 대한 키값을 반환
//* 
//* 
//* pszCode     : 서비스 코드(TR코드)
//*               
//* Return      : pszTRCode 의 유일한 키를 반환. 
//*               
//* Remark      : 이 함수 호출 시 내부적으로 데이터 패킷을 파싱하기 위한 메모리가 생성된다.
//*               
//*               이 함수에 의해 생성된 메모리 삭제는 OrdaSDK_Release 함수를 이용해야한다.
//*               동일한 서비스 코드에 대해 중복 호출 시 각각의 메모리가 생성된다.
//*               아래의 함수들은 이 함수에서 반환된 키 값을 사용한다.
//*               
//*               - OrdaSDK_SetField
//*               - OrdaSDK_GetRecordCount
//*               - OrdaSDK_GetField
//*               - OrdaSDK_Request
//*               - OrdaSDK_Release
//* 
//******************************************************************************
LONG_PTR OrdaSDK::GetService(LPCTSTR pszTRCode)
{
    if (m_hModule == 0)
        return NULL;

    if (m_pfnGetService == NULL)
    {
        m_pfnGetService = (PFN_GETSERVICE)GetProcAddress(m_hModule, _T("OrdaSDK_GetService"));

        if (m_pfnGetService == NULL)
            return NULL;
    }

    return m_pfnGetService(pszTRCode);
}

//******************************************************************************
//* CServiceIO 인스턴스의 서비스 코드를 반환
//* 
//* 
//* key         : OrdaSDK_GetService() 함수의 반환값.
//*               
//* Return      : Service Code 반환.
//*               
//* Remark      : 
//* 
//* 
//******************************************************************************
LPCTSTR OrdaSDK::GetServiceCode(LONG_PTR key)
{
    if (m_hModule == 0)
        return NULL;

    if (m_pfnGetServiceCode == NULL)
    {
        m_pfnGetServiceCode = (PFN_GETSERVICECODE)GetProcAddress(m_hModule, _T("OrdaSDK_GetServiceCode"));

        if (m_pfnGetServiceCode == NULL)
            return NULL;
    }

    return m_pfnGetServiceCode(key);

}


//******************************************************************************
//* CServiceIO 인스턴스의 출력블럭의 레코드 개수를 반환한다.
//* 
//* key         : CServiceIO의 인스턴스, OrdaSDK_GetService() 함수의 반환값.
//* 
//* Return      : 출력블럭의 레코드 개수
//*               
//* Remark      : 복수개의 OutBlock 으로 구성되었을 경우 반복 속성이 지정되어있는 블럭.
//* 
//******************************************************************************
int OrdaSDK::GetRecordCount(LONG_PTR key, LPCTSTR szBlock)
{
    if (m_hModule == 0)
        return ErrCode::ModuleNotLoaded;

    if (m_pfnGetRecordCount == NULL)
    {
        m_pfnGetRecordCount = (PFN_GETRECORDCOUNT)GetProcAddress(m_hModule, _T("OrdaSDK_GetRecordCount"));

        if (m_pfnGetRecordCount == NULL)
            return ErrCode::FuncLoadFailed;
    }

    return m_pfnGetRecordCount(key, szBlock);
}

//******************************************************************************
//* CServiceIO 인스턴스의 특정 항목 데이터를 설정한다.
//* 
//* key         : OrdaSDK_GetService() 함수의 반환값.
//* szBlock     : 블럭명
//* szField     : 필드명
//* szValue     : 설정값.
//*               
//* Return      : 1 이면 성공, 오류 일 경우 0 이하의 값을 반환. 
//* 
//* Remark      : 
//*               
//*               
//*               
//* 
//******************************************************************************
int OrdaSDK::SetField(LONG_PTR key, LPCTSTR szBlock, LPCTSTR szField, int index, LPCTSTR szValue)
{
    if (m_hModule == 0)
        return ErrCode::ModuleNotLoaded;

    if (m_pfnSetField == NULL)
    {
        m_pfnSetField = (PFN_SETFIELD)GetProcAddress(m_hModule, _T("OrdaSDK_SetField"));

        if (m_pfnSetField == NULL)
            return ErrCode::FuncLoadFailed;
    }

    return m_pfnSetField(key, szBlock, szField, index, szValue);
}

//******************************************************************************
//* CServiceIO 인스턴스의 특정 항목 데이터를 가져온다.
//* 
//* key         : CServiceIO의 인스턴스, OrdaSDK_GetService() 함수의 반환값.
//* szBlock     : 블럭명
//* szField     : 항목명
//* index       : 레코드 인덱스
//* szBuff      : 문자열값을 받아오기 위한 버퍼
//* nBuffLen    : szBuff 의 길이      
//*               
//* Return      : 1 이면 성공, 오류 일 경우 0 이하의 값을 반환. 
//*               
//* Remark      : 
//* 
//******************************************************************************
int OrdaSDK::GetField(LONG_PTR key, LPCTSTR szBlock, LPCTSTR szField, int index, LPTSTR szBuff, int nBuffLen)
{
    if (m_hModule == 0)
        return ErrCode::ModuleNotLoaded;

    if (m_pfnGetField == NULL)
    {
        m_pfnGetField = (PFN_GETFIELD)GetProcAddress(m_hModule, _T("OrdaSDK_GetField"));

        if (m_pfnGetField == NULL)
            return ErrCode::FuncLoadFailed;
    }

    return m_pfnGetField(key, szBlock, szField, index, szBuff, nBuffLen);
}

//******************************************************************************
//* 조회 데이터 전송
//*               
//* hWnd        : 데이터 수신 시 통보받을 윈도우 
//* key         : CServiceIO의 인스턴스, OrdaSDK_GetService() 함수의 반환값.
//* 
//* Return      : 0 ~ 255 의 값을 반환하면 성공, 아니면 오류.
//* 
//* Remark      : SetServerInfo() 함수의 nBaseID + WM_RECEIVE_DATA 
//*               윈도우 메시지로 데이터 수신 결과를 통보 받는다.
//*               
//*               wParam으로 TRCode의 스트링문자열, lParam으로 key 값 정보가 전달된다.
//*               
//*               
//* 
//******************************************************************************
int OrdaSDK::Request(HWND hWnd, LONG_PTR key)
{
    if (m_hModule == 0)
        return ErrCode::ModuleNotLoaded;

    if (m_pfnRequest == NULL)
    {
        m_pfnRequest = (PFN_REQUEST)GetProcAddress(m_hModule, _T("OrdaSDK_Request"));

        if (m_pfnRequest == NULL)
            return ErrCode::FuncLoadFailed;
    }

    return m_pfnRequest(hWnd, key);
}


//******************************************************************************
//* 실시간 데이터 요청.
//* 
//* hWnd        : 데이터 수신 시 통보받을 윈도우 
//* key         : CServiceIO의 인스턴스, OrdaSDK_GetService 함수에 의해 반환된 값.
//* 
//* Return      : 1 이면 성공, 오류 일 경우 0 이하의 값을 반환. 
//* 
//* Remark      : SetServerInfo() 함수의 nBaseID + WM_RECEIVE_REAL_DATA 
//*               윈도우 메시지로 데이터 수신 결과를 통보 받는다.
//*               
//*               wParam으로 TRCode의 스트링문자열, lParam으로 key 값 정보가 전달된다.
//* 
//******************************************************************************
int OrdaSDK::Advise(HWND hWnd, LONG_PTR key)
{
    if (m_hModule == 0)
        return ErrCode::ModuleNotLoaded;

    if (m_pfnAdvise == NULL)
    {
        m_pfnAdvise = (PFN_ADVISE)GetProcAddress(m_hModule, _T("OrdaSDK_Advise"));

        if (m_pfnAdvise == NULL)
            return ErrCode::FuncLoadFailed;
    }

    return m_pfnAdvise(hWnd, key);
}

//******************************************************************************
//* 실시간 데이터 요청 해제.
//* 
//* hWnd        : 데이터 수신 시 통보받을 윈도우 
//* key         : CServiceIO의 인스턴스, OrdaSDK_GetService 함수에 의해 반환된 값.
//* 
//* Return      : 1 이면 성공, 오류 일 경우 0 이하의 값을 반환. 
//* 
//* Remark      : OrdaSDK_Advise() 함수를 통해 요청된 실시간 데이터 해제한다.
//*               
//*               
//*               
//* 
//******************************************************************************
int OrdaSDK::Unadvise(HWND hWnd, LONG_PTR key)
{
    if (m_hModule == 0)
        return ErrCode::ModuleNotLoaded;

    if (m_pfnUnadvise == NULL)
    {
        m_pfnUnadvise = (PFN_UNADVISE)GetProcAddress(m_hModule, _T("OrdaSDK_Unadvise"));

        if (m_pfnUnadvise == NULL)
            return ErrCode::FuncLoadFailed;
    }

    return m_pfnUnadvise(hWnd, key);
}

//******************************************************************************
//* 송수신 데이터 파싱을 위한 메모리 영역을 해제한다.
//* 
//* key         : CServiceIO의 인스턴스, OrdaSDK_GetService 함수에 의해 반환된 값.
//* 
//* Return      : 1 이면 성공, 오류 일 경우 0 이하의 값을 반환. 
//* 
//* Remark      : OrdaSDK_GetService 함수의 호출로 인해 생성된 인스턴스는 
//*               이 함수를 호출하기 전에 메모리 해제하지 않는다.
//* 
//******************************************************************************
int OrdaSDK::Release(LONG_PTR key)
{
    if (m_hModule == 0)
        return ErrCode::ModuleNotLoaded;

    if (m_pfnRelease == NULL)
    {
        m_pfnRelease = (PFN_RELEASE)GetProcAddress(m_hModule, _T("OrdaSDK_Release"));

        if (m_pfnRelease == NULL)
            return ErrCode::FuncLoadFailed;
    }

    return m_pfnRelease(key);
}


//******************************************************************************
//* OrdaSDK 서비스 종료
//* 
//*               
//* Return      : 1 이면 성공, 오류 일 경우 0 이하의 값을 반환. 
//* 
//* Remark      : 서비스를 시작하기 위한 일련의 절차를 내부적으로 진행한다.
//*               
//*               아래와 같은 순서로 프로세스가 진행된다.
//*               - 로그아웃 처리
//*               - 접속 종료
//*               - 전문정보 메모리 해제
//*               - 마스터정보 메모리 해제
//*               
//* 
//* 
//******************************************************************************
int OrdaSDK::Stop()
{
    if (m_hModule == 0)
        return ErrCode::ModuleNotLoaded;

    if (m_pfnStop == NULL)
    {
        m_pfnStop = (PFN_STOP)GetProcAddress(m_hModule, _T("OrdaSDK_Stop"));

        if (m_pfnStop == NULL)
            return ErrCode::FuncLoadFailed;
    }

    return m_pfnStop();
}


//******************************************************************************
//* CServiceIO 가 블럭 모드인지 반환.
//* 
//* key         : CServiceIO의 인스턴스,              
//* 
//* Return      : 1 이면 블럭 모드.
//* 
//* Remark      : 
//*               
//* 
//* 
//******************************************************************************
int OrdaSDK::IsBlockMode(LONG_PTR key)
{
    if (m_hModule == 0)
        return ErrCode::ModuleNotLoaded;

    if (m_pfnIsBlockMode == NULL)
    {
        m_pfnIsBlockMode = (PFN_ISBLOCKMODE)GetProcAddress(m_hModule, _T("OrdaSDK_IsBlockMode"));

        if (m_pfnIsBlockMode == NULL)
            return ErrCode::FuncLoadFailed;
    }

    return m_pfnIsBlockMode(key);
}

//******************************************************************************
//* 수신된 데이터의 블럭 명을 반환한다.
//* 
//* key         : CServiceIO의 인스턴스,              
//* 
//* Return      : 1 이면 블럭 모드.
//* 
//* Remark      : 블럭모드일 경우에만 해당된다.
//*               WM_RECEIVEDATA 메시지 수신시에만 유효한 값을 가진다.
//* 
//* 
//******************************************************************************
int OrdaSDK::GetReceiveBlockName(LONG_PTR key, LPTSTR szBuff, int nBuffLen)
{
    if (m_hModule == 0)
        return ErrCode::ModuleNotLoaded;

    if (m_pfnGetReceiveBlockName == NULL)
    {
        m_pfnGetReceiveBlockName = (PFN_GETRECEIVEBLOCKNAME)GetProcAddress(m_hModule, _T("OrdaSDK_GetReceiveBlockName"));

        if (m_pfnGetReceiveBlockName == NULL)
            return ErrCode::FuncLoadFailed;
    }

    return m_pfnGetReceiveBlockName(key, szBuff, nBuffLen);
}

