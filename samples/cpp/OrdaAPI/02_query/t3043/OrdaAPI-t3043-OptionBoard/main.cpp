// 읽는 순서: WinMain 초기화 → 공통 로그인 통지 → 조회 요청 → 수신 → 해제 → 다음 조회 또는 종료.
// VS2010 v100 / Win32 / MultiByte / C++03. 공통 Helper는 변경하지 않습니다.
#include "../../../common/OrdaSampleCommon.h"
// 수신 블록명은 서비스 XML의 이름을 사용합니다.
#pragma warning(push)
#pragma warning(disable: 4005)
#include "t3043.h"
#pragma warning(pop)
#include <string.h>
#include <stdio.h>
#include <limits.h>
#include <string>
#include <vector>
#include <deque>
#include <commctrl.h>

namespace Common = OrdaSampleCommon;
const char SERVICE_CODE[] = NAME_t3043;
const wchar_t SERVICE_TITLE[] = L"옵션전광판(t3043)";
const int QUERY_BUTTON = 300, DISPLAY_LIMIT = 200;
const int INPUT_COUNT = 1;
HWND g_inputs[INPUT_COUNT], g_queryButton, g_output[2], g_list[2], g_fixedList;
BOOL g_fixedReceived = FALSE;
int g_requestId = -1, g_recordCount[2] = {0};
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
    ListView_DeleteAllItems(g_fixedList);
    g_fixedReceived = FALSE;
    for (int i = 0; i < 2; ++i)
    {
        SetWindowTextW(g_output[i], L"");
        ListView_DeleteAllItems(g_list[i]);
        g_recordCount[i] = 0;
    }
}

// 유효한 YYYYMM을 확인합니다. 실제 상장 월물은 서버에서 확인합니다.
BOOL BuildInput(t3043InBlock &input)
{
    memset(&input, ' ', sizeof(input));
    char month[7] = {0};
    if (GetWindowTextLengthA(g_inputs[0]) != sizeof(input.yyyymm) ||
        GetWindowTextA(g_inputs[0], month, sizeof(month)) != sizeof(input.yyyymm))
        return FALSE;
    for (int i = 0; i < 6; ++i)
        if (month[i] < '0' || month[i] > '9') return FALSE;
    int number = (month[4] - '0') * 10 + month[5] - '0';
    if (number < 1 || number > 12 || memcmp(month, "0000", 4) == 0) return FALSE;
    memcpy(input.yyyymm, month, sizeof(input.yyyymm));
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

// 요약과 옵션 행은 각 블록이 수신될 때 독립적으로 표시합니다.
void ReceiveFixed(const t3043OutBlock &fixed)
{
    g_fixedReceived = TRUE;
    ListView_DeleteAllItems(g_fixedList);
    SetCell(g_fixedList, 0, 0, "역사적변동성");
    SetCell(g_fixedList, 0, 1, Field(fixed.histimpv, sizeof(fixed.histimpv)));
    SetCell(g_fixedList, 1, 0, "옵션잔존일");
    SetCell(g_fixedList, 1, 1, Field(fixed.jandatecnt, sizeof(fixed.jandatecnt)));
    SetCell(g_fixedList, 2, 0, "콜옵션대표IV");
    SetCell(g_fixedList, 2, 1, Field(fixed.cimpv, sizeof(fixed.cimpv)));
    SetCell(g_fixedList, 3, 0, "풋옵션대표IV");
    SetCell(g_fixedList, 3, 1, Field(fixed.pimpv, sizeof(fixed.pimpv)));
    SetCell(g_fixedList, 4, 0, "근월물현재가");
    SetCell(g_fixedList, 4, 1, Field(fixed.gmprice, sizeof(fixed.gmprice)));
    SetCell(g_fixedList, 5, 0, "근월물전일대비구분");
    SetCell(g_fixedList, 5, 1, Field(fixed.gmsign, sizeof(fixed.gmsign)));
    SetCell(g_fixedList, 6, 0, "근월물전일대비");
    SetCell(g_fixedList, 6, 1, Field(fixed.gmchange, sizeof(fixed.gmchange)));
    SetCell(g_fixedList, 7, 0, "근월물등락율");
    SetCell(g_fixedList, 7, 1, Field(fixed.gmdiff, sizeof(fixed.gmdiff)));
    SetCell(g_fixedList, 8, 0, "근월물거래량");
    SetCell(g_fixedList, 8, 1, Field(fixed.gmvolume, sizeof(fixed.gmvolume)));
    SetCell(g_fixedList, 9, 0, "근월물선물코드");
    SetCell(g_fixedList, 9, 1, Field(fixed.gmshcode, sizeof(fixed.gmshcode)));
}

// 블록 길이를 레코드 크기로 나누어 실제 건수를 계산합니다.
BOOL ReceiveRows1(const unsigned char *data, int length)
{
    int count = length / sizeof(t3043OutBlock1);
    if (count > INT_MAX - g_recordCount[0]) return FALSE;
    for (int row = 0; row < count && g_recordCount[0] + row < DISPLAY_LIMIT; ++row)
    {
        t3043OutBlock1 output;
        memcpy(&output, data + row * sizeof(output), sizeof(output));
        int displayRow = g_recordCount[0] + row;
        SetCell(g_list[0], displayRow, 0, Field(output.actprice, sizeof(output.actprice)));
        SetCell(g_list[0], displayRow, 1, Field(output.optcode, sizeof(output.optcode)));
        SetCell(g_list[0], displayRow, 2, Field(output.price, sizeof(output.price)));
        SetCell(g_list[0], displayRow, 3, Field(output.sign, sizeof(output.sign)));
        SetCell(g_list[0], displayRow, 4, Field(output.change, sizeof(output.change)));
        SetCell(g_list[0], displayRow, 5, Field(output.diff, sizeof(output.diff)));
        SetCell(g_list[0], displayRow, 6, Field(output.volume, sizeof(output.volume)));
        SetCell(g_list[0], displayRow, 7, Field(output.iv, sizeof(output.iv)));
        SetCell(g_list[0], displayRow, 8, Field(output.mgjv, sizeof(output.mgjv)));
        SetCell(g_list[0], displayRow, 9, Field(output.mgjvupdn, sizeof(output.mgjvupdn)));
        SetCell(g_list[0], displayRow, 10, Field(output.offerho1, sizeof(output.offerho1)));
        SetCell(g_list[0], displayRow, 11, Field(output.bidho1, sizeof(output.bidho1)));
        SetCell(g_list[0], displayRow, 12, Field(output.cvolume, sizeof(output.cvolume)));
        SetCell(g_list[0], displayRow, 13, Field(output.delt, sizeof(output.delt)));
        SetCell(g_list[0], displayRow, 14, Field(output.gama, sizeof(output.gama)));
        SetCell(g_list[0], displayRow, 15, Field(output.vega, sizeof(output.vega)));
        SetCell(g_list[0], displayRow, 16, Field(output.ceta, sizeof(output.ceta)));
        SetCell(g_list[0], displayRow, 17, Field(output.rhox, sizeof(output.rhox)));
        SetCell(g_list[0], displayRow, 18, Field(output.theoryprice, sizeof(output.theoryprice)));
        SetCell(g_list[0], displayRow, 19, Field(output.impv, sizeof(output.impv)));
        SetCell(g_list[0], displayRow, 20, Field(output.timevl, sizeof(output.timevl)));
        SetCell(g_list[0], displayRow, 21, Field(output.jvolume, sizeof(output.jvolume)));
        SetCell(g_list[0], displayRow, 22, Field(output.parpl, sizeof(output.parpl)));
        SetCell(g_list[0], displayRow, 23, Field(output.jngo, sizeof(output.jngo)));
        SetCell(g_list[0], displayRow, 24, Field(output.offerrem1, sizeof(output.offerrem1)));
        SetCell(g_list[0], displayRow, 25, Field(output.bidrem1, sizeof(output.bidrem1)));
        SetCell(g_list[0], displayRow, 26, Field(output.open, sizeof(output.open)));
        SetCell(g_list[0], displayRow, 27, Field(output.high, sizeof(output.high)));
        SetCell(g_list[0], displayRow, 28, Field(output.low, sizeof(output.low)));
        SetCell(g_list[0], displayRow, 29, Field(output.atmgubun, sizeof(output.atmgubun)));
        SetCell(g_list[0], displayRow, 30, Field(output.jisuconv, sizeof(output.jisuconv)));
        SetCell(g_list[0], displayRow, 31, Field(output.value, sizeof(output.value)));
        SetCell(g_list[0], displayRow, 32, Field(output.yeprice, sizeof(output.yeprice)));
        SetCell(g_list[0], displayRow, 33, Field(output.yechange, sizeof(output.yechange)));
        SetCell(g_list[0], displayRow, 34, Field(output.yediff, sizeof(output.yediff)));
        SetCell(g_list[0], displayRow, 35, Field(output.yevolume, sizeof(output.yevolume)));
    }
    g_recordCount[0] += count;
    return TRUE;
}

BOOL ReceiveRows2(const unsigned char *data, int length)
{
    int count = length / sizeof(t3043OutBlock2);
    if (count > INT_MAX - g_recordCount[1]) return FALSE;
    for (int row = 0; row < count && g_recordCount[1] + row < DISPLAY_LIMIT; ++row)
    {
        t3043OutBlock2 output;
        memcpy(&output, data + row * sizeof(output), sizeof(output));
        int displayRow = g_recordCount[1] + row;
        SetCell(g_list[1], displayRow, 0, Field(output.actprice, sizeof(output.actprice)));
        SetCell(g_list[1], displayRow, 1, Field(output.optcode, sizeof(output.optcode)));
        SetCell(g_list[1], displayRow, 2, Field(output.price, sizeof(output.price)));
        SetCell(g_list[1], displayRow, 3, Field(output.sign, sizeof(output.sign)));
        SetCell(g_list[1], displayRow, 4, Field(output.change, sizeof(output.change)));
        SetCell(g_list[1], displayRow, 5, Field(output.diff, sizeof(output.diff)));
        SetCell(g_list[1], displayRow, 6, Field(output.volume, sizeof(output.volume)));
        SetCell(g_list[1], displayRow, 7, Field(output.iv, sizeof(output.iv)));
        SetCell(g_list[1], displayRow, 8, Field(output.mgjv, sizeof(output.mgjv)));
        SetCell(g_list[1], displayRow, 9, Field(output.mgjvupdn, sizeof(output.mgjvupdn)));
        SetCell(g_list[1], displayRow, 10, Field(output.offerho1, sizeof(output.offerho1)));
        SetCell(g_list[1], displayRow, 11, Field(output.bidho1, sizeof(output.bidho1)));
        SetCell(g_list[1], displayRow, 12, Field(output.cvolume, sizeof(output.cvolume)));
        SetCell(g_list[1], displayRow, 13, Field(output.delt, sizeof(output.delt)));
        SetCell(g_list[1], displayRow, 14, Field(output.gama, sizeof(output.gama)));
        SetCell(g_list[1], displayRow, 15, Field(output.vega, sizeof(output.vega)));
        SetCell(g_list[1], displayRow, 16, Field(output.ceta, sizeof(output.ceta)));
        SetCell(g_list[1], displayRow, 17, Field(output.rhox, sizeof(output.rhox)));
        SetCell(g_list[1], displayRow, 18, Field(output.theoryprice, sizeof(output.theoryprice)));
        SetCell(g_list[1], displayRow, 19, Field(output.impv, sizeof(output.impv)));
        SetCell(g_list[1], displayRow, 20, Field(output.timevl, sizeof(output.timevl)));
        SetCell(g_list[1], displayRow, 21, Field(output.jvolume, sizeof(output.jvolume)));
        SetCell(g_list[1], displayRow, 22, Field(output.parpl, sizeof(output.parpl)));
        SetCell(g_list[1], displayRow, 23, Field(output.jngo, sizeof(output.jngo)));
        SetCell(g_list[1], displayRow, 24, Field(output.offerrem1, sizeof(output.offerrem1)));
        SetCell(g_list[1], displayRow, 25, Field(output.bidrem1, sizeof(output.bidrem1)));
        SetCell(g_list[1], displayRow, 26, Field(output.open, sizeof(output.open)));
        SetCell(g_list[1], displayRow, 27, Field(output.high, sizeof(output.high)));
        SetCell(g_list[1], displayRow, 28, Field(output.low, sizeof(output.low)));
        SetCell(g_list[1], displayRow, 29, Field(output.atmgubun, sizeof(output.atmgubun)));
        SetCell(g_list[1], displayRow, 30, Field(output.jisuconv, sizeof(output.jisuconv)));
        SetCell(g_list[1], displayRow, 31, Field(output.value, sizeof(output.value)));
        SetCell(g_list[1], displayRow, 32, Field(output.yeprice, sizeof(output.yeprice)));
        SetCell(g_list[1], displayRow, 33, Field(output.yechange, sizeof(output.yechange)));
        SetCell(g_list[1], displayRow, 34, Field(output.yediff, sizeof(output.yediff)));
        SetCell(g_list[1], displayRow, 35, Field(output.yevolume, sizeof(output.yevolume)));
    }
    g_recordCount[1] += count;
    return TRUE;
}

void ShowOutput()
{
    const wchar_t *kind[] = {L"콜옵션", L"풋옵션"};
    for (int i = 0; i < 2; ++i)
    {
        wchar_t heading[128];
        swprintf_s(heading, L"%ls   수신 %d건 / 표시 %d건",
                   kind[i], g_recordCount[i], ListView_GetItemCount(g_list[i]));
        SetWindowTextW(g_output[i], heading);
    }
}

// SDK가 조립한 A형 통지는 블록 이름으로 구분합니다.
BOOL ValidBody(const RP_DATA_INFO *response)
{
    if (!response || response->nDataLength < 0 ||
        response->nDataLength > response->nTotalDataBufferSize ||
        (!response->lpData && response->nDataLength > 0)) return FALSE;
    std::string block = Field(response->szBlockName, sizeof(response->szBlockName));
    return (block == "t3043OutBlock" && response->nDataLength == sizeof(t3043OutBlock)) ||
           (block == "t3043OutBlock1" && response->nDataLength % sizeof(t3043OutBlock1) == 0) ||
           (block == "t3043OutBlock2" && response->nDataLength % sizeof(t3043OutBlock2) == 0);
}
BOOL ReceiveData(const RP_DATA_INFO *response)
{
    if (!response || response->nRqID != g_requestId || g_requestId < 0) return FALSE;
    if (Field(response->Header.szTrCode, sizeof(response->Header.szTrCode)) != SERVICE_CODE || !ValidBody(response)) return FALSE;
    if (response->Header.szRetFlag[0] >= '1' && response->Header.szRetFlag[0] <= '9') return FALSE;
    std::string block = Field(response->szBlockName, sizeof(response->szBlockName));
    if (block == "t3043OutBlock")
    {
        t3043OutBlock fixed;
        memcpy(&fixed, response->lpData, sizeof(fixed));
        ReceiveFixed(fixed);
        return TRUE;
    }
    if (block == "t3043OutBlock1")
        return ReceiveRows1(response->lpData, response->nDataLength);
    if (block == "t3043OutBlock2")
        return ReceiveRows2(response->lpData, response->nDataLength);
    return FALSE;
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
    t3043InBlock input;
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
        Common::AddControl(window, L"STATIC", L"월물 YYYYMM", 0, 580, 52, 150, 22, 0);
        g_inputs[0] = Common::AddControl(window, L"EDIT", L"", ES_AUTOHSCROLL | WS_TABSTOP,
                                     735, 50, 145, 24, 400);
        SendMessage(g_inputs[0], EM_SETLIMITTEXT, 6, 0);
        g_queryButton = Common::AddControl(window, L"BUTTON", L"조회", WS_TABSTOP, 900, 46, 140, 30, QUERY_BUTTON);

        g_fixedList = Common::AddControl(window, WC_LISTVIEWW, L"", LVS_REPORT | LVS_SINGLESEL | WS_TABSTOP,
                                        580, 90, 540, 382, 303);
        if (!g_fixedList) return -1;
        ListView_SetExtendedListViewStyle(g_fixedList, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
        AddColumn(g_fixedList, 0, L"요약 항목", 210);
        AddColumn(g_fixedList, 1, L"값", 300);
        g_output[0] = Common::AddControl(window, L"STATIC", L"", 0, 20, 486, 540, 22, 0);
        g_list[0] = Common::AddControl(window, WC_LISTVIEWW, L"", LVS_REPORT | LVS_SINGLESEL | WS_TABSTOP,
                                    20, 511, 540, 166, 310);
        if (!g_list[0]) return -1;
        ListView_SetExtendedListViewStyle(g_list[0], LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
        AddColumn(g_list[0], 0, L"행사가", 95);
        AddColumn(g_list[0], 1, L"콜옵션코드", 95);
        AddColumn(g_list[0], 2, L"현재가", 95);
        AddColumn(g_list[0], 3, L"전일대비구분", 106);
        AddColumn(g_list[0], 4, L"전일대비", 95);
        AddColumn(g_list[0], 5, L"등락율", 95);
        AddColumn(g_list[0], 6, L"거래량", 95);
        AddColumn(g_list[0], 7, L"IV", 95);
        AddColumn(g_list[0], 8, L"미결제약정", 95);
        AddColumn(g_list[0], 9, L"미결제약정증감", 120);
        AddColumn(g_list[0], 10, L"매도호가", 95);
        AddColumn(g_list[0], 11, L"매수호가", 95);
        AddColumn(g_list[0], 12, L"체결량", 95);
        AddColumn(g_list[0], 13, L"델타", 95);
        AddColumn(g_list[0], 14, L"감마", 95);
        AddColumn(g_list[0], 15, L"베가", 95);
        AddColumn(g_list[0], 16, L"쎄타", 95);
        AddColumn(g_list[0], 17, L"로우", 95);
        AddColumn(g_list[0], 18, L"이론가", 95);
        AddColumn(g_list[0], 19, L"내재가치", 95);
        AddColumn(g_list[0], 20, L"시간가치", 95);
        AddColumn(g_list[0], 21, L"잔고수량", 95);
        AddColumn(g_list[0], 22, L"평가손익", 95);
        AddColumn(g_list[0], 23, L"청산가능수량", 106);
        AddColumn(g_list[0], 24, L"매도잔량", 95);
        AddColumn(g_list[0], 25, L"매수잔량", 95);
        AddColumn(g_list[0], 26, L"시가", 95);
        AddColumn(g_list[0], 27, L"고가", 95);
        AddColumn(g_list[0], 28, L"저가", 95);
        AddColumn(g_list[0], 29, L"ATM구분", 95);
        AddColumn(g_list[0], 30, L"지수환산", 95);
        AddColumn(g_list[0], 31, L"거래대금", 95);
        AddColumn(g_list[0], 32, L"예상체결가", 95);
        AddColumn(g_list[0], 33, L"예상전일비", 95);
        AddColumn(g_list[0], 34, L"예상등락율", 95);
        AddColumn(g_list[0], 35, L"예상체결량", 95);
        g_output[1] = Common::AddControl(window, L"STATIC", L"", 0, 575, 486, 540, 22, 0);
        g_list[1] = Common::AddControl(window, WC_LISTVIEWW, L"", LVS_REPORT | LVS_SINGLESEL | WS_TABSTOP,
                                    575, 511, 540, 166, 311);
        if (!g_list[1]) return -1;
        ListView_SetExtendedListViewStyle(g_list[1], LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
        AddColumn(g_list[1], 0, L"행사가", 95);
        AddColumn(g_list[1], 1, L"풋옵션코드", 95);
        AddColumn(g_list[1], 2, L"현재가", 95);
        AddColumn(g_list[1], 3, L"전일대비구분", 106);
        AddColumn(g_list[1], 4, L"전일대비", 95);
        AddColumn(g_list[1], 5, L"등락율", 95);
        AddColumn(g_list[1], 6, L"거래량", 95);
        AddColumn(g_list[1], 7, L"IV", 95);
        AddColumn(g_list[1], 8, L"미결제약정", 95);
        AddColumn(g_list[1], 9, L"미결제약정증감", 120);
        AddColumn(g_list[1], 10, L"매도호가", 95);
        AddColumn(g_list[1], 11, L"매수호가", 95);
        AddColumn(g_list[1], 12, L"체결량", 95);
        AddColumn(g_list[1], 13, L"델타", 95);
        AddColumn(g_list[1], 14, L"감마", 95);
        AddColumn(g_list[1], 15, L"베가", 95);
        AddColumn(g_list[1], 16, L"쎄타", 95);
        AddColumn(g_list[1], 17, L"로우", 95);
        AddColumn(g_list[1], 18, L"이론가", 95);
        AddColumn(g_list[1], 19, L"내재가치", 95);
        AddColumn(g_list[1], 20, L"시간가치", 95);
        AddColumn(g_list[1], 21, L"잔고수량", 95);
        AddColumn(g_list[1], 22, L"평가손익", 95);
        AddColumn(g_list[1], 23, L"청산가능수량", 106);
        AddColumn(g_list[1], 24, L"매도잔량", 95);
        AddColumn(g_list[1], 25, L"매수잔량", 95);
        AddColumn(g_list[1], 26, L"시가", 95);
        AddColumn(g_list[1], 27, L"고가", 95);
        AddColumn(g_list[1], 28, L"저가", 95);
        AddColumn(g_list[1], 29, L"ATM구분", 95);
        AddColumn(g_list[1], 30, L"지수환산", 95);
        AddColumn(g_list[1], 31, L"거래대금", 95);
        AddColumn(g_list[1], 32, L"예상체결가", 95);
        AddColumn(g_list[1], 33, L"예상전일비", 95);
        AddColumn(g_list[1], 34, L"예상등락율", 95);
        AddColumn(g_list[1], 35, L"예상체결량", 95);
        ShowOutput();
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
    wc.lpszClassName = L"OrdaAPIt3043OptionBoard";
    if (!RegisterClassW(&wc))
        return 1;
    HWND window = CreateWindowW(wc.lpszClassName, SERVICE_TITLE,
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
        CW_USEDEFAULT, CW_USEDEFAULT, 1150, 725, NULL, NULL, instance, NULL);
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
