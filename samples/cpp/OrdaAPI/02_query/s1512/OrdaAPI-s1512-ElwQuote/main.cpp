// 읽는 순서: WinMain 초기화 → 공통 로그인 통지 → 조회 요청 → 수신 → 해제 → 다음 조회 또는 종료.
// VS2010 v100 / Win32 / MultiByte / C++03. 공통 Helper는 변경하지 않습니다.
#include "../../../common/OrdaSampleCommon.h"
// 수신 블록명은 서비스 XML의 이름을 사용합니다.
#pragma warning(push)
#pragma warning(disable: 4005)
#include "s1512.h"
#pragma warning(pop)
#include <string.h>
#include <stdio.h>
#include <limits.h>
#include <string>
#include <vector>
#include <deque>
#include <commctrl.h>

namespace Common = OrdaSampleCommon;
const char SERVICE_CODE[] = NAME_s1512;
const wchar_t SERVICE_TITLE[] = L"ELW 현재가 조회(s1512)";
const int QUERY_BUTTON = 300, DISPLAY_LIMIT = 200;
const int INPUT_COUNT = 1;
HWND g_inputs[INPUT_COUNT], g_queryButton, g_output, g_list[4], g_tabs;
int g_requestId = -1, g_received[4] = {0};
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
    for (int i = 0; i < 4; ++i)
    {
        ListView_DeleteAllItems(g_list[i]);
        g_received[i] = 0;
    }
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

BOOL BuildInput(s1512InBlock &input)
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

void ShowBlock0(const s1512OutBlock &output)
{
    ListView_DeleteAllItems(g_list[0]);
    SetCell(g_list[0], 0, 0, "표준코드");
    SetCell(g_list[0], 0, 1, Field(output.stdcode, sizeof(output.stdcode)));
    SetCell(g_list[0], 1, 0, "단축코드");
    SetCell(g_list[0], 1, 1, Field(output.shcode, sizeof(output.shcode)));
    SetCell(g_list[0], 2, 0, "한글명");
    SetCell(g_list[0], 2, 1, Field(output.hname, sizeof(output.hname)));
    SetCell(g_list[0], 3, 0, "체결시간");
    SetCell(g_list[0], 3, 1, Field(output.chetime, sizeof(output.chetime)));
    SetCell(g_list[0], 4, 0, "현재가");
    SetCell(g_list[0], 4, 1, Field(output.price, sizeof(output.price)));
    SetCell(g_list[0], 5, 0, "전일대비구분");
    SetCell(g_list[0], 5, 1, Field(output.sign, sizeof(output.sign)));
    SetCell(g_list[0], 6, 0, "전일대비");
    SetCell(g_list[0], 6, 1, Field(output.change, sizeof(output.change)));
    SetCell(g_list[0], 7, 0, "등락율");
    SetCell(g_list[0], 7, 1, Field(output.diff, sizeof(output.diff)));
    SetCell(g_list[0], 8, 0, "체결량");
    SetCell(g_list[0], 8, 1, Field(output.cvolume, sizeof(output.cvolume)));
    SetCell(g_list[0], 9, 0, "누적거래량");
    SetCell(g_list[0], 9, 1, Field(output.volume, sizeof(output.volume)));
    SetCell(g_list[0], 10, 0, "기준가");
    SetCell(g_list[0], 10, 1, Field(output.recprice, sizeof(output.recprice)));
    SetCell(g_list[0], 11, 0, "가중평균");
    SetCell(g_list[0], 11, 1, Field(output.avg, sizeof(output.avg)));
    SetCell(g_list[0], 12, 0, "전일거래량");
    SetCell(g_list[0], 12, 1, Field(output.jnilvolume, sizeof(output.jnilvolume)));
    SetCell(g_list[0], 13, 0, "전일동시간거래량");
    SetCell(g_list[0], 13, 1, Field(output.jvolume, sizeof(output.jvolume)));
    SetCell(g_list[0], 14, 0, "전일종가");
    SetCell(g_list[0], 14, 1, Field(output.jnilclose, sizeof(output.jnilclose)));
    SetCell(g_list[0], 15, 0, "거래량차");
    SetCell(g_list[0], 15, 1, Field(output.volumechg, sizeof(output.volumechg)));
    SetCell(g_list[0], 16, 0, "거래량차등락율");
    SetCell(g_list[0], 16, 1, Field(output.volumediff, sizeof(output.volumediff)));
    SetCell(g_list[0], 17, 0, "시가");
    SetCell(g_list[0], 17, 1, Field(output.open, sizeof(output.open)));
    SetCell(g_list[0], 18, 0, "시가등락율");
    SetCell(g_list[0], 18, 1, Field(output.odiff, sizeof(output.odiff)));
    SetCell(g_list[0], 19, 0, "시가시간");
    SetCell(g_list[0], 19, 1, Field(output.opentime, sizeof(output.opentime)));
    SetCell(g_list[0], 20, 0, "고가");
    SetCell(g_list[0], 20, 1, Field(output.high, sizeof(output.high)));
    SetCell(g_list[0], 21, 0, "고가등락율");
    SetCell(g_list[0], 21, 1, Field(output.hdiff, sizeof(output.hdiff)));
    SetCell(g_list[0], 22, 0, "고가시간");
    SetCell(g_list[0], 22, 1, Field(output.hightime, sizeof(output.hightime)));
    SetCell(g_list[0], 23, 0, "저가");
    SetCell(g_list[0], 23, 1, Field(output.low, sizeof(output.low)));
    SetCell(g_list[0], 24, 0, "저가등락율");
    SetCell(g_list[0], 24, 1, Field(output.ldiff, sizeof(output.ldiff)));
    SetCell(g_list[0], 25, 0, "저가시간");
    SetCell(g_list[0], 25, 1, Field(output.lowtime, sizeof(output.lowtime)));
    SetCell(g_list[0], 26, 0, "52최고가");
    SetCell(g_list[0], 26, 1, Field(output.high52w, sizeof(output.high52w)));
    SetCell(g_list[0], 27, 0, "52최고가등락율");
    SetCell(g_list[0], 27, 1, Field(output.high52wdiff, sizeof(output.high52wdiff)));
    SetCell(g_list[0], 28, 0, "52최고가일");
    SetCell(g_list[0], 28, 1, Field(output.high52wdate, sizeof(output.high52wdate)));
    SetCell(g_list[0], 29, 0, "52최저가");
    SetCell(g_list[0], 29, 1, Field(output.low52w, sizeof(output.low52w)));
    SetCell(g_list[0], 30, 0, "52최저가등락율");
    SetCell(g_list[0], 30, 1, Field(output.low52wdiff, sizeof(output.low52wdiff)));
    SetCell(g_list[0], 31, 0, "52최저가일");
    SetCell(g_list[0], 31, 1, Field(output.low52wdate, sizeof(output.low52wdate)));
    SetCell(g_list[0], 32, 0, "소진율");
    SetCell(g_list[0], 32, 1, Field(output.exhratio, sizeof(output.exhratio)));
    SetCell(g_list[0], 33, 0, "상장주식수(천)");
    SetCell(g_list[0], 33, 1, Field(output.listing, sizeof(output.listing)));
    SetCell(g_list[0], 34, 0, "수량단위");
    SetCell(g_list[0], 34, 1, Field(output.memedan, sizeof(output.memedan)));
    SetCell(g_list[0], 35, 0, "회전율");
    SetCell(g_list[0], 35, 1, Field(output.vol, sizeof(output.vol)));
    SetCell(g_list[0], 36, 0, "패리티");
    SetCell(g_list[0], 36, 1, Field(output.parity, sizeof(output.parity)));
    SetCell(g_list[0], 37, 0, "손익분기");
    SetCell(g_list[0], 37, 1, Field(output.berate, sizeof(output.berate)));
    SetCell(g_list[0], 38, 0, "기어링");
    SetCell(g_list[0], 38, 1, Field(output.gearing, sizeof(output.gearing)));
    SetCell(g_list[0], 39, 0, "행사가");
    SetCell(g_list[0], 39, 1, Field(output.elwexec, sizeof(output.elwexec)));
    SetCell(g_list[0], 40, 0, "발행가");
    SetCell(g_list[0], 40, 1, Field(output.issueprice, sizeof(output.issueprice)));
    SetCell(g_list[0], 41, 0, "전환비율");
    SetCell(g_list[0], 41, 1, Field(output.convrate, sizeof(output.convrate)));
    SetCell(g_list[0], 42, 0, "최종거래일");
    SetCell(g_list[0], 42, 1, Field(output.lastdate, sizeof(output.lastdate)));
    SetCell(g_list[0], 43, 0, "자본지지");
    SetCell(g_list[0], 43, 1, Field(output.capt, sizeof(output.capt)));
    SetCell(g_list[0], 44, 0, "e.기어링");
    SetCell(g_list[0], 44, 1, Field(output.egearing, sizeof(output.egearing)));
    SetCell(g_list[0], 45, 0, "프리미엄");
    SetCell(g_list[0], 45, 1, Field(output.premium, sizeof(output.premium)));
    SetCell(g_list[0], 46, 0, "스프레드");
    SetCell(g_list[0], 46, 1, Field(output.spread, sizeof(output.spread)));
    SetCell(g_list[0], 47, 0, "최대스프레드");
    SetCell(g_list[0], 47, 1, Field(output.espread, sizeof(output.espread)));
    SetCell(g_list[0], 48, 0, "이론가");
    SetCell(g_list[0], 48, 1, Field(output.theoryprice, sizeof(output.theoryprice)));
    SetCell(g_list[0], 49, 0, "내재변동성");
    SetCell(g_list[0], 49, 1, Field(output.impv, sizeof(output.impv)));
    SetCell(g_list[0], 50, 0, "상태");
    SetCell(g_list[0], 50, 1, Field(output.moneyness, sizeof(output.moneyness)));
    SetCell(g_list[0], 51, 0, "델타");
    SetCell(g_list[0], 51, 1, Field(output.delt, sizeof(output.delt)));
    SetCell(g_list[0], 52, 0, "감마");
    SetCell(g_list[0], 52, 1, Field(output.gama, sizeof(output.gama)));
    SetCell(g_list[0], 53, 0, "베가");
    SetCell(g_list[0], 53, 1, Field(output.vega, sizeof(output.vega)));
    SetCell(g_list[0], 54, 0, "쎄타");
    SetCell(g_list[0], 54, 1, Field(output.ceta, sizeof(output.ceta)));
    SetCell(g_list[0], 55, 0, "로");
    SetCell(g_list[0], 55, 1, Field(output.rhox, sizeof(output.rhox)));
    SetCell(g_list[0], 56, 0, "잔존일수");
    SetCell(g_list[0], 56, 1, Field(output.bjandatecnt, sizeof(output.bjandatecnt)));
    SetCell(g_list[0], 57, 0, "행사개시일");
    SetCell(g_list[0], 57, 1, Field(output.mmsdate, sizeof(output.mmsdate)));
    SetCell(g_list[0], 58, 0, "행사종료일");
    SetCell(g_list[0], 58, 1, Field(output.mmedate, sizeof(output.mmedate)));
    SetCell(g_list[0], 59, 0, "지급일");
    SetCell(g_list[0], 59, 1, Field(output.payday, sizeof(output.payday)));
    SetCell(g_list[0], 60, 0, "발행일");
    SetCell(g_list[0], 60, 1, Field(output.listdate, sizeof(output.listdate)));
    SetCell(g_list[0], 61, 0, "LP회원사");
    SetCell(g_list[0], 61, 1, Field(output.lpmem, sizeof(output.lpmem)));
    SetCell(g_list[0], 62, 0, "LP보유수량");
    SetCell(g_list[0], 62, 1, Field(output.lp_holdvol, sizeof(output.lp_holdvol)));
    SetCell(g_list[0], 63, 0, "기초자산코드");
    SetCell(g_list[0], 63, 1, Field(output.bcode, sizeof(output.bcode)));
    SetCell(g_list[0], 64, 0, "기초자산구분");
    SetCell(g_list[0], 64, 1, Field(output.bgubun, sizeof(output.bgubun)));
    SetCell(g_list[0], 65, 0, "기초자산현재가");
    SetCell(g_list[0], 65, 1, Field(output.bprice, sizeof(output.bprice)));
    SetCell(g_list[0], 66, 0, "기초자산전일비구분");
    SetCell(g_list[0], 66, 1, Field(output.bsign, sizeof(output.bsign)));
    SetCell(g_list[0], 67, 0, "기초자산전일비");
    SetCell(g_list[0], 67, 1, Field(output.bchange, sizeof(output.bchange)));
    SetCell(g_list[0], 68, 0, "기초자산등락율");
    SetCell(g_list[0], 68, 1, Field(output.bdiff, sizeof(output.bdiff)));
    SetCell(g_list[0], 69, 0, "기초자산거래량");
    SetCell(g_list[0], 69, 1, Field(output.bvolume, sizeof(output.bvolume)));
    SetCell(g_list[0], 70, 0, "락구분");
    SetCell(g_list[0], 70, 1, Field(output.info1, sizeof(output.info1)));
    SetCell(g_list[0], 71, 0, "관리/급등구분");
    SetCell(g_list[0], 71, 1, Field(output.info2, sizeof(output.info2)));
    SetCell(g_list[0], 72, 0, "정지/연장구분");
    SetCell(g_list[0], 72, 1, Field(output.info3, sizeof(output.info3)));
    SetCell(g_list[0], 73, 0, "투자/불성실구분");
    SetCell(g_list[0], 73, 1, Field(output.info4, sizeof(output.info4)));
    SetCell(g_list[0], 74, 0, "장구분");
    SetCell(g_list[0], 74, 1, Field(output.janginfo, sizeof(output.janginfo)));
    SetCell(g_list[0], 75, 0, "바스켓구분");
    SetCell(g_list[0], 75, 1, Field(output.basketgb, sizeof(output.basketgb)));
    SetCell(g_list[0], 76, 0, "바스켓갯수");
    SetCell(g_list[0], 76, 1, Field(output.basketcnt, sizeof(output.basketcnt)));
    SetCell(g_list[0], 77, 0, "ELW권리행사방식");
    SetCell(g_list[0], 77, 1, Field(output.elwtype, sizeof(output.elwtype)));
    SetCell(g_list[0], 78, 0, "ELW결제방법");
    SetCell(g_list[0], 78, 1, Field(output.settletype, sizeof(output.settletype)));
    SetCell(g_list[0], 79, 0, "LP사주문가능여부");
    SetCell(g_list[0], 79, 1, Field(output.lpord, sizeof(output.lpord)));
    SetCell(g_list[0], 80, 0, "권리내용");
    SetCell(g_list[0], 80, 1, Field(output.elwdetail, sizeof(output.elwdetail)));
    SetCell(g_list[0], 81, 0, "만기평가가격방식");
    SetCell(g_list[0], 81, 1, Field(output.valuation, sizeof(output.valuation)));
    SetCell(g_list[0], 82, 0, "배분정보구분");
    SetCell(g_list[0], 82, 1, Field(output.info5, sizeof(output.info5)));
}

void ShowBlock1(const s1512OutBlock1 &output)
{
    ListView_DeleteAllItems(g_list[1]);
    SetCell(g_list[1], 0, 0, "한글명");
    SetCell(g_list[1], 0, 1, Field(output.hname, sizeof(output.hname)));
    SetCell(g_list[1], 1, 0, "현재가");
    SetCell(g_list[1], 1, 1, Field(output.price, sizeof(output.price)));
    SetCell(g_list[1], 2, 0, "전일대비구분");
    SetCell(g_list[1], 2, 1, Field(output.sign, sizeof(output.sign)));
    SetCell(g_list[1], 3, 0, "전일대비");
    SetCell(g_list[1], 3, 1, Field(output.change, sizeof(output.change)));
    SetCell(g_list[1], 4, 0, "등락율");
    SetCell(g_list[1], 4, 1, Field(output.diff, sizeof(output.diff)));
    SetCell(g_list[1], 5, 0, "누적거래량");
    SetCell(g_list[1], 5, 1, Field(output.volume, sizeof(output.volume)));
    SetCell(g_list[1], 6, 0, "전일종가");
    SetCell(g_list[1], 6, 1, Field(output.jnilclose, sizeof(output.jnilclose)));
    SetCell(g_list[1], 7, 0, "매도호가1");
    SetCell(g_list[1], 7, 1, Field(output.offerho1, sizeof(output.offerho1)));
    SetCell(g_list[1], 8, 0, "매수호가1");
    SetCell(g_list[1], 8, 1, Field(output.bidho1, sizeof(output.bidho1)));
    SetCell(g_list[1], 9, 0, "매도호가수량1");
    SetCell(g_list[1], 9, 1, Field(output.offerrem1, sizeof(output.offerrem1)));
    SetCell(g_list[1], 10, 0, "LP매도호가수량1");
    SetCell(g_list[1], 10, 1, Field(output.lp_offerrem1, sizeof(output.lp_offerrem1)));
    SetCell(g_list[1], 11, 0, "매수호가수량1");
    SetCell(g_list[1], 11, 1, Field(output.bidrem1, sizeof(output.bidrem1)));
    SetCell(g_list[1], 12, 0, "LP매수호가수량1");
    SetCell(g_list[1], 12, 1, Field(output.lp_bidrem1, sizeof(output.lp_bidrem1)));
    SetCell(g_list[1], 13, 0, "직전매도대비수량1");
    SetCell(g_list[1], 13, 1, Field(output.preoffercha1, sizeof(output.preoffercha1)));
    SetCell(g_list[1], 14, 0, "직전매수대비수량1");
    SetCell(g_list[1], 14, 1, Field(output.prebidcha1, sizeof(output.prebidcha1)));
    SetCell(g_list[1], 15, 0, "매도호가2");
    SetCell(g_list[1], 15, 1, Field(output.offerho2, sizeof(output.offerho2)));
    SetCell(g_list[1], 16, 0, "매수호가2");
    SetCell(g_list[1], 16, 1, Field(output.bidho2, sizeof(output.bidho2)));
    SetCell(g_list[1], 17, 0, "매도호가수량2");
    SetCell(g_list[1], 17, 1, Field(output.offerrem2, sizeof(output.offerrem2)));
    SetCell(g_list[1], 18, 0, "LP매도호가수량2");
    SetCell(g_list[1], 18, 1, Field(output.lp_offerrem2, sizeof(output.lp_offerrem2)));
    SetCell(g_list[1], 19, 0, "매수호가수량2");
    SetCell(g_list[1], 19, 1, Field(output.bidrem2, sizeof(output.bidrem2)));
    SetCell(g_list[1], 20, 0, "LP매수호가수량2");
    SetCell(g_list[1], 20, 1, Field(output.lp_bidrem2, sizeof(output.lp_bidrem2)));
    SetCell(g_list[1], 21, 0, "직전매도대비수량2");
    SetCell(g_list[1], 21, 1, Field(output.preoffercha2, sizeof(output.preoffercha2)));
    SetCell(g_list[1], 22, 0, "직전매수대비수량2");
    SetCell(g_list[1], 22, 1, Field(output.prebidcha2, sizeof(output.prebidcha2)));
    SetCell(g_list[1], 23, 0, "매도호가3");
    SetCell(g_list[1], 23, 1, Field(output.offerho3, sizeof(output.offerho3)));
    SetCell(g_list[1], 24, 0, "매수호가3");
    SetCell(g_list[1], 24, 1, Field(output.bidho3, sizeof(output.bidho3)));
    SetCell(g_list[1], 25, 0, "매도호가수량3");
    SetCell(g_list[1], 25, 1, Field(output.offerrem3, sizeof(output.offerrem3)));
    SetCell(g_list[1], 26, 0, "LP매도호가수량3");
    SetCell(g_list[1], 26, 1, Field(output.lp_offerrem3, sizeof(output.lp_offerrem3)));
    SetCell(g_list[1], 27, 0, "매수호가수량3");
    SetCell(g_list[1], 27, 1, Field(output.bidrem3, sizeof(output.bidrem3)));
    SetCell(g_list[1], 28, 0, "LP매수호가수량3");
    SetCell(g_list[1], 28, 1, Field(output.lp_bidrem3, sizeof(output.lp_bidrem3)));
    SetCell(g_list[1], 29, 0, "직전매도대비수량3");
    SetCell(g_list[1], 29, 1, Field(output.preoffercha3, sizeof(output.preoffercha3)));
    SetCell(g_list[1], 30, 0, "직전매수대비수량3");
    SetCell(g_list[1], 30, 1, Field(output.prebidcha3, sizeof(output.prebidcha3)));
    SetCell(g_list[1], 31, 0, "매도호가4");
    SetCell(g_list[1], 31, 1, Field(output.offerho4, sizeof(output.offerho4)));
    SetCell(g_list[1], 32, 0, "매수호가4");
    SetCell(g_list[1], 32, 1, Field(output.bidho4, sizeof(output.bidho4)));
    SetCell(g_list[1], 33, 0, "매도호가수량4");
    SetCell(g_list[1], 33, 1, Field(output.offerrem4, sizeof(output.offerrem4)));
    SetCell(g_list[1], 34, 0, "LP매도호가수량4");
    SetCell(g_list[1], 34, 1, Field(output.lp_offerrem4, sizeof(output.lp_offerrem4)));
    SetCell(g_list[1], 35, 0, "매수호가수량4");
    SetCell(g_list[1], 35, 1, Field(output.bidrem4, sizeof(output.bidrem4)));
    SetCell(g_list[1], 36, 0, "LP매수호가수량4");
    SetCell(g_list[1], 36, 1, Field(output.lp_bidrem4, sizeof(output.lp_bidrem4)));
    SetCell(g_list[1], 37, 0, "직전매도대비수량4");
    SetCell(g_list[1], 37, 1, Field(output.preoffercha4, sizeof(output.preoffercha4)));
    SetCell(g_list[1], 38, 0, "직전매수대비수량4");
    SetCell(g_list[1], 38, 1, Field(output.prebidcha4, sizeof(output.prebidcha4)));
    SetCell(g_list[1], 39, 0, "매도호가5");
    SetCell(g_list[1], 39, 1, Field(output.offerho5, sizeof(output.offerho5)));
    SetCell(g_list[1], 40, 0, "매수호가5");
    SetCell(g_list[1], 40, 1, Field(output.bidho5, sizeof(output.bidho5)));
    SetCell(g_list[1], 41, 0, "매도호가수량5");
    SetCell(g_list[1], 41, 1, Field(output.offerrem5, sizeof(output.offerrem5)));
    SetCell(g_list[1], 42, 0, "LP매도호가수량5");
    SetCell(g_list[1], 42, 1, Field(output.lp_offerrem5, sizeof(output.lp_offerrem5)));
    SetCell(g_list[1], 43, 0, "매수호가수량5");
    SetCell(g_list[1], 43, 1, Field(output.bidrem5, sizeof(output.bidrem5)));
    SetCell(g_list[1], 44, 0, "LP매수호가수량5");
    SetCell(g_list[1], 44, 1, Field(output.lp_bidrem5, sizeof(output.lp_bidrem5)));
    SetCell(g_list[1], 45, 0, "직전매도대비수량5");
    SetCell(g_list[1], 45, 1, Field(output.preoffercha5, sizeof(output.preoffercha5)));
    SetCell(g_list[1], 46, 0, "직전매수대비수량5");
    SetCell(g_list[1], 46, 1, Field(output.prebidcha5, sizeof(output.prebidcha5)));
    SetCell(g_list[1], 47, 0, "매도호가6");
    SetCell(g_list[1], 47, 1, Field(output.offerho6, sizeof(output.offerho6)));
    SetCell(g_list[1], 48, 0, "매수호가6");
    SetCell(g_list[1], 48, 1, Field(output.bidho6, sizeof(output.bidho6)));
    SetCell(g_list[1], 49, 0, "매도호가수량6");
    SetCell(g_list[1], 49, 1, Field(output.offerrem6, sizeof(output.offerrem6)));
    SetCell(g_list[1], 50, 0, "LP매도호가수량6");
    SetCell(g_list[1], 50, 1, Field(output.lp_offerrem6, sizeof(output.lp_offerrem6)));
    SetCell(g_list[1], 51, 0, "매수호가수량6");
    SetCell(g_list[1], 51, 1, Field(output.bidrem6, sizeof(output.bidrem6)));
    SetCell(g_list[1], 52, 0, "LP매수호가수량6");
    SetCell(g_list[1], 52, 1, Field(output.lp_bidrem6, sizeof(output.lp_bidrem6)));
    SetCell(g_list[1], 53, 0, "직전매도대비수량6");
    SetCell(g_list[1], 53, 1, Field(output.preoffercha6, sizeof(output.preoffercha6)));
    SetCell(g_list[1], 54, 0, "직전매수대비수량6");
    SetCell(g_list[1], 54, 1, Field(output.prebidcha6, sizeof(output.prebidcha6)));
    SetCell(g_list[1], 55, 0, "매도호가7");
    SetCell(g_list[1], 55, 1, Field(output.offerho7, sizeof(output.offerho7)));
    SetCell(g_list[1], 56, 0, "매수호가7");
    SetCell(g_list[1], 56, 1, Field(output.bidho7, sizeof(output.bidho7)));
    SetCell(g_list[1], 57, 0, "매도호가수량7");
    SetCell(g_list[1], 57, 1, Field(output.offerrem7, sizeof(output.offerrem7)));
    SetCell(g_list[1], 58, 0, "LP매도호가수량7");
    SetCell(g_list[1], 58, 1, Field(output.lp_offerrem7, sizeof(output.lp_offerrem7)));
    SetCell(g_list[1], 59, 0, "매수호가수량7");
    SetCell(g_list[1], 59, 1, Field(output.bidrem7, sizeof(output.bidrem7)));
    SetCell(g_list[1], 60, 0, "LP매수호가수량7");
    SetCell(g_list[1], 60, 1, Field(output.lp_bidrem7, sizeof(output.lp_bidrem7)));
    SetCell(g_list[1], 61, 0, "직전매도대비수량7");
    SetCell(g_list[1], 61, 1, Field(output.preoffercha7, sizeof(output.preoffercha7)));
    SetCell(g_list[1], 62, 0, "직전매수대비수량7");
    SetCell(g_list[1], 62, 1, Field(output.prebidcha7, sizeof(output.prebidcha7)));
    SetCell(g_list[1], 63, 0, "매도호가8");
    SetCell(g_list[1], 63, 1, Field(output.offerho8, sizeof(output.offerho8)));
    SetCell(g_list[1], 64, 0, "매수호가8");
    SetCell(g_list[1], 64, 1, Field(output.bidho8, sizeof(output.bidho8)));
    SetCell(g_list[1], 65, 0, "매도호가수량8");
    SetCell(g_list[1], 65, 1, Field(output.offerrem8, sizeof(output.offerrem8)));
    SetCell(g_list[1], 66, 0, "LP매도호가수량8");
    SetCell(g_list[1], 66, 1, Field(output.lp_offerrem8, sizeof(output.lp_offerrem8)));
    SetCell(g_list[1], 67, 0, "매수호가수량8");
    SetCell(g_list[1], 67, 1, Field(output.bidrem8, sizeof(output.bidrem8)));
    SetCell(g_list[1], 68, 0, "LP매수호가수량8");
    SetCell(g_list[1], 68, 1, Field(output.lp_bidrem8, sizeof(output.lp_bidrem8)));
    SetCell(g_list[1], 69, 0, "직전매도대비수량8");
    SetCell(g_list[1], 69, 1, Field(output.preoffercha8, sizeof(output.preoffercha8)));
    SetCell(g_list[1], 70, 0, "직전매수대비수량8");
    SetCell(g_list[1], 70, 1, Field(output.prebidcha8, sizeof(output.prebidcha8)));
    SetCell(g_list[1], 71, 0, "매도호가9");
    SetCell(g_list[1], 71, 1, Field(output.offerho9, sizeof(output.offerho9)));
    SetCell(g_list[1], 72, 0, "매수호가9");
    SetCell(g_list[1], 72, 1, Field(output.bidho9, sizeof(output.bidho9)));
    SetCell(g_list[1], 73, 0, "매도호가수량9");
    SetCell(g_list[1], 73, 1, Field(output.offerrem9, sizeof(output.offerrem9)));
    SetCell(g_list[1], 74, 0, "LP매도호가수량9");
    SetCell(g_list[1], 74, 1, Field(output.lp_offerrem9, sizeof(output.lp_offerrem9)));
    SetCell(g_list[1], 75, 0, "매수호가수량9");
    SetCell(g_list[1], 75, 1, Field(output.bidrem9, sizeof(output.bidrem9)));
    SetCell(g_list[1], 76, 0, "LP매수호가수량9");
    SetCell(g_list[1], 76, 1, Field(output.lp_bidrem9, sizeof(output.lp_bidrem9)));
    SetCell(g_list[1], 77, 0, "직전매도대비수량9");
    SetCell(g_list[1], 77, 1, Field(output.preoffercha9, sizeof(output.preoffercha9)));
    SetCell(g_list[1], 78, 0, "직전매수대비수량9");
    SetCell(g_list[1], 78, 1, Field(output.prebidcha9, sizeof(output.prebidcha9)));
    SetCell(g_list[1], 79, 0, "매도호가10");
    SetCell(g_list[1], 79, 1, Field(output.offerho10, sizeof(output.offerho10)));
    SetCell(g_list[1], 80, 0, "매수호가10");
    SetCell(g_list[1], 80, 1, Field(output.bidho10, sizeof(output.bidho10)));
    SetCell(g_list[1], 81, 0, "매도호가수량10");
    SetCell(g_list[1], 81, 1, Field(output.offerrem10, sizeof(output.offerrem10)));
    SetCell(g_list[1], 82, 0, "LP매도호가수량10");
    SetCell(g_list[1], 82, 1, Field(output.lp_offerrem10, sizeof(output.lp_offerrem10)));
    SetCell(g_list[1], 83, 0, "매수호가수량10");
    SetCell(g_list[1], 83, 1, Field(output.bidrem10, sizeof(output.bidrem10)));
    SetCell(g_list[1], 84, 0, "LP매수호가수량10");
    SetCell(g_list[1], 84, 1, Field(output.lp_bidrem10, sizeof(output.lp_bidrem10)));
    SetCell(g_list[1], 85, 0, "직전매도대비수량10");
    SetCell(g_list[1], 85, 1, Field(output.preoffercha10, sizeof(output.preoffercha10)));
    SetCell(g_list[1], 86, 0, "직전매수대비수량10");
    SetCell(g_list[1], 86, 1, Field(output.prebidcha10, sizeof(output.prebidcha10)));
    SetCell(g_list[1], 87, 0, "매도호가수량합");
    SetCell(g_list[1], 87, 1, Field(output.offer, sizeof(output.offer)));
    SetCell(g_list[1], 88, 0, "매수호가수량합");
    SetCell(g_list[1], 88, 1, Field(output.bid, sizeof(output.bid)));
    SetCell(g_list[1], 89, 0, "직전매도대비수량합");
    SetCell(g_list[1], 89, 1, Field(output.preoffercha, sizeof(output.preoffercha)));
    SetCell(g_list[1], 90, 0, "직전매수대비수량합");
    SetCell(g_list[1], 90, 1, Field(output.prebidcha, sizeof(output.prebidcha)));
    SetCell(g_list[1], 91, 0, "수신시간");
    SetCell(g_list[1], 91, 1, Field(output.hotime, sizeof(output.hotime)));
    SetCell(g_list[1], 92, 0, "예상체결가격");
    SetCell(g_list[1], 92, 1, Field(output.yeprice, sizeof(output.yeprice)));
    SetCell(g_list[1], 93, 0, "예상체결수량");
    SetCell(g_list[1], 93, 1, Field(output.yevolume, sizeof(output.yevolume)));
    SetCell(g_list[1], 94, 0, "예상체결전일구분");
    SetCell(g_list[1], 94, 1, Field(output.yesign, sizeof(output.yesign)));
    SetCell(g_list[1], 95, 0, "예상체결전일대비");
    SetCell(g_list[1], 95, 1, Field(output.yechange, sizeof(output.yechange)));
    SetCell(g_list[1], 96, 0, "예상체결등락율");
    SetCell(g_list[1], 96, 1, Field(output.yediff, sizeof(output.yediff)));
    SetCell(g_list[1], 97, 0, "시간외매도잔량");
    SetCell(g_list[1], 97, 1, Field(output.tmoffer, sizeof(output.tmoffer)));
    SetCell(g_list[1], 98, 0, "시간외매수잔량");
    SetCell(g_list[1], 98, 1, Field(output.tmbid, sizeof(output.tmbid)));
    SetCell(g_list[1], 99, 0, "동시구분");
    SetCell(g_list[1], 99, 1, Field(output.ho_status, sizeof(output.ho_status)));
    SetCell(g_list[1], 100, 0, "시가");
    SetCell(g_list[1], 100, 1, Field(output.open, sizeof(output.open)));
    SetCell(g_list[1], 101, 0, "고가");
    SetCell(g_list[1], 101, 1, Field(output.high, sizeof(output.high)));
    SetCell(g_list[1], 102, 0, "저가");
    SetCell(g_list[1], 102, 1, Field(output.low, sizeof(output.low)));
}

void ShowBlock2(const s1512OutBlock2 &output)
{
    ListView_DeleteAllItems(g_list[2]);
    SetCell(g_list[2], 0, 0, "기초자산표준코드");
    SetCell(g_list[2], 0, 1, Field(output.bskstdcode, sizeof(output.bskstdcode)));
    SetCell(g_list[2], 1, 0, "기초자산코드");
    SetCell(g_list[2], 1, 1, Field(output.bskcode, sizeof(output.bskcode)));
    SetCell(g_list[2], 2, 0, "기초자산비율");
    SetCell(g_list[2], 2, 1, Field(output.bskbno, sizeof(output.bskbno)));
    SetCell(g_list[2], 3, 0, "기초자산현재가");
    SetCell(g_list[2], 3, 1, Field(output.bskprice, sizeof(output.bskprice)));
    SetCell(g_list[2], 4, 0, "기초자산전일비구분");
    SetCell(g_list[2], 4, 1, Field(output.bsksign, sizeof(output.bsksign)));
    SetCell(g_list[2], 5, 0, "기초자산전일비");
    SetCell(g_list[2], 5, 1, Field(output.bskchange, sizeof(output.bskchange)));
    SetCell(g_list[2], 6, 0, "기초자산등락율");
    SetCell(g_list[2], 6, 1, Field(output.bskdiff, sizeof(output.bskdiff)));
    SetCell(g_list[2], 7, 0, "기초자산거래량");
    SetCell(g_list[2], 7, 1, Field(output.bskvolume, sizeof(output.bskvolume)));
    SetCell(g_list[2], 8, 0, "기초자산전일종가");
    SetCell(g_list[2], 8, 1, Field(output.bskjnilclose, sizeof(output.bskjnilclose)));
}

void ShowBlock3(const s1512OutBlock3 &output)
{
    ListView_DeleteAllItems(g_list[3]);
    SetCell(g_list[3], 0, 0, "한글명");
    SetCell(g_list[3], 0, 1, Field(output.hname, sizeof(output.hname)));
    SetCell(g_list[3], 1, 0, "현재가");
    SetCell(g_list[3], 1, 1, Field(output.price, sizeof(output.price)));
    SetCell(g_list[3], 2, 0, "전일대비구분");
    SetCell(g_list[3], 2, 1, Field(output.sign, sizeof(output.sign)));
    SetCell(g_list[3], 3, 0, "전일대비");
    SetCell(g_list[3], 3, 1, Field(output.change, sizeof(output.change)));
    SetCell(g_list[3], 4, 0, "등락율");
    SetCell(g_list[3], 4, 1, Field(output.diff, sizeof(output.diff)));
    SetCell(g_list[3], 5, 0, "누적거래량");
    SetCell(g_list[3], 5, 1, Field(output.volume, sizeof(output.volume)));
    SetCell(g_list[3], 6, 0, "전일종가");
    SetCell(g_list[3], 6, 1, Field(output.jnilclose, sizeof(output.jnilclose)));
    SetCell(g_list[3], 7, 0, "매도호가1");
    SetCell(g_list[3], 7, 1, Field(output.offerho1, sizeof(output.offerho1)));
    SetCell(g_list[3], 8, 0, "매수호가1");
    SetCell(g_list[3], 8, 1, Field(output.bidho1, sizeof(output.bidho1)));
    SetCell(g_list[3], 9, 0, "매도호가수량1");
    SetCell(g_list[3], 9, 1, Field(output.offerrem1, sizeof(output.offerrem1)));
    SetCell(g_list[3], 10, 0, "매수호가수량1");
    SetCell(g_list[3], 10, 1, Field(output.bidrem1, sizeof(output.bidrem1)));
    SetCell(g_list[3], 11, 0, "직전매도대비수량1");
    SetCell(g_list[3], 11, 1, Field(output.preoffercha1, sizeof(output.preoffercha1)));
    SetCell(g_list[3], 12, 0, "직전매수대비수량1");
    SetCell(g_list[3], 12, 1, Field(output.prebidcha1, sizeof(output.prebidcha1)));
    SetCell(g_list[3], 13, 0, "매도호가2");
    SetCell(g_list[3], 13, 1, Field(output.offerho2, sizeof(output.offerho2)));
    SetCell(g_list[3], 14, 0, "매수호가2");
    SetCell(g_list[3], 14, 1, Field(output.bidho2, sizeof(output.bidho2)));
    SetCell(g_list[3], 15, 0, "매도호가수량2");
    SetCell(g_list[3], 15, 1, Field(output.offerrem2, sizeof(output.offerrem2)));
    SetCell(g_list[3], 16, 0, "매수호가수량2");
    SetCell(g_list[3], 16, 1, Field(output.bidrem2, sizeof(output.bidrem2)));
    SetCell(g_list[3], 17, 0, "직전매도대비수량2");
    SetCell(g_list[3], 17, 1, Field(output.preoffercha2, sizeof(output.preoffercha2)));
    SetCell(g_list[3], 18, 0, "직전매수대비수량2");
    SetCell(g_list[3], 18, 1, Field(output.prebidcha2, sizeof(output.prebidcha2)));
    SetCell(g_list[3], 19, 0, "매도호가3");
    SetCell(g_list[3], 19, 1, Field(output.offerho3, sizeof(output.offerho3)));
    SetCell(g_list[3], 20, 0, "매수호가3");
    SetCell(g_list[3], 20, 1, Field(output.bidho3, sizeof(output.bidho3)));
    SetCell(g_list[3], 21, 0, "매도호가수량3");
    SetCell(g_list[3], 21, 1, Field(output.offerrem3, sizeof(output.offerrem3)));
    SetCell(g_list[3], 22, 0, "매수호가수량3");
    SetCell(g_list[3], 22, 1, Field(output.bidrem3, sizeof(output.bidrem3)));
    SetCell(g_list[3], 23, 0, "직전매도대비수량3");
    SetCell(g_list[3], 23, 1, Field(output.preoffercha3, sizeof(output.preoffercha3)));
    SetCell(g_list[3], 24, 0, "직전매수대비수량3");
    SetCell(g_list[3], 24, 1, Field(output.prebidcha3, sizeof(output.prebidcha3)));
    SetCell(g_list[3], 25, 0, "매도호가4");
    SetCell(g_list[3], 25, 1, Field(output.offerho4, sizeof(output.offerho4)));
    SetCell(g_list[3], 26, 0, "매수호가4");
    SetCell(g_list[3], 26, 1, Field(output.bidho4, sizeof(output.bidho4)));
    SetCell(g_list[3], 27, 0, "매도호가수량4");
    SetCell(g_list[3], 27, 1, Field(output.offerrem4, sizeof(output.offerrem4)));
    SetCell(g_list[3], 28, 0, "매수호가수량4");
    SetCell(g_list[3], 28, 1, Field(output.bidrem4, sizeof(output.bidrem4)));
    SetCell(g_list[3], 29, 0, "직전매도대비수량4");
    SetCell(g_list[3], 29, 1, Field(output.preoffercha4, sizeof(output.preoffercha4)));
    SetCell(g_list[3], 30, 0, "직전매수대비수량4");
    SetCell(g_list[3], 30, 1, Field(output.prebidcha4, sizeof(output.prebidcha4)));
    SetCell(g_list[3], 31, 0, "매도호가5");
    SetCell(g_list[3], 31, 1, Field(output.offerho5, sizeof(output.offerho5)));
    SetCell(g_list[3], 32, 0, "매수호가5");
    SetCell(g_list[3], 32, 1, Field(output.bidho5, sizeof(output.bidho5)));
    SetCell(g_list[3], 33, 0, "매도호가수량5");
    SetCell(g_list[3], 33, 1, Field(output.offerrem5, sizeof(output.offerrem5)));
    SetCell(g_list[3], 34, 0, "매수호가수량5");
    SetCell(g_list[3], 34, 1, Field(output.bidrem5, sizeof(output.bidrem5)));
    SetCell(g_list[3], 35, 0, "직전매도대비수량5");
    SetCell(g_list[3], 35, 1, Field(output.preoffercha5, sizeof(output.preoffercha5)));
    SetCell(g_list[3], 36, 0, "직전매수대비수량5");
    SetCell(g_list[3], 36, 1, Field(output.prebidcha5, sizeof(output.prebidcha5)));
    SetCell(g_list[3], 37, 0, "매도호가6");
    SetCell(g_list[3], 37, 1, Field(output.offerho6, sizeof(output.offerho6)));
    SetCell(g_list[3], 38, 0, "매수호가6");
    SetCell(g_list[3], 38, 1, Field(output.bidho6, sizeof(output.bidho6)));
    SetCell(g_list[3], 39, 0, "매도호가수량6");
    SetCell(g_list[3], 39, 1, Field(output.offerrem6, sizeof(output.offerrem6)));
    SetCell(g_list[3], 40, 0, "매수호가수량6");
    SetCell(g_list[3], 40, 1, Field(output.bidrem6, sizeof(output.bidrem6)));
    SetCell(g_list[3], 41, 0, "직전매도대비수량6");
    SetCell(g_list[3], 41, 1, Field(output.preoffercha6, sizeof(output.preoffercha6)));
    SetCell(g_list[3], 42, 0, "직전매수대비수량6");
    SetCell(g_list[3], 42, 1, Field(output.prebidcha6, sizeof(output.prebidcha6)));
    SetCell(g_list[3], 43, 0, "매도호가7");
    SetCell(g_list[3], 43, 1, Field(output.offerho7, sizeof(output.offerho7)));
    SetCell(g_list[3], 44, 0, "매수호가7");
    SetCell(g_list[3], 44, 1, Field(output.bidho7, sizeof(output.bidho7)));
    SetCell(g_list[3], 45, 0, "매도호가수량7");
    SetCell(g_list[3], 45, 1, Field(output.offerrem7, sizeof(output.offerrem7)));
    SetCell(g_list[3], 46, 0, "매수호가수량7");
    SetCell(g_list[3], 46, 1, Field(output.bidrem7, sizeof(output.bidrem7)));
    SetCell(g_list[3], 47, 0, "직전매도대비수량7");
    SetCell(g_list[3], 47, 1, Field(output.preoffercha7, sizeof(output.preoffercha7)));
    SetCell(g_list[3], 48, 0, "직전매수대비수량7");
    SetCell(g_list[3], 48, 1, Field(output.prebidcha7, sizeof(output.prebidcha7)));
    SetCell(g_list[3], 49, 0, "매도호가8");
    SetCell(g_list[3], 49, 1, Field(output.offerho8, sizeof(output.offerho8)));
    SetCell(g_list[3], 50, 0, "매수호가8");
    SetCell(g_list[3], 50, 1, Field(output.bidho8, sizeof(output.bidho8)));
    SetCell(g_list[3], 51, 0, "매도호가수량8");
    SetCell(g_list[3], 51, 1, Field(output.offerrem8, sizeof(output.offerrem8)));
    SetCell(g_list[3], 52, 0, "매수호가수량8");
    SetCell(g_list[3], 52, 1, Field(output.bidrem8, sizeof(output.bidrem8)));
    SetCell(g_list[3], 53, 0, "직전매도대비수량8");
    SetCell(g_list[3], 53, 1, Field(output.preoffercha8, sizeof(output.preoffercha8)));
    SetCell(g_list[3], 54, 0, "직전매수대비수량8");
    SetCell(g_list[3], 54, 1, Field(output.prebidcha8, sizeof(output.prebidcha8)));
    SetCell(g_list[3], 55, 0, "매도호가9");
    SetCell(g_list[3], 55, 1, Field(output.offerho9, sizeof(output.offerho9)));
    SetCell(g_list[3], 56, 0, "매수호가9");
    SetCell(g_list[3], 56, 1, Field(output.bidho9, sizeof(output.bidho9)));
    SetCell(g_list[3], 57, 0, "매도호가수량9");
    SetCell(g_list[3], 57, 1, Field(output.offerrem9, sizeof(output.offerrem9)));
    SetCell(g_list[3], 58, 0, "매수호가수량9");
    SetCell(g_list[3], 58, 1, Field(output.bidrem9, sizeof(output.bidrem9)));
    SetCell(g_list[3], 59, 0, "직전매도대비수량9");
    SetCell(g_list[3], 59, 1, Field(output.preoffercha9, sizeof(output.preoffercha9)));
    SetCell(g_list[3], 60, 0, "직전매수대비수량9");
    SetCell(g_list[3], 60, 1, Field(output.prebidcha9, sizeof(output.prebidcha9)));
    SetCell(g_list[3], 61, 0, "매도호가10");
    SetCell(g_list[3], 61, 1, Field(output.offerho10, sizeof(output.offerho10)));
    SetCell(g_list[3], 62, 0, "매수호가10");
    SetCell(g_list[3], 62, 1, Field(output.bidho10, sizeof(output.bidho10)));
    SetCell(g_list[3], 63, 0, "매도호가수량10");
    SetCell(g_list[3], 63, 1, Field(output.offerrem10, sizeof(output.offerrem10)));
    SetCell(g_list[3], 64, 0, "매수호가수량10");
    SetCell(g_list[3], 64, 1, Field(output.bidrem10, sizeof(output.bidrem10)));
    SetCell(g_list[3], 65, 0, "직전매도대비수량10");
    SetCell(g_list[3], 65, 1, Field(output.preoffercha10, sizeof(output.preoffercha10)));
    SetCell(g_list[3], 66, 0, "직전매수대비수량10");
    SetCell(g_list[3], 66, 1, Field(output.prebidcha10, sizeof(output.prebidcha10)));
    SetCell(g_list[3], 67, 0, "매도호가수량합");
    SetCell(g_list[3], 67, 1, Field(output.offer, sizeof(output.offer)));
    SetCell(g_list[3], 68, 0, "매수호가수량합");
    SetCell(g_list[3], 68, 1, Field(output.bid, sizeof(output.bid)));
    SetCell(g_list[3], 69, 0, "직전매도대비수량합");
    SetCell(g_list[3], 69, 1, Field(output.preoffercha, sizeof(output.preoffercha)));
    SetCell(g_list[3], 70, 0, "직전매수대비수량합");
    SetCell(g_list[3], 70, 1, Field(output.prebidcha, sizeof(output.prebidcha)));
    SetCell(g_list[3], 71, 0, "수신시간");
    SetCell(g_list[3], 71, 1, Field(output.hotime, sizeof(output.hotime)));
    SetCell(g_list[3], 72, 0, "예상체결가격");
    SetCell(g_list[3], 72, 1, Field(output.yeprice, sizeof(output.yeprice)));
    SetCell(g_list[3], 73, 0, "예상체결수량");
    SetCell(g_list[3], 73, 1, Field(output.yevolume, sizeof(output.yevolume)));
    SetCell(g_list[3], 74, 0, "예상체결전일구분");
    SetCell(g_list[3], 74, 1, Field(output.yesign, sizeof(output.yesign)));
    SetCell(g_list[3], 75, 0, "예상체결전일대비");
    SetCell(g_list[3], 75, 1, Field(output.yechange, sizeof(output.yechange)));
    SetCell(g_list[3], 76, 0, "예상체결등락율");
    SetCell(g_list[3], 76, 1, Field(output.yediff, sizeof(output.yediff)));
    SetCell(g_list[3], 77, 0, "시간외매도잔량");
    SetCell(g_list[3], 77, 1, Field(output.tmoffer, sizeof(output.tmoffer)));
    SetCell(g_list[3], 78, 0, "시간외매수잔량");
    SetCell(g_list[3], 78, 1, Field(output.tmbid, sizeof(output.tmbid)));
    SetCell(g_list[3], 79, 0, "동시구분");
    SetCell(g_list[3], 79, 1, Field(output.ho_status, sizeof(output.ho_status)));
    SetCell(g_list[3], 80, 0, "단축코드");
    SetCell(g_list[3], 80, 1, Field(output.shcode, sizeof(output.shcode)));
    SetCell(g_list[3], 81, 0, "상한가");
    SetCell(g_list[3], 81, 1, Field(output.uplmtprice, sizeof(output.uplmtprice)));
    SetCell(g_list[3], 82, 0, "하한가");
    SetCell(g_list[3], 82, 1, Field(output.dnlmtprice, sizeof(output.dnlmtprice)));
    SetCell(g_list[3], 83, 0, "시가");
    SetCell(g_list[3], 83, 1, Field(output.open, sizeof(output.open)));
    SetCell(g_list[3], 84, 0, "고가");
    SetCell(g_list[3], 84, 1, Field(output.high, sizeof(output.high)));
    SetCell(g_list[3], 85, 0, "저가");
    SetCell(g_list[3], 85, 1, Field(output.low, sizeof(output.low)));
}

// 고정 블록은 이름과 정확한 크기가 일치할 때만 읽습니다.
BOOL ValidBody(const RP_DATA_INFO *response)
{
    if (!response || !response->lpData || response->nDataLength < 0 ||
        response->nDataLength > response->nTotalDataBufferSize) return FALSE;
    std::string block = Field(response->szBlockName, sizeof(response->szBlockName));
    return (block == "s1512OutBlock" && response->nDataLength == sizeof(s1512OutBlock)) ||
           (block == "s1512OutBlock1" && response->nDataLength == sizeof(s1512OutBlock1)) ||
           (block == "s1512OutBlock2" && response->nDataLength == sizeof(s1512OutBlock2)) ||
           (block == "s1512OutBlock3" && response->nDataLength == sizeof(s1512OutBlock3));
}

BOOL ReceiveData(const RP_DATA_INFO *response)
{
    if (!response || response->nRqID != g_requestId || g_requestId < 0 || !ValidBody(response)) return FALSE;
    if (Field(response->Header.szTrCode, sizeof(response->Header.szTrCode)) != SERVICE_CODE) return FALSE;
    if (response->Header.szRetFlag[0] >= '1' && response->Header.szRetFlag[0] <= '9') return FALSE;
    std::string block = Field(response->szBlockName, sizeof(response->szBlockName));
    if (block == "s1512OutBlock")
    {
        s1512OutBlock output;
        memcpy(&output, response->lpData, sizeof(output));
        ShowBlock0(output);
        g_received[0] = 1;
        return TRUE;
    }
    if (block == "s1512OutBlock1")
    {
        s1512OutBlock1 output;
        memcpy(&output, response->lpData, sizeof(output));
        ShowBlock1(output);
        g_received[1] = 1;
        return TRUE;
    }
    if (block == "s1512OutBlock2")
    {
        s1512OutBlock2 output;
        memcpy(&output, response->lpData, sizeof(output));
        ShowBlock2(output);
        g_received[2] = 1;
        return TRUE;
    }
    if (block == "s1512OutBlock3")
    {
        s1512OutBlock3 output;
        memcpy(&output, response->lpData, sizeof(output));
        ShowBlock3(output);
        g_received[3] = 1;
        return TRUE;
    }
    return FALSE;
}

void ShowOutput()
{
    int index = TabCtrl_GetCurSel(g_tabs);
    if (index < 0 || index >= 4) return;
    SetWindowTextW(g_output, g_received[index] ? L"고정 블록 수신" : L"고정 블록 미수신");
}

void ShowSelectedTab()
{
    int selected = TabCtrl_GetCurSel(g_tabs);
    for (int i = 0; i < 4; ++i)
        ShowWindow(g_list[i], i == selected ? SW_SHOW : SW_HIDE);
    ShowOutput();
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
        if (!g_queryError && (!g_received[0] || !g_received[1] || !g_received[2] || !g_received[3]))
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
    s1512InBlock input;
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
        g_tabs = Common::AddControl(window, WC_TABCONTROLW, L"", WS_TABSTOP | WS_CLIPSIBLINGS,
                                    580, 90, 540, 500, 303);
        if (!g_tabs) return -1;
        const wchar_t *names[] = {L"ELW 종목·시세", L"ELW 호가·LP", L"기초자산 시세", L"추가 호가"};
        for (int i = 0; i < 4; ++i)
        {
            TCITEMW tab = {0};
            tab.mask = TCIF_TEXT;
            tab.pszText = const_cast<wchar_t *>(names[i]);
            SendMessageW(g_tabs, TCM_INSERTITEMW, i, (LPARAM)&tab);
            g_list[i] = Common::AddControl(window, WC_LISTVIEWW, L"", LVS_REPORT | LVS_SINGLESEL | WS_TABSTOP,
                                          590, 146, 520, 432, 310 + i);
            if (!g_list[i]) return -1;
            ListView_SetExtendedListViewStyle(g_list[i], LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
            AddColumn(g_list[i], 0, L"항목", 218);
            AddColumn(g_list[i], 1, L"값", 268);
        }
        g_output = Common::AddControl(window, L"STATIC", L"", 0, 590, 120, 510, 22, 0);
        SetWindowPos(g_tabs, HWND_BOTTOM, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
        ShowSelectedTab();
        UpdateQueryButton();
        return 0;
    }
    case WM_COMMAND:
        if (LOWORD(wParam) == QUERY_BUTTON && HIWORD(wParam) == BN_CLICKED)
            RequestData(window);
        return 0;
    case WM_NOTIFY:
        if (lParam && ((const NMHDR *)lParam)->hwndFrom == g_tabs &&
            ((const NMHDR *)lParam)->code == TCN_SELCHANGE)
            ShowSelectedTab();
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
    INITCOMMONCONTROLSEX controls = {sizeof(controls), ICC_LISTVIEW_CLASSES | ICC_TAB_CLASSES};
    if (!InitCommonControlsEx(&controls)) return 1;
    WNDCLASSW wc = {0};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = instance;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
    wc.lpszClassName = L"OrdaAPIs1512ElwQuote";
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
