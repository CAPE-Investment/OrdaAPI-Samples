// 읽는 순서: WinMain 초기화 → 공통 로그인 통지 → 조회 요청 → 수신 → 해제 → 다음 조회 또는 종료.
// VS2010 v100 / Win32 / MultiByte / C++03. 공통 Helper는 변경하지 않습니다.
#include "../../../common/OrdaSampleCommon.h"
// 수신 블록명은 서비스 XML의 이름을 사용합니다.
#pragma warning(push)
#pragma warning(disable: 4005)
#include "t2012.h"
#pragma warning(pop)
#include <string.h>
#include <stdio.h>
#include <limits.h>
#include <string>
#include <vector>
#include <deque>
#include <commctrl.h>

namespace Common = OrdaSampleCommon;
const char SERVICE_CODE[] = NAME_t2012;
const wchar_t SERVICE_TITLE[] = L"시간대별투자자매매추이(t2012)";
const int QUERY_BUTTON = 300, DISPLAY_LIMIT = 200;
const int INPUT_COUNT = 5;
HWND g_inputs[INPUT_COUNT], g_queryButton, g_output, g_list, g_fixedList;
int g_requestId = -1, g_recordCount = 0;
const wchar_t *g_measure = L"";
BOOL g_submitting = FALSE, g_processing = FALSE, g_closing = FALSE;
BOOL g_closeRequested = FALSE, g_queryError = FALSE;
BOOL g_serverMessageReceived = FALSE;
BOOL g_fixedReceived = FALSE;

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

BOOL BuildInput(t2012InBlock &input)
{
    // 속성 바이트와 전문에서 공백이 기본값인 필드를 함께 초기화합니다.
    memset(&input, ' ', sizeof(input));
    if (!CopyText(g_inputs[0], input.market, sizeof(input.market), FALSE, TRUE)) return FALSE;
    if (!CopyText(g_inputs[1], input.upcode, sizeof(input.upcode), FALSE, TRUE)) return FALSE;
    if (!CopyCode(g_inputs[2], input.gubun1[0])) return FALSE;
    if (!CopyCode(g_inputs[3], input.gubun2[0])) return FALSE;
    if (!CopyText(g_inputs[4], input.cnt, sizeof(input.cnt), FALSE, TRUE)) return FALSE;
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

void ReceiveFixed(const t2012OutBlock &fixed)
{
    g_fixedReceived = TRUE;
    ListView_DeleteAllItems(g_fixedList);
    SetCell(g_fixedList, 0, 0, "CTSTIME");
    SetCell(g_fixedList, 0, 1, Field(fixed.cts_time, sizeof(fixed.cts_time)));
    SetCell(g_fixedList, 1, 0, "개인투자자코드");
    SetCell(g_fixedList, 1, 1, Field(fixed.tjjcode_08, sizeof(fixed.tjjcode_08)));
    SetCell(g_fixedList, 2, 0, "개인매수");
    SetCell(g_fixedList, 2, 1, Field(fixed.ms_08, sizeof(fixed.ms_08)));
    SetCell(g_fixedList, 3, 0, "개인매도");
    SetCell(g_fixedList, 3, 1, Field(fixed.md_08, sizeof(fixed.md_08)));
    SetCell(g_fixedList, 4, 0, "개인증감");
    SetCell(g_fixedList, 4, 1, Field(fixed.rate_08, sizeof(fixed.rate_08)));
    SetCell(g_fixedList, 5, 0, "개인순매수");
    SetCell(g_fixedList, 5, 1, Field(fixed.svolume_08, sizeof(fixed.svolume_08)));
    SetCell(g_fixedList, 6, 0, "외국인투자자코드");
    SetCell(g_fixedList, 6, 1, Field(fixed.tjjcode_17, sizeof(fixed.tjjcode_17)));
    SetCell(g_fixedList, 7, 0, "외국인매수");
    SetCell(g_fixedList, 7, 1, Field(fixed.ms_17, sizeof(fixed.ms_17)));
    SetCell(g_fixedList, 8, 0, "외국인매도");
    SetCell(g_fixedList, 8, 1, Field(fixed.md_17, sizeof(fixed.md_17)));
    SetCell(g_fixedList, 9, 0, "외국인증감");
    SetCell(g_fixedList, 9, 1, Field(fixed.rate_17, sizeof(fixed.rate_17)));
    SetCell(g_fixedList, 10, 0, "외국인순매수");
    SetCell(g_fixedList, 10, 1, Field(fixed.svolume_17, sizeof(fixed.svolume_17)));
    SetCell(g_fixedList, 11, 0, "기관계투자자코드");
    SetCell(g_fixedList, 11, 1, Field(fixed.tjjcode_18, sizeof(fixed.tjjcode_18)));
    SetCell(g_fixedList, 12, 0, "기관계매수");
    SetCell(g_fixedList, 12, 1, Field(fixed.ms_18, sizeof(fixed.ms_18)));
    SetCell(g_fixedList, 13, 0, "기관계매도");
    SetCell(g_fixedList, 13, 1, Field(fixed.md_18, sizeof(fixed.md_18)));
    SetCell(g_fixedList, 14, 0, "기관계증감");
    SetCell(g_fixedList, 14, 1, Field(fixed.rate_18, sizeof(fixed.rate_18)));
    SetCell(g_fixedList, 15, 0, "기관계순매수");
    SetCell(g_fixedList, 15, 1, Field(fixed.svolume_18, sizeof(fixed.svolume_18)));
    SetCell(g_fixedList, 16, 0, "증권투자자코드");
    SetCell(g_fixedList, 16, 1, Field(fixed.tjjcode_01, sizeof(fixed.tjjcode_01)));
    SetCell(g_fixedList, 17, 0, "증권매수");
    SetCell(g_fixedList, 17, 1, Field(fixed.ms_01, sizeof(fixed.ms_01)));
    SetCell(g_fixedList, 18, 0, "증권매도");
    SetCell(g_fixedList, 18, 1, Field(fixed.md_01, sizeof(fixed.md_01)));
    SetCell(g_fixedList, 19, 0, "증권증감");
    SetCell(g_fixedList, 19, 1, Field(fixed.rate_01, sizeof(fixed.rate_01)));
    SetCell(g_fixedList, 20, 0, "증권순매수");
    SetCell(g_fixedList, 20, 1, Field(fixed.svolume_01, sizeof(fixed.svolume_01)));
    SetCell(g_fixedList, 21, 0, "투신투자자코드");
    SetCell(g_fixedList, 21, 1, Field(fixed.tjjcode_03, sizeof(fixed.tjjcode_03)));
    SetCell(g_fixedList, 22, 0, "투신매수");
    SetCell(g_fixedList, 22, 1, Field(fixed.ms_03, sizeof(fixed.ms_03)));
    SetCell(g_fixedList, 23, 0, "투신매도");
    SetCell(g_fixedList, 23, 1, Field(fixed.md_03, sizeof(fixed.md_03)));
    SetCell(g_fixedList, 24, 0, "투신증감");
    SetCell(g_fixedList, 24, 1, Field(fixed.rate_03, sizeof(fixed.rate_03)));
    SetCell(g_fixedList, 25, 0, "투신순매수");
    SetCell(g_fixedList, 25, 1, Field(fixed.svolume_03, sizeof(fixed.svolume_03)));
    SetCell(g_fixedList, 26, 0, "은행투자자코드");
    SetCell(g_fixedList, 26, 1, Field(fixed.tjjcode_04, sizeof(fixed.tjjcode_04)));
    SetCell(g_fixedList, 27, 0, "은행매수");
    SetCell(g_fixedList, 27, 1, Field(fixed.ms_04, sizeof(fixed.ms_04)));
    SetCell(g_fixedList, 28, 0, "은행매도");
    SetCell(g_fixedList, 28, 1, Field(fixed.md_04, sizeof(fixed.md_04)));
    SetCell(g_fixedList, 29, 0, "은행증감");
    SetCell(g_fixedList, 29, 1, Field(fixed.rate_04, sizeof(fixed.rate_04)));
    SetCell(g_fixedList, 30, 0, "은행순매수");
    SetCell(g_fixedList, 30, 1, Field(fixed.svolume_04, sizeof(fixed.svolume_04)));
    SetCell(g_fixedList, 31, 0, "보험투자자코드");
    SetCell(g_fixedList, 31, 1, Field(fixed.tjjcode_02, sizeof(fixed.tjjcode_02)));
    SetCell(g_fixedList, 32, 0, "보험매수");
    SetCell(g_fixedList, 32, 1, Field(fixed.ms_02, sizeof(fixed.ms_02)));
    SetCell(g_fixedList, 33, 0, "보험매도");
    SetCell(g_fixedList, 33, 1, Field(fixed.md_02, sizeof(fixed.md_02)));
    SetCell(g_fixedList, 34, 0, "보험증감");
    SetCell(g_fixedList, 34, 1, Field(fixed.rate_02, sizeof(fixed.rate_02)));
    SetCell(g_fixedList, 35, 0, "보험순매수");
    SetCell(g_fixedList, 35, 1, Field(fixed.svolume_02, sizeof(fixed.svolume_02)));
    SetCell(g_fixedList, 36, 0, "종금투자자코드");
    SetCell(g_fixedList, 36, 1, Field(fixed.tjjcode_05, sizeof(fixed.tjjcode_05)));
    SetCell(g_fixedList, 37, 0, "종금매수");
    SetCell(g_fixedList, 37, 1, Field(fixed.ms_05, sizeof(fixed.ms_05)));
    SetCell(g_fixedList, 38, 0, "종금매도");
    SetCell(g_fixedList, 38, 1, Field(fixed.md_05, sizeof(fixed.md_05)));
    SetCell(g_fixedList, 39, 0, "종금증감");
    SetCell(g_fixedList, 39, 1, Field(fixed.rate_05, sizeof(fixed.rate_05)));
    SetCell(g_fixedList, 40, 0, "종금순매수");
    SetCell(g_fixedList, 40, 1, Field(fixed.svolume_05, sizeof(fixed.svolume_05)));
    SetCell(g_fixedList, 41, 0, "기금투자자코드");
    SetCell(g_fixedList, 41, 1, Field(fixed.tjjcode_06, sizeof(fixed.tjjcode_06)));
    SetCell(g_fixedList, 42, 0, "기금매수");
    SetCell(g_fixedList, 42, 1, Field(fixed.ms_06, sizeof(fixed.ms_06)));
    SetCell(g_fixedList, 43, 0, "기금매도");
    SetCell(g_fixedList, 43, 1, Field(fixed.md_06, sizeof(fixed.md_06)));
    SetCell(g_fixedList, 44, 0, "기금증감");
    SetCell(g_fixedList, 44, 1, Field(fixed.rate_06, sizeof(fixed.rate_06)));
    SetCell(g_fixedList, 45, 0, "기금순매수");
    SetCell(g_fixedList, 45, 1, Field(fixed.svolume_06, sizeof(fixed.svolume_06)));
    SetCell(g_fixedList, 46, 0, "기타투자자코드");
    SetCell(g_fixedList, 46, 1, Field(fixed.tjjcode_07, sizeof(fixed.tjjcode_07)));
    SetCell(g_fixedList, 47, 0, "기타매수");
    SetCell(g_fixedList, 47, 1, Field(fixed.ms_07, sizeof(fixed.ms_07)));
    SetCell(g_fixedList, 48, 0, "기타매도");
    SetCell(g_fixedList, 48, 1, Field(fixed.md_07, sizeof(fixed.md_07)));
    SetCell(g_fixedList, 49, 0, "기타증감");
    SetCell(g_fixedList, 49, 1, Field(fixed.rate_07, sizeof(fixed.rate_07)));
    SetCell(g_fixedList, 50, 0, "기타순매수");
    SetCell(g_fixedList, 50, 1, Field(fixed.svolume_07, sizeof(fixed.svolume_07)));
    SetCell(g_fixedList, 51, 0, "국가투자자코드");
    SetCell(g_fixedList, 51, 1, Field(fixed.tjjcode_11, sizeof(fixed.tjjcode_11)));
    SetCell(g_fixedList, 52, 0, "국가매수");
    SetCell(g_fixedList, 52, 1, Field(fixed.ms_11, sizeof(fixed.ms_11)));
    SetCell(g_fixedList, 53, 0, "국가매도");
    SetCell(g_fixedList, 53, 1, Field(fixed.md_11, sizeof(fixed.md_11)));
    SetCell(g_fixedList, 54, 0, "국가증감");
    SetCell(g_fixedList, 54, 1, Field(fixed.rate_11, sizeof(fixed.rate_11)));
    SetCell(g_fixedList, 55, 0, "국가순매수");
    SetCell(g_fixedList, 55, 1, Field(fixed.svolume_11, sizeof(fixed.svolume_11)));
    SetCell(g_fixedList, 56, 0, "사모펀드코드");
    SetCell(g_fixedList, 56, 1, Field(fixed.tjjcode_00, sizeof(fixed.tjjcode_00)));
    SetCell(g_fixedList, 57, 0, "사모펀드매수");
    SetCell(g_fixedList, 57, 1, Field(fixed.ms_00, sizeof(fixed.ms_00)));
    SetCell(g_fixedList, 58, 0, "사모펀드매도");
    SetCell(g_fixedList, 58, 1, Field(fixed.md_00, sizeof(fixed.md_00)));
    SetCell(g_fixedList, 59, 0, "사모펀드증감");
    SetCell(g_fixedList, 59, 1, Field(fixed.rate_00, sizeof(fixed.rate_00)));
    SetCell(g_fixedList, 60, 0, "사모펀드순매수");
    SetCell(g_fixedList, 60, 1, Field(fixed.svolume_00, sizeof(fixed.svolume_00)));
}

// 블록 길이를 레코드 크기로 나누어 실제 건수를 계산합니다.
BOOL ReceiveRows(const unsigned char *data, int length)
{
    int count = length / sizeof(t2012OutBlock1);
    if (count > INT_MAX - g_recordCount) return FALSE;
    for (int row = 0; row < count && g_recordCount + row < DISPLAY_LIMIT; ++row)
    {
        t2012OutBlock1 output;
        memcpy(&output, data + row * sizeof(output), sizeof(output));
        int displayRow = g_recordCount + row;
        // 전문의 부호와 소수점 표현을 유지합니다.
        SetCell(g_list, displayRow, 0, Field(output.time, sizeof(output.time)));
        SetCell(g_list, displayRow, 1, Field(output.sv_08, sizeof(output.sv_08)));
        SetCell(g_list, displayRow, 2, Field(output.sv_17, sizeof(output.sv_17)));
        SetCell(g_list, displayRow, 3, Field(output.sv_18, sizeof(output.sv_18)));
        SetCell(g_list, displayRow, 4, Field(output.sv_01, sizeof(output.sv_01)));
        SetCell(g_list, displayRow, 5, Field(output.sv_03, sizeof(output.sv_03)));
        SetCell(g_list, displayRow, 6, Field(output.sv_04, sizeof(output.sv_04)));
        SetCell(g_list, displayRow, 7, Field(output.sv_02, sizeof(output.sv_02)));
        SetCell(g_list, displayRow, 8, Field(output.sv_05, sizeof(output.sv_05)));
        SetCell(g_list, displayRow, 9, Field(output.sv_06, sizeof(output.sv_06)));
        SetCell(g_list, displayRow, 10, Field(output.sv_07, sizeof(output.sv_07)));
        SetCell(g_list, displayRow, 11, Field(output.sv_11, sizeof(output.sv_11)));
        SetCell(g_list, displayRow, 12, Field(output.sv_00, sizeof(output.sv_00)));
    }
    g_recordCount += count;
    return TRUE;
}

void ShowOutput()
{
    wchar_t heading[128];
    swprintf_s(heading, L"%ls / 수신 %d건 / 표시 %d건",
               g_measure, g_recordCount, ListView_GetItemCount(g_list));
    SetWindowTextW(g_output, heading);
}

// SDK가 조립한 A형 통지는 블록 이름으로 구분합니다.
BOOL ValidBody(const RP_DATA_INFO *response)
{
    if (!response || response->nDataLength < 0 ||
        response->nDataLength > response->nTotalDataBufferSize ||
        (!response->lpData && response->nDataLength > 0)) return FALSE;
    std::string block = Field(response->szBlockName, sizeof(response->szBlockName));
    return (block == "t2012OutBlock" && response->nDataLength == sizeof(t2012OutBlock)) ||
           (block == "t2012OutBlock1" && response->nDataLength % sizeof(t2012OutBlock1) == 0);
}
BOOL ReceiveData(const RP_DATA_INFO *response)
{
    if (!response || response->nRqID != g_requestId || g_requestId < 0) return FALSE;
    if (Field(response->Header.szTrCode, sizeof(response->Header.szTrCode)) != SERVICE_CODE || !ValidBody(response)) return FALSE;
    if (response->Header.szRetFlag[0] >= '1' && response->Header.szRetFlag[0] <= '9') return FALSE;
    if (Field(response->szBlockName, sizeof(response->szBlockName)) == "t2012OutBlock")
    {
        t2012OutBlock fixed;
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
    t2012InBlock input;
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
    g_fixedReceived = FALSE;
    ClearOutput();
    g_measure = input.gubun1[0] == '2' ? L"금액 조회" : L"수량 조회";
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
        Common::AddControl(window, L"STATIC", L"시장구분", 0, 580, 52, 155, 22, 0);
        g_inputs[0] = Common::AddControl(window, L"EDIT", L"1", ES_AUTOHSCROLL | WS_TABSTOP, 735, 50, 100, 24, 400);
        SendMessage(g_inputs[0], EM_SETLIMITTEXT, 1, 0);
        Common::AddControl(window, L"STATIC", L"업종코드", 0, 580, 84, 155, 22, 0);
        g_inputs[1] = Common::AddControl(window, L"EDIT", L"001", ES_AUTOHSCROLL | WS_TABSTOP, 735, 82, 100, 24, 401);
        SendMessage(g_inputs[1], EM_SETLIMITTEXT, 3, 0);
        Common::AddControl(window, L"STATIC", L"수량구분", 0, 580, 116, 155, 22, 0);
        g_inputs[2] = Common::AddControl(window, L"COMBOBOX", L"", CBS_DROPDOWNLIST | WS_TABSTOP, 735, 114, 100, 150, 402);
        SendMessage(g_inputs[2], CB_SETDROPPEDWIDTH, 155, 0);
        AddChoice(g_inputs[2], L"1 수량", '1', '2');
        AddChoice(g_inputs[2], L"2 금액", '2', '2');
        Common::AddControl(window, L"STATIC", L"전일분구분", 0, 580, 148, 155, 22, 0);
        g_inputs[3] = Common::AddControl(window, L"COMBOBOX", L"", CBS_DROPDOWNLIST | WS_TABSTOP, 735, 146, 100, 150, 403);
        SendMessage(g_inputs[3], CB_SETDROPPEDWIDTH, 155, 0);
        AddChoice(g_inputs[3], L"0 당일", '0', '1');
        AddChoice(g_inputs[3], L"1 전일", '1', '1');
        Common::AddControl(window, L"STATIC", L"조회건수", 0, 845, 52, 155, 22, 0);
        g_inputs[4] = Common::AddControl(window, L"EDIT", L"0020", ES_AUTOHSCROLL | WS_TABSTOP, 1000, 50, 100, 24, 404);
        SendMessage(g_inputs[4], EM_SETLIMITTEXT, 4, 0);
        g_queryButton = Common::AddControl(window, L"BUTTON", L"조회", WS_TABSTOP, 790, 182, 140, 30, QUERY_BUTTON);

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
        AddColumn(g_list, 0, L"시간", 90);
        AddColumn(g_list, 1, L"개인순매수", 92);
        AddColumn(g_list, 2, L"외국인순매수", 106);
        AddColumn(g_list, 3, L"기관계순매수", 106);
        AddColumn(g_list, 4, L"증권순매수", 92);
        AddColumn(g_list, 5, L"투신순매수", 92);
        AddColumn(g_list, 6, L"은행순매수", 92);
        AddColumn(g_list, 7, L"보험순매수", 92);
        AddColumn(g_list, 8, L"종금순매수", 92);
        AddColumn(g_list, 9, L"기금순매수", 92);
        AddColumn(g_list, 10, L"기타순매수", 92);
        AddColumn(g_list, 11, L"국가순매수", 92);
        AddColumn(g_list, 12, L"사모펀드순매수", 120);
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
    wc.lpszClassName = L"OrdaAPIt2012InvestorTrends";
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
