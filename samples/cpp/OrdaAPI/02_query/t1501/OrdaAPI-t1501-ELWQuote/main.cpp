// 읽는 순서: WinMain 초기화 → 공통 로그인 통지 → 조회 요청 → 수신 → 해제 → 다음 조회 또는 종료.
// VS2010 v100 / Win32 / MultiByte / C++03. 공통 Helper는 변경하지 않습니다.
#include "../../../common/OrdaSampleCommon.h"
// 수신 블록명은 서비스 XML의 이름을 사용합니다.
#pragma warning(push)
#pragma warning(disable: 4005)
#include "t1501.h"
#pragma warning(pop)
#include <string.h>
#include <stdio.h>
#include <limits.h>
#include <string>
#include <vector>
#include <deque>
#include <commctrl.h>

namespace Common = OrdaSampleCommon;
const char SERVICE_CODE[] = NAME_t1501;
const wchar_t SERVICE_TITLE[] = L"ELW현재가조회(t1501)";
const int QUERY_BUTTON = 300, DISPLAY_LIMIT = 200;
const int INPUT_COUNT = 1;
HWND g_inputs[INPUT_COUNT], g_queryButton, g_output, g_list, g_fixedList;
int g_requestId = -1, g_recordCount = 0;
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

BOOL BuildInput(t1501InBlock &input)
{
    // 속성 바이트와 전문에서 공백이 기본값인 필드를 함께 초기화합니다.
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

void ReceiveFixed(const t1501OutBlock &fixed)
{
    g_fixedReceived = TRUE;
    ListView_DeleteAllItems(g_fixedList);
    SetCell(g_fixedList, 0, 0, "한글명");
    SetCell(g_fixedList, 0, 1, Field(fixed.hname, sizeof(fixed.hname)));
    SetCell(g_fixedList, 1, 0, "체결시간");
    SetCell(g_fixedList, 1, 1, Field(fixed.chetime, sizeof(fixed.chetime)));
    SetCell(g_fixedList, 2, 0, "현재가");
    SetCell(g_fixedList, 2, 1, Field(fixed.price, sizeof(fixed.price)));
    SetCell(g_fixedList, 3, 0, "전일대비구분");
    SetCell(g_fixedList, 3, 1, Field(fixed.sign, sizeof(fixed.sign)));
    SetCell(g_fixedList, 4, 0, "전일대비");
    SetCell(g_fixedList, 4, 1, Field(fixed.change, sizeof(fixed.change)));
    SetCell(g_fixedList, 5, 0, "등락율");
    SetCell(g_fixedList, 5, 1, Field(fixed.diff, sizeof(fixed.diff)));
    SetCell(g_fixedList, 6, 0, "체결량");
    SetCell(g_fixedList, 6, 1, Field(fixed.cvolume, sizeof(fixed.cvolume)));
    SetCell(g_fixedList, 7, 0, "누적거래량");
    SetCell(g_fixedList, 7, 1, Field(fixed.volume, sizeof(fixed.volume)));
    SetCell(g_fixedList, 8, 0, "기준가");
    SetCell(g_fixedList, 8, 1, Field(fixed.recprice, sizeof(fixed.recprice)));
    SetCell(g_fixedList, 9, 0, "가중평균");
    SetCell(g_fixedList, 9, 1, Field(fixed.avg, sizeof(fixed.avg)));
    SetCell(g_fixedList, 10, 0, "전일거래량");
    SetCell(g_fixedList, 10, 1, Field(fixed.jnilvolume, sizeof(fixed.jnilvolume)));
    SetCell(g_fixedList, 11, 0, "전일동시간거래량");
    SetCell(g_fixedList, 11, 1, Field(fixed.jvolume, sizeof(fixed.jvolume)));
    SetCell(g_fixedList, 12, 0, "전일종가");
    SetCell(g_fixedList, 12, 1, Field(fixed.jnilclose, sizeof(fixed.jnilclose)));
    SetCell(g_fixedList, 13, 0, "거래량차");
    SetCell(g_fixedList, 13, 1, Field(fixed.volumechg, sizeof(fixed.volumechg)));
    SetCell(g_fixedList, 14, 0, "거래량차등락율");
    SetCell(g_fixedList, 14, 1, Field(fixed.volumediff, sizeof(fixed.volumediff)));
    SetCell(g_fixedList, 15, 0, "시가");
    SetCell(g_fixedList, 15, 1, Field(fixed.open, sizeof(fixed.open)));
    SetCell(g_fixedList, 16, 0, "시가등락율");
    SetCell(g_fixedList, 16, 1, Field(fixed.odiff, sizeof(fixed.odiff)));
    SetCell(g_fixedList, 17, 0, "시가시간");
    SetCell(g_fixedList, 17, 1, Field(fixed.opentime, sizeof(fixed.opentime)));
    SetCell(g_fixedList, 18, 0, "고가");
    SetCell(g_fixedList, 18, 1, Field(fixed.high, sizeof(fixed.high)));
    SetCell(g_fixedList, 19, 0, "고가등락율");
    SetCell(g_fixedList, 19, 1, Field(fixed.hdiff, sizeof(fixed.hdiff)));
    SetCell(g_fixedList, 20, 0, "고가시간");
    SetCell(g_fixedList, 20, 1, Field(fixed.hightime, sizeof(fixed.hightime)));
    SetCell(g_fixedList, 21, 0, "저가");
    SetCell(g_fixedList, 21, 1, Field(fixed.low, sizeof(fixed.low)));
    SetCell(g_fixedList, 22, 0, "저가등락율");
    SetCell(g_fixedList, 22, 1, Field(fixed.ldiff, sizeof(fixed.ldiff)));
    SetCell(g_fixedList, 23, 0, "저가시간");
    SetCell(g_fixedList, 23, 1, Field(fixed.lowtime, sizeof(fixed.lowtime)));
    SetCell(g_fixedList, 24, 0, "52최고가");
    SetCell(g_fixedList, 24, 1, Field(fixed.high52w, sizeof(fixed.high52w)));
    SetCell(g_fixedList, 25, 0, "52최고가등락율");
    SetCell(g_fixedList, 25, 1, Field(fixed.high52wdiff, sizeof(fixed.high52wdiff)));
    SetCell(g_fixedList, 26, 0, "52최고가일");
    SetCell(g_fixedList, 26, 1, Field(fixed.high52wdate, sizeof(fixed.high52wdate)));
    SetCell(g_fixedList, 27, 0, "52최저가");
    SetCell(g_fixedList, 27, 1, Field(fixed.low52w, sizeof(fixed.low52w)));
    SetCell(g_fixedList, 28, 0, "52최저가등락율");
    SetCell(g_fixedList, 28, 1, Field(fixed.low52wdiff, sizeof(fixed.low52wdiff)));
    SetCell(g_fixedList, 29, 0, "52최저가일");
    SetCell(g_fixedList, 29, 1, Field(fixed.low52wdate, sizeof(fixed.low52wdate)));
    SetCell(g_fixedList, 30, 0, "소진율");
    SetCell(g_fixedList, 30, 1, Field(fixed.exhratio, sizeof(fixed.exhratio)));
    SetCell(g_fixedList, 31, 0, "상장주식수(천)");
    SetCell(g_fixedList, 31, 1, Field(fixed.listing, sizeof(fixed.listing)));
    SetCell(g_fixedList, 32, 0, "수량단위");
    SetCell(g_fixedList, 32, 1, Field(fixed.memedan, sizeof(fixed.memedan)));
    SetCell(g_fixedList, 33, 0, "회전율");
    SetCell(g_fixedList, 33, 1, Field(fixed.vol, sizeof(fixed.vol)));
    SetCell(g_fixedList, 34, 0, "패리티");
    SetCell(g_fixedList, 34, 1, Field(fixed.parity, sizeof(fixed.parity)));
    SetCell(g_fixedList, 35, 0, "손익분기");
    SetCell(g_fixedList, 35, 1, Field(fixed.berate, sizeof(fixed.berate)));
    SetCell(g_fixedList, 36, 0, "기어링");
    SetCell(g_fixedList, 36, 1, Field(fixed.gearing, sizeof(fixed.gearing)));
    SetCell(g_fixedList, 37, 0, "행사가");
    SetCell(g_fixedList, 37, 1, Field(fixed.elwexec, sizeof(fixed.elwexec)));
    SetCell(g_fixedList, 38, 0, "발행가");
    SetCell(g_fixedList, 38, 1, Field(fixed.issueprice, sizeof(fixed.issueprice)));
    SetCell(g_fixedList, 39, 0, "전환비율");
    SetCell(g_fixedList, 39, 1, Field(fixed.convrate, sizeof(fixed.convrate)));
    SetCell(g_fixedList, 40, 0, "최종거래일");
    SetCell(g_fixedList, 40, 1, Field(fixed.lastdate, sizeof(fixed.lastdate)));
    SetCell(g_fixedList, 41, 0, "자본지지");
    SetCell(g_fixedList, 41, 1, Field(fixed.capt, sizeof(fixed.capt)));
    SetCell(g_fixedList, 42, 0, "e.기어링");
    SetCell(g_fixedList, 42, 1, Field(fixed.egearing, sizeof(fixed.egearing)));
    SetCell(g_fixedList, 43, 0, "프리미엄");
    SetCell(g_fixedList, 43, 1, Field(fixed.premium, sizeof(fixed.premium)));
    SetCell(g_fixedList, 44, 0, "스프레드");
    SetCell(g_fixedList, 44, 1, Field(fixed.spread, sizeof(fixed.spread)));
    SetCell(g_fixedList, 45, 0, "최대스프레드");
    SetCell(g_fixedList, 45, 1, Field(fixed.espread, sizeof(fixed.espread)));
    SetCell(g_fixedList, 46, 0, "이론가");
    SetCell(g_fixedList, 46, 1, Field(fixed.theoryprice, sizeof(fixed.theoryprice)));
    SetCell(g_fixedList, 47, 0, "내재변동성");
    SetCell(g_fixedList, 47, 1, Field(fixed.impv, sizeof(fixed.impv)));
    SetCell(g_fixedList, 48, 0, "상태");
    SetCell(g_fixedList, 48, 1, Field(fixed.moneyness, sizeof(fixed.moneyness)));
    SetCell(g_fixedList, 49, 0, "델타");
    SetCell(g_fixedList, 49, 1, Field(fixed.delt, sizeof(fixed.delt)));
    SetCell(g_fixedList, 50, 0, "감마");
    SetCell(g_fixedList, 50, 1, Field(fixed.gama, sizeof(fixed.gama)));
    SetCell(g_fixedList, 51, 0, "베가");
    SetCell(g_fixedList, 51, 1, Field(fixed.vega, sizeof(fixed.vega)));
    SetCell(g_fixedList, 52, 0, "쎄타");
    SetCell(g_fixedList, 52, 1, Field(fixed.ceta, sizeof(fixed.ceta)));
    SetCell(g_fixedList, 53, 0, "로");
    SetCell(g_fixedList, 53, 1, Field(fixed.rhox, sizeof(fixed.rhox)));
    SetCell(g_fixedList, 54, 0, "잔존일수");
    SetCell(g_fixedList, 54, 1, Field(fixed.bjandatecnt, sizeof(fixed.bjandatecnt)));
    SetCell(g_fixedList, 55, 0, "행사개시일");
    SetCell(g_fixedList, 55, 1, Field(fixed.mmsdate, sizeof(fixed.mmsdate)));
    SetCell(g_fixedList, 56, 0, "행사종료일");
    SetCell(g_fixedList, 56, 1, Field(fixed.mmedate, sizeof(fixed.mmedate)));
    SetCell(g_fixedList, 57, 0, "지급일");
    SetCell(g_fixedList, 57, 1, Field(fixed.payday, sizeof(fixed.payday)));
    SetCell(g_fixedList, 58, 0, "발행일");
    SetCell(g_fixedList, 58, 1, Field(fixed.listdate, sizeof(fixed.listdate)));
    SetCell(g_fixedList, 59, 0, "LP회원사");
    SetCell(g_fixedList, 59, 1, Field(fixed.lpmem, sizeof(fixed.lpmem)));
    SetCell(g_fixedList, 60, 0, "LP보유수량");
    SetCell(g_fixedList, 60, 1, Field(fixed.lp_holdvol, sizeof(fixed.lp_holdvol)));
    SetCell(g_fixedList, 61, 0, "기초자산코드");
    SetCell(g_fixedList, 61, 1, Field(fixed.bcode, sizeof(fixed.bcode)));
    SetCell(g_fixedList, 62, 0, "기초자산구분");
    SetCell(g_fixedList, 62, 1, Field(fixed.bgubun, sizeof(fixed.bgubun)));
    SetCell(g_fixedList, 63, 0, "기초자산현재가");
    SetCell(g_fixedList, 63, 1, Field(fixed.bprice, sizeof(fixed.bprice)));
    SetCell(g_fixedList, 64, 0, "기초자산전일비구분");
    SetCell(g_fixedList, 64, 1, Field(fixed.bsign, sizeof(fixed.bsign)));
    SetCell(g_fixedList, 65, 0, "기초자산전일비");
    SetCell(g_fixedList, 65, 1, Field(fixed.bchange, sizeof(fixed.bchange)));
    SetCell(g_fixedList, 66, 0, "기초자산등락율");
    SetCell(g_fixedList, 66, 1, Field(fixed.bdiff, sizeof(fixed.bdiff)));
    SetCell(g_fixedList, 67, 0, "기초자산거래량");
    SetCell(g_fixedList, 67, 1, Field(fixed.bvolume, sizeof(fixed.bvolume)));
    SetCell(g_fixedList, 68, 0, "락구분");
    SetCell(g_fixedList, 68, 1, Field(fixed.info1, sizeof(fixed.info1)));
    SetCell(g_fixedList, 69, 0, "관리/급등구분");
    SetCell(g_fixedList, 69, 1, Field(fixed.info2, sizeof(fixed.info2)));
    SetCell(g_fixedList, 70, 0, "정지/연장구분");
    SetCell(g_fixedList, 70, 1, Field(fixed.info3, sizeof(fixed.info3)));
    SetCell(g_fixedList, 71, 0, "투자/불성실구분");
    SetCell(g_fixedList, 71, 1, Field(fixed.info4, sizeof(fixed.info4)));
    SetCell(g_fixedList, 72, 0, "장구분");
    SetCell(g_fixedList, 72, 1, Field(fixed.janginfo, sizeof(fixed.janginfo)));
    SetCell(g_fixedList, 73, 0, "바스켓구분");
    SetCell(g_fixedList, 73, 1, Field(fixed.basketgb, sizeof(fixed.basketgb)));
    SetCell(g_fixedList, 74, 0, "바스켓갯수");
    SetCell(g_fixedList, 74, 1, Field(fixed.basketcnt, sizeof(fixed.basketcnt)));
    SetCell(g_fixedList, 75, 0, "ELW권리행사방식");
    SetCell(g_fixedList, 75, 1, Field(fixed.elwtype, sizeof(fixed.elwtype)));
    SetCell(g_fixedList, 76, 0, "ELW결제방법");
    SetCell(g_fixedList, 76, 1, Field(fixed.settletype, sizeof(fixed.settletype)));
    SetCell(g_fixedList, 77, 0, "LP사주문가능여부");
    SetCell(g_fixedList, 77, 1, Field(fixed.lpord, sizeof(fixed.lpord)));
    SetCell(g_fixedList, 78, 0, "권리내용");
    SetCell(g_fixedList, 78, 1, Field(fixed.elwdetail, sizeof(fixed.elwdetail)));
    SetCell(g_fixedList, 79, 0, "만기평가가격방식");
    SetCell(g_fixedList, 79, 1, Field(fixed.valuation, sizeof(fixed.valuation)));
    SetCell(g_fixedList, 80, 0, "확정지급액");
    SetCell(g_fixedList, 80, 1, Field(fixed.givemoney, sizeof(fixed.givemoney)));
}

// 블록 길이를 레코드 크기로 나누어 실제 건수를 계산합니다.
BOOL ReceiveRows(const unsigned char *data, int length)
{
    int count = length / sizeof(t1501OutBlock1);
    if (count > INT_MAX - g_recordCount) return FALSE;
    for (int row = 0; row < count && g_recordCount + row < DISPLAY_LIMIT; ++row)
    {
        t1501OutBlock1 output;
        memcpy(&output, data + row * sizeof(output), sizeof(output));
        int displayRow = g_recordCount + row;
        // 전문의 부호와 소수점 표현을 유지합니다.
        SetCell(g_list, displayRow, 0, Field(output.bskcode, sizeof(output.bskcode)));
        SetCell(g_list, displayRow, 1, Field(output.bskbno, sizeof(output.bskbno)));
        SetCell(g_list, displayRow, 2, Field(output.bskprice, sizeof(output.bskprice)));
        SetCell(g_list, displayRow, 3, Field(output.bsksign, sizeof(output.bsksign)));
        SetCell(g_list, displayRow, 4, Field(output.bskchange, sizeof(output.bskchange)));
        SetCell(g_list, displayRow, 5, Field(output.bskdiff, sizeof(output.bskdiff)));
        SetCell(g_list, displayRow, 6, Field(output.bskvolume, sizeof(output.bskvolume)));
        SetCell(g_list, displayRow, 7, Field(output.bskjnilclose, sizeof(output.bskjnilclose)));
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
    return (block == "t1501OutBlock" && response->nDataLength == sizeof(t1501OutBlock)) ||
           (block == "t1501OutBlock1" && response->nDataLength % sizeof(t1501OutBlock1) == 0);
}
BOOL ReceiveData(const RP_DATA_INFO *response)
{
    if (!response || response->nRqID != g_requestId || g_requestId < 0) return FALSE;
    if (Field(response->Header.szTrCode, sizeof(response->Header.szTrCode)) != SERVICE_CODE || !ValidBody(response)) return FALSE;
    if (response->Header.szRetFlag[0] >= '1' && response->Header.szRetFlag[0] <= '9') return FALSE;
    if (Field(response->szBlockName, sizeof(response->szBlockName)) == "t1501OutBlock")
    {
        t1501OutBlock fixed;
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
    t1501InBlock input;
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
        Common::AddControl(window, L"STATIC", L"단축코드", 0, 580, 52, 155, 22, 0);
        g_inputs[0] = Common::AddControl(window, L"EDIT", L"", ES_AUTOHSCROLL | WS_TABSTOP, 735, 50, 100, 24, 400);
        SendMessage(g_inputs[0], EM_SETLIMITTEXT, 6, 0);
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
        AddColumn(g_list, 0, L"기초자산코드", 106);
        AddColumn(g_list, 1, L"기초자산비율", 106);
        AddColumn(g_list, 2, L"기초자산현재가", 120);
        AddColumn(g_list, 3, L"기초자산전일비구분", 148);
        AddColumn(g_list, 4, L"기초자산전일비", 120);
        AddColumn(g_list, 5, L"기초자산등락율", 120);
        AddColumn(g_list, 6, L"기초자산거래량", 120);
        AddColumn(g_list, 7, L"기초자산전일종가", 134);
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
    wc.lpszClassName = L"OrdaAPIt1501ELWQuote";
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
