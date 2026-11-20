// 읽는 순서: WinMain 초기화 → 공통 로그인 통지 → 조회 요청 → 수신 → 해제 → 다음 조회 또는 종료.
// VS2010 v100 / Win32 / MultiByte / C++03. 공통 Helper는 변경하지 않습니다.
#include "../../../common/OrdaSampleCommon.h"
// 수신 블록명은 서비스 XML의 이름을 사용합니다.
#include "s3009.h"
#include <string.h>
#include <stdio.h>
#include <limits.h>
#include <string>
#include <vector>
#include <deque>
#include <commctrl.h>

namespace Common = OrdaSampleCommon;
const char SERVICE_CODE[] = NAME_s3009;
const wchar_t SERVICE_TITLE[] = L"주식선물 현재가 조회(s3009)";
const int QUERY_BUTTON = 300;
HWND g_queryButton, g_output, g_list;

HWND g_focode;
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

BOOL BuildInput(s3009InBlock &input)
{
    // 선택 항목, 속성 바이트 및 최초 조회 연속키를 공백으로 초기화합니다.
    memset(&input, ' ', sizeof(input));
    if (!ReadCode(g_focode, input.focode, sizeof(input.focode))) return FALSE;
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
    if (block == NAME_s3009OutBlock) return response->nDataLength == sizeof(s3009OutBlock);
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
    if (Field(response->szBlockName, sizeof(response->szBlockName)) == NAME_s3009OutBlock)
    {
        s3009OutBlock output;
        memcpy(&output, response->lpData, sizeof(output));
        ListView_DeleteAllItems(g_list);
        SetCell(g_list, 0, 0, "한글명 (hname)");
        SetCell(g_list, 0, 1, WireField(output.hname, sizeof(output.hname)));
        SetCell(g_list, 1, 0, "현재가 (price)");
        SetCell(g_list, 1, 1, WireField(output.price, sizeof(output.price)));
        SetCell(g_list, 2, 0, "전일대비구분 (sign)");
        SetCell(g_list, 2, 1, WireField(output.sign, sizeof(output.sign)));
        SetCell(g_list, 3, 0, "전일대비 (change)");
        SetCell(g_list, 3, 1, WireField(output.change, sizeof(output.change)));
        SetCell(g_list, 4, 0, "전일종가 (jnilclose)");
        SetCell(g_list, 4, 1, WireField(output.jnilclose, sizeof(output.jnilclose)));
        SetCell(g_list, 5, 0, "등락율 (diff)");
        SetCell(g_list, 5, 1, WireField(output.diff, sizeof(output.diff)));
        SetCell(g_list, 6, 0, "거래량 (volume)");
        SetCell(g_list, 6, 1, WireField(output.volume, sizeof(output.volume)));
        SetCell(g_list, 7, 0, "거래량전일동시간비율 (stimeqrt)");
        SetCell(g_list, 7, 1, WireField(output.stimeqrt, sizeof(output.stimeqrt)));
        SetCell(g_list, 8, 0, "거래대금 (value)");
        SetCell(g_list, 8, 1, WireField(output.value, sizeof(output.value)));
        SetCell(g_list, 9, 0, "미결제량 (mgjv)");
        SetCell(g_list, 9, 1, WireField(output.mgjv, sizeof(output.mgjv)));
        SetCell(g_list, 10, 0, "미결제증감 (mgjvdiff)");
        SetCell(g_list, 10, 1, WireField(output.mgjvdiff, sizeof(output.mgjvdiff)));
        SetCell(g_list, 11, 0, "시가 (open)");
        SetCell(g_list, 11, 1, WireField(output.open, sizeof(output.open)));
        SetCell(g_list, 12, 0, "고가 (high)");
        SetCell(g_list, 12, 1, WireField(output.high, sizeof(output.high)));
        SetCell(g_list, 13, 0, "저가 (low)");
        SetCell(g_list, 13, 1, WireField(output.low, sizeof(output.low)));
        SetCell(g_list, 14, 0, "52최고가 (high52w)");
        SetCell(g_list, 14, 1, WireField(output.high52w, sizeof(output.high52w)));
        SetCell(g_list, 15, 0, "52최저가 (low52w)");
        SetCell(g_list, 15, 1, WireField(output.low52w, sizeof(output.low52w)));
        SetCell(g_list, 16, 0, "베이시스 (basis)");
        SetCell(g_list, 16, 1, WireField(output.basis, sizeof(output.basis)));
        SetCell(g_list, 17, 0, "기준가 (recprice)");
        SetCell(g_list, 17, 1, WireField(output.recprice, sizeof(output.recprice)));
        SetCell(g_list, 18, 0, "이론가 (theoryprice)");
        SetCell(g_list, 18, 1, WireField(output.theoryprice, sizeof(output.theoryprice)));
        SetCell(g_list, 19, 0, "괴리율 (glyl)");
        SetCell(g_list, 19, 1, WireField(output.glyl, sizeof(output.glyl)));
        SetCell(g_list, 20, 0, "만기일 (lastmonth)");
        SetCell(g_list, 20, 1, WireField(output.lastmonth, sizeof(output.lastmonth)));
        SetCell(g_list, 21, 0, "잔여일 (jandatecnt)");
        SetCell(g_list, 21, 1, WireField(output.jandatecnt, sizeof(output.jandatecnt)));
        SetCell(g_list, 22, 0, "종합지수 (pricejisu)");
        SetCell(g_list, 22, 1, WireField(output.pricejisu, sizeof(output.pricejisu)));
        SetCell(g_list, 23, 0, "종합지수전일대비구분 (jisusign)");
        SetCell(g_list, 23, 1, WireField(output.jisusign, sizeof(output.jisusign)));
        SetCell(g_list, 24, 0, "종합지수전일대비 (jisuchange)");
        SetCell(g_list, 24, 1, WireField(output.jisuchange, sizeof(output.jisuchange)));
        SetCell(g_list, 25, 0, "종합지수등락율 (jisudiff)");
        SetCell(g_list, 25, 1, WireField(output.jisudiff, sizeof(output.jisudiff)));
        SetCell(g_list, 26, 0, "KOSPI200지수 (kospijisu)");
        SetCell(g_list, 26, 1, WireField(output.kospijisu, sizeof(output.kospijisu)));
        SetCell(g_list, 27, 0, "KOSPI200전일대비구분 (kospisign)");
        SetCell(g_list, 27, 1, WireField(output.kospisign, sizeof(output.kospisign)));
        SetCell(g_list, 28, 0, "KOSPI200전일대비 (kospichange)");
        SetCell(g_list, 28, 1, WireField(output.kospichange, sizeof(output.kospichange)));
        SetCell(g_list, 29, 0, "KOSPI200등락율 (kospidiff)");
        SetCell(g_list, 29, 1, WireField(output.kospidiff, sizeof(output.kospidiff)));
        SetCell(g_list, 30, 0, "상장최고가 (listhprice)");
        SetCell(g_list, 30, 1, WireField(output.listhprice, sizeof(output.listhprice)));
        SetCell(g_list, 31, 0, "상장최저가 (listlprice)");
        SetCell(g_list, 31, 1, WireField(output.listlprice, sizeof(output.listlprice)));
        SetCell(g_list, 32, 0, "델타 (delt)");
        SetCell(g_list, 32, 1, WireField(output.delt, sizeof(output.delt)));
        SetCell(g_list, 33, 0, "감마 (gama)");
        SetCell(g_list, 33, 1, WireField(output.gama, sizeof(output.gama)));
        SetCell(g_list, 34, 0, "세타 (ceta)");
        SetCell(g_list, 34, 1, WireField(output.ceta, sizeof(output.ceta)));
        SetCell(g_list, 35, 0, "베가 (vega)");
        SetCell(g_list, 35, 1, WireField(output.vega, sizeof(output.vega)));
        SetCell(g_list, 36, 0, "로우 (rhox)");
        SetCell(g_list, 36, 1, WireField(output.rhox, sizeof(output.rhox)));
        SetCell(g_list, 37, 0, "근월물현재가 (gmprice)");
        SetCell(g_list, 37, 1, WireField(output.gmprice, sizeof(output.gmprice)));
        SetCell(g_list, 38, 0, "근월물전일대비구분 (gmsign)");
        SetCell(g_list, 38, 1, WireField(output.gmsign, sizeof(output.gmsign)));
        SetCell(g_list, 39, 0, "근월물전일대비 (gmchange)");
        SetCell(g_list, 39, 1, WireField(output.gmchange, sizeof(output.gmchange)));
        SetCell(g_list, 40, 0, "근월물등락율 (gmdiff)");
        SetCell(g_list, 40, 1, WireField(output.gmdiff, sizeof(output.gmdiff)));
        SetCell(g_list, 41, 0, "이론가 (theorypriceg)");
        SetCell(g_list, 41, 1, WireField(output.theorypriceg, sizeof(output.theorypriceg)));
        SetCell(g_list, 42, 0, "역사적변동성 (histimpv)");
        SetCell(g_list, 42, 1, WireField(output.histimpv, sizeof(output.histimpv)));
        SetCell(g_list, 43, 0, "내재변동성 (impv)");
        SetCell(g_list, 43, 1, WireField(output.impv, sizeof(output.impv)));
        SetCell(g_list, 44, 0, "시장BASIS (sbasis)");
        SetCell(g_list, 44, 1, WireField(output.sbasis, sizeof(output.sbasis)));
        SetCell(g_list, 45, 0, "이론BASIS (ibasis)");
        SetCell(g_list, 45, 1, WireField(output.ibasis, sizeof(output.ibasis)));
        SetCell(g_list, 46, 0, "근월물종목코드 (gmfutcode)");
        SetCell(g_list, 46, 1, WireField(output.gmfutcode, sizeof(output.gmfutcode)));
        SetCell(g_list, 47, 0, "행사가 (actprice)");
        SetCell(g_list, 47, 1, WireField(output.actprice, sizeof(output.actprice)));
        SetCell(g_list, 48, 0, "기초자산단축코드 (shcode)");
        SetCell(g_list, 48, 1, WireField(output.shcode, sizeof(output.shcode)));
        SetCell(g_list, 49, 0, "기초자산한글명 (basehname)");
        SetCell(g_list, 49, 1, WireField(output.basehname, sizeof(output.basehname)));
        SetCell(g_list, 50, 0, "기초자산현재가 (baseprice)");
        SetCell(g_list, 50, 1, WireField(output.baseprice, sizeof(output.baseprice)));
        SetCell(g_list, 51, 0, "기초자산현재가대비구분 (basesign)");
        SetCell(g_list, 51, 1, WireField(output.basesign, sizeof(output.basesign)));
        SetCell(g_list, 52, 0, "기초자산현재가전일대비 (basechange)");
        SetCell(g_list, 52, 1, WireField(output.basechange, sizeof(output.basechange)));
        SetCell(g_list, 53, 0, "기초자산등락률 (basediff)");
        SetCell(g_list, 53, 1, WireField(output.basediff, sizeof(output.basediff)));
        SetCell(g_list, 54, 0, "기초자산거래량 (basevol)");
        SetCell(g_list, 54, 1, WireField(output.basevol, sizeof(output.basevol)));
        SetCell(g_list, 55, 0, "기초자산전일거래량 (baseprevol)");
        SetCell(g_list, 55, 1, WireField(output.baseprevol, sizeof(output.baseprevol)));
        SetCell(g_list, 56, 0, "기초자산매수호가 (basebidprc)");
        SetCell(g_list, 56, 1, WireField(output.basebidprc, sizeof(output.basebidprc)));
        SetCell(g_list, 57, 0, "기초자산매도호가 (baseaskprc)");
        SetCell(g_list, 57, 1, WireField(output.baseaskprc, sizeof(output.baseaskprc)));
        SetCell(g_list, 58, 0, "기초자산외국계회원사순매수 (basefornetbid)");
        SetCell(g_list, 58, 1, WireField(output.basefornetbid, sizeof(output.basefornetbid)));
        SetCell(g_list, 59, 0, "상품군 (prodgrp)");
        SetCell(g_list, 59, 1, WireField(output.prodgrp, sizeof(output.prodgrp)));
        SetCell(g_list, 60, 0, "승수 (mulcnt)");
        SetCell(g_list, 60, 1, WireField(output.mulcnt, sizeof(output.mulcnt)));
        SetCell(g_list, 61, 0, "실시간상/하한가적용여부 (realupdnyn)");
        SetCell(g_list, 61, 1, WireField(output.realupdnyn, sizeof(output.realupdnyn)));
        SetCell(g_list, 62, 0, "실시간상한가 (realuplmtprice)");
        SetCell(g_list, 62, 1, WireField(output.realuplmtprice, sizeof(output.realuplmtprice)));
        SetCell(g_list, 63, 0, "실시간하한가 (realdnlmtprice)");
        SetCell(g_list, 63, 1, WireField(output.realdnlmtprice, sizeof(output.realdnlmtprice)));
        SetCell(g_list, 64, 0, "상한가적용단계 (uplmtpricestep)");
        SetCell(g_list, 64, 1, WireField(output.uplmtpricestep, sizeof(output.uplmtpricestep)));
        SetCell(g_list, 65, 0, "상한가1 (uplmtprice1)");
        SetCell(g_list, 65, 1, WireField(output.uplmtprice1, sizeof(output.uplmtprice1)));
        SetCell(g_list, 66, 0, "상한가2 (uplmtprice2)");
        SetCell(g_list, 66, 1, WireField(output.uplmtprice2, sizeof(output.uplmtprice2)));
        SetCell(g_list, 67, 0, "상한가3 (uplmtprice3)");
        SetCell(g_list, 67, 1, WireField(output.uplmtprice3, sizeof(output.uplmtprice3)));
        SetCell(g_list, 68, 0, "하한가적용단계 (dnlmtpricestep)");
        SetCell(g_list, 68, 1, WireField(output.dnlmtpricestep, sizeof(output.dnlmtpricestep)));
        SetCell(g_list, 69, 0, "하한가1 (dnlmtprice1)");
        SetCell(g_list, 69, 1, WireField(output.dnlmtprice1, sizeof(output.dnlmtprice1)));
        SetCell(g_list, 70, 0, "하한가2 (dnlmtprice2)");
        SetCell(g_list, 70, 1, WireField(output.dnlmtprice2, sizeof(output.dnlmtprice2)));
        SetCell(g_list, 71, 0, "하한가3 (dnlmtprice3)");
        SetCell(g_list, 71, 1, WireField(output.dnlmtprice3, sizeof(output.dnlmtprice3)));
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
    s3009InBlock input;
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
        g_focode = Common::AddControl(window, L"EDIT", L"", WS_BORDER | WS_TABSTOP | ES_AUTOHSCROLL,
                                  717, 50, 106, 24, 310);
        if (!g_focode) return -1;
        SendMessage(g_focode, EM_SETLIMITTEXT, 8, 0);
        g_queryButton = Common::AddControl(window, L"BUTTON", L"조회", WS_TABSTOP,
                                           950, 48, 140, 29, QUERY_BUTTON);
        g_output = Common::AddControl(window, L"STATIC", L"", 0, 580, 91, 365, 22, 0);
        g_list = Common::AddControl(window, WC_LISTVIEWW, L"", LVS_REPORT | LVS_SINGLESEL | WS_TABSTOP,
                                    580, 120, 540, 460, 302);
        if (!g_list) return -1;
        ListView_SetExtendedListViewStyle(g_list, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
        AddColumn(g_list, 0, L"항목 (전문 필드)", 335);
        AddColumn(g_list, 1, L"수신값", 177);
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
    wc.lpszClassName = L"OrdaAPIs3009StockFuturesQuote";
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
