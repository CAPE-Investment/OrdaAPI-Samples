// 읽는 순서: WinMain 초기화 → 공통 로그인 통지 → RequestMaster → 수신 → 종료.
// VS2010 v100 / Win32 / MultiByte / C++03. 공통 Helper는 변경하지 않습니다.
#include "../../../common/OrdaSampleCommon.h"
#include "t3088.h"
#include <string.h>
#include <stdio.h>
#include <limits.h>
#include <string>
#include <vector>
#include <deque>

namespace Common = OrdaSampleCommon;
const char SERVICE_CODE[] = NAME_t3088;
const wchar_t SERVICE_TITLE[] = L"ELW마스터조회(t3088)";
const int QUERY_BUTTON = 300, DISPLAY_LIMIT = 200;
HWND g_input, g_queryButton, g_output;
int g_requestId = -1, g_recordCount = 0;
BOOL g_submitting = FALSE, g_processing = FALSE, g_closing = FALSE;
BOOL g_closeRequested = FALSE, g_receivedData = FALSE, g_queryError = FALSE;
std::string g_rows;
HWND g_nextButton;
BOOL g_gotCursor = FALSE, g_gotRecords = FALSE, g_hasNext = FALSE, g_moreFlag = FALSE;
char g_pageCursor[13] = {0}, g_requestCursor[13] = {0};

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
    EnableWindow(g_input, idle);
    EnableWindow(g_nextButton, idle && Common::IsReady() && g_hasNext && !g_queryError);
}

void ClearOutput()
{
    SetWindowTextA(g_output, "");
    g_rows.clear();
    g_recordCount = 0;
}

// 실제 입력 구조체 전체를 속성 바이트까지 공백 초기화합니다. NUL은 전송하지 않습니다.
BOOL BuildInput(t3088InBlock &input, BOOL next)
{
    memset(&input, ' ', sizeof(input)); // 첫 조회 expcode의 XML 기본값은 공백 12바이트.
    if (next)
    {
        if (!g_hasNext || g_queryError)
            return FALSE;
        memcpy(input.expcode, g_pageCursor, sizeof(input.expcode));
    }
    return TRUE;
}

// A형 반복 블록은 건수 접두부 없이 레코드만 연결됩니다.
// 길이를 레코드 크기로 나눌 수 있는지 먼저 검사하고 0건도 정상 처리합니다.
BOOL ValidBody(const RP_DATA_INFO *response)
{
    if (!response || response->nDataLength < 0 ||
        (!response->lpData && response->nDataLength > 0))
        return FALSE;
    std::string block = Field(response->szBlockName, sizeof(response->szBlockName));
    if (block == NAME_t3088OutBlock)
        return response->lpData && response->nDataLength == sizeof(t3088OutBlock);
    return block == NAME_t3088OutBlock1 && response->nDataLength % sizeof(t3088OutBlock1) == 0;
}

BOOL ReceiveMaster(const RP_DATA_INFO *response)
{
    if (!response || response->nRqID != g_requestId || g_requestId < 0)
        return FALSE;
    if (Field(response->Header.szTrCode, sizeof(response->Header.szTrCode)) != SERVICE_CODE ||
        !ValidBody(response))
    {
        Common::SetResult(L"응답 서비스·블록명·길이 또는 반복 레코드 경계가 맞지 않습니다.");
        return FALSE;
    }
    if (response->Header.szRetFlag[0] >= '1' && response->Header.szRetFlag[0] <= '9')
    {
        std::string code = Field(response->Header.szMsgCode, sizeof(response->Header.szMsgCode));
        std::string text = Field(response->Header.szMsgTxt, sizeof(response->Header.szMsgTxt));
        Common::SetServerResult(code.c_str(), text.c_str());
        return FALSE;
    }
    g_moreFlag = g_moreFlag || response->Header.szPNFlag[0] == '1';
    if (Field(response->szBlockName, sizeof(response->szBlockName)) == NAME_t3088OutBlock)
    {
        t3088OutBlock output;
        memcpy(&output, response->lpData, sizeof(output));
        if (g_gotCursor && memcmp(g_pageCursor, output.expcode, sizeof(output.expcode)))
        {
            Common::SetResult(L"같은 요청에서 서로 다른 연속키가 수신되었습니다.");
            return FALSE;
        }
        memcpy(g_pageCursor, output.expcode, sizeof(output.expcode));
        g_pageCursor[sizeof(output.expcode)] = 0;
        g_gotCursor = TRUE;
        SetWindowTextA(g_input, Field(output.expcode, sizeof(output.expcode)).c_str());
        return TRUE;
    }
    int count = response->nDataLength / sizeof(t3088OutBlock1);
    if (count > INT_MAX - g_recordCount)
    {
        Common::SetResult(L"수신 레코드 수가 표시 가능한 범위를 초과했습니다.");
        return FALSE;
    }
    int remaining = g_recordCount < DISPLAY_LIMIT ? DISPLAY_LIMIT - g_recordCount : 0;
    int shown = count < remaining ? count : remaining;
    for (int row = 0; row < shown; ++row)
    {
        t3088OutBlock1 output;
        memcpy(&output, response->lpData + row * sizeof(output), sizeof(output));
        char number[32];
        sprintf_s(number, "[%d] ", g_recordCount + row + 1);
        g_rows += number;
        g_rows += "shcode=" + Field(output.shcode, sizeof(output.shcode));
        g_rows += " | expcode=" + Field(output.expcode, sizeof(output.expcode));
        g_rows += " | hname=" + Field(output.hname, sizeof(output.hname));
        g_rows += " | issuercd=" + Field(output.issuercd, sizeof(output.issuercd));
        g_rows += " | issuernmk=" + Field(output.issuernmk, sizeof(output.issuernmk));
        g_rows += " | lastymd=" + Field(output.lastymd, sizeof(output.lastymd));
        g_rows += " | item1=" + Field(output.item1, sizeof(output.item1));
        g_rows += " | item2=" + Field(output.item2, sizeof(output.item2));
        g_rows += " | item3=" + Field(output.item3, sizeof(output.item3));
        g_rows += " | item4=" + Field(output.item4, sizeof(output.item4));
        g_rows += " | item5=" + Field(output.item5, sizeof(output.item5));
        g_rows += " | elwopt=" + Field(output.elwopt, sizeof(output.elwopt));
        g_rows += "\r\n";
    }
    g_recordCount += count;
    char heading[128];
    sprintf_s(heading, "수신 레코드 수: %d (이 페이지 최대 %d행 표시)\r\n",
              g_recordCount, DISPLAY_LIMIT);
    SetWindowTextA(g_output, (std::string(heading) + g_rows).c_str());
    g_gotRecords = TRUE;
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
        g_hasNext = FALSE;
        ReleaseActiveRequest();
        Common::SetResult(L"조회 응답 시간이 초과되었습니다. 자동 재조회하지 않습니다.");
    }
    else if (item.kind == REQUEST_DATA)
    {
        item.response.lpData = item.data.empty() ? NULL : &item.data[0];
        if (item.malformed || !ReceiveMaster(&item.response))
        {
            g_queryError = TRUE;
            if (item.malformed)
                Common::SetResult(L"조회 응답 데이터가 없거나 길이가 맞지 않습니다.");
        }
        else
        {
            g_receivedData = TRUE;
            if (!g_queryError)
                Common::SetResult(L"마스터 응답을 수신했습니다. 요청 해제 통지를 기다립니다.");
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
        if (!g_gotCursor || !g_gotRecords)
        {
            g_queryError = TRUE;
            Common::SetResult(L"연속키 또는 ELW 목록 블록이 누락되었습니다. 처음 조회를 다시 실행하십시오.");
        }
        g_hasNext = !g_queryError && g_moreFlag &&
            !Field(g_pageCursor, 12).empty() && memcmp(g_pageCursor, g_requestCursor, 12) != 0;
        if (!g_queryError)
            Common::SetResult(g_hasNext ? L"페이지 수신 완료. 다음 조회를 수동으로 실행할 수 있습니다."
                : g_moreFlag ? L"다음 페이지 표시가 있으나 새 연속키가 없어 다음 조회를 차단했습니다."
                             : L"마스터 조회가 완료되었습니다. 0건도 정상 결과입니다.");
        if (!g_receivedData && !g_queryError)
            Common::SetResult(L"조회가 종료되었으나 마스터 출력 블록이 없습니다.");
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
        item.malformed = !ValidBody(response);
        if (!item.malformed && response->nDataLength > 0)
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
void RequestMaster(HWND window, BOOL next = FALSE)
{
    if (g_closing || g_submitting || g_processing || Common::IsBusy() || g_requestId >= 0)
        return;
    if (!Common::IsReady())
    {
        Common::SetResult(L"로그인이 필요합니다.");
        return;
    }
    t3088InBlock input;
    if (!BuildInput(input, next))
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
    memcpy(g_requestCursor, input.expcode, sizeof(input.expcode));
    g_requestCursor[sizeof(input.expcode)] = 0;
    g_gotCursor = g_gotRecords = g_hasNext = g_moreFlag = FALSE;
    memset(g_pageCursor, ' ', 12);
    g_pageCursor[12] = 0;
    SetWindowTextA(g_input, Field(input.expcode, sizeof(input.expcode)).c_str());
    ClearOutput();
    g_receivedData = FALSE;
    g_queryError = FALSE;
    // A형은 5번째 인자의 존재로 연속 플래그를 설정합니다(제품 ComGate.cpp).
    // 실제 커서는 XML의 expcode로 전달하며 A형에 B/D형 Header 키를 가정하지 않습니다.
    int result;
    if (next)
        result = Common::Api()->RequestData(window, NAME_t3088, &input, sizeof(input), g_requestCursor);
    else
        result = Common::Api()->RequestData(window, NAME_t3088, &input, sizeof(input));
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
        Common::AddControl(window, L"STATIC", SERVICE_TITLE, 0, 580, 20, 540, 22, 0);
        Common::AddControl(window, L"STATIC", L"연속키 (expcode, 수신 전용)", 0, 580, 55, 210, 22, 0);
        g_input = Common::AddControl(window, L"EDIT", L"", ES_AUTOHSCROLL | WS_TABSTOP | ES_READONLY,
                                     790, 52, 300, 24, 0);
        SendMessage(g_input, EM_SETLIMITTEXT, 12, 0);
        SetWindowTextA(g_input, "            ");
        g_queryButton = Common::AddControl(window, L"BUTTON", L"처음 조회", WS_TABSTOP,
                                           790, 90, 140, 30, QUERY_BUTTON);
        g_nextButton = Common::AddControl(window, L"BUTTON", L"다음 조회", WS_TABSTOP,
                                          950, 90, 140, 30, 301);
        Common::AddControl(window, L"STATIC", L"조회 결과 (이 페이지 최대 200행 표시)",
                           0, 580, 135, 540, 22, 0);
        g_output = Common::AddControl(window, L"EDIT", L"",
            ES_READONLY | ES_MULTILINE | ES_AUTOVSCROLL | ES_AUTOHSCROLL | WS_VSCROLL | WS_HSCROLL,
            580, 165, 540, 400, 0);
        SendMessage(g_output, EM_SETLIMITTEXT, 1024 * 1024, 0);
        UpdateQueryButton();
        return 0;
    }
    case WM_COMMAND:
        if ((LOWORD(wParam) == QUERY_BUTTON || LOWORD(wParam) == 301) &&
            HIWORD(wParam) == BN_CLICKED)
            RequestMaster(window, LOWORD(wParam) == 301);
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
    wc.lpszClassName = L"OrdaAPIt3088ELWMaster";
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
