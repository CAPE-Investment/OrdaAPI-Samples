#include <windows.h>
#include <assert.h>
#include <crtdbg.h>
#include <stdio.h>
#include <string>

static int confirmationAnswer = IDYES;
static std::wstring lastConfirmation;
static int TestMessageBox(HWND, LPCWSTR text, LPCWSTR, UINT)
{
    lastConfirmation = text;
    return confirmationAnswer;
}
#define MessageBoxW TestMessageBox
#define WinMain SampleWinMain
#include "../OrdaAPI-b8502-FuturesAmend/main.cpp"
#undef WinMain
#undef MessageBoxW
#include "../../../../common/OrdaSampleCommon.cpp"
#include "xml-checks.h"

typedef void (*SetupFunction)(int);
typedef void (*LoginFailureFunction)(int);
typedef int (*MetricFunction)(int);
typedef int (*CopyFunction)(void *, int);
typedef const char *(*CodeFunction)();
static SetupFunction setup;
static MetricFunction metric;
static CopyFunction copyRequest;

static void PumpMessages()
{
    MSG message;
    while (PeekMessage(&message, NULL, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&message);
        DispatchMessage(&message);
    }
}
static void Click(HWND window, int control)
{
    SendMessage(window, WM_COMMAND, MAKEWPARAM(control, BN_CLICKED), 0);
}
static void Login(HWND window)
{
    SetWindowTextA(Common::g_password, "offline");
    Click(window, 100);
    PumpMessages();
    assert(Common::IsReady());
}
static void PrepareOrder(HWND window)
{
    confirmationAnswer = IDYES;
    if (!g_armed)
        Click(window, 301);
    assert(g_armed && !g_uncertain);
    SetWindowTextA(g_inputs[3], "secret");
}
static void VerifyCapturedRequest(HWND window, const char *price, const char *quantity, const char *originalOrderNumber = "0000012345")
{
    PrepareOrder(window);
    OrderInput input;
    assert(BuildInput(input));
    setup(1);
    int requestsBefore = metric(0);
    Click(window, 300);
    assert(metric(0) == requestsBefore + 1 && g_received && g_requestId == -1);
    OrderInput captured;
    assert(copyRequest(&captured, sizeof(captured)) == sizeof(captured));
    assert(memcmp(&captured, &input, sizeof(input)) == 0);
    VerifyExpectedBuffer(captured, price, quantity, originalOrderNumber);
    assert(memcmp(captured.price, price, 13) == 0);
    assert(memcmp(captured.qty, quantity, 16) == 0);
    assert(memchr(captured.price, 0, sizeof(captured.price)) == NULL);
    assert(memchr(captured.qty, 0, sizeof(captured.qty)) == NULL);
    assert(memcmp(captured.dummyorgordno, originalOrderNumber, 3) == 0);
    assert(memcmp(captured.orgordno, originalOrderNumber + 3, 7) == 0);
    assert(memchr(captured.dummyorgordno, 0, 3) == NULL);
    assert(memchr(captured.orgordno, 0, 7) == NULL);
    assert(lastConfirmation.find(L"ACCOUNT-TEST") != std::wstring::npos);
    assert(lastConfirmation.find(L"FUTURES-TEST") != std::wstring::npos);
    assert(lastConfirmation.find(L"원주문번호=") != std::wstring::npos);
    assert(lastConfirmation.find(L"(offergb)=") != std::wstring::npos);
    assert(lastConfirmation.find(L"secret") == std::wstring::npos);
    char log[30000] = {0};
    GetWindowTextA(g_output, log, sizeof(log));
    assert(strstr(log, "orderNumber=3141234567"));
    assert(strstr(log, "passwd=[redacted]"));
    assert(!strstr(log, "secret") && !strstr(log, "XXXXXXXX"));
    Click(window, 300);
    assert(metric(0) == requestsBefore + 1); // 재진입과 완료 후 연타 모두 차단합니다.
}
static void VerifyRejectedInput(HWND window, int index, const char *value)
{
    PrepareOrder(window);
    SetWindowTextA(g_inputs[index], value);
    OrderInput input;
    assert(!BuildInput(input));
    int requestsBefore = metric(0);
    Click(window, 300);
    assert(metric(0) == requestsBefore);
}

int main()
{
    // 실패 시 대화상자로 테스트가 정지하지 않도록 표준 오류로 출력합니다.
    _CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_FILE);
    _CrtSetReportFile(_CRT_ASSERT, _CRTDBG_FILE_STDERR);
    HMODULE dll = LoadLibraryA("OrdaAPI.dll");
    assert(dll);
    setup = (SetupFunction)GetProcAddress(dll, "TestSetup");
    metric = (MetricFunction)GetProcAddress(dll, "TestMetric");
    copyRequest = (CopyFunction)GetProcAddress(dll, "TestCopy");
    LoginFailureFunction failLogin = (LoginFailureFunction)GetProcAddress(dll, "TestLoginFailure");
    CodeFunction service = (CodeFunction)GetProcAddress(dll, "TestCode");
    assert(setup && metric && copyRequest && failLogin && service);
    WNDCLASSW windowClass = {0};
    windowClass.lpfnWndProc = WindowProc;
    windowClass.hInstance = GetModuleHandle(NULL);
    windowClass.lpszClassName = L"B8500OfflineHarness";
    assert(RegisterClassW(&windowClass));
    HWND window = CreateWindowW(windowClass.lpszClassName, L"", WS_OVERLAPPED,
                                0, 0, 1140, 840, NULL, NULL, windowClass.hInstance, NULL);
    assert(window && g_panel && g_order && g_prepare && g_retry && g_logout && g_output);
    assert(GetWindowLong(Common::g_password, GWL_STYLE) & ES_PASSWORD);
    assert(GetWindowLong(Common::g_certPassword, GWL_STYLE) & ES_PASSWORD);
    assert(GetWindowLong(g_inputs[3], GWL_STYLE) & ES_PASSWORD);
    char value[256];
    for (int i = 0; i < _countof(INPUT_FIELDS); ++i)
    {
        const InputField &field = INPUT_FIELDS[i];
        if (field.choiceCount)
            assert(SendMessage(g_inputs[i], CB_GETCOUNT, 0, 0) == field.choiceCount);
        else
        {
            GetWindowTextA(g_inputs[i], value, sizeof(value));
            assert(strcmp(value, field.initial) == 0);
            assert(SendMessage(g_inputs[i], EM_GETLIMITTEXT, 0, 0) == field.width);
        }
    }
    SendMessage(g_panel, WM_VSCROLL, SB_PAGEDOWN, 0);
    assert(g_scroll > 0);
    Common::BeginCall();
    assert(Common::Initialize());
    Common::EndCall(window);
    failLogin(1);
    Click(window, 100);
    PumpMessages();
    assert(!Common::IsReady() && metric(0) == 0);
    char server[256], id[32];
    GetWindowTextA(Common::g_server, server, sizeof(server));
    GetWindowTextA(Common::g_id, id, sizeof(id));
    Click(window, 302);
    GetWindowTextA(Common::g_server, value, sizeof(value));
    assert(strcmp(server, value) == 0);
    GetWindowTextA(Common::g_id, value, sizeof(value));
    assert(strcmp(id, value) == 0);
    assert(GetWindowTextLengthA(Common::g_password) == 0);
    assert(GetWindowTextLengthA(Common::g_certPassword) == 0);
    failLogin(0);
    Login(window);
    assert(IsWindowEnabled(g_logout));
    int logoutsBefore = metric(3), disconnectsBefore = metric(4);
    Click(window, 303);
    assert(!Common::IsReady() && metric(3) == logoutsBefore + 1);
    assert(metric(4) == disconnectsBefore + 1);
    Login(window);
    SetWindowTextA(g_inputs[2], "ACCOUNT-TEST");
    SetWindowTextA(g_inputs[4], "FUTURES-TEST");
    SetWindowTextA(g_inputs[6], "12345");
    SetWindowTextA(g_inputs[8], "350.25");
    SetWindowTextA(g_inputs[9], "10");
    OrderInput input;
    assert(BuildInput(input));
    VerifyXmlDefaults(input);
    VerifyXmlCodes(input);

    PrepareOrder(window);
    confirmationAnswer = IDNO;
    Click(window, 300);
    assert(metric(0) == 0 && g_armed);
    confirmationAnswer = IDYES;
    const char *prices[] = {"350.25", "350", "12.5", "0", "1964000", "9999999999.99", " 12.5 ", "000350.05"};
    const char *formatted[] = {"0000000350.25", "0000000350.00", "0000000012.50", "0000000000.00",
                               "0001964000.00", "9999999999.99", "0000000012.50", "0000000350.05"};
    for (int i = 0; i < _countof(prices); ++i)
    {
        SetWindowTextA(g_inputs[8], prices[i]);
        VerifyCapturedRequest(window, formatted[i], "0000000000000010");
        GetWindowTextA(g_inputs[8], value, sizeof(value));
        assert(strcmp(value, prices[i]) == 0);
    }
    assert(strcmp(service(), "b8502") == 0);
    const char *invalidPrices[] = {"", "  ", "-1", "+1", "abc", ".25", "350.", "1.2.3",
                                   "1.234", "12345678901", "00000000000", "1 2", "1e2"};
    for (int i = 0; i < _countof(invalidPrices); ++i)
        VerifyRejectedInput(window, 8, invalidPrices[i]);
    SetWindowTextA(g_inputs[8], "350.25");
    const char *quantities[] = {"1", "100", "0", "9999999999999999", "00010", " 10 "};
    const char *padded[] = {"0000000000000001", "0000000000000100", "0000000000000000",
                            "9999999999999999", "0000000000000010", "0000000000000010"};
    for (int i = 0; i < _countof(quantities); ++i)
    {
        SetWindowTextA(g_inputs[9], quantities[i]);
        VerifyCapturedRequest(window, "0000000350.25", padded[i]);
    }
    const char *invalidQuantities[] = {"", "   ", "-1", "+1", "1.0", "12A", "1 2", "12345678901234567"};
    for (int i = 0; i < _countof(invalidQuantities); ++i)
        VerifyRejectedInput(window, 9, invalidQuantities[i]);
    SetWindowTextA(g_inputs[9], "10");
    SendMessage(g_inputs[7], CB_SETCURSEL, -1, 0);
    assert(!BuildInput(input));
    SendMessage(g_inputs[7], CB_SETCURSEL, 0, 0);
    for (int i = 0; i < _countof(INPUT_FIELDS); ++i)
    {
        const InputField &field = INPUT_FIELDS[i];
        if (field.choiceCount || field.readOnly || i == 9 || i == 8)
            continue;
        GetWindowTextA(g_inputs[i], value, sizeof(value));
        std::string restore = value;
        std::string overlong(field.width + 1, 'X');
        VerifyRejectedInput(window, i, overlong.c_str());
        SetWindowTextA(g_inputs[i], restore.c_str());
    }
    printf("b8502 XML, UI, serialization, confirmation and captured buffers PASS\n");

    const char *quoteCodes[] = {"00", "03", "05", "06"};
    for (int i = 0; i < 4; ++i)
    {
        SendMessage(g_inputs[7], CB_SETCURSEL, i, 0);
        VerifyCapturedRequest(window, "0000000350.25", "0000000000000010");
        OrderInput captured;
        assert(copyRequest(&captured, sizeof(captured)) == sizeof(captured));
        assert(memcmp(captured.offergb, quoteCodes[i], 2) == 0);
    }
    SendMessage(g_inputs[7], CB_SETCURSEL, 0, 0);

    const char *originals[] = {"12345", "12345678", "1", "00012345", "1234567890", "0000000000", " 12345 "};
    const char *paddedOriginal[] = {"0000012345", "0012345678", "0000000001", "0000012345", "1234567890", "0000000000", "0000012345"};
    for (int i = 0; i < _countof(originals); ++i)
    {
        SetWindowTextA(g_inputs[6], originals[i]);
        VerifyCapturedRequest(window, "0000000350.25", "0000000000000010", paddedOriginal[i]);
        GetWindowTextA(g_inputs[6], value, sizeof(value));
        assert(strcmp(value, originals[i]) == 0);
    }
    const char *invalidOriginals[] = {"", "   ", "-1", "+1", "12A", "12 34", "1.0", "12345678901"};
    for (int i = 0; i < _countof(invalidOriginals); ++i)
        VerifyRejectedInput(window, 6, invalidOriginals[i]);
    SetWindowTextA(g_inputs[6], "12345");

    for (int mode = 2; mode <= 12; ++mode)
    {
        PrepareOrder(window);
        setup(mode);
        int requestsBefore = metric(0), releasesBefore = metric(1);
        int messagesBefore = metric(2);
        Click(window, 300);
        assert(metric(0) == requestsBefore + 1 && !g_armed);
        if (mode == 4)
        {
            assert(g_requestId == 0);
            assert(!IsWindowEnabled(g_order) && !IsWindowEnabled(g_logout));
            Click(window, 300);
            Click(window, 303);
            assert(metric(0) == requestsBefore + 1 && g_requestId == 0 && Common::IsReady());
            SendMessage(window, Common::API_MESSAGE_BASE + WM_DISCONNECT, 0, 0);
            assert(g_uncertain);
            SendMessage(window, Common::API_MESSAGE_BASE + WM_TIMEOUT_DATA, 0, 0);
            assert(g_requestId == -1);
            Click(window, 302);
            Login(window);
        }
        else
            assert(g_requestId == -1);
        assert(metric(1) == releasesBefore + (mode == 5 ? 0 : 1));
        assert(metric(2) == messagesBefore + (mode == 3 || mode == 9 ? 1 : 0));
        if (mode == 2 || mode == 4 || (mode >= 6 && mode <= 11))
            assert(g_uncertain);
        if (mode == 3 || mode == 12)
            assert(g_error);
        PumpMessages();
        assert(metric(0) == requestsBefore + 1);
    }
    PrepareOrder(window);
    setup(4);
    Click(window, 300);
    assert(g_requestId == 0);
    int releasesBefore = metric(1);
    logoutsBefore = metric(3);
    disconnectsBefore = metric(4);
    SendMessage(window, WM_CLOSE, 0, 0);
    assert(!IsWindow(window) && metric(1) == releasesBefore + 1);
    assert(metric(3) == logoutsBefore + 1 && metric(4) == disconnectsBefore + 1);
    FreeLibrary(dll);
    printf("b8502 errors, timeout, reentrancy, logout/relogin and cleanup PASS\n");
    return 0;
}
