// 읽는 순서: WinMain 초기화 → 공통 로그인 통지 → 조회 요청 → 수신 → 해제 → 다음 조회 또는 종료.
// VS2010 v100 / Win32 / MultiByte / C++03. 공통 Helper는 변경하지 않습니다.
#include "../../../common/OrdaSampleCommon.h"
// 수신 블록명은 서비스 XML의 이름을 사용합니다.
#pragma warning(push)
#pragma warning(disable: 4005)
#include "t2082.h"
#pragma warning(pop)
#include <string.h>
#include <stdio.h>
#include <limits.h>
#include <string>
#include <vector>
#include <deque>
#include <commctrl.h>
#include <set>

namespace Common = OrdaSampleCommon;
const char SERVICE_CODE[] = NAME_t2082;
const wchar_t SERVICE_TITLE[] = L"종목별 상위회원사(t2082)";
const int QUERY_BUTTON = 300, NEXT_BUTTON = 301, DISPLAY_LIMIT = 200;
const int INPUT_COUNT = 5;
HWND g_inputs[INPUT_COUNT], g_queryButton, g_nextButton, g_output, g_list, g_fixedList;
int g_requestId = -1, g_recordCount = 0;
BOOL g_submitting = FALSE, g_processing = FALSE, g_closing = FALSE;
BOOL g_closeRequested = FALSE, g_queryError = FALSE;
BOOL g_serverMessageReceived = FALSE;
BOOL g_moreFlag = FALSE, g_hasNext = FALSE, g_fixedReceived = FALSE;
std::string g_cursor;
std::set<std::string> g_usedCursors;
// 비밀번호를 저장하지 않고 조건 변경 통지로 연속 상태를 초기화합니다.
void ResetContinuation()
{
    g_hasNext = FALSE;
    g_cursor.clear();
    g_usedCursors.clear();
}


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
    EnableWindow(g_nextButton, idle && Common::IsReady() && g_hasNext);
    for (int i = 0; i < INPUT_COUNT; ++i) EnableWindow(g_inputs[i], idle);
}

void ClearOutput()
{
    SetWindowTextA(g_output, "");
    ListView_DeleteAllItems(g_list);
    ListView_DeleteAllItems(g_fixedList);
    g_recordCount = 0;
}

// 전문의 char 필드는 공백으로 채우고 코드와 날짜는 길이를 확인합니다.
BOOL CopyCode(HWND control, char &field)
{
    LRESULT selected = SendMessage(control, CB_GETCURSEL, 0, 0);
    if (selected == CB_ERR) return FALSE;
    LRESULT value = SendMessage(control, CB_GETITEMDATA, selected, 0);
    if (value == CB_ERR) return FALSE;
    field = (char)value;
    return TRUE;
}

BOOL CopyText(HWND control, char *field, int width, BOOL optional, BOOL digitsOnly)
{
    char value[32] = {0};
    int length = GetWindowTextLengthA(control);
    if (length == 0) return optional;
    if (length != width || GetWindowTextA(control, value, sizeof(value)) != length)
        return FALSE;
    for (int i = 0; i < length; ++i)
    {
        char ch = value[i];
        if (!(ch >= '0' && ch <= '9') && (digitsOnly || !(ch >= 'A' && ch <= 'Z')))
            return FALSE;
    }
    memcpy(field, value, length);
    return TRUE;
}

// 8자리 YYYYMMDD의 달력 날짜를 확인합니다.
BOOL ValidDate(const char *value)
{
    SYSTEMTIME date = {0};
    for (int i = 0; i < 4; ++i) date.wYear = (WORD)(date.wYear * 10 + value[i] - '0');
    date.wMonth = (WORD)((value[4] - '0') * 10 + value[5] - '0');
    date.wDay = (WORD)((value[6] - '0') * 10 + value[7] - '0');
    FILETIME checked;
    return SystemTimeToFileTime(&date, &checked);
}

BOOL BuildInput(t2082InBlock &input, BOOL next = FALSE)
{
    // 속성 바이트와 전문에서 공백이 기본값인 필드를 함께 초기화합니다.
    memset(&input, ' ', sizeof(input));
    if (!CopyText(g_inputs[0], input.shcode, sizeof(input.shcode), FALSE, FALSE)) return FALSE;
    if (!CopyText(g_inputs[1], input.traddate1, sizeof(input.traddate1), FALSE, TRUE)) return FALSE;
    if (!CopyText(g_inputs[2], input.traddate2, sizeof(input.traddate2), FALSE, TRUE)) return FALSE;
    if (!CopyCode(g_inputs[3], input.fwgubun1[0])) return FALSE;
    if (!CopyCode(g_inputs[4], input.exchange[0])) return FALSE;
    if (!ValidDate(input.traddate1) || !ValidDate(input.traddate2) ||
        memcmp(input.traddate1, input.traddate2, sizeof(input.traddate1)) > 0) return FALSE;
    // 다음 조회에만 응답 본문 키를 원래 길이대로 복사합니다.
    if (next)
    {
        if (!g_hasNext) return FALSE;
        memcpy(input.cts_idx, g_cursor.data(), sizeof(input.cts_idx));
    }
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

void ReceiveFixed(const t2082OutBlock &fixed)
{
    std::string cursor = std::string(fixed.cts_idx, sizeof(fixed.cts_idx));
    if (g_fixedReceived && cursor != g_cursor) g_queryError = TRUE;
    g_cursor = cursor;
    g_fixedReceived = TRUE;
    ListView_DeleteAllItems(g_fixedList);
    SetCell(g_fixedList, 0, 0, "외국계매도");
    SetCell(g_fixedList, 0, 1, Field(fixed.fwdvl, sizeof(fixed.fwdvl)));
    SetCell(g_fixedList, 1, 0, "외국계매수");
    SetCell(g_fixedList, 1, 1, Field(fixed.fwsvl, sizeof(fixed.fwsvl)));
    SetCell(g_fixedList, 2, 0, "CTSIDX");
    SetCell(g_fixedList, 2, 1, Field(fixed.cts_idx, sizeof(fixed.cts_idx)));
}

// 블록 길이를 레코드 크기로 나누어 실제 건수를 계산합니다.
BOOL ReceiveRows(const unsigned char *data, int length)
{
    int count = length / sizeof(t2082OutBlock1);
    if (count > INT_MAX - g_recordCount) return FALSE;
    for (int row = 0; row < count && g_recordCount + row < DISPLAY_LIMIT; ++row)
    {
        t2082OutBlock1 output;
        memcpy(&output, data + row * sizeof(output), sizeof(output));
        int displayRow = g_recordCount + row;
        // 전문의 부호와 소수점 표현을 유지합니다.
        SetCell(g_list, displayRow, 0, Field(output.tradname, sizeof(output.tradname)));
        SetCell(g_list, displayRow, 1, Field(output.tradmdvol, sizeof(output.tradmdvol)));
        SetCell(g_list, displayRow, 2, Field(output.tradmsvol, sizeof(output.tradmsvol)));
        SetCell(g_list, displayRow, 3, Field(output.tradmssvol, sizeof(output.tradmssvol)));
        SetCell(g_list, displayRow, 4, Field(output.wintrd, sizeof(output.wintrd)));
        SetCell(g_list, displayRow, 5, Field(output.winrat, sizeof(output.winrat)));
        SetCell(g_list, displayRow, 6, Field(output.tradno, sizeof(output.tradno)));
        SetCell(g_list, displayRow, 7, Field(output.wgubun, sizeof(output.wgubun)));
        SetCell(g_list, displayRow, 8, Field(output.swinrat, sizeof(output.swinrat)));
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

// SDK가 조립한 A형 통지는 블록 이름으로 구분합니다.
BOOL ValidBody(const RP_DATA_INFO *response)
{
    if (!response || response->nDataLength < 0 ||
        response->nDataLength > response->nTotalDataBufferSize ||
        (!response->lpData && response->nDataLength > 0)) return FALSE;
    std::string block = Field(response->szBlockName, sizeof(response->szBlockName));
    return (block == "t2082OutBlock" && response->nDataLength == sizeof(t2082OutBlock)) ||
           (block == "t2082OutBlock1" && response->nDataLength % sizeof(t2082OutBlock1) == 0);
}
BOOL ReceiveData(const RP_DATA_INFO *response)
{
    if (!response || response->nRqID != g_requestId || g_requestId < 0) return FALSE;
    if (Field(response->Header.szTrCode, sizeof(response->Header.szTrCode)) != SERVICE_CODE || !ValidBody(response)) return FALSE;
    if (response->Header.szRetFlag[0] >= '1' && response->Header.szRetFlag[0] <= '9') return FALSE;
    if (Field(response->szBlockName, sizeof(response->szBlockName)) == "t2082OutBlock")
    {
        g_moreFlag = response->Header.szPNFlag[0] == '1';
        t2082OutBlock fixed;
        memcpy(&fixed, response->lpData, sizeof(fixed));
        ReceiveFixed(fixed);
        return TRUE;
    }
    return ReceiveRows(response->lpData, response->nDataLength);
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
        if (!g_fixedReceived && !g_queryError)
        {
            g_queryError = TRUE;
            Common::SetResult(L"고정 출력이 누락되었습니다.");
        }
        g_hasNext = !g_queryError && g_fixedReceived && g_moreFlag &&
                    !Field(g_cursor.data(), (int)g_cursor.size()).empty() &&
                    g_usedCursors.count(g_cursor) == 0;
        ShowOutput();
        if (!g_queryError && !g_serverMessageReceived)
            Common::SetResult(g_hasNext ? L"조회 완료. 다음 조회가 가능합니다." : L"조회 완료.");
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
void RequestData(HWND window, BOOL next = FALSE)
{
    if (g_closing || g_submitting || g_processing || Common::IsBusy() || g_requestId >= 0)
        return;
    if (!Common::IsReady())
    {
        Common::SetResult(L"로그인이 필요합니다.");
        return;
    }
    t2082InBlock input;
    if (!BuildInput(input, next))
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
    if (next)
        g_usedCursors.insert(g_cursor);
    else
        g_usedCursors.clear();
    g_hasNext = FALSE;
    g_cursor.clear();
    g_fixedReceived = FALSE;
    g_moreFlag = FALSE;
    ClearOutput();
    g_queryError = FALSE;
    g_serverMessageReceived = FALSE;
    // A형은 5번째 인자가 있으면 헤더의 연속 플래그를 설정합니다. 키는 본문 cts_idx입니다.
    int result = next
        ? Common::Api()->RequestData(window, NAME_t2082, &input, sizeof(input), "")
        : Common::Api()->RequestData(window, NAME_t2082, &input, sizeof(input));
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

void AddChoice(HWND control, const wchar_t *label, char value, char defaultValue)
{
    LRESULT index = SendMessageW(control, CB_ADDSTRING, 0, (LPARAM)label);
    if (index < 0) return;
    SendMessage(control, CB_SETITEMDATA, index, value);
    if (value == defaultValue) SendMessage(control, CB_SETCURSEL, index, 0);
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
        Common::AddControl(window, L"STATIC", L"종목코드", 0, 580, 52, 155, 22, 0);
        g_inputs[0] = Common::AddControl(window, L"EDIT", L"", ES_AUTOHSCROLL | WS_TABSTOP, 727, 50, 113, 24, 400);
        SendMessage(g_inputs[0], EM_SETLIMITTEXT, 6, 0);
        Common::AddControl(window, L"STATIC", L"조회날짜1 YYYYMMDD", 0, 580, 84, 155, 22, 0);
        g_inputs[1] = Common::AddControl(window, L"EDIT", L"", ES_AUTOHSCROLL | WS_TABSTOP, 727, 82, 113, 24, 401);
        SendMessage(g_inputs[1], EM_SETLIMITTEXT, 8, 0);
        Common::AddControl(window, L"STATIC", L"조회날짜2 YYYYMMDD", 0, 580, 116, 155, 22, 0);
        g_inputs[2] = Common::AddControl(window, L"EDIT", L"", ES_AUTOHSCROLL | WS_TABSTOP, 727, 114, 113, 24, 402);
        SendMessage(g_inputs[2], EM_SETLIMITTEXT, 8, 0);
        Common::AddControl(window, L"STATIC", L"외국계구분", 0, 580, 148, 155, 22, 0);
        g_inputs[3] = Common::AddControl(window, L"COMBOBOX", L"", CBS_DROPDOWNLIST | WS_TABSTOP, 727, 146, 113, 150, 403);
        SendMessage(g_inputs[3], CB_SETDROPPEDWIDTH, 180, 0);
        AddChoice(g_inputs[3], L"0 미적용", '0', '0');
        AddChoice(g_inputs[3], L"1 적용", '1', '0');
        Common::AddControl(window, L"STATIC", L"거래소구분", 0, 845, 52, 155, 22, 0);
        g_inputs[4] = Common::AddControl(window, L"COMBOBOX", L"", CBS_DROPDOWNLIST | WS_TABSTOP, 992, 50, 113, 150, 404);
        SendMessage(g_inputs[4], CB_SETDROPPEDWIDTH, 180, 0);
        AddChoice(g_inputs[4], L"1 KRX", '1', '3');
        AddChoice(g_inputs[4], L"2 NXT", '2', '3');
        AddChoice(g_inputs[4], L"3 통합", '3', '3');
        g_queryButton = Common::AddControl(window, L"BUTTON", L"처음 조회", WS_TABSTOP, 790, 182, 140, 30, QUERY_BUTTON);
        g_nextButton = Common::AddControl(window, L"BUTTON", L"다음 조회", WS_TABSTOP, 940, 182, 140, 30, NEXT_BUTTON);

        g_fixedList = Common::AddControl(window, WC_LISTVIEWW, L"", LVS_REPORT | LVS_SINGLESEL | WS_TABSTOP,
                                        580, 222, 540, 120, 303);
        if (!g_fixedList) return -1;
        ListView_SetExtendedListViewStyle(g_fixedList, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
        AddColumn(g_fixedList, 0, L"고정 출력 항목", 180);
        AddColumn(g_fixedList, 1, L"값", 330);
        g_output = Common::AddControl(window, L"STATIC", L"", 0, 580, 352, 540, 22, 0);
        g_list = Common::AddControl(window, WC_LISTVIEWW, L"", LVS_REPORT | LVS_SINGLESEL | WS_TABSTOP,
                                    580, 378, 540, 210, 302);
        if (!g_list) return -1;
        ListView_SetExtendedListViewStyle(g_list, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
        AddColumn(g_list, 0, L"회원사", 90);
        AddColumn(g_list, 1, L"매도수량", 90);
        AddColumn(g_list, 2, L"매수수량", 90);
        AddColumn(g_list, 3, L"순매수", 90);
        AddColumn(g_list, 4, L"창구거래", 90);
        AddColumn(g_list, 5, L"비중", 90);
        AddColumn(g_list, 6, L"회원사코드", 92);
        AddColumn(g_list, 7, L"외국계여부", 92);
        AddColumn(g_list, 8, L"순비중", 90);
        UpdateQueryButton();
        return 0;
    }
    case WM_COMMAND:
        if (LOWORD(wParam) == QUERY_BUTTON && HIWORD(wParam) == BN_CLICKED)
            RequestData(window);
        if (LOWORD(wParam) == NEXT_BUTTON && HIWORD(wParam) == BN_CLICKED)
            RequestData(window, TRUE);
        if (LOWORD(wParam) >= 400 && LOWORD(wParam) < 500 &&
            (HIWORD(wParam) == EN_CHANGE || HIWORD(wParam) == CBN_SELCHANGE))
        {
            ResetContinuation();
            UpdateQueryButton();
        }
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
    wc.lpszClassName = L"OrdaAPIt2082TopStockBrokers";
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
