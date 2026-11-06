// Visual Studio 2010 / v100 / Win32 / MultiByte. UTF-8 BOM for Korean UI.
#include "../../../common/OrdaSampleCommon.h"
#include "t1001.h"
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <wchar.h>
#include <deque>

namespace Common = OrdaSampleCommon;

// Keep the supplied packed layout identical to the existing XML request.
static_assert(sizeof(t1001InBlock) == 9, "Request length differs from XML");
static_assert(__alignof(t1001InBlock) == 1, "Request alignment differs from XML");
static_assert(offsetof(t1001InBlock, shcode) == 0 && sizeof(((t1001InBlock*)0)->shcode) == 6, "shcode layout differs from XML");
static_assert(offsetof(t1001InBlock, _shcode) == 6, "shcode attribute offset differs from XML");
static_assert(offsetof(t1001InBlock, exchange) == 7 && sizeof(((t1001InBlock*)0)->exchange) == 1, "exchange layout differs from XML");
static_assert(offsetof(t1001InBlock, _exchange) == 8, "exchange attribute offset differs from XML");

const char T1001_CODE[] = "t1001";
const char T1001_OUTPUT_BLOCK[] = "t1001OutBlock";
const int T1001_INPUT_SIZE = 9;
const int T1001_OUTPUT_SIZE = 1184;

// C++03-compatible checks for the displayed response fields.
typedef char CheckT1001OutputSize[(sizeof(t1001OutBlock) == T1001_OUTPUT_SIZE) ? 1 : -1];
typedef char CheckT1001OutputAlignment[(__alignof(t1001OutBlock) == 1) ? 1 : -1];
typedef char CheckOut_hname[(offsetof(t1001OutBlock, hname) == 0 && sizeof(((t1001OutBlock*)0)->hname) == 20 && offsetof(t1001OutBlock, _hname) == 20) ? 1 : -1];
typedef char CheckOut_price[(offsetof(t1001OutBlock, price) == 21 && sizeof(((t1001OutBlock*)0)->price) == 8 && offsetof(t1001OutBlock, _price) == 29) ? 1 : -1];
typedef char CheckOut_sign[(offsetof(t1001OutBlock, sign) == 30 && sizeof(((t1001OutBlock*)0)->sign) == 1 && offsetof(t1001OutBlock, _sign) == 31) ? 1 : -1];
typedef char CheckOut_change[(offsetof(t1001OutBlock, change) == 32 && sizeof(((t1001OutBlock*)0)->change) == 8 && offsetof(t1001OutBlock, _change) == 40) ? 1 : -1];
typedef char CheckOut_diff[(offsetof(t1001OutBlock, diff) == 41 && sizeof(((t1001OutBlock*)0)->diff) == 6 && offsetof(t1001OutBlock, _diff) == 47) ? 1 : -1];
typedef char CheckOut_volume[(offsetof(t1001OutBlock, volume) == 48 && sizeof(((t1001OutBlock*)0)->volume) == 12 && offsetof(t1001OutBlock, _volume) == 60) ? 1 : -1];

const int QUERY_BUTTON = 300;
HWND g_symbol, g_exchange, g_queryButton, g_requestedSymbol;
HWND g_name, g_price, g_sign, g_change, g_diff, g_volume;
int g_requestId = -1; // 0 is a valid request ID; -1 means no outstanding request.
BOOL g_closing = FALSE;
BOOL g_submitting = FALSE;
BOOL g_receivedData = FALSE;
BOOL g_queryError = FALSE;
char g_requestSymbol[7] = {0};
struct DeferredNotification
{
    UINT message;
    WPARAM wParam;
    LPARAM lParam;
};
std::deque<DeferredNotification> g_deferred;

void UpdateQueryButton()
{
    EnableWindow(g_queryButton, Common::IsReady() && !Common::IsBusy() &&
        !g_closing && g_requestId < 0 && !g_submitting);
}

// Fixed-width XML fields are not NUL-terminated. Copy into our own buffer,
// trim padding, and never retain an OrdaAPI-owned pointer.
BOOL CopyField(const void* data, int dataLength, int offset, int width,
               char* output, int capacity)
{
    if (!data || !output || offset < 0 || width < 0 || dataLength < 0 ||
        offset > dataLength || width > dataLength - offset || capacity <= width)
        return FALSE;
    memcpy(output, (const char*)data + offset, width);
    output[width] = 0;
    int end = width;
    while (end > 0 && (output[end - 1] == ' ' || output[end - 1] == 0))
        output[--end] = 0;
    int start = 0;
    while (start < end && output[start] == ' ')
        ++start;
    if (start)
        memmove(output, output + start, end - start + 1);
    return TRUE;
}

void ClearOutput()
{
    HWND controls[] = {g_requestedSymbol, g_name, g_price, g_sign, g_change, g_diff, g_volume};
    for (int i = 0; i < _countof(controls); ++i)
        SetWindowTextA(controls[i], "");
}

// ------------------------------------------------------------
// t1001 주식현재가 조회 요청: t1001InBlock -> 9-byte request -> Public API.
// ------------------------------------------------------------
void RequestT1001(HWND window)
{
    if (!Common::IsReady())
    {
        Common::SetResult(L"로그인이 필요합니다.");
        return;
    }
    if (g_requestId >= 0 || Common::IsBusy() || g_closing || g_submitting)
        return;
    char symbol[32] = {0};
    GetWindowTextA(g_symbol, symbol, sizeof(symbol));
    // shcode is a fixed six-byte short code; no guessed market-specific format.
    if (strlen(symbol) != 6)
    {
        Common::SetResult(L"종목코드를 6자리로 입력하십시오.");
        return;
    }
    for (int i = 0; i < 6; ++i)
    {
        if (!((symbol[i] >= '0' && symbol[i] <= '9') ||
              (symbol[i] >= 'A' && symbol[i] <= 'Z') ||
              (symbol[i] >= 'a' && symbol[i] <= 'z')))
        {
            Common::SetResult(L"종목코드는 영문 또는 숫자 6자리로 입력하십시오.");
            return;
        }
    }
    int exchange = (int)SendMessage(g_exchange, CB_GETCURSEL, 0, 0);
    if (exchange < 0 || exchange > 2)
    {
        Common::SetResult(L"거래소를 선택하십시오.");
        return;
    }
    Common::BeginCall();
    if (!Common::CheckSession())
    {
        Common::SetResult(L"로그인이 필요합니다. 프로그램을 다시 실행해 로그인하십시오.");
        Common::EndCall(window);
        UpdateQueryButton();
        return;
    }
    // t1001.xml: attr="1". SDK ServiceIO::CreateMemory uses space padding.
    // shcode: offset 0, char[6]; attribute byte: offset 6.
    // exchange: offset 7, char[1]; attribute byte: offset 8.
    // XML char fields and their attr bytes use spaces; no NUL is sent.
    t1001InBlock input;
    memset(&input, ' ', sizeof(input));
    memcpy(input.shcode, symbol, sizeof(input.shcode));
    input.exchange[0] = (char)('1' + exchange); // codes.xml EXCHANGE: 1 KRX, 2 NXT, 3 통합.
    memcpy(g_requestSymbol, symbol, 7);
    ClearOutput();
    g_receivedData = FALSE;
    g_queryError = FALSE;
    g_submitting = TRUE;
    UpdateQueryButton();
    // No Query wrapper in the common helper. NULL means first request.
    // Product code copies this stack buffer before RequestData returns.
    int result = Common::Api()->RequestData(window, T1001_CODE, &input, sizeof(input), NULL);
    g_requestId = result >= 0 && result <= 255 ? result : -1;
    if (g_requestId >= 0)
    {
        wchar_t text[128];
        swprintf_s(text, _countof(text), L"주식현재가 조회를 요청했습니다. 요청 ID: %d", g_requestId);
        Common::SetResult(text);
    }
    else
        Common::ShowCallResult(L"조회 요청 실패", result);
    g_submitting = FALSE;
    Common::EndCall(window);
    UpdateQueryButton();
}

// ------------------------------------------------------------
// t1001 Response: validate the public packet, then copy its output block.
// lpData is the block body; Header and szBlockName are separate API members.
// ------------------------------------------------------------
void ReceiveT1001(const RP_DATA_INFO* response)
{
    if (!response || g_requestId < 0 || response->nRqID != g_requestId || g_closing)
        return;
    char code[7], block[18];
    if (!CopyField(response->Header.szTrCode, sizeof(response->Header.szTrCode),
                   0, sizeof(response->Header.szTrCode), code, sizeof(code)) ||
        !CopyField(response->szBlockName, sizeof(response->szBlockName),
                   0, sizeof(response->szBlockName), block, sizeof(block)) ||
        strcmp(code, T1001_CODE) != 0 || strcmp(block, T1001_OUTPUT_BLOCK) != 0)
    {
        g_queryError = TRUE;
        Common::SetResult(L"요청한 t1001 서비스 또는 출력 블록과 다른 응답입니다.");
        return;
    }
    if (!response->lpData || response->nDataLength != T1001_OUTPUT_SIZE)
    {
        g_queryError = TRUE;
        Common::SetResult(L"t1001 응답 길이가 XML 정의(1,184바이트)와 다릅니다.");
        return;
    }
    if (response->Header.szRetFlag[0] >= '1' && response->Header.szRetFlag[0] <= '9')
    {
        g_queryError = TRUE;
        Common::SetResult(L"서버에서 조회 오류를 반환했습니다. 결과 코드와 원본 메시지를 확인하십시오.");
        return;
    }
    char name[21], price[9], sign[2], change[9], diff[7], volume[13];
    // Copy only the validated body. No DLL-owned pointer escapes this call.
    t1001OutBlock output;
    memcpy(&output, response->lpData, sizeof(output));
    CopyField(output.hname, sizeof(output.hname), 0, sizeof(output.hname), name, sizeof(name));
    CopyField(output.price, sizeof(output.price), 0, sizeof(output.price), price, sizeof(price));
    CopyField(output.sign, sizeof(output.sign), 0, sizeof(output.sign), sign, sizeof(sign));
    CopyField(output.change, sizeof(output.change), 0, sizeof(output.change), change, sizeof(change));
    CopyField(output.diff, sizeof(output.diff), 0, sizeof(output.diff), diff, sizeof(diff));
    CopyField(output.volume, sizeof(output.volume), 0, sizeof(output.volume), volume, sizeof(volume));
    SetWindowTextA(g_requestedSymbol, g_requestSymbol); // Request context, NOT an Output field.
    SetWindowTextA(g_name, name);
    SetWindowTextA(g_price, price);
    SetWindowTextA(g_sign, sign); // XML has no enum mapping; show the original sign value.
    SetWindowTextA(g_change, change);
    SetWindowTextA(g_diff, diff);
    SetWindowTextA(g_volume, volume);
    g_receivedData = TRUE;
    if (!g_queryError)
        Common::SetResult(L"주식현재가 조회 결과를 수신했습니다.");
    // No delete/free: response and lpData belong to OrdaAPI until RELEASE_DATA.
}

void HandleQueryNotification(UINT message, WPARAM wParam, LPARAM lParam)
{
    OrdaAPI* api = Common::Api();
    if (!api)
        return;
    if (message == Common::API_MESSAGE_BASE + WM_TIMEOUT_DATA)
    {
        int id = (int)lParam;
        if (id == g_requestId && id >= 0)
        {
            api->ReleaseRequestData(id);
            g_requestId = -1;
            g_queryError = TRUE;
            Common::SetResult(L"조회 응답 시간이 초과되었습니다. 다시 조회하십시오.");
        }
    }
    else if (wParam == REQUEST_DATA)
        ReceiveT1001((const RP_DATA_INFO*)lParam);
    else if (wParam == MESSAGE_DATA || wParam == SYSTEM_ERROR_DATA)
    {
        const MSG_PACKET* packet = (const MSG_PACKET*)lParam;
        if (packet)
        {
            if (!g_closing && g_requestId >= 0 && packet->nRqID == g_requestId)
            {
                char code[7] = {0}, text[2048] = {0};
                CopyField(packet->szCode, sizeof(packet->szCode), 0,
                          sizeof(packet->szCode), code, sizeof(code));
                int length = packet->nMsgLength;
                if (length > (int)sizeof(text) - 1)
                    length = sizeof(text) - 1;
                if (length > 0 && packet->lpszMessageData)
                    memcpy(text, packet->lpszMessageData, length);
                Common::SetServerResult(code, text); // Original server text; no translation.
                if (wParam == SYSTEM_ERROR_DATA || packet->nIsSystemError)
                    g_queryError = TRUE;
            }
            api->ReleaseMessageData(lParam); // Always release message packets, even unmatched ones.
        }
    }
    else if (wParam == RELEASE_DATA)
    {
        int id = (int)lParam;
        if (id == g_requestId && id >= 0)
        {
            api->ReleaseRequestData(id);
            g_requestId = -1;
            if (!g_receivedData && !g_queryError && !g_closing)
                Common::SetResult(L"조회가 종료되었으나 현재가 데이터가 없습니다.");
        }
    }
    UpdateQueryButton();
}

void DrainDeferredNotifications()
{
    while (!g_submitting && !g_deferred.empty())
    {
        DeferredNotification item = g_deferred.front();
        g_deferred.pop_front();
        HandleQueryNotification(item.message, item.wParam, item.lParam);
    }
}

void AddOutput(HWND window, const wchar_t* label, int y, HWND* value)
{
    Common::AddControl(window, L"STATIC", label, 0, 20, y, 150, 22, 0);
    *value = Common::AddControl(window, L"EDIT", L"", ES_READONLY | ES_AUTOHSCROLL,
                               180, y - 2, 350, 24, 0);
}

LRESULT CALLBACK WindowProc(HWND window, UINT message, WPARAM wParam, LPARAM lParam)
{
    if (message == Common::API_MESSAGE_BASE + WM_RECEIVE_DATA ||
        message == Common::API_MESSAGE_BASE + WM_TIMEOUT_DATA)
    {
        // The DLL posts data/release messages in order. If its RequestData call
        // pumps messages before returning an ID, preserve that order until it returns.
        if (g_submitting)
        {
            DeferredNotification item = {message, wParam, lParam};
            g_deferred.push_back(item);
        }
        else
            HandleQueryNotification(message, wParam, lParam);
        return 0;
    }
    if (Common::HandleMessage(window, message, wParam, lParam))
    {
        UpdateQueryButton();
        return 0;
    }
    switch (message)
    {
    case WM_CREATE:
        Common::CreateLoginControls(window);
        Common::AddControl(window, L"STATIC", L"주식현재가 조회 (t1001)", 0, 20, 495, 350, 22, 0);
        Common::AddControl(window, L"STATIC", L"종목코드", 0, 20, 527, 150, 22, 0);
        g_symbol = Common::AddControl(window, L"EDIT", L"", ES_AUTOHSCROLL | WS_TABSTOP,
                                     180, 525, 140, 24, 0);
        SendMessage(g_symbol, EM_SETLIMITTEXT, 6, 0);
        Common::AddControl(window, L"STATIC", L"거래소구분", 0, 20, 557, 150, 22, 0);
        g_exchange = Common::AddControl(window, L"COMBOBOX", L"", CBS_DROPDOWNLIST | WS_TABSTOP,
                                       180, 555, 140, 120, 0);
        SendMessageW(g_exchange, CB_ADDSTRING, 0, (LPARAM)L"KRX (1)");
        SendMessageW(g_exchange, CB_ADDSTRING, 0, (LPARAM)L"NXT (2)");
        SendMessageW(g_exchange, CB_ADDSTRING, 0, (LPARAM)L"통합 (3)");
        SendMessage(g_exchange, CB_SETCURSEL, 2, 0); // XML default="3".
        g_queryButton = Common::AddControl(window, L"BUTTON", L"조회", WS_TABSTOP,
                                          420, 525, 110, 54, QUERY_BUTTON);
        EnableWindow(g_queryButton, FALSE);
        AddOutput(window, L"요청 종목코드", 597, &g_requestedSymbol);
        AddOutput(window, L"종목명 (hname)", 627, &g_name);
        AddOutput(window, L"현재가 (price)", 657, &g_price);
        AddOutput(window, L"대비 구분 (sign)", 687, &g_sign);
        AddOutput(window, L"전일대비 (change)", 717, &g_change);
        AddOutput(window, L"등락률 (diff)", 747, &g_diff);
        AddOutput(window, L"거래량 (volume)", 777, &g_volume);
        return 0;
    case WM_COMMAND:
        if (LOWORD(wParam) == QUERY_BUTTON && HIWORD(wParam) == BN_CLICKED)
        {
            RequestT1001(window);
            DrainDeferredNotifications();
        }
        return 0;
    case WM_CLOSE:
        if (!Common::BeginClose())
            return 0;
        if (!g_closing)
        {
            g_closing = TRUE;
            UpdateQueryButton();
            DrainDeferredNotifications();
            if (g_requestId >= 0 && Common::Api())
            {
                Common::Api()->ReleaseRequestData(g_requestId);
                g_requestId = -1;
            }
            Common::Cleanup(); // Logout -> Disconnect -> FreeLibrary, before HWND destruction.
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
    wc.lpszClassName = L"OrdaAPIT1001Sample";
    if (!RegisterClassW(&wc))
        return 1;
    HWND window = CreateWindowW(wc.lpszClassName, L"OrdaAPI 주식현재가 조회 예제 (t1001)",
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
        CW_USEDEFAULT, CW_USEDEFAULT, 570, 855, NULL, NULL, instance, NULL);
    if (!window)
        return 1;
    ShowWindow(window, show);
    UpdateWindow(window);
    Common::BeginCall();
    Common::Initialize(); // Current working directory stays at shared sdk/bin.
    Common::EndCall(window);
    UpdateQueryButton();
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
        SendMessage(window, WM_CLOSE, 0, 0);
    return status == -1 ? 1 : (int)message.wParam;
}
