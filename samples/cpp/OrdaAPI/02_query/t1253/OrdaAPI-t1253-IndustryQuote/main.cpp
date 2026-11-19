// 읽는 순서: WinMain 초기화 → 공통 로그인 통지 → 조회 요청 → 수신 → 해제 → 다음 조회 또는 종료.
// VS2010 v100 / Win32 / MultiByte / C++03. 공통 Helper는 변경하지 않습니다.
#include "../../../common/OrdaSampleCommon.h"
// 수신 블록명은 서비스 XML의 이름을 사용합니다.
#pragma warning(push)
#pragma warning(disable: 4005)
#include "t1253.h"
#pragma warning(pop)
#include <string.h>
#include <stdio.h>
#include <limits.h>
#include <string>
#include <vector>
#include <deque>
#include <commctrl.h>

namespace Common = OrdaSampleCommon;
const char SERVICE_CODE[] = NAME_t1253;
const wchar_t SERVICE_TITLE[] = L"업종현재가(t1253)";
const int QUERY_BUTTON = 300, DISPLAY_LIMIT = 200;
const int INPUT_COUNT = 1;
HWND g_inputs[INPUT_COUNT], g_queryButton, g_output, g_list;
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
    for (int i = 0; i < INPUT_COUNT; ++i) EnableWindow(g_inputs[i], idle);
}

void ClearOutput()
{
    SetWindowTextA(g_output, "");
    ListView_DeleteAllItems(g_list);
    g_recordCount = 0;
}

// 전문의 char 필드는 공백으로 채우고 코드와 날짜는 길이를 확인합니다.
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

BOOL BuildInput(t1253InBlock &input)
{
    memset(&input, ' ', sizeof(input));
    if (!CopyText(g_inputs[0], input.upcode, sizeof(input.upcode), FALSE, TRUE)) return FALSE;
    return TRUE;
}

void SetCell(int row, int column, const std::string &value)
{
    LVITEMA item = {0};
    item.mask = LVIF_TEXT;
    item.iItem = row;
    item.iSubItem = column;
    item.pszText = const_cast<char *>(value.c_str());
    if (column == 0)
        SendMessageA(g_list, LVM_INSERTITEMA, 0, (LPARAM)&item);
    else
        SendMessageA(g_list, LVM_SETITEMTEXTA, row, (LPARAM)&item);
}

// 블록 길이를 레코드 크기로 나누어 실제 건수를 계산합니다.
BOOL ReceiveRows(const unsigned char *data, int length)
{
    int count = length / sizeof(t1253OutBlock);
    if (count > INT_MAX - g_recordCount) return FALSE;
    for (int row = 0; row < count && g_recordCount + row < DISPLAY_LIMIT; ++row)
    {
        t1253OutBlock output;
        memcpy(&output, data + row * sizeof(output), sizeof(output));
        int displayRow = g_recordCount + row;
        // 전문의 부호와 소수점 표현을 유지합니다.
        SetCell(displayRow, 0, Field(output.gubun, sizeof(output.gubun)));
        SetCell(displayRow, 1, Field(output.hname, sizeof(output.hname)));
        SetCell(displayRow, 2, Field(output.pricejisu, sizeof(output.pricejisu)));
        SetCell(displayRow, 3, Field(output.jniljisu, sizeof(output.jniljisu)));
        SetCell(displayRow, 4, Field(output.sign, sizeof(output.sign)));
        SetCell(displayRow, 5, Field(output.change, sizeof(output.change)));
        SetCell(displayRow, 6, Field(output.diffjisu, sizeof(output.diffjisu)));
        SetCell(displayRow, 7, Field(output.jnilvolume, sizeof(output.jnilvolume)));
        SetCell(displayRow, 8, Field(output.volume, sizeof(output.volume)));
        SetCell(displayRow, 9, Field(output.volumechange, sizeof(output.volumechange)));
        SetCell(displayRow, 10, Field(output.volumerate, sizeof(output.volumerate)));
        SetCell(displayRow, 11, Field(output.jnilvalue, sizeof(output.jnilvalue)));
        SetCell(displayRow, 12, Field(output.value, sizeof(output.value)));
        SetCell(displayRow, 13, Field(output.valuechange, sizeof(output.valuechange)));
        SetCell(displayRow, 14, Field(output.valuerate, sizeof(output.valuerate)));
        SetCell(displayRow, 15, Field(output.openjisu, sizeof(output.openjisu)));
        SetCell(displayRow, 16, Field(output.opendiff, sizeof(output.opendiff)));
        SetCell(displayRow, 17, Field(output.opentime, sizeof(output.opentime)));
        SetCell(displayRow, 18, Field(output.highjisu, sizeof(output.highjisu)));
        SetCell(displayRow, 19, Field(output.highdiff, sizeof(output.highdiff)));
        SetCell(displayRow, 20, Field(output.hightime, sizeof(output.hightime)));
        SetCell(displayRow, 21, Field(output.lowjisu, sizeof(output.lowjisu)));
        SetCell(displayRow, 22, Field(output.lowdiff, sizeof(output.lowdiff)));
        SetCell(displayRow, 23, Field(output.lowtime, sizeof(output.lowtime)));
        SetCell(displayRow, 24, Field(output.whjisu, sizeof(output.whjisu)));
        SetCell(displayRow, 25, Field(output.whchange, sizeof(output.whchange)));
        SetCell(displayRow, 26, Field(output.whjday, sizeof(output.whjday)));
        SetCell(displayRow, 27, Field(output.wljisu, sizeof(output.wljisu)));
        SetCell(displayRow, 28, Field(output.wlchange, sizeof(output.wlchange)));
        SetCell(displayRow, 29, Field(output.wljday, sizeof(output.wljday)));
        SetCell(displayRow, 30, Field(output.yhjisu, sizeof(output.yhjisu)));
        SetCell(displayRow, 31, Field(output.yhchange, sizeof(output.yhchange)));
        SetCell(displayRow, 32, Field(output.yhjday, sizeof(output.yhjday)));
        SetCell(displayRow, 33, Field(output.yljisu, sizeof(output.yljisu)));
        SetCell(displayRow, 34, Field(output.ylchange, sizeof(output.ylchange)));
        SetCell(displayRow, 35, Field(output.yljday, sizeof(output.yljday)));
        SetCell(displayRow, 36, Field(output.firstjcode, sizeof(output.firstjcode)));
        SetCell(displayRow, 37, Field(output.firstjname, sizeof(output.firstjname)));
        SetCell(displayRow, 38, Field(output.firstjisu, sizeof(output.firstjisu)));
        SetCell(displayRow, 39, Field(output.firsign, sizeof(output.firsign)));
        SetCell(displayRow, 40, Field(output.firchange, sizeof(output.firchange)));
        SetCell(displayRow, 41, Field(output.firdiff, sizeof(output.firdiff)));
        SetCell(displayRow, 42, Field(output.secondjcode, sizeof(output.secondjcode)));
        SetCell(displayRow, 43, Field(output.secondjname, sizeof(output.secondjname)));
        SetCell(displayRow, 44, Field(output.secondjisu, sizeof(output.secondjisu)));
        SetCell(displayRow, 45, Field(output.secsign, sizeof(output.secsign)));
        SetCell(displayRow, 46, Field(output.secchange, sizeof(output.secchange)));
        SetCell(displayRow, 47, Field(output.secdiff, sizeof(output.secdiff)));
        SetCell(displayRow, 48, Field(output.thirdjcode, sizeof(output.thirdjcode)));
        SetCell(displayRow, 49, Field(output.thirdjname, sizeof(output.thirdjname)));
        SetCell(displayRow, 50, Field(output.thirdjisu, sizeof(output.thirdjisu)));
        SetCell(displayRow, 51, Field(output.thrsign, sizeof(output.thrsign)));
        SetCell(displayRow, 52, Field(output.thrchange, sizeof(output.thrchange)));
        SetCell(displayRow, 53, Field(output.thrdiff, sizeof(output.thrdiff)));
        SetCell(displayRow, 54, Field(output.fourthjcode, sizeof(output.fourthjcode)));
        SetCell(displayRow, 55, Field(output.fourthjname, sizeof(output.fourthjname)));
        SetCell(displayRow, 56, Field(output.fourthjisu, sizeof(output.fourthjisu)));
        SetCell(displayRow, 57, Field(output.forsign, sizeof(output.forsign)));
        SetCell(displayRow, 58, Field(output.forchange, sizeof(output.forchange)));
        SetCell(displayRow, 59, Field(output.fordiff, sizeof(output.fordiff)));
        SetCell(displayRow, 60, Field(output.highjo, sizeof(output.highjo)));
        SetCell(displayRow, 61, Field(output.upjo, sizeof(output.upjo)));
        SetCell(displayRow, 62, Field(output.unchgjo, sizeof(output.unchgjo)));
        SetCell(displayRow, 63, Field(output.lowjo, sizeof(output.lowjo)));
        SetCell(displayRow, 64, Field(output.downjo, sizeof(output.downjo)));
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
    return (block == "t1253OutBlock" && response->nDataLength % sizeof(t1253OutBlock) == 0);
}
BOOL ReceiveData(const RP_DATA_INFO *response)
{
    if (!response || response->nRqID != g_requestId || g_requestId < 0) return FALSE;
    if (Field(response->Header.szTrCode, sizeof(response->Header.szTrCode)) != SERVICE_CODE || !ValidBody(response)) return FALSE;
    if (response->Header.szRetFlag[0] >= '1' && response->Header.szRetFlag[0] <= '9') return FALSE;
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
    t1253InBlock input;
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

void AddColumn(int index, const wchar_t *title, int width)
{
    LVCOLUMNW column = {0};
    column.mask = LVCF_TEXT | LVCF_WIDTH;
    column.pszText = const_cast<wchar_t *>(title);
    column.cx = width;
    SendMessageW(g_list, LVM_INSERTCOLUMNW, index, (LPARAM)&column);
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
        Common::AddControl(window, L"STATIC", L"업종코드", 0, 580, 52, 150, 22, 0);
        g_inputs[0] = Common::AddControl(window, L"EDIT", L"", ES_AUTOHSCROLL | WS_TABSTOP, 735, 50, 200, 24, 400);
        SendMessage(g_inputs[0], EM_SETLIMITTEXT, 3, 0);
        g_queryButton = Common::AddControl(window, L"BUTTON", L"조회", WS_TABSTOP, 790, 184, 140, 30, QUERY_BUTTON);
        g_output = Common::AddControl(window, L"STATIC", L"", 0, 580, 222, 520, 50, 0);
        g_list = Common::AddControl(window, WC_LISTVIEWW, L"", LVS_REPORT | LVS_SINGLESEL | WS_TABSTOP,
                                    580, 280, 520, 305, 302);
        if (!g_list) return -1;
        ListView_SetExtendedListViewStyle(g_list, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
        AddColumn(0, L"업종구분", 90);
        AddColumn(1, L"업종명", 90);
        AddColumn(2, L"현재지수", 90);
        AddColumn(3, L"전일지수", 90);
        AddColumn(4, L"전일대비구분", 106);
        AddColumn(5, L"전일대비", 90);
        AddColumn(6, L"지수등락율", 92);
        AddColumn(7, L"전일거래량", 92);
        AddColumn(8, L"당일거래량", 92);
        AddColumn(9, L"거래량전일대비", 120);
        AddColumn(10, L"거래량비율", 92);
        AddColumn(11, L"전일거래대금", 106);
        AddColumn(12, L"당일거래대금", 106);
        AddColumn(13, L"거래대금전일대비", 134);
        AddColumn(14, L"거래대금비율", 106);
        AddColumn(15, L"시가지수", 90);
        AddColumn(16, L"시가등락율", 92);
        AddColumn(17, L"시가시간", 90);
        AddColumn(18, L"고가지수", 90);
        AddColumn(19, L"고가등락율", 92);
        AddColumn(20, L"고가시간", 90);
        AddColumn(21, L"저가지수", 90);
        AddColumn(22, L"저가등락율", 92);
        AddColumn(23, L"저가시간", 90);
        AddColumn(24, L"52주최고지수", 120);
        AddColumn(25, L"52주최고현재가대비", 162);
        AddColumn(26, L"52주최고지수일자", 148);
        AddColumn(27, L"52주최저지수", 120);
        AddColumn(28, L"52주최저현재가대비", 162);
        AddColumn(29, L"52주최저지수일자", 148);
        AddColumn(30, L"연중최고지수", 106);
        AddColumn(31, L"연중최고현재가대비", 148);
        AddColumn(32, L"연중최고지수일자", 134);
        AddColumn(33, L"연중최저지수", 106);
        AddColumn(34, L"연중최저현재가대비", 148);
        AddColumn(35, L"연중최저지수일자", 134);
        AddColumn(36, L"첫번째지수코드", 120);
        AddColumn(37, L"첫번째지수명", 106);
        AddColumn(38, L"첫번째지수", 92);
        AddColumn(39, L"첫번째대비구분", 120);
        AddColumn(40, L"첫번째전일대비", 120);
        AddColumn(41, L"첫번째등락율", 106);
        AddColumn(42, L"두번째지수코드", 120);
        AddColumn(43, L"두번째지수명", 106);
        AddColumn(44, L"두번째지수", 92);
        AddColumn(45, L"두번째대비구분", 120);
        AddColumn(46, L"두번째전일대비", 120);
        AddColumn(47, L"두번째등락율", 106);
        AddColumn(48, L"세번째지수코드", 120);
        AddColumn(49, L"세번째지수명", 106);
        AddColumn(50, L"세번째지수", 92);
        AddColumn(51, L"세번째대비구분", 120);
        AddColumn(52, L"세번째전일대비", 120);
        AddColumn(53, L"세번째등락율", 106);
        AddColumn(54, L"네번째지수코드", 120);
        AddColumn(55, L"네번째지수명", 106);
        AddColumn(56, L"네번째지수", 92);
        AddColumn(57, L"네번째대비구분", 120);
        AddColumn(58, L"네번째전일대비", 120);
        AddColumn(59, L"네번째등락율", 106);
        AddColumn(60, L"상승종목수", 92);
        AddColumn(61, L"상한종목수", 92);
        AddColumn(62, L"보합종목수", 92);
        AddColumn(63, L"하락종목수", 92);
        AddColumn(64, L"하한종목수", 92);
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
    wc.lpszClassName = L"OrdaAPIt1253IndustryQuote";
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
