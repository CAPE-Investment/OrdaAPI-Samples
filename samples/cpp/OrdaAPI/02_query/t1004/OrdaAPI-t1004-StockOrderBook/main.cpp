// 읽는 순서: WinMain 초기화 → 공통 로그인 통지 → RequestQuote → 수신 → 종료.
// VS2010 v100 / Win32 / MultiByte / C++03. 공통 Helper는 변경하지 않습니다.
#include "../../../common/OrdaSampleCommon.h"
#include "t1004.h"
#include <string.h>
#include <stdio.h>
#include <string>
#include <vector>
#include <deque>

namespace Common = OrdaSampleCommon;
const char SERVICE_CODE[] = NAME_t1004;
const wchar_t SERVICE_TITLE[] = L"주식현재가호가조회 (t1004)";
const int DEPTH = 10, COLUMN_COUNT = 4, QUERY_BUTTON = 300;
HWND g_symbol, g_exchange, g_queryButton, g_summary;
HWND g_cells[DEPTH][COLUMN_COUNT];
int g_requestId = -1; // 0도 유효한 요청 ID입니다.
BOOL g_submitting = FALSE, g_processing = FALSE, g_closing = FALSE;
BOOL g_closeRequested = FALSE, g_receivedData = FALSE, g_queryError = FALSE;
char g_requestedSymbol[7] = {0};

// RequestData 안에서 재진입한 통지는 빌린 버퍼가 유효할 때 복사합니다.
struct Notification
{
    UINT message;
    WPARAM kind;
    int id, systemError;
    BOOL malformed;
    RP_DATA_INFO response;
    std::vector<unsigned char> data;
    std::string code, text;
    Notification() : message(0), kind(0), id(-1), systemError(0), malformed(FALSE) {}
};
std::deque<Notification> g_pending;

// 고정 길이 문자열의 표시용 복사본만 만듭니다. 가격을 숫자로 변환하지 않습니다.
std::string Field(const void *data, int width)
{
    std::string value((const char *)data, width);
    while (!value.empty() && (value[value.size() - 1] == ' ' || value[value.size() - 1] == 0))
        value.erase(value.size() - 1);
    size_t first = value.find_first_not_of(' ');
    if (first != std::string::npos)
        value.erase(0, first);
    for (size_t i = 0; i < value.size(); ++i)
        if ((unsigned char)value[i] < 32)
            value[i] = '?';
    return value;
}

void UpdateQueryButton()
{
    BOOL idle = !g_closing && !g_submitting && !g_processing && !Common::IsBusy() &&
                g_requestId < 0;
    EnableWindow(g_queryButton, idle && Common::IsReady());
    EnableWindow(g_symbol, idle);
    EnableWindow(g_exchange, idle);
}

void ClearOutput()
{
    SetWindowTextA(g_summary, "");
    for (int row = 0; row < DEPTH; ++row)
        for (int column = 0; column < COLUMN_COUNT; ++column)
            SetWindowTextA(g_cells[row][column], "");
}

BOOL BuildInput(t1004InBlock &input)
{
    // attr=1의 속성 바이트까지 공백 초기화하며 고정 필드에 NUL을 전송하지 않습니다.
    memset(&input, ' ', sizeof(input));
    char symbol[64] = {0};
    int length = GetWindowTextA(g_symbol, symbol, sizeof(symbol));
    if (GetWindowTextLengthA(g_symbol) != sizeof(input.shcode) ||
        length != sizeof(input.shcode))
    {
        Common::SetResult(L"종목코드는 영문 또는 숫자 6자리로 입력하십시오.");
        return FALSE;
    }
    for (int i = 0; i < length; ++i)
        if (!((symbol[i] >= '0' && symbol[i] <= '9') ||
              (symbol[i] >= 'A' && symbol[i] <= 'Z') ||
              (symbol[i] >= 'a' && symbol[i] <= 'z')))
        {
            Common::SetResult(L"종목코드는 영문 또는 숫자 6자리로 입력하십시오.");
            return FALSE;
        }
    int exchange = (int)SendMessage(g_exchange, CB_GETCURSEL, 0, 0);
    if (exchange < 0 || exchange > 2)
    {
        Common::SetResult(L"거래소를 선택하십시오.");
        return FALSE;
    }
    memcpy(input.shcode, symbol, sizeof(input.shcode));
    input.exchange[0] = (char)('1' + exchange); // codes.xml EXCHANGE: 1=KRX, 2=NXT, 3=통합.
    return TRUE;
}

// 길이를 검사한 뒤 지역 구조체로 복사해 실제 멤버를 표시합니다.
BOOL ReceiveQuote(const RP_DATA_INFO *response)
{
    if (!response || response->nRqID != g_requestId || g_requestId < 0)
        return FALSE;
    if (Field(response->Header.szTrCode, sizeof(response->Header.szTrCode)) != SERVICE_CODE ||
        Field(response->szBlockName, sizeof(response->szBlockName)) != NAME_t1004OutBlock ||
        !response->lpData || response->nDataLength != sizeof(t1004OutBlock))
    {
        Common::SetResult(L"조회 응답의 서비스·출력 블록·데이터 길이가 맞지 않습니다.");
        return FALSE;
    }
    if (response->Header.szRetFlag[0] >= '1' && response->Header.szRetFlag[0] <= '9')
    {
        std::string code = Field(response->Header.szMsgCode, sizeof(response->Header.szMsgCode));
        std::string text = Field(response->Header.szMsgTxt, sizeof(response->Header.szMsgTxt));
        Common::SetServerResult(code.c_str(), text.c_str());
        return FALSE;
    }
    t1004OutBlock output;
    memcpy(&output, response->lpData, sizeof(output));
    std::string summary = "요청 종목: " + std::string(g_requestedSymbol);
    summary += "\r\n종목명: " + Field(output.hname, sizeof(output.hname));
    summary += "    현재가: " + Field(output.price, sizeof(output.price));
    summary += "\r\n호가 시간 (hotime): " + Field(output.hotime, sizeof(output.hotime));
    summary += "    거래량: " + Field(output.volume, sizeof(output.volume));
    summary += "\r\n매도 총잔량: " + Field(output.offer, sizeof(output.offer));
    summary += "    매수 총잔량: " + Field(output.bid, sizeof(output.bid));
    SetWindowTextA(g_summary, summary.c_str());
    const char *rows[DEPTH][COLUMN_COUNT] = {
        {output.offerho1, output.offerrem1, output.bidho1, output.bidrem1},
        {output.offerho2, output.offerrem2, output.bidho2, output.bidrem2},
        {output.offerho3, output.offerrem3, output.bidho3, output.bidrem3},
        {output.offerho4, output.offerrem4, output.bidho4, output.bidrem4},
        {output.offerho5, output.offerrem5, output.bidho5, output.bidrem5},
        {output.offerho6, output.offerrem6, output.bidho6, output.bidrem6},
        {output.offerho7, output.offerrem7, output.bidho7, output.bidrem7},
        {output.offerho8, output.offerrem8, output.bidho8, output.bidrem8},
        {output.offerho9, output.offerrem9, output.bidho9, output.bidrem9},
        {output.offerho10, output.offerrem10, output.bidho10, output.bidrem10}
    };
    const int widths[COLUMN_COUNT] = {sizeof(output.offerho1), sizeof(output.offerrem1),
                                     sizeof(output.bidho1), sizeof(output.bidrem1)};
    for (int row = 0; row < DEPTH; ++row)
        for (int column = 0; column < COLUMN_COUNT; ++column)
            SetWindowTextA(g_cells[row][column], Field(rows[row][column], widths[column]).c_str());
    return TRUE;
}

void ReleaseActiveRequest()
{
    int id = g_requestId;
    g_requestId = -1; // 해제 함수 안의 재진입에서도 같은 요청을 다시 해제하지 않습니다.
    if (id >= 0 && Common::Api())
        Common::Api()->ReleaseRequestData(id);
}

void ProcessNotification(Notification &item)
{
    if (g_closing || g_requestId < 0 || item.id != g_requestId)
        return;
    if (item.message == Common::API_MESSAGE_BASE + WM_TIMEOUT_DATA)
    {
        g_queryError = TRUE;
        ReleaseActiveRequest();
        Common::SetResult(L"조회 응답 시간이 초과되었습니다. 자동 재조회하지 않습니다.");
    }
    else if (item.kind == REQUEST_DATA)
    {
        item.response.lpData = item.data.empty() ? NULL : &item.data[0];
        if (item.malformed || !ReceiveQuote(&item.response))
        {
            g_queryError = TRUE;
            if (item.malformed)
                Common::SetResult(L"조회 응답 데이터가 없거나 길이가 맞지 않습니다.");
        }
        else
        {
            g_receivedData = TRUE;
            if (!g_queryError)
                Common::SetResult(L"호가 조회 결과를 수신했습니다. 실시간 자동갱신은 하지 않습니다.");
        }
    }
    else if (item.kind == MESSAGE_DATA || item.kind == SYSTEM_ERROR_DATA)
    {
        Common::SetServerResult(item.code.c_str(), item.text.c_str());
        if (item.malformed || item.systemError || item.kind == SYSTEM_ERROR_DATA)
            g_queryError = TRUE;
        // 제품은 nIsSystemError==1인 메시지를 해제할 때 해당 요청도 해제합니다.
        // 메시지 형식 오류와 SDK의 원본 시스템 오류 값은 구분합니다.
        if (item.systemError == 1)
            g_requestId = -1;
    }
    else if (item.kind == RELEASE_DATA)
    {
        ReleaseActiveRequest();
        if (!g_receivedData && !g_queryError)
            Common::SetResult(L"조회가 종료되었으나 호가 데이터가 없습니다.");
    }
}

// 메시지는 즉시 해제하고, 데이터는 SDK 소유 버퍼에서 복사해 순서대로 처리합니다.
void CaptureNotification(UINT message, WPARAM kind, LPARAM parameter)
{
    Notification item;
    item.message = message;
    item.kind = kind;
    if (message == Common::API_MESSAGE_BASE + WM_TIMEOUT_DATA || kind == RELEASE_DATA)
        item.id = (int)parameter;
    else if (kind == REQUEST_DATA)
    {
        if (g_closing || (g_requestId < 0 && !g_submitting))
            return;
        const RP_DATA_INFO *response = (const RP_DATA_INFO *)parameter;
        if (!response)
            return;
        item.id = response->nRqID;
        item.response = *response;
        item.response.lpData = NULL;
        item.malformed = !response->lpData || response->nDataLength != sizeof(t1004OutBlock);
        if (!item.malformed)
            item.data.assign(response->lpData, response->lpData + response->nDataLength);
    }
    else if (kind == MESSAGE_DATA || kind == SYSTEM_ERROR_DATA)
    {
        const MSG_PACKET *packet = (const MSG_PACKET *)parameter;
        if (!packet)
            return;
        item.id = packet->nRqID;
        item.systemError = packet->nIsSystemError;
        item.code = Field(packet->szCode, sizeof(packet->szCode));
        item.malformed = packet->nMsgLength < 0 || packet->nMsgLength > 4096 ||
                         (packet->nMsgLength > 0 && !packet->lpszMessageData);
        item.text = item.malformed ? "Invalid server message length/data"
                                  : Field(packet->lpszMessageData ? packet->lpszMessageData :
                                          (const unsigned char *)"", packet->nMsgLength);
        g_pending.push_back(item);
        if (Common::Api())
            Common::Api()->ReleaseMessageData(parameter);
        return;
    }
    else
        return;
    g_pending.push_back(item);
}

void DrainNotifications(HWND window)
{
    if (g_submitting || g_processing)
        return;
    g_processing = TRUE;
    while (!g_pending.empty())
    {
        Notification item = g_pending.front();
        g_pending.pop_front();
        ProcessNotification(item);
    }
    g_processing = FALSE;
    UpdateQueryButton();
    if (g_closeRequested)
        PostMessage(window, WM_CLOSE, 0, 0);
}

// 조회 요청: 입력 확인 → 로그인 상태 재확인 → 4인자 요청 → 비동기 결과 대기.
void RequestQuote(HWND window)
{
    if (g_closing || g_submitting || g_processing || Common::IsBusy() || g_requestId >= 0)
        return;
    if (!Common::IsReady())
    {
        Common::SetResult(L"로그인이 필요합니다.");
        return;
    }
    t1004InBlock input;
    if (!BuildInput(input))
        return;
    g_submitting = TRUE;
    UpdateQueryButton();
    Common::BeginCall();
    if (!Common::CheckSession() || g_closeRequested || !Common::IsReady())
    {
        Common::SetResult(L"로그인 상태를 확인하십시오. 필요하면 프로그램을 다시 실행하십시오.");
        g_submitting = FALSE;
        Common::EndCall(window);
        DrainNotifications(window);
        return;
    }
    memcpy(g_requestedSymbol, input.shcode, sizeof(input.shcode));
    g_requestedSymbol[sizeof(input.shcode)] = 0; // 표시용 복사본에만 NUL을 붙입니다.
    ClearOutput();
    g_receivedData = FALSE;
    g_queryError = FALSE;
    int result = Common::Api()->RequestData(window, NAME_t1004, &input, sizeof(input));
    g_requestId = result >= 0 && result <= 255 ? result : -1;
    if (g_requestId >= 0)
        Common::SetResult(L"조회 요청 접수. 최종 결과는 비동기 응답을 확인하십시오.");
    else
        Common::ShowCallResult(L"조회 요청 실패", result);
    g_submitting = FALSE;
    Common::EndCall(window);
    DrainNotifications(window);
}

// 종료 시 게시 대기 중인 메시지 패킷도 해제합니다. 데이터 포인터는 읽지 않습니다.
void DiscardQueuedNotifications(HWND window)
{
    MSG message;
    while (PeekMessage(&message, window, Common::API_MESSAGE_BASE + WM_RECEIVE_DATA,
                       Common::API_MESSAGE_BASE + WM_RECEIVE_DATA, PM_REMOVE))
        if ((message.wParam == MESSAGE_DATA || message.wParam == SYSTEM_ERROR_DATA) &&
            message.lParam && Common::Api())
        {
            const MSG_PACKET *packet = (const MSG_PACKET *)message.lParam;
            if (packet->nIsSystemError == 1 && packet->nRqID == g_requestId)
                g_requestId = -1;
            Common::Api()->ReleaseMessageData(message.lParam);
        }
    while (PeekMessage(&message, window, Common::API_MESSAGE_BASE + WM_TIMEOUT_DATA,
                       Common::API_MESSAGE_BASE + WM_TIMEOUT_DATA, PM_REMOVE)) {}
}

LRESULT CALLBACK WindowProc(HWND window, UINT message, WPARAM wParam, LPARAM lParam)
{
    if (message == Common::API_MESSAGE_BASE + WM_RECEIVE_DATA ||
        message == Common::API_MESSAGE_BASE + WM_TIMEOUT_DATA)
    {
        BOOL processing = g_processing;
        g_processing = TRUE;
        CaptureNotification(message, wParam, lParam);
        g_processing = processing;
        DrainNotifications(window);
        return 0;
    }
    if (!g_closing && Common::HandleMessage(window, message, wParam, lParam))
    {
        UpdateQueryButton();
        return 0;
    }
    switch (message)
    {
    case WM_CREATE:
    {
        Common::CreateLoginControls(window);
        Common::AddControl(window, L"STATIC", SERVICE_TITLE, 0, 20, 495, 500, 22, 0);
        Common::AddControl(window, L"STATIC", L"종목코드 (6자리)", 0, 20, 527, 150, 22, 0);
        g_symbol = Common::AddControl(window, L"EDIT", L"", ES_AUTOHSCROLL | WS_TABSTOP,
                                      180, 525, 140, 24, 0);
        SendMessage(g_symbol, EM_SETLIMITTEXT, 6, 0);
        Common::AddControl(window, L"STATIC", L"거래소구분", 0, 20, 557, 150, 22, 0);
        g_exchange = Common::AddControl(window, L"COMBOBOX", L"",
                                        CBS_DROPDOWNLIST | WS_TABSTOP, 180, 555, 160, 120, 0);
        SendMessageW(g_exchange, CB_ADDSTRING, 0, (LPARAM)L"한국거래소 KRX (1)");
        SendMessageW(g_exchange, CB_ADDSTRING, 0, (LPARAM)L"넥스트레이드 NXT (2)");
        SendMessageW(g_exchange, CB_ADDSTRING, 0, (LPARAM)L"통합 (3)");
        SendMessage(g_exchange, CB_SETCURSEL, 2, 0); // t1004.xml 및 t1001 정책의 기본값 3.
        g_queryButton = Common::AddControl(window, L"BUTTON", L"조회", WS_TABSTOP,
                                           420, 525, 110, 54, QUERY_BUTTON);
        Common::AddControl(window, L"STATIC", L"조회 결과 (서버 원문 값)", 0, 580, 20, 530, 22, 0);
        g_summary = Common::AddControl(window, L"EDIT", L"",
                                       ES_READONLY | ES_MULTILINE | ES_AUTOHSCROLL | WS_HSCROLL,
                                       580, 50, 540, 115, 0);
        const wchar_t *headers[COLUMN_COUNT] = {L"매도호가", L"매도잔량", L"매수호가", L"매수잔량"};
        const int cellWidth = 117;
        Common::AddControl(window, L"STATIC", L"단계", 0, 580, 185, 50, 22, 0);
        for (int column = 0; column < COLUMN_COUNT; ++column)
            Common::AddControl(window, L"STATIC", headers[column], 0,
                               630 + column * cellWidth, 185, cellWidth, 22, 0);
        for (int row = 0; row < DEPTH; ++row)
        {
            wchar_t label[16];
            swprintf_s(label, _countof(label), L"%d", row + 1);
            Common::AddControl(window, L"STATIC", label, 0, 580, 217 + row * 30, 45, 22, 0);
            for (int column = 0; column < COLUMN_COUNT; ++column)
                g_cells[row][column] = Common::AddControl(window, L"EDIT", L"",
                    ES_READONLY | ES_RIGHT | ES_AUTOHSCROLL,
                    630 + column * cellWidth, 215 + row * 30, cellWidth - 4, 24, 0);
        }
        UpdateQueryButton();
        return 0;
    }
    case WM_COMMAND:
        if (LOWORD(wParam) == QUERY_BUTTON && HIWORD(wParam) == BN_CLICKED)
            RequestQuote(window);
        return 0;
    case WM_CLOSE:
        if (g_submitting || g_processing)
        {
            g_closeRequested = TRUE;
            return 0;
        }
        if (!Common::BeginClose() || g_closing)
            return 0;
        g_closing = TRUE;
        UpdateQueryButton();
        DiscardQueuedNotifications(window);
        ReleaseActiveRequest();
        g_pending.clear();
        Common::Cleanup(); // Logout → Disconnect → FreeLibrary. HWND는 마지막에 파괴합니다.
        DestroyWindow(window);
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
    wc.lpszClassName = L"OrdaAPIt1004StockOrderBook";
    if (!RegisterClassW(&wc))
        return 1;
    HWND window = CreateWindowW(wc.lpszClassName, SERVICE_TITLE,
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
        CW_USEDEFAULT, CW_USEDEFAULT, 1150, 650, NULL, NULL, instance, NULL);
    if (!window)
        return 1;
    ShowWindow(window, show);
    UpdateWindow(window);
    Common::BeginCall();
    Common::Initialize(); // 작업 폴더 sdk/bin의 DLL을 LoadLibrary/GetProcAddress로 사용합니다.
    Common::EndCall(window);
    UpdateQueryButton();
    MSG message;
    int status;
    while ((status = GetMessageW(&message, NULL, 0, 0)) > 0)
        if (!IsDialogMessageW(window, &message))
        {
            TranslateMessage(&message);
            DispatchMessageW(&message);
        }
    if (status == -1 && IsWindow(window))
        SendMessage(window, WM_CLOSE, 0, 0);
    return status == -1 ? 1 : (int)message.wParam;
}
