// 읽는 순서: WinMain 초기화 → 공통 로그인 통지 → 조회 요청 → 수신 → 해제 → 다음 조회 또는 종료.
// VS2010 v100 / Win32 / MultiByte / C++03. 공통 Helper는 변경하지 않습니다.
#include "../../../common/OrdaSampleCommon.h"
// 수신 블록명은 서비스 XML의 이름을 사용합니다.
#pragma warning(push)
#pragma warning(disable: 4005)
#include "t1351.h"
#pragma warning(pop)
#include <string.h>
#include <stdio.h>
#include <limits.h>
#include <string>
#include <vector>
#include <deque>
#include <commctrl.h>

namespace Common = OrdaSampleCommon;
const char SERVICE_CODE[] = NAME_t1351;
const wchar_t SERVICE_TITLE[] = L"ETF현재가조회(t1351)";
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

BOOL BuildInput(t1351InBlock &input)
{
    memset(&input, ' ', sizeof(input));
    if (!CopyText(g_inputs[0], input.shcode, sizeof(input.shcode), FALSE, FALSE)) return FALSE;
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
    int count = length / sizeof(t1351OutBlock);
    if (count > INT_MAX - g_recordCount) return FALSE;
    for (int row = 0; row < count && g_recordCount + row < DISPLAY_LIMIT; ++row)
    {
        t1351OutBlock output;
        memcpy(&output, data + row * sizeof(output), sizeof(output));
        int displayRow = g_recordCount + row;
        // 전문의 부호와 소수점 표현을 유지합니다.
        SetCell(displayRow, 0, Field(output.hname, sizeof(output.hname)));
        SetCell(displayRow, 1, Field(output.price, sizeof(output.price)));
        SetCell(displayRow, 2, Field(output.sign, sizeof(output.sign)));
        SetCell(displayRow, 3, Field(output.change, sizeof(output.change)));
        SetCell(displayRow, 4, Field(output.diff, sizeof(output.diff)));
        SetCell(displayRow, 5, Field(output.volume, sizeof(output.volume)));
        SetCell(displayRow, 6, Field(output.recprice, sizeof(output.recprice)));
        SetCell(displayRow, 7, Field(output.avg, sizeof(output.avg)));
        SetCell(displayRow, 8, Field(output.uplmtprice, sizeof(output.uplmtprice)));
        SetCell(displayRow, 9, Field(output.dnlmtprice, sizeof(output.dnlmtprice)));
        SetCell(displayRow, 10, Field(output.jnilvolume, sizeof(output.jnilvolume)));
        SetCell(displayRow, 11, Field(output.volumediff, sizeof(output.volumediff)));
        SetCell(displayRow, 12, Field(output.open, sizeof(output.open)));
        SetCell(displayRow, 13, Field(output.opentime, sizeof(output.opentime)));
        SetCell(displayRow, 14, Field(output.high, sizeof(output.high)));
        SetCell(displayRow, 15, Field(output.hightime, sizeof(output.hightime)));
        SetCell(displayRow, 16, Field(output.low, sizeof(output.low)));
        SetCell(displayRow, 17, Field(output.lowtime, sizeof(output.lowtime)));
        SetCell(displayRow, 18, Field(output.high52w, sizeof(output.high52w)));
        SetCell(displayRow, 19, Field(output.high52wdate, sizeof(output.high52wdate)));
        SetCell(displayRow, 20, Field(output.low52w, sizeof(output.low52w)));
        SetCell(displayRow, 21, Field(output.low52wdate, sizeof(output.low52wdate)));
        SetCell(displayRow, 22, Field(output.exhratio, sizeof(output.exhratio)));
        SetCell(displayRow, 23, Field(output.flmtvol, sizeof(output.flmtvol)));
        SetCell(displayRow, 24, Field(output.per, sizeof(output.per)));
        SetCell(displayRow, 25, Field(output.listing, sizeof(output.listing)));
        SetCell(displayRow, 26, Field(output.jkrate, sizeof(output.jkrate)));
        SetCell(displayRow, 27, Field(output.vol, sizeof(output.vol)));
        SetCell(displayRow, 28, Field(output.shcode, sizeof(output.shcode)));
        SetCell(displayRow, 29, Field(output.value, sizeof(output.value)));
        SetCell(displayRow, 30, Field(output.highyear, sizeof(output.highyear)));
        SetCell(displayRow, 31, Field(output.highyeardate, sizeof(output.highyeardate)));
        SetCell(displayRow, 32, Field(output.lowyear, sizeof(output.lowyear)));
        SetCell(displayRow, 33, Field(output.lowyeardate, sizeof(output.lowyeardate)));
        SetCell(displayRow, 34, Field(output.upname, sizeof(output.upname)));
        SetCell(displayRow, 35, Field(output.upcode, sizeof(output.upcode)));
        SetCell(displayRow, 36, Field(output.upprice, sizeof(output.upprice)));
        SetCell(displayRow, 37, Field(output.upsign, sizeof(output.upsign)));
        SetCell(displayRow, 38, Field(output.upchange, sizeof(output.upchange)));
        SetCell(displayRow, 39, Field(output.updiff, sizeof(output.updiff)));
        SetCell(displayRow, 40, Field(output.futname, sizeof(output.futname)));
        SetCell(displayRow, 41, Field(output.futcode, sizeof(output.futcode)));
        SetCell(displayRow, 42, Field(output.futprice, sizeof(output.futprice)));
        SetCell(displayRow, 43, Field(output.futsign, sizeof(output.futsign)));
        SetCell(displayRow, 44, Field(output.futchange, sizeof(output.futchange)));
        SetCell(displayRow, 45, Field(output.futdiff, sizeof(output.futdiff)));
        SetCell(displayRow, 46, Field(output.nav, sizeof(output.nav)));
        SetCell(displayRow, 47, Field(output.navsign, sizeof(output.navsign)));
        SetCell(displayRow, 48, Field(output.navchange, sizeof(output.navchange)));
        SetCell(displayRow, 49, Field(output.navdiff, sizeof(output.navdiff)));
        SetCell(displayRow, 50, Field(output.cocrate, sizeof(output.cocrate)));
        SetCell(displayRow, 51, Field(output.kasis, sizeof(output.kasis)));
        SetCell(displayRow, 52, Field(output.subprice, sizeof(output.subprice)));
        SetCell(displayRow, 53, Field(output.offerno1, sizeof(output.offerno1)));
        SetCell(displayRow, 54, Field(output.bidno1, sizeof(output.bidno1)));
        SetCell(displayRow, 55, Field(output.dvol1, sizeof(output.dvol1)));
        SetCell(displayRow, 56, Field(output.svol1, sizeof(output.svol1)));
        SetCell(displayRow, 57, Field(output.dcha1, sizeof(output.dcha1)));
        SetCell(displayRow, 58, Field(output.scha1, sizeof(output.scha1)));
        SetCell(displayRow, 59, Field(output.ddiff1, sizeof(output.ddiff1)));
        SetCell(displayRow, 60, Field(output.sdiff1, sizeof(output.sdiff1)));
        SetCell(displayRow, 61, Field(output.offerno2, sizeof(output.offerno2)));
        SetCell(displayRow, 62, Field(output.bidno2, sizeof(output.bidno2)));
        SetCell(displayRow, 63, Field(output.dvol2, sizeof(output.dvol2)));
        SetCell(displayRow, 64, Field(output.svol2, sizeof(output.svol2)));
        SetCell(displayRow, 65, Field(output.dcha2, sizeof(output.dcha2)));
        SetCell(displayRow, 66, Field(output.scha2, sizeof(output.scha2)));
        SetCell(displayRow, 67, Field(output.ddiff2, sizeof(output.ddiff2)));
        SetCell(displayRow, 68, Field(output.sdiff2, sizeof(output.sdiff2)));
        SetCell(displayRow, 69, Field(output.offerno3, sizeof(output.offerno3)));
        SetCell(displayRow, 70, Field(output.bidno3, sizeof(output.bidno3)));
        SetCell(displayRow, 71, Field(output.dvol3, sizeof(output.dvol3)));
        SetCell(displayRow, 72, Field(output.svol3, sizeof(output.svol3)));
        SetCell(displayRow, 73, Field(output.dcha3, sizeof(output.dcha3)));
        SetCell(displayRow, 74, Field(output.scha3, sizeof(output.scha3)));
        SetCell(displayRow, 75, Field(output.ddiff3, sizeof(output.ddiff3)));
        SetCell(displayRow, 76, Field(output.sdiff3, sizeof(output.sdiff3)));
        SetCell(displayRow, 77, Field(output.offerno4, sizeof(output.offerno4)));
        SetCell(displayRow, 78, Field(output.bidno4, sizeof(output.bidno4)));
        SetCell(displayRow, 79, Field(output.dvol4, sizeof(output.dvol4)));
        SetCell(displayRow, 80, Field(output.svol4, sizeof(output.svol4)));
        SetCell(displayRow, 81, Field(output.dcha4, sizeof(output.dcha4)));
        SetCell(displayRow, 82, Field(output.scha4, sizeof(output.scha4)));
        SetCell(displayRow, 83, Field(output.ddiff4, sizeof(output.ddiff4)));
        SetCell(displayRow, 84, Field(output.sdiff4, sizeof(output.sdiff4)));
        SetCell(displayRow, 85, Field(output.offerno5, sizeof(output.offerno5)));
        SetCell(displayRow, 86, Field(output.bidno5, sizeof(output.bidno5)));
        SetCell(displayRow, 87, Field(output.dvol5, sizeof(output.dvol5)));
        SetCell(displayRow, 88, Field(output.svol5, sizeof(output.svol5)));
        SetCell(displayRow, 89, Field(output.dcha5, sizeof(output.dcha5)));
        SetCell(displayRow, 90, Field(output.scha5, sizeof(output.scha5)));
        SetCell(displayRow, 91, Field(output.ddiff5, sizeof(output.ddiff5)));
        SetCell(displayRow, 92, Field(output.sdiff5, sizeof(output.sdiff5)));
        SetCell(displayRow, 93, Field(output.fwdvl, sizeof(output.fwdvl)));
        SetCell(displayRow, 94, Field(output.ftradmdcha, sizeof(output.ftradmdcha)));
        SetCell(displayRow, 95, Field(output.ftradmddiff, sizeof(output.ftradmddiff)));
        SetCell(displayRow, 96, Field(output.fwsvl, sizeof(output.fwsvl)));
        SetCell(displayRow, 97, Field(output.ftradmscha, sizeof(output.ftradmscha)));
        SetCell(displayRow, 98, Field(output.ftradmsdiff, sizeof(output.ftradmsdiff)));
        SetCell(displayRow, 99, Field(output.tax, sizeof(output.tax)));
        SetCell(displayRow, 100, Field(output.etnbasnm, sizeof(output.etnbasnm)));
        SetCell(displayRow, 101, Field(output.etnnum, sizeof(output.etnnum)));
        SetCell(displayRow, 102, Field(output.etnlpqty, sizeof(output.etnlpqty)));
        SetCell(displayRow, 103, Field(output.etnissuer, sizeof(output.etnissuer)));
        SetCell(displayRow, 104, Field(output.etnjisuassetid, sizeof(output.etnjisuassetid)));
        SetCell(displayRow, 105, Field(output.etnlastdt, sizeof(output.etnlastdt)));
        SetCell(displayRow, 106, Field(output.etnpayday, sizeof(output.etnpayday)));
        SetCell(displayRow, 107, Field(output.etnprebaseprice, sizeof(output.etnprebaseprice)));
        SetCell(displayRow, 108, Field(output.etnlast_iiv, sizeof(output.etnlast_iiv)));
        SetCell(displayRow, 109, Field(output.etniiv_amt, sizeof(output.etniiv_amt)));
        SetCell(displayRow, 110, Field(output.midrfunqty, sizeof(output.midrfunqty)));
        SetCell(displayRow, 111, Field(output.midrfunfee, sizeof(output.midrfunfee)));
        SetCell(displayRow, 112, Field(output.investatt, sizeof(output.investatt)));
        SetCell(displayRow, 113, Field(output.etftrcrate, sizeof(output.etftrcrate)));
        SetCell(displayRow, 114, Field(output.ordpopup, sizeof(output.ordpopup)));
        SetCell(displayRow, 115, Field(output.operhname, sizeof(output.operhname)));
        SetCell(displayRow, 116, Field(output.main_close, sizeof(output.main_close)));
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
    return (block == "t1351OutBlock" && response->nDataLength % sizeof(t1351OutBlock) == 0);
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
    t1351InBlock input;
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
        Common::AddControl(window, L"STATIC", L"단축코드", 0, 580, 52, 150, 22, 0);
        g_inputs[0] = Common::AddControl(window, L"EDIT", L"", ES_AUTOHSCROLL | WS_TABSTOP, 735, 50, 200, 24, 400);
        SendMessage(g_inputs[0], EM_SETLIMITTEXT, 6, 0);
        g_queryButton = Common::AddControl(window, L"BUTTON", L"조회", WS_TABSTOP, 790, 184, 140, 30, QUERY_BUTTON);
        g_output = Common::AddControl(window, L"STATIC", L"", 0, 580, 222, 520, 50, 0);
        g_list = Common::AddControl(window, WC_LISTVIEWW, L"", LVS_REPORT | LVS_SINGLESEL | WS_TABSTOP,
                                    580, 280, 520, 305, 302);
        if (!g_list) return -1;
        ListView_SetExtendedListViewStyle(g_list, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
        AddColumn(0, L"한글명", 90);
        AddColumn(1, L"현재가", 90);
        AddColumn(2, L"전일대비구분", 106);
        AddColumn(3, L"전일대비", 90);
        AddColumn(4, L"등락율", 90);
        AddColumn(5, L"누적거래량", 92);
        AddColumn(6, L"기준가", 90);
        AddColumn(7, L"가중평균", 90);
        AddColumn(8, L"상한가", 90);
        AddColumn(9, L"하한가", 90);
        AddColumn(10, L"전일거래량", 92);
        AddColumn(11, L"거래량차", 90);
        AddColumn(12, L"시가", 90);
        AddColumn(13, L"시가시간", 90);
        AddColumn(14, L"고가", 90);
        AddColumn(15, L"고가시간", 90);
        AddColumn(16, L"저가", 90);
        AddColumn(17, L"저가시간", 90);
        AddColumn(18, L"52최고가", 92);
        AddColumn(19, L"52최고가일", 106);
        AddColumn(20, L"52최저가", 92);
        AddColumn(21, L"52최저가일", 106);
        AddColumn(22, L"소진율", 90);
        AddColumn(23, L"외국인보유수량", 120);
        AddColumn(24, L"PER", 90);
        AddColumn(25, L"상장주식수(천)", 134);
        AddColumn(26, L"증거금율", 90);
        AddColumn(27, L"회전율", 90);
        AddColumn(28, L"단축코드", 90);
        AddColumn(29, L"누적거래대금", 106);
        AddColumn(30, L"연중최고가", 92);
        AddColumn(31, L"연중최고일자", 106);
        AddColumn(32, L"연중최저가", 92);
        AddColumn(33, L"연중최저일자", 106);
        AddColumn(34, L"업종명", 90);
        AddColumn(35, L"업종코드", 90);
        AddColumn(36, L"업종현재가", 92);
        AddColumn(37, L"업종전일비구분", 120);
        AddColumn(38, L"업종전일대비", 106);
        AddColumn(39, L"업종등락율", 92);
        AddColumn(40, L"선물최근월물명", 120);
        AddColumn(41, L"선물최근월물코드", 134);
        AddColumn(42, L"선물현재가", 92);
        AddColumn(43, L"선물전일비구분", 120);
        AddColumn(44, L"선물전일대비", 106);
        AddColumn(45, L"선물등락율", 92);
        AddColumn(46, L"NAV", 90);
        AddColumn(47, L"NAV전일대비구분", 148);
        AddColumn(48, L"NAV전일대비", 120);
        AddColumn(49, L"NAV등락율", 106);
        AddColumn(50, L"추적오차율", 92);
        AddColumn(51, L"괴리율", 90);
        AddColumn(52, L"대용가", 90);
        AddColumn(53, L"매도증권사코드1", 134);
        AddColumn(54, L"매수증권사코드1", 134);
        AddColumn(55, L"총매도수량1", 106);
        AddColumn(56, L"총매수수량1", 106);
        AddColumn(57, L"매도증감1", 92);
        AddColumn(58, L"매수증감1", 92);
        AddColumn(59, L"매도비율1", 92);
        AddColumn(60, L"매수비율1", 92);
        AddColumn(61, L"매도증권사코드2", 134);
        AddColumn(62, L"매수증권사코드2", 134);
        AddColumn(63, L"총매도수량2", 106);
        AddColumn(64, L"총매수수량2", 106);
        AddColumn(65, L"매도증감2", 92);
        AddColumn(66, L"매수증감2", 92);
        AddColumn(67, L"매도비율2", 92);
        AddColumn(68, L"매수비율2", 92);
        AddColumn(69, L"매도증권사코드3", 134);
        AddColumn(70, L"매수증권사코드3", 134);
        AddColumn(71, L"총매도수량3", 106);
        AddColumn(72, L"총매수수량3", 106);
        AddColumn(73, L"매도증감3", 92);
        AddColumn(74, L"매수증감3", 92);
        AddColumn(75, L"매도비율3", 92);
        AddColumn(76, L"매수비율3", 92);
        AddColumn(77, L"매도증권사코드4", 134);
        AddColumn(78, L"매수증권사코드4", 134);
        AddColumn(79, L"총매도수량4", 106);
        AddColumn(80, L"총매수수량4", 106);
        AddColumn(81, L"매도증감4", 92);
        AddColumn(82, L"매수증감4", 92);
        AddColumn(83, L"매도비율4", 92);
        AddColumn(84, L"매수비율4", 92);
        AddColumn(85, L"매도증권사코드5", 134);
        AddColumn(86, L"매수증권사코드5", 134);
        AddColumn(87, L"총매도수량5", 106);
        AddColumn(88, L"총매수수량5", 106);
        AddColumn(89, L"매도증감5", 92);
        AddColumn(90, L"매수증감5", 92);
        AddColumn(91, L"매도비율5", 92);
        AddColumn(92, L"매수비율5", 92);
        AddColumn(93, L"외국계매도합계수량", 148);
        AddColumn(94, L"외국계매도직전대비", 148);
        AddColumn(95, L"외국계매도비율", 120);
        AddColumn(96, L"외국계매수합계수량", 148);
        AddColumn(97, L"외국계매수직전대비", 148);
        AddColumn(98, L"외국계매수비율", 120);
        AddColumn(99, L"세금종류", 90);
        AddColumn(100, L"ETN대상지수", 120);
        AddColumn(101, L"ETN구성종목수", 134);
        AddColumn(102, L"ETNLP보유수량", 148);
        AddColumn(103, L"ETN발행사", 106);
        AddColumn(104, L"ETN자산분류", 120);
        AddColumn(105, L"ETN최종거래일", 134);
        AddColumn(106, L"ETN지급일", 106);
        AddColumn(107, L"ETN과표기준가", 134);
        AddColumn(108, L"ETN지표가치", 120);
        AddColumn(109, L"ETN지표가치금액", 148);
        AddColumn(110, L"중도상환최소청구수량", 162);
        AddColumn(111, L"중도상환수수료율", 134);
        AddColumn(112, L"투자유의구분Y/N", 148);
        AddColumn(113, L"추적수익율배수", 120);
        AddColumn(114, L"투자유의주문팝업", 134);
        AddColumn(115, L"운용사명", 90);
        AddColumn(116, L"KRX종가", 92);
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
    wc.lpszClassName = L"OrdaAPIt1351ETFQuote";
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
