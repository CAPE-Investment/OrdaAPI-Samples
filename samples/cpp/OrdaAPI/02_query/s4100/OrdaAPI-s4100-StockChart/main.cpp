// 읽는 순서: WinMain 초기화 → 공통 로그인 통지 → 조회 요청 → 수신 → 해제 → 다음 조회 또는 종료.
// VS2010 v100 / Win32 / MultiByte / C++03. 공통 Helper는 변경하지 않습니다.
#include "../../../common/OrdaSampleCommon.h"
// 수신 블록명은 서비스 XML의 이름을 사용합니다.
#include "s4100.h"
#include <string.h>
#include <stdio.h>
#include <limits.h>
#include <string>
#include <vector>
#include <deque>
#include <commctrl.h>

namespace Common = OrdaSampleCommon;
const char SERVICE_CODE[] = NAME_s4100;
const wchar_t SERVICE_TITLE[] = L"주식챠트(종합)(s4100)";
const int QUERY_BUTTON = 300, DISPLAY_LIMIT = 200;
HWND g_queryButton, g_output, g_list;

HWND g_shcode;
HWND g_gubun;
HWND g_ncnt;
HWND g_qrycnt;
HWND g_tdgb;
HWND g_sdate;
HWND g_edate;
HWND g_detail;
int g_requestId = -1;
int g_recordCount = 0;
BOOL g_submitting = FALSE, g_processing = FALSE, g_closing = FALSE;
BOOL g_closeRequested = FALSE, g_queryError = FALSE;
BOOL g_serverMessageReceived = FALSE;

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
    if (first == std::string::npos) return std::string();
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
}

// 필드의 고정 폭과 공백을 유지하며 수치를 변환하지 않습니다.
std::string WireField(const char *data, size_t width)
{
    return std::string(data, width);
}

void ClearOutput()
{
    SetWindowTextA(g_output, "");
    ListView_DeleteAllItems(g_list);
    g_recordCount = 0;
    ListView_DeleteAllItems(g_detail);
}

// 종목 단축코드는 고정 폭 문자열이며 선행 0을 보존합니다.
BOOL ReadCode(HWND edit, char *field, int width)
{
    wchar_t value[16];
    if (GetWindowTextW(edit, value, _countof(value)) != width ||
        GetWindowTextLengthW(edit) != width)
        return FALSE;
    for (int i = 0; i < width; ++i)
    {
        wchar_t ch = value[i];
        if (!(ch >= L'0' && ch <= L'9') && !(ch >= L'A' && ch <= L'Z'))
            return FALSE;
        field[i] = (char)ch;
    }
    return TRUE;
}

// 선택 항목을 비워 두면 요청에 초기화한 공백이 유지됩니다.
int ReadDigits(HWND edit, char *field, int width)
{
    wchar_t value[16];
    int length = GetWindowTextW(edit, value, _countof(value));
    if (GetWindowTextLengthW(edit) > width || length > width) return -1;
    for (int i = 0; i < length; ++i)
    {
        if (value[i] < L'0' || value[i] > L'9') return -1;
        field[i] = (char)value[i];
    }
    return length;
}

BOOL ReadDate(HWND edit, char *field)
{
    int length = ReadDigits(edit, field, 8);
    if (length == 0) return TRUE;
    if (length != 8) return FALSE;
    SYSTEMTIME date = {0};
    for (int i = 0; i < 4; ++i)
        date.wYear = (WORD)(date.wYear * 10 + field[i] - '0');
    date.wMonth = (WORD)((field[4] - '0') * 10 + field[5] - '0');
    date.wDay = (WORD)((field[6] - '0') * 10 + field[7] - '0');
    FILETIME checked;
    return SystemTimeToFileTime(&date, &checked);
}

// 주기·당일 구분의 코드표 연결은 전문에 정의되어 있지 않습니다.
BOOL ReadCategory(HWND edit, char &field)
{
    wchar_t value[2];
    int length = GetWindowTextLengthW(edit);
    if (length == 0) return TRUE;
    if (length != 1 || GetWindowTextW(edit, value, _countof(value)) != 1 ||
        value[0] < L' ' || value[0] > L'~')
        return FALSE;
    field = (char)value[0];
    return TRUE;
}


BOOL BuildInput(s4100InBlock &input)
{
    // 선택 항목, 속성 바이트 및 최초 조회 연속키를 공백으로 초기화합니다.
    memset(&input, ' ', sizeof(input));
    if (!ReadCode(g_shcode, input.shcode, sizeof(input.shcode))) return FALSE;
    if (ReadDigits(g_ncnt, input.ncnt, sizeof(input.ncnt)) < 0) return FALSE;
    int digits = ReadDigits(g_qrycnt, input.qrycnt, sizeof(input.qrycnt));
    if (digits < 0) return FALSE;
    if (!ReadDate(g_sdate, input.sdate) || !ReadDate(g_edate, input.edate)) return FALSE;
    if (input.sdate[0] != ' ' && input.edate[0] != ' ' &&
        memcmp(input.sdate, input.edate, sizeof(input.sdate)) > 0)
        return FALSE;
    if (!ReadCategory(g_gubun, input.gubun[0]) || !ReadCategory(g_tdgb, input.tdgb[0]))
        return FALSE;
    return TRUE;
}

void SetCell(HWND list, int row, int column, const std::string &value)
{
    LVITEMA item = {0};
    item.mask = LVIF_TEXT;
    item.iItem = row;
    item.iSubItem = column;
    item.pszText = const_cast<char *>(value.c_str());
    if (column == 0)
        SendMessageA(list, LVM_INSERTITEMA, 0, (LPARAM)&item);
    else
        SendMessageA(list, LVM_SETITEMTEXTA, row, (LPARAM)&item);
}

// SDK가 BlockName별로 전달한 순수 블록 데이터의 경계를 검사합니다.
BOOL ValidBody(const RP_DATA_INFO *response)
{
    if (!response || response->nDataLength < 0 ||
        response->nDataLength > response->nTotalDataBufferSize ||
        (!response->lpData && response->nDataLength != 0))
        return FALSE;
    std::string block = Field(response->szBlockName, sizeof(response->szBlockName));
    if (block == NAME_s4100OutBlock) return response->nDataLength == sizeof(s4100OutBlock);
    return block == NAME_s4100OutBlock1 && response->nDataLength % sizeof(s4100OutBlock1) == 0;
}

BOOL ReceiveData(const RP_DATA_INFO *response)
{
    if (!response || response->nRqID != g_requestId || g_requestId < 0 || !ValidBody(response))
        return FALSE;
    if (Field(response->Header.szTrCode, sizeof(response->Header.szTrCode)) != SERVICE_CODE)
        return FALSE;
    if (response->Header.szRetFlag[0] >= '1' && response->Header.szRetFlag[0] <= '9')
        return FALSE;
    if (Field(response->szBlockName, sizeof(response->szBlockName)) == NAME_s4100OutBlock)
    {
        s4100OutBlock output;
        memcpy(&output, response->lpData, sizeof(output));
        ListView_DeleteAllItems(g_list);
        SetCell(g_list, 0, 0, "단축코드 (shcode)");
        SetCell(g_list, 0, 1, WireField(output.shcode, sizeof(output.shcode)));
        SetCell(g_list, 1, 0, "전일시가 (jisiga)");
        SetCell(g_list, 1, 1, WireField(output.jisiga, sizeof(output.jisiga)));
        SetCell(g_list, 2, 0, "전일고가 (jihigh)");
        SetCell(g_list, 2, 1, WireField(output.jihigh, sizeof(output.jihigh)));
        SetCell(g_list, 3, 0, "전일저가 (jilow)");
        SetCell(g_list, 3, 1, WireField(output.jilow, sizeof(output.jilow)));
        SetCell(g_list, 4, 0, "전일종가 (jiclose)");
        SetCell(g_list, 4, 1, WireField(output.jiclose, sizeof(output.jiclose)));
        SetCell(g_list, 5, 0, "전일거래량 (jivolume)");
        SetCell(g_list, 5, 1, WireField(output.jivolume, sizeof(output.jivolume)));
        SetCell(g_list, 6, 0, "당일시가 (disiga)");
        SetCell(g_list, 6, 1, WireField(output.disiga, sizeof(output.disiga)));
        SetCell(g_list, 7, 0, "당일고가 (dihigh)");
        SetCell(g_list, 7, 1, WireField(output.dihigh, sizeof(output.dihigh)));
        SetCell(g_list, 8, 0, "당일저가 (dilow)");
        SetCell(g_list, 8, 1, WireField(output.dilow, sizeof(output.dilow)));
        SetCell(g_list, 9, 0, "당일종가 (diclose)");
        SetCell(g_list, 9, 1, WireField(output.diclose, sizeof(output.diclose)));
        SetCell(g_list, 10, 0, "상한가 (highend)");
        SetCell(g_list, 10, 1, WireField(output.highend, sizeof(output.highend)));
        SetCell(g_list, 11, 0, "하한가 (lowend)");
        SetCell(g_list, 11, 1, WireField(output.lowend, sizeof(output.lowend)));
        SetCell(g_list, 12, 0, "연속일자 (cts_date)");
        SetCell(g_list, 12, 1, WireField(output.cts_date, sizeof(output.cts_date)));
        SetCell(g_list, 13, 0, "연속시간 (cts_time)");
        SetCell(g_list, 13, 1, WireField(output.cts_time, sizeof(output.cts_time)));
        SetCell(g_list, 14, 0, "연속당일구분 (cts_daygb)");
        SetCell(g_list, 14, 1, WireField(output.cts_daygb, sizeof(output.cts_daygb)));
        return TRUE;
    }
    // 정렬하거나 중복을 제거하지 않고 수신 순서대로 표시합니다.
    int count = response->nDataLength / sizeof(s4100OutBlock1);
    if (count > INT_MAX - g_recordCount) return FALSE;
    for (int row = 0; row < count && g_recordCount + row < DISPLAY_LIMIT; ++row)
    {
        s4100OutBlock1 output;
        memcpy(&output, response->lpData + row * sizeof(output), sizeof(output));
        int displayRow = g_recordCount + row;
        SetCell(g_detail, displayRow, 0, WireField(output.date, sizeof(output.date)));
        SetCell(g_detail, displayRow, 1, WireField(output.time, sizeof(output.time)));
        SetCell(g_detail, displayRow, 2, WireField(output.open, sizeof(output.open)));
        SetCell(g_detail, displayRow, 3, WireField(output.high, sizeof(output.high)));
        SetCell(g_detail, displayRow, 4, WireField(output.low, sizeof(output.low)));
        SetCell(g_detail, displayRow, 5, WireField(output.close, sizeof(output.close)));
        SetCell(g_detail, displayRow, 6, WireField(output.jdiff_vol, sizeof(output.jdiff_vol)));
        SetCell(g_detail, displayRow, 7, WireField(output.value, sizeof(output.value)));
        SetCell(g_detail, displayRow, 8, WireField(output.jongchk, sizeof(output.jongchk)));
        SetCell(g_detail, displayRow, 9, WireField(output.rate, sizeof(output.rate)));
        SetCell(g_detail, displayRow, 10, WireField(output.pricechk, sizeof(output.pricechk)));
        SetCell(g_detail, displayRow, 11, WireField(output.ratevalue, sizeof(output.ratevalue)));
    }
    g_recordCount += count;
    return TRUE;
}

void ShowOutput()
{
    char heading[128];
    sprintf_s(heading, "차트 수신 %d건 / 표시 %d건", g_recordCount, ListView_GetItemCount(g_detail));
    SetWindowTextA(g_output, heading);
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
        if (item.malformed || !ReceiveData(&item.response))
        {
            g_queryError = TRUE;
            Common::SetResult(L"응답 서비스·블록·길이 또는 서버 오류를 확인하십시오.");
            if (item.malformed)
                Common::SetResult(L"조회 응답 데이터가 없거나 길이가 맞지 않습니다.");
        }
        else
        {
            if (!g_queryError && !g_serverMessageReceived)
                Common::SetResult(L"조회 응답을 수신했습니다. 요청 해제 통지를 기다립니다.");
        }
    }
    else if (item.kind == MESSAGE_DATA || item.kind == SYSTEM_ERROR_DATA)
    {
        if (!g_queryError) Common::SetServerResult(item.code.c_str(), item.text.c_str());
        g_serverMessageReceived = TRUE;
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
        ShowOutput();
        if (!g_queryError && !g_serverMessageReceived)
            Common::SetResult(L"조회 완료.");
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
        if (!response || (!g_submitting && response->nRqID != g_requestId))
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
void RequestData(HWND window)
{
    if (g_closing || g_submitting || g_processing || Common::IsBusy() || g_requestId >= 0)
        return;
    if (!Common::IsReady())
    {
        Common::SetResult(L"로그인이 필요합니다.");
        return;
    }
    s4100InBlock input;
    if (!BuildInput(input))
    {
        SecureZeroMemory(&input, sizeof(input));
        Common::SetResult(L"입력 길이·형식과 필수 항목을 확인하십시오.");
        return;
    }
    g_submitting = TRUE;
    UpdateQueryButton();
    Common::BeginCall();
    if (!Common::CheckSession() || g_closeRequested || !Common::IsReady())
    {
        Common::SetResult(L"로그인 상태를 확인하십시오. 필요하면 프로그램을 다시 실행하십시오.");
        SecureZeroMemory(&input, sizeof(input));
        g_submitting = FALSE;
        Common::EndCall(window);
        DrainNotifications(window);
        return;
    }
    ClearOutput();
    g_queryError = FALSE;
    g_serverMessageReceived = FALSE;
    int result = Common::Api()->RequestData(window, SERVICE_CODE, &input, sizeof(input));
    SecureZeroMemory(&input, sizeof(input));
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

void AddColumn(HWND list, int index, const wchar_t *title, int width)
{
    LVCOLUMNW column = {0};
    column.mask = LVCF_TEXT | LVCF_WIDTH;
    column.pszText = const_cast<wchar_t *>(title);
    column.cx = width;
    SendMessageW(list, LVM_INSERTCOLUMNW, index, (LPARAM)&column);
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
        Common::AddControl(window, L"STATIC", L"종목 단축코드", 0, 580, 53, 135, 22, 0);
        g_shcode = Common::AddControl(window, L"EDIT", L"", WS_BORDER | WS_TABSTOP | ES_AUTOHSCROLL,
                                  717, 50, 106, 24, 310);
        if (!g_shcode) return -1;
        SendMessage(g_shcode, EM_SETLIMITTEXT, 6, 0);
        Common::AddControl(window, L"STATIC", L"주기구분 (1바이트)", 0, 845, 53, 135, 22, 0);
        g_gubun = Common::AddControl(window, L"EDIT", L"", WS_BORDER | WS_TABSTOP | ES_AUTOHSCROLL,
                                  982, 50, 106, 24, 311);
        if (!g_gubun) return -1;
        SendMessage(g_gubun, EM_SETLIMITTEXT, 1, 0);
        Common::AddControl(window, L"STATIC", L"틱개수", 0, 580, 83, 135, 22, 0);
        g_ncnt = Common::AddControl(window, L"EDIT", L"", WS_BORDER | WS_TABSTOP | ES_AUTOHSCROLL,
                                  717, 80, 106, 24, 312);
        if (!g_ncnt) return -1;
        SendMessage(g_ncnt, EM_SETLIMITTEXT, 4, 0);
        Common::AddControl(window, L"STATIC", L"건수", 0, 845, 83, 135, 22, 0);
        g_qrycnt = Common::AddControl(window, L"EDIT", L"", WS_BORDER | WS_TABSTOP | ES_AUTOHSCROLL,
                                  982, 80, 106, 24, 313);
        if (!g_qrycnt) return -1;
        SendMessage(g_qrycnt, EM_SETLIMITTEXT, 4, 0);
        Common::AddControl(window, L"STATIC", L"당일구분 (1바이트)", 0, 580, 113, 135, 22, 0);
        g_tdgb = Common::AddControl(window, L"EDIT", L"", WS_BORDER | WS_TABSTOP | ES_AUTOHSCROLL,
                                  717, 110, 106, 24, 314);
        if (!g_tdgb) return -1;
        SendMessage(g_tdgb, EM_SETLIMITTEXT, 1, 0);
        Common::AddControl(window, L"STATIC", L"시작일 (YYYYMMDD)", 0, 845, 113, 135, 22, 0);
        g_sdate = Common::AddControl(window, L"EDIT", L"", WS_BORDER | WS_TABSTOP | ES_AUTOHSCROLL,
                                  982, 110, 106, 24, 315);
        if (!g_sdate) return -1;
        SendMessage(g_sdate, EM_SETLIMITTEXT, 8, 0);
        Common::AddControl(window, L"STATIC", L"종료일 (YYYYMMDD)", 0, 580, 143, 135, 22, 0);
        g_edate = Common::AddControl(window, L"EDIT", L"", WS_BORDER | WS_TABSTOP | ES_AUTOHSCROLL,
                                  717, 140, 106, 24, 316);
        if (!g_edate) return -1;
        SendMessage(g_edate, EM_SETLIMITTEXT, 8, 0);
        g_queryButton = Common::AddControl(window, L"BUTTON", L"조회", WS_TABSTOP,
                                           950, 174, 140, 29, QUERY_BUTTON);
        g_output = Common::AddControl(window, L"STATIC", L"", 0, 580, 180, 365, 22, 0);
        g_list = Common::AddControl(window, WC_LISTVIEWW, L"", LVS_REPORT | LVS_SINGLESEL | WS_TABSTOP,
                                    580, 210, 540, 155, 302);
        if (!g_list) return -1;
        ListView_SetExtendedListViewStyle(g_list, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
        AddColumn(g_list, 0, L"항목 (전문 필드)", 335);
        AddColumn(g_list, 1, L"수신값", 177);
        g_detail = Common::AddControl(window, WC_LISTVIEWW, L"", LVS_REPORT | LVS_SINGLESEL | WS_TABSTOP,
                                    580, 375, 540, 205, 303);
        if (!g_detail) return -1;
        ListView_SetExtendedListViewStyle(g_detail, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
        AddColumn(g_detail, 0, L"날짜 (date)", 168);
        AddColumn(g_detail, 1, L"시간 (time)", 168);
        AddColumn(g_detail, 2, L"시가 (open)", 168);
        AddColumn(g_detail, 3, L"고가 (high)", 168);
        AddColumn(g_detail, 4, L"저가 (low)", 168);
        AddColumn(g_detail, 5, L"종가 (close)", 168);
        AddColumn(g_detail, 6, L"거래량 (jdiff_vol)", 168);
        AddColumn(g_detail, 7, L"거래대금 (value)", 168);
        AddColumn(g_detail, 8, L"수정구분 (jongchk)", 168);
        AddColumn(g_detail, 9, L"수정비율 (rate)", 168);
        AddColumn(g_detail, 10, L"수정주가반영항목 (pricechk)", 168);
        AddColumn(g_detail, 11, L"수정비율반영거래대금 (ratevalue)", 168);
        UpdateQueryButton();
        return 0;
    }
    case WM_COMMAND:
        if (LOWORD(wParam) == QUERY_BUTTON && HIWORD(wParam) == BN_CLICKED)
            RequestData(window);
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
    INITCOMMONCONTROLSEX controls = {sizeof(controls), ICC_LISTVIEW_CLASSES};
    if (!InitCommonControlsEx(&controls)) return 1;
    WNDCLASSW wc = {0};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = instance;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
    wc.lpszClassName = L"OrdaAPIs4100StockChart";
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
