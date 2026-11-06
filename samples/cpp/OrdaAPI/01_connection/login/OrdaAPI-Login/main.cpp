// Visual Studio 2010 / Win32 / MultiByte. Shared SDK headers: sdk/include.
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <wchar.h>
#include <ordalib.h>

const UINT API_MESSAGE_BASE = WM_APP + 100;
const UINT SHOW_LOGIN_RESULT = WM_APP + 200;
const UINT CLOSE_AFTER_CALL = WM_APP + 201;
const int LOGIN_BUTTON = 100;
const int DEV_RADIO = 101;
const int PROD_RADIO = 102;
// 개발서버 기본 접속정보. 변경 시 OrdaAPI 관련 자료실을 확인합니다.
const char* DEV_SERVER = "api-dev.capefn.com";
const char* PROD_SERVER = "api.capefn.com";
const char* SERVER_PORT = "8080";
const char* DEV_ID = "APITEST";
const char* DEV_PASSWORD = "1q2w3e4r!!";
const char* DEV_CERT_PASSWORD = "1q2w3e4r!!";
HWND g_devRadio, g_prodRadio;

OrdaAPI* g_api = NULL;
HWND g_server, g_port, g_id, g_password, g_certPassword, g_loginButton;
HWND g_connection, g_login, g_code, g_message;
BOOL g_busy = FALSE;
BOOL g_closing = FALSE;
BOOL g_closeRequested = FALSE;
BOOL g_connectAttempted = FALSE;
BOOL g_initialized = FALSE;
char g_resultCode[32] = {0};
char g_resultMessage[2048] = {0};

void SelectServer(BOOL development)
{
    SendMessage(g_devRadio, BM_SETCHECK, development ? BST_CHECKED : BST_UNCHECKED, 0);
    SendMessage(g_prodRadio, BM_SETCHECK, development ? BST_UNCHECKED : BST_CHECKED, 0);
    SetWindowTextA(g_server, development ? DEV_SERVER : PROD_SERVER);
    SetWindowTextA(g_port, SERVER_PORT);
    // 운영서버는 사용등록 계좌의 접속정보를 고객이 직접 입력합니다.
    SetWindowTextA(g_id, development ? DEV_ID : "");
    SetWindowTextA(g_password, development ? DEV_PASSWORD : "");
    SetWindowTextA(g_certPassword, development ? DEV_CERT_PASSWORD : "");
}

void ShowCallResult(const wchar_t* stage, int result)
{
    wchar_t text[160];
    swprintf_s(text, _countof(text), L"%s 반환값: %d. 자세한 내용은 SDK 문서를 확인하세요.", stage, result);
    SetWindowTextW(g_message, text);
    swprintf_s(text, _countof(text), L"%d", result);
    SetWindowTextW(g_code, text);
}

BOOL InitializeOrdaAPI()
{
    // ordalib.h: Load() uses LoadLibrary; methods resolve exports with
    // GetProcAddress. Its destructor calls FreeLibrary.
    char directory[MAX_PATH];
    // The debugger starts this program with sdk/bin as its working directory.
    // Keep that directory for the complete runtime's relative file accesses.
    DWORD length = GetCurrentDirectoryA(MAX_PATH, directory);
    if (length == 0 || length >= MAX_PATH)
    {
        SetWindowTextW(g_message, L"런타임 작업 폴더를 확인할 수 없습니다.");
        return FALSE;
    }
    g_api = new OrdaAPI;
    int result = g_api->Load(directory);
    if (result <= 0)
    {
        ShowCallResult(L"DLL 로드", result);
        return FALSE;
    }
    result = g_api->Initialize();
    if (result <= 0)
    {
        ShowCallResult(L"초기화", result);
        return FALSE;
    }
    g_initialized = TRUE;
    SetWindowTextW(g_message, L"초기화 완료. 접속정보를 확인한 뒤 로그인하세요.");
    EnableWindow(g_loginButton, TRUE);
    return TRUE;
}

void ConnectAndLogin(HWND window)
{
    char server[256], portText[16], id[16], password[16], certPassword[16];
    GetWindowTextA(g_server, server, sizeof(server));
    GetWindowTextA(g_port, portText, sizeof(portText));
    GetWindowTextA(g_id, id, sizeof(id));
    GetWindowTextA(g_password, password, sizeof(password));
    GetWindowTextA(g_certPassword, certPassword, sizeof(certPassword));
    char* end = NULL;
    long port = strtol(portText, &end, 10);
    if (!server[0] || !portText[0] || *end || port < 1 || port > 65535 || !id[0])
    {
        SetWindowTextW(g_message, L"서버, 포트(1~65535), 로그인 ID를 입력하세요.");
        SecureZeroMemory(password, sizeof(password));
        SecureZeroMemory(certPassword, sizeof(certPassword));
        return;
    }

    // One connection/login attempt per run keeps this example focused.
    EnableWindow(g_loginButton, FALSE);
    g_busy = TRUE;
    SetWindowTextW(g_login, L"로그인 전");
    g_connectAttempted = TRUE;
    int result = g_api->Connect(window, server, (int)port, API_MESSAGE_BASE);
    ShowCallResult(L"서버 연결", result);
    int connected = g_api->IsConnected();
    SetWindowTextW(g_connection, connected == 1 ? L"접속 성공" : L"접속 안 됨");
    if (connected < 0)
        ShowCallResult(L"접속 상태 확인", connected);
    if (result > 0 && !g_closeRequested)
    {
        SetWindowTextW(g_login, L"로그인 요청 중");
        result = g_api->Login(id, password, certPassword);
        // A positive return is NOT the final login result.
        // WM_LOGIN may also arrive before Login() has returned.
        ShowCallResult(L"로그인 요청", result);
        SetWindowTextW(g_login, result > 0 ? L"로그인 결과 수신 대기" : L"로그인 요청 실패");
    }
    SecureZeroMemory(password, sizeof(password));
    SecureZeroMemory(certPassword, sizeof(certPassword));
    SetWindowTextA(g_password, "");
    SetWindowTextA(g_certPassword, "");
    g_busy = FALSE;
    if (g_closeRequested)
        PostMessage(window, CLOSE_AFTER_CALL, 0, 0);
}

void HandleLoginResult()
{
    // These are our own buffers, copied while the DLL's SendMessage was active.
    SetWindowTextA(g_code, g_resultCode);
    SetWindowTextA(g_message, g_resultMessage);
    int loggedIn = g_api->IsLogin();
    if (loggedIn < 0)
    {
        ShowCallResult(L"로그인 상태 확인", loggedIn);
        SetWindowTextW(g_login, L"로그인 상태 확인 실패");
    }
    else
    {
        BOOL success = strcmp(g_resultCode, "0000") == 0 && loggedIn == 1;
        SetWindowTextW(g_login, success ? L"로그인 성공" : L"로그인 실패");
    }
    int connected = g_api->IsConnected();
    SetWindowTextW(g_connection, connected == 1 ? L"접속 성공" : L"접속 안 됨");
    if (connected < 0)
        ShowCallResult(L"접속 상태 확인", connected);
}

void CleanupOrdaAPI()
{
    if (!g_api)
        return;
    int loggedIn = g_initialized ? g_api->IsLogin() : 0;
    if (loggedIn < 0)
        ShowCallResult(L"로그인 상태 확인", loggedIn);
    if (loggedIn == 1)
    {
        // The wrapper returns call status; the public DLL Logout export is void.
        int result = g_api->Logout();
        if (result <= 0)
            ShowCallResult(L"로그아웃", result);
    }
    if (g_connectAttempted)
    {
        int result = g_api->Disconnect();
        if (result <= 0)
            ShowCallResult(L"연결 종료", result);
    }
    delete g_api; // OrdaAPI destructor -> FreeLibrary, before destroying HWND.
    g_api = NULL;
    g_initialized = FALSE;
    SetWindowTextA(g_password, "");
    SetWindowTextA(g_certPassword, "");
}

HWND AddControl(HWND parent, const wchar_t* type, const wchar_t* text,
                DWORD style, int x, int y, int width, int height, int id)
{
    HWND control = CreateWindowExW(wcscmp(type, L"EDIT") == 0 ? WS_EX_CLIENTEDGE : 0,
        type, text, WS_CHILD | WS_VISIBLE | style, x, y, width, height,
        parent, (HMENU)(INT_PTR)id, GetModuleHandle(NULL), NULL);
    SendMessage(control, WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), TRUE);
    return control;
}

LRESULT CALLBACK WindowProc(HWND window, UINT message, WPARAM wParam, LPARAM lParam)
{
    if (message == API_MESSAGE_BASE + WM_LOGIN)
    {
        // Borrowed strings: copy now; never store/free the DLL pointers.
        // Do not call Logout/Disconnect/FreeLibrary from this notification.
        if (!g_closing)
        {
            lstrcpynA(g_resultCode, wParam ? (const char*)wParam : "", sizeof(g_resultCode));
            lstrcpynA(g_resultMessage, lParam ? (const char*)lParam : "", sizeof(g_resultMessage));
            PostMessage(window, SHOW_LOGIN_RESULT, 0, 0);
        }
        return 0;
    }
    switch (message)
    {
    case WM_CREATE:
        AddControl(window, L"STATIC", L"접속 서버", 0, 20, 22, 150, 22, 0);
        g_devRadio = AddControl(window, L"BUTTON", L"개발서버", BS_AUTORADIOBUTTON | WS_GROUP | WS_TABSTOP, 180, 20, 110, 24, DEV_RADIO);
        g_prodRadio = AddControl(window, L"BUTTON", L"운영서버", BS_AUTORADIOBUTTON, 310, 20, 110, 24, PROD_RADIO);
        AddControl(window, L"STATIC", L"서버", 0, 20, 62, 150, 22, 0);
        g_server = AddControl(window, L"EDIT", L"", ES_AUTOHSCROLL | WS_TABSTOP | WS_GROUP, 180, 60, 350, 24, 0);
        AddControl(window, L"STATIC", L"포트", 0, 20, 97, 150, 22, 0);
        g_port = AddControl(window, L"EDIT", L"", ES_NUMBER | WS_TABSTOP, 180, 95, 90, 24, 0);
        AddControl(window, L"STATIC", L"로그인 ID", 0, 20, 132, 150, 22, 0);
        g_id = AddControl(window, L"EDIT", L"", ES_AUTOHSCROLL | WS_TABSTOP, 180, 130, 350, 24, 0);
        AddControl(window, L"STATIC", L"로그인 비밀번호", 0, 20, 167, 150, 22, 0);
        g_password = AddControl(window, L"EDIT", L"", ES_PASSWORD | ES_AUTOHSCROLL | WS_TABSTOP, 180, 165, 350, 24, 0);
        AddControl(window, L"STATIC", L"공동인증 비밀번호", 0, 20, 202, 150, 22, 0);
        g_certPassword = AddControl(window, L"EDIT", L"", ES_PASSWORD | ES_AUTOHSCROLL | WS_TABSTOP, 180, 200, 350, 24, 0);
        SendMessage(g_id, EM_SETLIMITTEXT, 15, 0);
        SendMessage(g_password, EM_SETLIMITTEXT, 15, 0);
        SendMessage(g_certPassword, EM_SETLIMITTEXT, 15, 0);
        SendMessage(g_server, EM_SETLIMITTEXT, 255, 0);
        SendMessage(g_port, EM_SETLIMITTEXT, 5, 0);
        g_loginButton = AddControl(window, L"BUTTON", L"로그인", WS_TABSTOP, 420, 240, 110, 30, LOGIN_BUTTON);
        EnableWindow(g_loginButton, FALSE);
        AddControl(window, L"STATIC", L"접속 상태", 0, 20, 292, 150, 22, 0);
        g_connection = AddControl(window, L"STATIC", L"접속 전", 0, 180, 292, 350, 22, 0);
        AddControl(window, L"STATIC", L"로그인 상태", 0, 20, 322, 150, 22, 0);
        g_login = AddControl(window, L"STATIC", L"로그인 전", 0, 180, 322, 350, 22, 0);
        AddControl(window, L"STATIC", L"결과 코드", 0, 20, 352, 150, 22, 0);
        g_code = AddControl(window, L"STATIC", L"", 0, 180, 352, 350, 22, 0);
        AddControl(window, L"STATIC", L"결과 메시지", 0, 20, 382, 150, 22, 0);
        g_message = AddControl(window, L"EDIT", L"", ES_READONLY | ES_MULTILINE | ES_AUTOVSCROLL | WS_VSCROLL,
            180, 380, 350, 100, 0);
        SelectServer(TRUE);
        return 0;
    case WM_COMMAND:
        if (g_busy || g_closing)
            return 0;
        if ((LOWORD(wParam) == DEV_RADIO || LOWORD(wParam) == PROD_RADIO) && HIWORD(wParam) == BN_CLICKED)
            SelectServer(LOWORD(wParam) == DEV_RADIO);
        else if (LOWORD(wParam) == LOGIN_BUTTON && HIWORD(wParam) == BN_CLICKED)
            ConnectAndLogin(window);
        return 0;
    case SHOW_LOGIN_RESULT:
        if (g_api && !g_closing)
            HandleLoginResult();
        return 0;
    case CLOSE_AFTER_CALL:
    case WM_CLOSE:
        if (g_busy)
        {
            g_closeRequested = TRUE;
            return 0;
        }
        if (!g_closing)
        {
            g_closing = TRUE;
            EnableWindow(g_loginButton, FALSE);
            CleanupOrdaAPI();
            DestroyWindow(window);
        }
        return 0;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProcW(window, message, wParam, lParam);
}

int APIENTRY WinMain(HINSTANCE instance, HINSTANCE, LPSTR, int show)
{
    WNDCLASSW wc = {0};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = instance;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
    wc.lpszClassName = L"OrdaAPILoginSample";
    if (!RegisterClassW(&wc))
        return 1;
    HWND window = CreateWindowW(wc.lpszClassName, L"OrdaAPI 로그인 예제",
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
        CW_USEDEFAULT, CW_USEDEFAULT, 570, 530, NULL, NULL, instance, NULL);
    if (!window)
        return 1;
    ShowWindow(window, show);
    UpdateWindow(window);
    g_busy = TRUE;
    InitializeOrdaAPI();
    g_busy = FALSE;
    if (g_closeRequested)
        PostMessage(window, CLOSE_AFTER_CALL, 0, 0);
    MSG message;
    int status;
    while ((status = GetMessageW(&message, NULL, 0, 0)) > 0)
    {
        if (!IsDialogMessageW(window, &message))
        {
            TranslateMessage(&message);
            DispatchMessageW(&message);
        }
    }
    if (status == -1 && IsWindow(window))
    {
        g_closing = TRUE;
        CleanupOrdaAPI();
        DestroyWindow(window);
    }
    return status == -1 ? 1 : (int)message.wParam;
}
