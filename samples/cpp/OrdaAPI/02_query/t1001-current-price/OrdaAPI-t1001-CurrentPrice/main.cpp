// 읽는 순서: WinMain 초기화 → WindowProc 로그인 통지 → 요청 함수 → 수신 함수 → 종료 처리.
// Load/Initialize와 Connect/Login은 기존 Common Helper를 사용합니다.
// 로그인 결과는 Common::HandleMessage가 처리하며 요청 API는 아래에서 직접 호출합니다.
// Visual Studio 2010 / v100 / Win32 / MultiByte. 한글 소스는 UTF-8 BOM으로 저장합니다.
#include "../../../common/OrdaSampleCommon.h"
#include "t1001.h"
#include <stdio.h>
#include <string.h>
#include <wchar.h>
#include <deque>

namespace Common = OrdaSampleCommon;

const char T1001_CODE[] = "t1001";
const char T1001_OUTPUT_BLOCK[] = "t1001OutBlock";
const int T1001_INPUT_SIZE = 9;
const int T1001_OUTPUT_SIZE = 1184;


const int QUERY_BUTTON = 300;
HWND g_symbol, g_exchange, g_queryButton, g_requestedSymbol;
HWND g_name, g_price, g_sign, g_change, g_diff, g_volume;
int g_requestId = -1; // 요청 ID 0도 유효하며 -1은 진행 중인 요청이 없음을 뜻합니다.
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
    EnableWindow(g_queryButton,
                 Common::IsReady() && !Common::IsBusy() && !g_closing && g_requestId < 0 &&
                     !g_submitting);
}

// 고정 길이 XML 필드는 NUL 종료를 보장하지 않으므로 길이를 지정해 복사합니다.
// 패딩을 제거한 복사본만 사용하고 OrdaAPI 소유 포인터는 보관하지 않습니다.
BOOL CopyField(const void *data, int dataLength, int offset, int width, char *output, int capacity)
{
    if (!data || !output || offset < 0 || width < 0 || dataLength < 0 || offset > dataLength ||
        width > dataLength - offset || capacity <= width)
        return FALSE;
    memcpy(output, (const char *)data + offset, width);
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
// t1001 주식현재가 요청: 입력 구조체를 구성해 9바이트 본문을 공개 API로 전달합니다.
// ------------------------------------------------------------

// 조회 요청: 입력 확인 → 구조체 설정 → RequestData → 요청 ID 보관.
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
    // shcode는 6바이트 단축코드이며 시장별 형식을 임의로 추가하지 않습니다.
    if (strlen(symbol) != 6)
    {
        Common::SetResult(L"종목코드를 6자리로 입력하십시오.");
        return;
    }
    for (int i = 0; i < 6; ++i)
    {
        if (!((symbol[i] >= '0' && symbol[i] <= '9') || (symbol[i] >= 'A' && symbol[i] <= 'Z') ||
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
    // t1001.xml은 attr=1이며 SDK ServiceIO::CreateMemory와 같은 공백 패딩을 사용합니다.
    // shcode는 offset 0의 char[6]이며 offset 6은 속성 바이트입니다.
    // exchange는 offset 7의 char[1]이며 offset 8은 속성 바이트입니다.
    // 문자 필드와 속성 바이트는 공백으로 채우며 종료 NUL은 전송하지 않습니다.
    t1001InBlock input;
    memset(&input, ' ', sizeof(input));
    memcpy(input.shcode, symbol, sizeof(input.shcode));
    input.exchange[0] = (char)('1' + exchange); // codes.xml EXCHANGE 정의: 1=KRX, 2=NXT, 3=통합.
    memcpy(g_requestSymbol, symbol, 7);
    ClearOutput();
    g_receivedData = FALSE;
    g_queryError = FALSE;
    g_submitting = TRUE;
    UpdateQueryButton();
    // 조회 API를 공통 Helper에 숨기지 않습니다. NULL은 처음 조회를 뜻합니다.
    // 제품은 RequestData가 반환하기 전에 지역 입력 버퍼를 내부에 복사합니다.
    int result = Common::Api()->RequestData(window, T1001_CODE, &input, sizeof(input), NULL);
    g_requestId = result >= 0 && result <= 255 ? result : -1;
    if (g_requestId >= 0)
    {
        wchar_t text[128];
        swprintf_s(text,
                   _countof(text),
                   L"주식현재가 조회를 요청했습니다. 요청 ID: %d",
                   g_requestId);
        Common::SetResult(text);
    }
    else
        Common::ShowCallResult(L"조회 요청 실패", result);
    g_submitting = FALSE;
    Common::EndCall(window);
    UpdateQueryButton();
}

// ------------------------------------------------------------
// t1001 응답: 공개 패킷을 검증한 뒤 출력 블록을 복사합니다.
// lpData는 출력 본문이며 Header와 szBlockName은 공개 API의 별도 멤버입니다.
// ------------------------------------------------------------

// 조회 응답: 요청 ID·서비스·블록·길이를 검증하고 필요한 필드를 표시합니다.
void ReceiveT1001(const RP_DATA_INFO *response)
{
    if (!response || g_requestId < 0 || response->nRqID != g_requestId || g_closing)
        return;
    char code[7], block[18];
    if (!CopyField(response->Header.szTrCode,
                   sizeof(response->Header.szTrCode),
                   0,
                   sizeof(response->Header.szTrCode),
                   code,
                   sizeof(code)) ||
        !CopyField(response->szBlockName,
                   sizeof(response->szBlockName),
                   0,
                   sizeof(response->szBlockName),
                   block,
                   sizeof(block)) ||
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
        Common::SetResult(
            L"서버에서 조회 오류를 반환했습니다. 결과 코드와 원본 메시지를 확인하십시오.");
        return;
    }
    char name[21], price[9], sign[2], change[9], diff[7], volume[13];
    // 검증한 본문만 복사하며 DLL 소유 포인터는 이 함수 밖으로 보관하지 않습니다.
    t1001OutBlock output;
    memcpy(&output, response->lpData, sizeof(output));
    CopyField(output.hname, sizeof(output.hname), 0, sizeof(output.hname), name, sizeof(name));
    CopyField(output.price, sizeof(output.price), 0, sizeof(output.price), price, sizeof(price));
    CopyField(output.sign, sizeof(output.sign), 0, sizeof(output.sign), sign, sizeof(sign));
    CopyField(output.change,
              sizeof(output.change),
              0,
              sizeof(output.change),
              change,
              sizeof(change));
    CopyField(output.diff, sizeof(output.diff), 0, sizeof(output.diff), diff, sizeof(diff));
    CopyField(output.volume,
              sizeof(output.volume),
              0,
              sizeof(output.volume),
              volume,
              sizeof(volume));
    SetWindowTextA(g_requestedSymbol,
                   g_requestSymbol); // 보관한 요청 종목코드이며 출력 블록의 필드가 아닙니다.
    SetWindowTextA(g_name, name);
    SetWindowTextA(g_price, price);
    SetWindowTextA(g_sign, sign); // XML에 선택값 정의가 없으므로 수신한 sign 원문을 표시합니다.
    SetWindowTextA(g_change, change);
    SetWindowTextA(g_diff, diff);
    SetWindowTextA(g_volume, volume);
    g_receivedData = TRUE;
    if (!g_queryError)
        Common::SetResult(L"주식현재가 조회 결과를 수신했습니다.");
    // 응답과 lpData는 RELEASE_DATA까지 OrdaAPI 소유이므로 직접 delete/free하지 않습니다.
}

// 요청 통지: 데이터·서버 메시지·오류·해제를 구분하고 소유권에 맞춰 정리합니다.
void HandleQueryNotification(UINT message, WPARAM wParam, LPARAM lParam)
{
    OrdaAPI *api = Common::Api();
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
        ReceiveT1001((const RP_DATA_INFO *)lParam);
    else if (wParam == MESSAGE_DATA || wParam == SYSTEM_ERROR_DATA)
    {
        const MSG_PACKET *packet = (const MSG_PACKET *)lParam;
        if (packet)
        {
            if (!g_closing && g_requestId >= 0 && packet->nRqID == g_requestId)
            {
                char code[7] = {0}, text[2048] = {0};
                CopyField(packet->szCode,
                          sizeof(packet->szCode),
                          0,
                          sizeof(packet->szCode),
                          code,
                          sizeof(code));
                int length = packet->nMsgLength;
                if (length > (int)sizeof(text) - 1)
                    length = sizeof(text) - 1;
                if (length > 0 && packet->lpszMessageData)
                    memcpy(text, packet->lpszMessageData, length);
                Common::SetServerResult(code, text); // 번역하거나 변경하지 않은 서버 원문입니다.
                if (wParam == SYSTEM_ERROR_DATA || packet->nIsSystemError)
                    g_queryError = TRUE;
            }
            api->ReleaseMessageData(
                lParam); // 요청 ID가 일치하지 않아도 메시지 패킷은 반드시 해제합니다.
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

void AddOutput(HWND window, const wchar_t *label, int y, HWND *value)
{
    Common::AddControl(window, L"STATIC", label, 0, 20, y, 150, 22, 0);
    *value = Common::AddControl(window,
                                L"EDIT",
                                L"",
                                ES_READONLY | ES_AUTOHSCROLL,
                                180,
                                y - 2,
                                350,
                                24,
                                0);
}

// 메인 메시지 처리: API 통지와 사용자 입력을 분리하고 종료 수명을 보호합니다.
LRESULT CALLBACK WindowProc(HWND window, UINT message, WPARAM wParam, LPARAM lParam)
{
    if (message == Common::API_MESSAGE_BASE + WM_RECEIVE_DATA ||
        message == Common::API_MESSAGE_BASE + WM_TIMEOUT_DATA)
    {
        // DLL은 데이터와 해제 메시지를 순서대로 게시합니다. RequestData 반환 전에
        // 재진입하면 요청 ID를 받은 뒤 같은 순서로 처리하도록 통지를 보관합니다.
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
        g_symbol = Common::AddControl(window,
                                      L"EDIT",
                                      L"",
                                      ES_AUTOHSCROLL | WS_TABSTOP,
                                      180,
                                      525,
                                      140,
                                      24,
                                      0);
        SendMessage(g_symbol, EM_SETLIMITTEXT, 6, 0);
        Common::AddControl(window, L"STATIC", L"거래소구분", 0, 20, 557, 150, 22, 0);
        g_exchange = Common::AddControl(window,
                                        L"COMBOBOX",
                                        L"",
                                        CBS_DROPDOWNLIST | WS_TABSTOP,
                                        180,
                                        555,
                                        140,
                                        120,
                                        0);
        SendMessageW(g_exchange, CB_ADDSTRING, 0, (LPARAM)L"KRX (1)");
        SendMessageW(g_exchange, CB_ADDSTRING, 0, (LPARAM)L"NXT (2)");
        SendMessageW(g_exchange, CB_ADDSTRING, 0, (LPARAM)L"통합 (3)");
        SendMessage(g_exchange, CB_SETCURSEL, 2, 0); // XML 기본값은 3입니다.
        g_queryButton = Common::AddControl(window,
                                           L"BUTTON",
                                           L"조회",
                                           WS_TABSTOP,
                                           420,
                                           525,
                                           110,
                                           54,
                                           QUERY_BUTTON);
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
            Common::
                Cleanup(); // 윈도우를 파괴하기 전에 Logout → Disconnect → FreeLibrary 순서로 정리합니다.
            DestroyWindow(window);
        }
        return 0;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProcW(window, message, wParam, lParam);
}

// 시작과 종료: 화면 생성 → Common 초기화 → 메시지 루프 → 종료 처리.
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
    HWND window = CreateWindowW(wc.lpszClassName,
                                L"OrdaAPI 주식현재가 조회 예제 (t1001)",
                                WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
                                CW_USEDEFAULT,
                                CW_USEDEFAULT,
                                570,
                                855,
                                NULL,
                                NULL,
                                instance,
                                NULL);
    if (!window)
        return 1;
    ShowWindow(window, show);
    UpdateWindow(window);
    Common::BeginCall();
    Common::Initialize(); // 공통 Runtime의 상대경로 접근을 위해 작업 폴더를 sdk/bin으로 유지합니다.
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
