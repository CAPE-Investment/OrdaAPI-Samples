// 읽는 순서: WinMain 초기화 → 공통 로그인 통지 → 조회 요청 → 수신 → 해제 → 다음 조회 또는 종료.
// VS2010 v100 / Win32 / MultiByte / C++03. 공통 Helper는 변경하지 않습니다.
#include "../../../common/OrdaSampleCommon.h"
// 수신 블록명은 서비스 XML의 이름을 사용합니다.
#pragma warning(push)
#pragma warning(disable: 4005)
#include "s1501.h"
#pragma warning(pop)
#include <string.h>
#include <stdio.h>
#include <limits.h>
#include <string>
#include <vector>
#include <deque>
#include <commctrl.h>

namespace Common = OrdaSampleCommon;
const char SERVICE_CODE[] = NAME_s1501;
const wchar_t SERVICE_TITLE[] = L"ELW 현재가 호가조회(s1501)";
const int QUERY_BUTTON = 300, DISPLAY_LIMIT = 200;
const int INPUT_COUNT = 1;
HWND g_inputs[INPUT_COUNT], g_queryButton, g_output, g_list, g_book;
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
    ListView_DeleteAllItems(g_book);
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

BOOL BuildInput(s1501InBlock &input)
{
    memset(&input, ' ', sizeof(input));
    if (!CopyText(g_inputs[0], input.shcode, sizeof(input.shcode), FALSE, FALSE)) return FALSE;
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

// 항목별 문자를 그대로 표시합니다. 빈 호가와 0 수량을 구분합니다.
void ShowBlock0(const s1501OutBlock &output)
{
    ListView_DeleteAllItems(g_list);
    SetCell(g_list, 0, 0, "한글명");
    SetCell(g_list, 0, 1, Field(output.hname, sizeof(output.hname)));
    SetCell(g_list, 1, 0, "현재가");
    SetCell(g_list, 1, 1, Field(output.price, sizeof(output.price)));
    SetCell(g_list, 2, 0, "전일대비구분");
    SetCell(g_list, 2, 1, Field(output.sign, sizeof(output.sign)));
    SetCell(g_list, 3, 0, "전일대비");
    SetCell(g_list, 3, 1, Field(output.change, sizeof(output.change)));
    SetCell(g_list, 4, 0, "등락율");
    SetCell(g_list, 4, 1, Field(output.diff, sizeof(output.diff)));
    SetCell(g_list, 5, 0, "누적거래량");
    SetCell(g_list, 5, 1, Field(output.volume, sizeof(output.volume)));
    SetCell(g_list, 6, 0, "전일종가");
    SetCell(g_list, 6, 1, Field(output.jnilclose, sizeof(output.jnilclose)));
    SetCell(g_list, 7, 0, "매도호가수량합");
    SetCell(g_list, 7, 1, Field(output.offer, sizeof(output.offer)));
    SetCell(g_list, 8, 0, "매수호가수량합");
    SetCell(g_list, 8, 1, Field(output.bid, sizeof(output.bid)));
    SetCell(g_list, 9, 0, "직전매도대비수량합");
    SetCell(g_list, 9, 1, Field(output.preoffercha, sizeof(output.preoffercha)));
    SetCell(g_list, 10, 0, "직전매수대비수량합");
    SetCell(g_list, 10, 1, Field(output.prebidcha, sizeof(output.prebidcha)));
    SetCell(g_list, 11, 0, "수신시간");
    SetCell(g_list, 11, 1, Field(output.hotime, sizeof(output.hotime)));
    SetCell(g_list, 12, 0, "예상체결가격");
    SetCell(g_list, 12, 1, Field(output.yeprice, sizeof(output.yeprice)));
    SetCell(g_list, 13, 0, "예상체결수량");
    SetCell(g_list, 13, 1, Field(output.yevolume, sizeof(output.yevolume)));
    SetCell(g_list, 14, 0, "예상체결전일구분");
    SetCell(g_list, 14, 1, Field(output.yesign, sizeof(output.yesign)));
    SetCell(g_list, 15, 0, "예상체결전일대비");
    SetCell(g_list, 15, 1, Field(output.yechange, sizeof(output.yechange)));
    SetCell(g_list, 16, 0, "예상체결등락율");
    SetCell(g_list, 16, 1, Field(output.yediff, sizeof(output.yediff)));
    SetCell(g_list, 17, 0, "시간외매도잔량");
    SetCell(g_list, 17, 1, Field(output.tmoffer, sizeof(output.tmoffer)));
    SetCell(g_list, 18, 0, "시간외매수잔량");
    SetCell(g_list, 18, 1, Field(output.tmbid, sizeof(output.tmbid)));
    SetCell(g_list, 19, 0, "동시구분");
    SetCell(g_list, 19, 1, Field(output.ho_status, sizeof(output.ho_status)));
    SetCell(g_list, 20, 0, "시가");
    SetCell(g_list, 20, 1, Field(output.open, sizeof(output.open)));
    SetCell(g_list, 21, 0, "고가");
    SetCell(g_list, 21, 1, Field(output.high, sizeof(output.high)));
    SetCell(g_list, 22, 0, "저가");
    SetCell(g_list, 22, 1, Field(output.low, sizeof(output.low)));
    ListView_DeleteAllItems(g_book);
    SetCell(g_book, 0, 0, "1");
    SetCell(g_book, 0, 1, Field(output.offerho1, sizeof(output.offerho1)));
    SetCell(g_book, 0, 2, Field(output.bidho1, sizeof(output.bidho1)));
    SetCell(g_book, 0, 3, Field(output.offerrem1, sizeof(output.offerrem1)));
    SetCell(g_book, 0, 4, Field(output.bidrem1, sizeof(output.bidrem1)));
    SetCell(g_book, 0, 5, Field(output.lp_offerrem1, sizeof(output.lp_offerrem1)));
    SetCell(g_book, 0, 6, Field(output.lp_bidrem1, sizeof(output.lp_bidrem1)));
    SetCell(g_book, 0, 7, Field(output.preoffercha1, sizeof(output.preoffercha1)));
    SetCell(g_book, 0, 8, Field(output.prebidcha1, sizeof(output.prebidcha1)));
    SetCell(g_book, 1, 0, "2");
    SetCell(g_book, 1, 1, Field(output.offerho2, sizeof(output.offerho2)));
    SetCell(g_book, 1, 2, Field(output.bidho2, sizeof(output.bidho2)));
    SetCell(g_book, 1, 3, Field(output.offerrem2, sizeof(output.offerrem2)));
    SetCell(g_book, 1, 4, Field(output.bidrem2, sizeof(output.bidrem2)));
    SetCell(g_book, 1, 5, Field(output.lp_offerrem2, sizeof(output.lp_offerrem2)));
    SetCell(g_book, 1, 6, Field(output.lp_bidrem2, sizeof(output.lp_bidrem2)));
    SetCell(g_book, 1, 7, Field(output.preoffercha2, sizeof(output.preoffercha2)));
    SetCell(g_book, 1, 8, Field(output.prebidcha2, sizeof(output.prebidcha2)));
    SetCell(g_book, 2, 0, "3");
    SetCell(g_book, 2, 1, Field(output.offerho3, sizeof(output.offerho3)));
    SetCell(g_book, 2, 2, Field(output.bidho3, sizeof(output.bidho3)));
    SetCell(g_book, 2, 3, Field(output.offerrem3, sizeof(output.offerrem3)));
    SetCell(g_book, 2, 4, Field(output.bidrem3, sizeof(output.bidrem3)));
    SetCell(g_book, 2, 5, Field(output.lp_offerrem3, sizeof(output.lp_offerrem3)));
    SetCell(g_book, 2, 6, Field(output.lp_bidrem3, sizeof(output.lp_bidrem3)));
    SetCell(g_book, 2, 7, Field(output.preoffercha3, sizeof(output.preoffercha3)));
    SetCell(g_book, 2, 8, Field(output.prebidcha3, sizeof(output.prebidcha3)));
    SetCell(g_book, 3, 0, "4");
    SetCell(g_book, 3, 1, Field(output.offerho4, sizeof(output.offerho4)));
    SetCell(g_book, 3, 2, Field(output.bidho4, sizeof(output.bidho4)));
    SetCell(g_book, 3, 3, Field(output.offerrem4, sizeof(output.offerrem4)));
    SetCell(g_book, 3, 4, Field(output.bidrem4, sizeof(output.bidrem4)));
    SetCell(g_book, 3, 5, Field(output.lp_offerrem4, sizeof(output.lp_offerrem4)));
    SetCell(g_book, 3, 6, Field(output.lp_bidrem4, sizeof(output.lp_bidrem4)));
    SetCell(g_book, 3, 7, Field(output.preoffercha4, sizeof(output.preoffercha4)));
    SetCell(g_book, 3, 8, Field(output.prebidcha4, sizeof(output.prebidcha4)));
    SetCell(g_book, 4, 0, "5");
    SetCell(g_book, 4, 1, Field(output.offerho5, sizeof(output.offerho5)));
    SetCell(g_book, 4, 2, Field(output.bidho5, sizeof(output.bidho5)));
    SetCell(g_book, 4, 3, Field(output.offerrem5, sizeof(output.offerrem5)));
    SetCell(g_book, 4, 4, Field(output.bidrem5, sizeof(output.bidrem5)));
    SetCell(g_book, 4, 5, Field(output.lp_offerrem5, sizeof(output.lp_offerrem5)));
    SetCell(g_book, 4, 6, Field(output.lp_bidrem5, sizeof(output.lp_bidrem5)));
    SetCell(g_book, 4, 7, Field(output.preoffercha5, sizeof(output.preoffercha5)));
    SetCell(g_book, 4, 8, Field(output.prebidcha5, sizeof(output.prebidcha5)));
    SetCell(g_book, 5, 0, "6");
    SetCell(g_book, 5, 1, Field(output.offerho6, sizeof(output.offerho6)));
    SetCell(g_book, 5, 2, Field(output.bidho6, sizeof(output.bidho6)));
    SetCell(g_book, 5, 3, Field(output.offerrem6, sizeof(output.offerrem6)));
    SetCell(g_book, 5, 4, Field(output.bidrem6, sizeof(output.bidrem6)));
    SetCell(g_book, 5, 5, Field(output.lp_offerrem6, sizeof(output.lp_offerrem6)));
    SetCell(g_book, 5, 6, Field(output.lp_bidrem6, sizeof(output.lp_bidrem6)));
    SetCell(g_book, 5, 7, Field(output.preoffercha6, sizeof(output.preoffercha6)));
    SetCell(g_book, 5, 8, Field(output.prebidcha6, sizeof(output.prebidcha6)));
    SetCell(g_book, 6, 0, "7");
    SetCell(g_book, 6, 1, Field(output.offerho7, sizeof(output.offerho7)));
    SetCell(g_book, 6, 2, Field(output.bidho7, sizeof(output.bidho7)));
    SetCell(g_book, 6, 3, Field(output.offerrem7, sizeof(output.offerrem7)));
    SetCell(g_book, 6, 4, Field(output.bidrem7, sizeof(output.bidrem7)));
    SetCell(g_book, 6, 5, Field(output.lp_offerrem7, sizeof(output.lp_offerrem7)));
    SetCell(g_book, 6, 6, Field(output.lp_bidrem7, sizeof(output.lp_bidrem7)));
    SetCell(g_book, 6, 7, Field(output.preoffercha7, sizeof(output.preoffercha7)));
    SetCell(g_book, 6, 8, Field(output.prebidcha7, sizeof(output.prebidcha7)));
    SetCell(g_book, 7, 0, "8");
    SetCell(g_book, 7, 1, Field(output.offerho8, sizeof(output.offerho8)));
    SetCell(g_book, 7, 2, Field(output.bidho8, sizeof(output.bidho8)));
    SetCell(g_book, 7, 3, Field(output.offerrem8, sizeof(output.offerrem8)));
    SetCell(g_book, 7, 4, Field(output.bidrem8, sizeof(output.bidrem8)));
    SetCell(g_book, 7, 5, Field(output.lp_offerrem8, sizeof(output.lp_offerrem8)));
    SetCell(g_book, 7, 6, Field(output.lp_bidrem8, sizeof(output.lp_bidrem8)));
    SetCell(g_book, 7, 7, Field(output.preoffercha8, sizeof(output.preoffercha8)));
    SetCell(g_book, 7, 8, Field(output.prebidcha8, sizeof(output.prebidcha8)));
    SetCell(g_book, 8, 0, "9");
    SetCell(g_book, 8, 1, Field(output.offerho9, sizeof(output.offerho9)));
    SetCell(g_book, 8, 2, Field(output.bidho9, sizeof(output.bidho9)));
    SetCell(g_book, 8, 3, Field(output.offerrem9, sizeof(output.offerrem9)));
    SetCell(g_book, 8, 4, Field(output.bidrem9, sizeof(output.bidrem9)));
    SetCell(g_book, 8, 5, Field(output.lp_offerrem9, sizeof(output.lp_offerrem9)));
    SetCell(g_book, 8, 6, Field(output.lp_bidrem9, sizeof(output.lp_bidrem9)));
    SetCell(g_book, 8, 7, Field(output.preoffercha9, sizeof(output.preoffercha9)));
    SetCell(g_book, 8, 8, Field(output.prebidcha9, sizeof(output.prebidcha9)));
    SetCell(g_book, 9, 0, "10");
    SetCell(g_book, 9, 1, Field(output.offerho10, sizeof(output.offerho10)));
    SetCell(g_book, 9, 2, Field(output.bidho10, sizeof(output.bidho10)));
    SetCell(g_book, 9, 3, Field(output.offerrem10, sizeof(output.offerrem10)));
    SetCell(g_book, 9, 4, Field(output.bidrem10, sizeof(output.bidrem10)));
    SetCell(g_book, 9, 5, Field(output.lp_offerrem10, sizeof(output.lp_offerrem10)));
    SetCell(g_book, 9, 6, Field(output.lp_bidrem10, sizeof(output.lp_bidrem10)));
    SetCell(g_book, 9, 7, Field(output.preoffercha10, sizeof(output.preoffercha10)));
    SetCell(g_book, 9, 8, Field(output.prebidcha10, sizeof(output.prebidcha10)));
}

// 고정 블록은 이름과 정확한 크기가 일치할 때만 읽습니다.
BOOL ValidBody(const RP_DATA_INFO *response)
{
    if (!response || !response->lpData || response->nDataLength < 0 ||
        response->nDataLength > response->nTotalDataBufferSize) return FALSE;
    std::string block = Field(response->szBlockName, sizeof(response->szBlockName));
    return (block == "s1501OutBlock" && response->nDataLength == sizeof(s1501OutBlock));
}

BOOL ReceiveData(const RP_DATA_INFO *response)
{
    if (!response || response->nRqID != g_requestId || g_requestId < 0 || !ValidBody(response)) return FALSE;
    if (Field(response->Header.szTrCode, sizeof(response->Header.szTrCode)) != SERVICE_CODE) return FALSE;
    if (response->Header.szRetFlag[0] >= '1' && response->Header.szRetFlag[0] <= '9') return FALSE;
    s1501OutBlock output;
    memcpy(&output, response->lpData, sizeof(output));
    ShowBlock0(output);
    g_recordCount = 1;
    return TRUE;
}

void ShowOutput()
{
    SetWindowTextW(g_output, g_recordCount ? L"고정 블록 수신" : L"고정 블록 미수신");
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
        if (!g_queryError && (!g_recordCount))
        {
            g_queryError = TRUE;
            Common::SetResult(L"고정 출력 블록 일부 또는 전체가 누락되었습니다.");
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
    s1501InBlock input;
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
        Common::AddControl(window, L"STATIC", L"단축코드 (6자리)", 0, 580, 52, 150, 22, 0);
        g_inputs[0] = Common::AddControl(window, L"EDIT", L"", ES_AUTOHSCROLL | WS_TABSTOP,
                                     735, 50, 200, 24, 400);
        SendMessage(g_inputs[0], EM_SETLIMITTEXT, 6, 0);
        g_queryButton = Common::AddControl(window, L"BUTTON", L"조회", WS_TABSTOP, 950, 48, 140, 29, QUERY_BUTTON);
        g_output = Common::AddControl(window, L"STATIC", L"", 0, 580, 85, 540, 22, 0);
        g_list = Common::AddControl(window, WC_LISTVIEWW, L"", LVS_REPORT | LVS_SINGLESEL | WS_TABSTOP,
                                    580, 110, 540, 220, 302);
        g_book = Common::AddControl(window, WC_LISTVIEWW, L"", LVS_REPORT | LVS_SINGLESEL | WS_TABSTOP,
                                    580, 339, 540, 242, 303);
        if (!g_list || !g_book) return -1;
        ListView_SetExtendedListViewStyle(g_list, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
        ListView_SetExtendedListViewStyle(g_book, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
        AddColumn(g_list, 0, L"현재가 및 종목 정보", 220);
        AddColumn(g_list, 1, L"값", 280);
        AddColumn(g_book, 0, L"단계", 55);
        AddColumn(g_book, 1, L"매도호가", 96);
        AddColumn(g_book, 2, L"매수호가", 96);
        AddColumn(g_book, 3, L"매도잔량", 96);
        AddColumn(g_book, 4, L"매수잔량", 96);
        AddColumn(g_book, 5, L"LP 매도수량", 96);
        AddColumn(g_book, 6, L"LP 매수수량", 96);
        AddColumn(g_book, 7, L"매도 증감", 96);
        AddColumn(g_book, 8, L"매수 증감", 96);
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
    wc.lpszClassName = L"OrdaAPIs1501ElwOrderBook";
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
