// 읽는 순서: WinMain 초기화 → 공통 로그인 통지 → 조회 요청 → 수신 → 해제 → 다음 조회 또는 종료.
// VS2010 v100 / Win32 / MultiByte / C++03. 공통 Helper는 변경하지 않습니다.
#include "../../../common/OrdaSampleCommon.h"
// 수신 블록명은 서비스 XML의 이름을 사용합니다.
#include "s3010.h"
#include <string.h>
#include <stdio.h>
#include <limits.h>
#include <string>
#include <vector>
#include <deque>
#include <commctrl.h>

namespace Common = OrdaSampleCommon;
const char SERVICE_CODE[] = NAME_s3010;
const wchar_t SERVICE_TITLE[] = L"주식선물 현재가 조회(s3010)";
const int QUERY_BUTTON = 300;
HWND g_queryButton, g_output, g_list;

HWND g_shcode;
HWND g_detail;
int g_requestId = -1;
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

BOOL BuildInput(s3010InBlock &input)
{
    // 선택 항목, 속성 바이트 및 최초 조회 연속키를 공백으로 초기화합니다.
    memset(&input, ' ', sizeof(input));
    if (!ReadCode(g_shcode, input.shcode, sizeof(input.shcode))) return FALSE;
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
    if (block == NAME_s3010OutBlock) return response->nDataLength == sizeof(s3010OutBlock);
    return FALSE;
}

BOOL ReceiveData(const RP_DATA_INFO *response)
{
    if (!response || response->nRqID != g_requestId || g_requestId < 0 || !ValidBody(response))
        return FALSE;
    if (Field(response->Header.szTrCode, sizeof(response->Header.szTrCode)) != SERVICE_CODE)
        return FALSE;
    if (response->Header.szRetFlag[0] >= '1' && response->Header.szRetFlag[0] <= '9')
        return FALSE;
    if (Field(response->szBlockName, sizeof(response->szBlockName)) == NAME_s3010OutBlock)
    {
        s3010OutBlock output;
        memcpy(&output, response->lpData, sizeof(output));
        ListView_DeleteAllItems(g_list);
        SetCell(g_list, 0, 0, "종목명 (hname)");
        SetCell(g_list, 0, 1, WireField(output.hname, sizeof(output.hname)));
        SetCell(g_list, 1, 0, "현재가 (price)");
        SetCell(g_list, 1, 1, WireField(output.price, sizeof(output.price)));
        SetCell(g_list, 2, 0, "전일대비구분 (sign)");
        SetCell(g_list, 2, 1, WireField(output.sign, sizeof(output.sign)));
        SetCell(g_list, 3, 0, "전일대비 (change)");
        SetCell(g_list, 3, 1, WireField(output.change, sizeof(output.change)));
        SetCell(g_list, 4, 0, "등락율 (diff)");
        SetCell(g_list, 4, 1, WireField(output.diff, sizeof(output.diff)));
        SetCell(g_list, 5, 0, "거래량 (volume)");
        SetCell(g_list, 5, 1, WireField(output.volume, sizeof(output.volume)));
        SetCell(g_list, 6, 0, "거래량전일동시간비율 (stimeqrt)");
        SetCell(g_list, 6, 1, WireField(output.stimeqrt, sizeof(output.stimeqrt)));
        SetCell(g_list, 7, 0, "전일종가 (jnilclose)");
        SetCell(g_list, 7, 1, WireField(output.jnilclose, sizeof(output.jnilclose)));
        SetCell(g_list, 8, 0, "매도호가총수량 (dvol)");
        SetCell(g_list, 8, 1, WireField(output.dvol, sizeof(output.dvol)));
        SetCell(g_list, 9, 0, "매수호가총수량 (svol)");
        SetCell(g_list, 9, 1, WireField(output.svol, sizeof(output.svol)));
        SetCell(g_list, 10, 0, "총매도호가건수 (toffernum)");
        SetCell(g_list, 10, 1, WireField(output.toffernum, sizeof(output.toffernum)));
        SetCell(g_list, 11, 0, "총매수호가건수 (tbidnum)");
        SetCell(g_list, 11, 1, WireField(output.tbidnum, sizeof(output.tbidnum)));
        SetCell(g_list, 12, 0, "수신시간 (time)");
        SetCell(g_list, 12, 1, WireField(output.time, sizeof(output.time)));
        SetCell(g_list, 13, 0, "단축코드 (shcode)");
        SetCell(g_list, 13, 1, WireField(output.shcode, sizeof(output.shcode)));
        ListView_DeleteAllItems(g_detail);
        SetCell(g_detail, 0, 0, "1");
        SetCell(g_detail, 0, 1, WireField(output.offerho1, sizeof(output.offerho1)));
        SetCell(g_detail, 0, 2, WireField(output.offerrem1, sizeof(output.offerrem1)));
        SetCell(g_detail, 0, 3, WireField(output.dcnt1, sizeof(output.dcnt1)));
        SetCell(g_detail, 0, 4, WireField(output.bidho1, sizeof(output.bidho1)));
        SetCell(g_detail, 0, 5, WireField(output.bidrem1, sizeof(output.bidrem1)));
        SetCell(g_detail, 0, 6, WireField(output.scnt1, sizeof(output.scnt1)));
        SetCell(g_detail, 1, 0, "2");
        SetCell(g_detail, 1, 1, WireField(output.offerho2, sizeof(output.offerho2)));
        SetCell(g_detail, 1, 2, WireField(output.offerrem2, sizeof(output.offerrem2)));
        SetCell(g_detail, 1, 3, WireField(output.dcnt2, sizeof(output.dcnt2)));
        SetCell(g_detail, 1, 4, WireField(output.bidho2, sizeof(output.bidho2)));
        SetCell(g_detail, 1, 5, WireField(output.bidrem2, sizeof(output.bidrem2)));
        SetCell(g_detail, 1, 6, WireField(output.scnt2, sizeof(output.scnt2)));
        SetCell(g_detail, 2, 0, "3");
        SetCell(g_detail, 2, 1, WireField(output.offerho3, sizeof(output.offerho3)));
        SetCell(g_detail, 2, 2, WireField(output.offerrem3, sizeof(output.offerrem3)));
        SetCell(g_detail, 2, 3, WireField(output.dcnt3, sizeof(output.dcnt3)));
        SetCell(g_detail, 2, 4, WireField(output.bidho3, sizeof(output.bidho3)));
        SetCell(g_detail, 2, 5, WireField(output.bidrem3, sizeof(output.bidrem3)));
        SetCell(g_detail, 2, 6, WireField(output.scnt3, sizeof(output.scnt3)));
        SetCell(g_detail, 3, 0, "4");
        SetCell(g_detail, 3, 1, WireField(output.offerho4, sizeof(output.offerho4)));
        SetCell(g_detail, 3, 2, WireField(output.offerrem4, sizeof(output.offerrem4)));
        SetCell(g_detail, 3, 3, WireField(output.dcnt4, sizeof(output.dcnt4)));
        SetCell(g_detail, 3, 4, WireField(output.bidho4, sizeof(output.bidho4)));
        SetCell(g_detail, 3, 5, WireField(output.bidrem4, sizeof(output.bidrem4)));
        SetCell(g_detail, 3, 6, WireField(output.scnt4, sizeof(output.scnt4)));
        SetCell(g_detail, 4, 0, "5");
        SetCell(g_detail, 4, 1, WireField(output.offerho5, sizeof(output.offerho5)));
        SetCell(g_detail, 4, 2, WireField(output.offerrem5, sizeof(output.offerrem5)));
        SetCell(g_detail, 4, 3, WireField(output.dcnt5, sizeof(output.dcnt5)));
        SetCell(g_detail, 4, 4, WireField(output.bidho5, sizeof(output.bidho5)));
        SetCell(g_detail, 4, 5, WireField(output.bidrem5, sizeof(output.bidrem5)));
        SetCell(g_detail, 4, 6, WireField(output.scnt5, sizeof(output.scnt5)));
        SetCell(g_detail, 5, 0, "6");
        SetCell(g_detail, 5, 1, WireField(output.offerho6, sizeof(output.offerho6)));
        SetCell(g_detail, 5, 2, WireField(output.offerrem6, sizeof(output.offerrem6)));
        SetCell(g_detail, 5, 3, WireField(output.dcnt6, sizeof(output.dcnt6)));
        SetCell(g_detail, 5, 4, WireField(output.bidho6, sizeof(output.bidho6)));
        SetCell(g_detail, 5, 5, WireField(output.bidrem6, sizeof(output.bidrem6)));
        SetCell(g_detail, 5, 6, WireField(output.scnt6, sizeof(output.scnt6)));
        SetCell(g_detail, 6, 0, "7");
        SetCell(g_detail, 6, 1, WireField(output.offerho7, sizeof(output.offerho7)));
        SetCell(g_detail, 6, 2, WireField(output.offerrem7, sizeof(output.offerrem7)));
        SetCell(g_detail, 6, 3, WireField(output.dcnt7, sizeof(output.dcnt7)));
        SetCell(g_detail, 6, 4, WireField(output.bidho7, sizeof(output.bidho7)));
        SetCell(g_detail, 6, 5, WireField(output.bidrem7, sizeof(output.bidrem7)));
        SetCell(g_detail, 6, 6, WireField(output.scnt7, sizeof(output.scnt7)));
        SetCell(g_detail, 7, 0, "8");
        SetCell(g_detail, 7, 1, WireField(output.offerho8, sizeof(output.offerho8)));
        SetCell(g_detail, 7, 2, WireField(output.offerrem8, sizeof(output.offerrem8)));
        SetCell(g_detail, 7, 3, WireField(output.dcnt8, sizeof(output.dcnt8)));
        SetCell(g_detail, 7, 4, WireField(output.bidho8, sizeof(output.bidho8)));
        SetCell(g_detail, 7, 5, WireField(output.bidrem8, sizeof(output.bidrem8)));
        SetCell(g_detail, 7, 6, WireField(output.scnt8, sizeof(output.scnt8)));
        SetCell(g_detail, 8, 0, "9");
        SetCell(g_detail, 8, 1, WireField(output.offerho9, sizeof(output.offerho9)));
        SetCell(g_detail, 8, 2, WireField(output.offerrem9, sizeof(output.offerrem9)));
        SetCell(g_detail, 8, 3, WireField(output.dcnt9, sizeof(output.dcnt9)));
        SetCell(g_detail, 8, 4, WireField(output.bidho9, sizeof(output.bidho9)));
        SetCell(g_detail, 8, 5, WireField(output.bidrem9, sizeof(output.bidrem9)));
        SetCell(g_detail, 8, 6, WireField(output.scnt9, sizeof(output.scnt9)));
        SetCell(g_detail, 9, 0, "10");
        SetCell(g_detail, 9, 1, WireField(output.offerho10, sizeof(output.offerho10)));
        SetCell(g_detail, 9, 2, WireField(output.offerrem10, sizeof(output.offerrem10)));
        SetCell(g_detail, 9, 3, WireField(output.dcnt10, sizeof(output.dcnt10)));
        SetCell(g_detail, 9, 4, WireField(output.bidho10, sizeof(output.bidho10)));
        SetCell(g_detail, 9, 5, WireField(output.bidrem10, sizeof(output.bidrem10)));
        SetCell(g_detail, 9, 6, WireField(output.scnt10, sizeof(output.scnt10)));
        return TRUE;
    }
    return FALSE;
}

void ShowOutput()
{
    SetWindowTextW(g_output, ListView_GetItemCount(g_list) ? L"시세 수신" : L"시세 응답 없음");
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
    s3010InBlock input;
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
        Common::AddControl(window, L"STATIC", L"주식선물 단축코드", 0, 580, 53, 135, 22, 0);
        g_shcode = Common::AddControl(window, L"EDIT", L"", WS_BORDER | WS_TABSTOP | ES_AUTOHSCROLL,
                                  717, 50, 106, 24, 310);
        if (!g_shcode) return -1;
        SendMessage(g_shcode, EM_SETLIMITTEXT, 8, 0);
        g_queryButton = Common::AddControl(window, L"BUTTON", L"조회", WS_TABSTOP,
                                           950, 48, 140, 29, QUERY_BUTTON);
        g_output = Common::AddControl(window, L"STATIC", L"", 0, 580, 91, 365, 22, 0);
        g_list = Common::AddControl(window, WC_LISTVIEWW, L"", LVS_REPORT | LVS_SINGLESEL | WS_TABSTOP,
                                    580, 120, 540, 230, 302);
        if (!g_list) return -1;
        ListView_SetExtendedListViewStyle(g_list, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
        AddColumn(g_list, 0, L"항목 (전문 필드)", 335);
        AddColumn(g_list, 1, L"수신값", 177);
        g_detail = Common::AddControl(window, WC_LISTVIEWW, L"", LVS_REPORT | LVS_SINGLESEL | WS_TABSTOP,
                                    580, 360, 540, 220, 303);
        if (!g_detail) return -1;
        ListView_SetExtendedListViewStyle(g_detail, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
        AddColumn(g_detail, 0, L"단계", 48);
        AddColumn(g_detail, 1, L"매도호가", 77);
        AddColumn(g_detail, 2, L"매도잔량", 77);
        AddColumn(g_detail, 3, L"매도건수", 77);
        AddColumn(g_detail, 4, L"매수호가", 77);
        AddColumn(g_detail, 5, L"매수잔량", 77);
        AddColumn(g_detail, 6, L"매수건수", 77);
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
    wc.lpszClassName = L"OrdaAPIs3010StockFuturesQuote";
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
