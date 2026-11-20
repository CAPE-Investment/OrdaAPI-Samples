// 읽는 순서: WinMain 초기화 → 공통 로그인 통지 → 조회 요청 → 수신 → 해제 → 다음 조회 또는 종료.
// VS2010 v100 / Win32 / MultiByte / C++03. 공통 Helper는 변경하지 않습니다.
#include "../../../common/OrdaSampleCommon.h"
// 수신 블록명은 서비스 XML의 이름을 사용합니다.
#include "s2106.h"
#include <string.h>
#include <stdio.h>
#include <limits.h>
#include <string>
#include <vector>
#include <deque>
#include <commctrl.h>

namespace Common = OrdaSampleCommon;
const char SERVICE_CODE[] = NAME_s2106;
const wchar_t SERVICE_TITLE[] = L"주식선물투자자 시간별 동향(s2106)";
const int QUERY_BUTTON = 300, DISPLAY_LIMIT = 200;
HWND g_queryButton, g_output, g_list;
HWND g_code;
HWND g_flag, g_unit, g_day, g_totals, g_cont;
int g_requestId = -1, g_recordCount = 0;
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

// 문자열 코드는 변환 없이 공백으로 초기화된 요청 필드에 복사합니다.
BOOL ReadInput(HWND edit, char *field, size_t width, BOOL required)
{
    char value[32];
    int length = GetWindowTextA(edit, value, sizeof(value));
    if (GetWindowTextLengthA(edit) > (int)width || length > (int)width ||
        (required && strspn(value, " ") == (size_t)length))
        return FALSE;
    memcpy(field, value, length);
    return TRUE;
}

HWND AddChoice(HWND window, int x, int id, const wchar_t *first,
               const wchar_t *second, const wchar_t *third, int selection)
{
    HWND combo = Common::AddControl(window, L"COMBOBOX", L"", CBS_DROPDOWNLIST | WS_TABSTOP,
                                    x, 81, 175, 120, id);
    SendMessageW(combo, CB_ADDSTRING, 0, (LPARAM)first);
    SendMessageW(combo, CB_ADDSTRING, 0, (LPARAM)second);
    if (third) SendMessageW(combo, CB_ADDSTRING, 0, (LPARAM)third);
    SendMessage(combo, CB_SETCURSEL, selection, 0);
    return combo;
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
    ListView_DeleteAllItems(g_totals);
    SetWindowTextA(g_cont, "");
}

BOOL BuildInput(s2106InBlock &input)
{
    // 미입력 문자 필드와 속성 바이트의 초기값은 공백입니다.
    memset(&input, ' ', sizeof(input));
    if (!ReadInput(g_code, input.eitem, sizeof(input.eitem), TRUE)) return FALSE;
    int flag = (int)SendMessage(g_flag, CB_GETCURSEL, 0, 0);
    if (flag < 0 || flag > 1) return FALSE;
    input.flag[0] = char('1' + flag);
    int unit = (int)SendMessage(g_unit, CB_GETCURSEL, 0, 0);
    if (unit < 0 || unit > 1) return FALSE;
    input.unit[0] = char('1' + unit);
    int jnilflag = (int)SendMessage(g_day, CB_GETCURSEL, 0, 0);
    if (jnilflag < 0 || jnilflag > 1) return FALSE;
    input.jnilflag[0] = char('1' + jnilflag);
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

// 블록을 독립적으로 검증하고 반복 건수는 바이트 길이에서 계산합니다.
BOOL ValidBody(const RP_DATA_INFO *response)
{
    if (!response || response->nDataLength < 0 ||
        response->nDataLength > response->nTotalDataBufferSize ||
        (!response->lpData && response->nDataLength != 0))
        return FALSE;
    std::string block = Field(response->szBlockName, sizeof(response->szBlockName));
    if (block == NAME_s2106OutBlock) return response->nDataLength == sizeof(s2106OutBlock);
    return block == NAME_s2106OutBlock1 && response->nDataLength % sizeof(s2106OutBlock1) == 0;
}

BOOL ReceiveData(const RP_DATA_INFO *response)
{
    if (!response || response->nRqID != g_requestId || g_requestId < 0 || !ValidBody(response)) return FALSE;
    if (Field(response->Header.szTrCode, sizeof(response->Header.szTrCode)) != SERVICE_CODE) return FALSE;
    if (response->Header.szRetFlag[0] >= '1' && response->Header.szRetFlag[0] <= '9') return FALSE;
    std::string block = Field(response->szBlockName, sizeof(response->szBlockName));
    if (block == NAME_s2106OutBlock)
    {
        s2106OutBlock output;
        memcpy(&output, response->lpData, sizeof(output));
        ListView_DeleteAllItems(g_totals);
        SetCell(g_totals, 0, 0, "사모");
        SetCell(g_totals, 0, 1, WireField(output.val00md, sizeof(output.val00md)));
        SetCell(g_totals, 0, 2, WireField(output.val00ms, sizeof(output.val00ms)));
        SetCell(g_totals, 0, 3, WireField(output.val00ss, sizeof(output.val00ss)));
        SetCell(g_totals, 1, 0, "금투");
        SetCell(g_totals, 1, 1, WireField(output.val01md, sizeof(output.val01md)));
        SetCell(g_totals, 1, 2, WireField(output.val01ms, sizeof(output.val01ms)));
        SetCell(g_totals, 1, 3, WireField(output.val01ss, sizeof(output.val01ss)));
        SetCell(g_totals, 2, 0, "보험");
        SetCell(g_totals, 2, 1, WireField(output.val02md, sizeof(output.val02md)));
        SetCell(g_totals, 2, 2, WireField(output.val02ms, sizeof(output.val02ms)));
        SetCell(g_totals, 2, 3, WireField(output.val02ss, sizeof(output.val02ss)));
        SetCell(g_totals, 3, 0, "투신");
        SetCell(g_totals, 3, 1, WireField(output.val03md, sizeof(output.val03md)));
        SetCell(g_totals, 3, 2, WireField(output.val03ms, sizeof(output.val03ms)));
        SetCell(g_totals, 3, 3, WireField(output.val03ss, sizeof(output.val03ss)));
        SetCell(g_totals, 4, 0, "은행");
        SetCell(g_totals, 4, 1, WireField(output.val04md, sizeof(output.val04md)));
        SetCell(g_totals, 4, 2, WireField(output.val04ms, sizeof(output.val04ms)));
        SetCell(g_totals, 4, 3, WireField(output.val04ss, sizeof(output.val04ss)));
        SetCell(g_totals, 5, 0, "기타금융");
        SetCell(g_totals, 5, 1, WireField(output.val05md, sizeof(output.val05md)));
        SetCell(g_totals, 5, 2, WireField(output.val05ms, sizeof(output.val05ms)));
        SetCell(g_totals, 5, 3, WireField(output.val05ss, sizeof(output.val05ss)));
        SetCell(g_totals, 6, 0, "연기금");
        SetCell(g_totals, 6, 1, WireField(output.val06md, sizeof(output.val06md)));
        SetCell(g_totals, 6, 2, WireField(output.val06ms, sizeof(output.val06ms)));
        SetCell(g_totals, 6, 3, WireField(output.val06ss, sizeof(output.val06ss)));
        SetCell(g_totals, 7, 0, "기타");
        SetCell(g_totals, 7, 1, WireField(output.val07md, sizeof(output.val07md)));
        SetCell(g_totals, 7, 2, WireField(output.val07ms, sizeof(output.val07ms)));
        SetCell(g_totals, 7, 3, WireField(output.val07ss, sizeof(output.val07ss)));
        SetCell(g_totals, 8, 0, "개인");
        SetCell(g_totals, 8, 1, WireField(output.val08md, sizeof(output.val08md)));
        SetCell(g_totals, 8, 2, WireField(output.val08ms, sizeof(output.val08ms)));
        SetCell(g_totals, 8, 3, WireField(output.val08ss, sizeof(output.val08ss)));
        SetCell(g_totals, 9, 0, "외국인");
        SetCell(g_totals, 9, 1, WireField(output.val09md, sizeof(output.val09md)));
        SetCell(g_totals, 9, 2, WireField(output.val09ms, sizeof(output.val09ms)));
        SetCell(g_totals, 9, 3, WireField(output.val09ss, sizeof(output.val09ss)));
        SetCell(g_totals, 10, 0, "기타외국인");
        SetCell(g_totals, 10, 1, WireField(output.val10md, sizeof(output.val10md)));
        SetCell(g_totals, 10, 2, WireField(output.val10ms, sizeof(output.val10ms)));
        SetCell(g_totals, 10, 3, WireField(output.val10ss, sizeof(output.val10ss)));
        SetCell(g_totals, 11, 0, "국가");
        SetCell(g_totals, 11, 1, WireField(output.val11md, sizeof(output.val11md)));
        SetCell(g_totals, 11, 2, WireField(output.val11ms, sizeof(output.val11ms)));
        SetCell(g_totals, 11, 3, WireField(output.val11ss, sizeof(output.val11ss)));
        SetCell(g_totals, 12, 0, "기관");
        SetCell(g_totals, 12, 1, WireField(output.valOrmd, sizeof(output.valOrmd)));
        SetCell(g_totals, 12, 2, WireField(output.valOrms, sizeof(output.valOrms)));
        SetCell(g_totals, 12, 3, WireField(output.valOrss, sizeof(output.valOrss)));
        SetCell(g_totals, 13, 0, "전체");
        SetCell(g_totals, 13, 1, WireField(output.val19md, sizeof(output.val19md)));
        SetCell(g_totals, 13, 2, WireField(output.val19ms, sizeof(output.val19ms)));
        SetCell(g_totals, 13, 3, WireField(output.val19ss, sizeof(output.val19ss)));
        SetWindowTextA(g_cont, ("cont: " + Field(output.cont, sizeof(output.cont))).c_str());
        return TRUE;
    }
    int count = response->nDataLength / sizeof(s2106OutBlock1);
    if (count > INT_MAX - g_recordCount) return FALSE;
    for (int row = 0; row < count && g_recordCount + row < DISPLAY_LIMIT; ++row)
    {
        s2106OutBlock1 output;
        memcpy(&output, response->lpData + row * sizeof(output), sizeof(output));
        int displayRow = g_recordCount + row;
        SetCell(g_list, displayRow, 0, WireField(output.date, sizeof(output.date)));
        SetCell(g_list, displayRow, 1, WireField(output.time, sizeof(output.time)));
        SetCell(g_list, displayRow, 2, WireField(output.val00, sizeof(output.val00)));
        SetCell(g_list, displayRow, 3, WireField(output.val01, sizeof(output.val01)));
        SetCell(g_list, displayRow, 4, WireField(output.val02, sizeof(output.val02)));
        SetCell(g_list, displayRow, 5, WireField(output.val03, sizeof(output.val03)));
        SetCell(g_list, displayRow, 6, WireField(output.val04, sizeof(output.val04)));
        SetCell(g_list, displayRow, 7, WireField(output.val05, sizeof(output.val05)));
        SetCell(g_list, displayRow, 8, WireField(output.val06, sizeof(output.val06)));
        SetCell(g_list, displayRow, 9, WireField(output.val07, sizeof(output.val07)));
        SetCell(g_list, displayRow, 10, WireField(output.val08, sizeof(output.val08)));
        SetCell(g_list, displayRow, 11, WireField(output.val09, sizeof(output.val09)));
        SetCell(g_list, displayRow, 12, WireField(output.val10, sizeof(output.val10)));
        SetCell(g_list, displayRow, 13, WireField(output.val11, sizeof(output.val11)));
        SetCell(g_list, displayRow, 14, WireField(output.valOr, sizeof(output.valOr)));
        SetCell(g_list, displayRow, 15, WireField(output.val19, sizeof(output.val19)));
    }
    g_recordCount += count;
    return TRUE;
}

void ShowOutput()
{
    char heading[128];
    sprintf_s(heading, "수신 %d건 / 표시 %d건", g_recordCount, ListView_GetItemCount(g_list));
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
    s2106InBlock input;
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
        Common::AddControl(window, L"STATIC", L"기초자산코드", 0, 580, 53, 100, 22, 0);
        g_code = Common::AddControl(window, L"EDIT", L"", WS_BORDER | WS_TABSTOP | ES_AUTOHSCROLL, 685, 49, 245, 24, 303);
        SendMessage(g_code, EM_SETLIMITTEXT, 10, 0);
        g_flag = AddChoice(window, 580, 304, L"1 금액", L"2 수량", NULL, 0);
        g_unit = AddChoice(window, 760, 305, L"1 백만원", L"2 억원", NULL, 0);
        g_day = AddChoice(window, 940, 306, L"1 당일", L"2 전일", NULL, 0);
        Common::AddControl(window, L"STATIC", L"투자자별 합계", 0, 580, 115, 140, 22, 0);
        g_cont = Common::AddControl(window, L"STATIC", L"", 0, 730, 115, 390, 22, 0);
        g_totals = Common::AddControl(window, WC_LISTVIEWW, L"", LVS_REPORT | WS_TABSTOP, 580, 140, 540, 190, 307);
        ListView_SetExtendedListViewStyle(g_totals, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
        AddColumn(g_totals, 0, L"투자자", 125);
        AddColumn(g_totals, 1, L"매도", 125);
        AddColumn(g_totals, 2, L"매수", 125);
        AddColumn(g_totals, 3, L"순매수", 125);
        Common::AddControl(window, L"STATIC", L"시간별 상세", 0, 580, 340, 140, 22, 0);
        g_queryButton = Common::AddControl(window, L"BUTTON", L"조회", WS_TABSTOP, 950, 48, 140, 29, QUERY_BUTTON);
        g_output = Common::AddControl(window, L"STATIC", L"", 0, 730, 340, 390, 22, 0);
        g_list = Common::AddControl(window, WC_LISTVIEWW, L"", LVS_REPORT | LVS_SINGLESEL | WS_TABSTOP,
                                    580, 366, 540, 214, 302);
        if (!g_list) return -1;
        ListView_SetExtendedListViewStyle(g_list, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
        AddColumn(g_list, 0, L"일자", 105);
        AddColumn(g_list, 1, L"시간", 105);
        AddColumn(g_list, 2, L"사모", 105);
        AddColumn(g_list, 3, L"금투", 105);
        AddColumn(g_list, 4, L"보험", 105);
        AddColumn(g_list, 5, L"투신", 105);
        AddColumn(g_list, 6, L"은행", 105);
        AddColumn(g_list, 7, L"기타금융", 105);
        AddColumn(g_list, 8, L"연기금", 105);
        AddColumn(g_list, 9, L"기타", 105);
        AddColumn(g_list, 10, L"개인", 105);
        AddColumn(g_list, 11, L"외국인", 105);
        AddColumn(g_list, 12, L"기타외국인", 105);
        AddColumn(g_list, 13, L"국가", 105);
        AddColumn(g_list, 14, L"기관", 105);
        AddColumn(g_list, 15, L"전체", 105);
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
    wc.lpszClassName = L"OrdaAPIs2106StockFuturesInvestorTrends";
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
