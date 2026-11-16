// 읽는 순서: WinMain 초기화 → WindowProc 로그인 통지 → 요청 함수 → 수신 함수 → 종료 처리.
// Load/Initialize와 Connect/Login은 기존 Common Helper를 사용합니다.
// 로그인 결과는 Common::HandleMessage가 처리하며 요청 API는 아래에서 직접 호출합니다.
// VS2010 v100 / Win32 / MultiByte / C++03. 주문을 자동 전송하거나 재시도하지 않습니다.
#include "../../../../common/OrdaSampleCommon.h"
#include "b8001.h"
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <string>
#include <vector>
#include <deque>
namespace Common = OrdaSampleCommon;
const char SERVICE_CODE[] = NAME_b8001;
const wchar_t SERVICE_NAME[] = L"현물매도주문";
typedef b8001InBlock OrderInput;
struct Choice
{
    const wchar_t *label;
    const char *code;
};
const Choice BSCD[] = {{L"매도 (1)", "1"}, {L"매수 (2)", "2"}};
const Choice PB_ORDTYPE[] = {{L"지정가 (00)", "00"},
                             {L"시장가 (03)", "03"},
                             {L"최유리지정가 (06)", "06"},
                             {L"최우선지정가 (07)", "07"}};
struct InputField
{
    const wchar_t *label;
    int offset, width;
    const char *initial;
    BOOL password, readOnly;
    const Choice *choices;
    int choiceCount;
};
// 승인된 XML 예외: SorTrxMthd의 공백 41개 기본값을 char[40]에 맞춰 공백 40개로 적용합니다.
const InputField INPUT_FIELDS[] = {
    {L"레코드갯수 (reccnt)", 0, 5, "00001", FALSE, FALSE, NULL, 0},
    {L"계좌번호 (accno)", 5, 20, "", FALSE, FALSE, NULL, 0},
    {L"계좌비밀번호 (passwd)", 25, 8, "", TRUE, FALSE, NULL, 0},
    {L"종목번호 (expcode)", 33, 12, "", FALSE, FALSE, NULL, 0},
    {L"주문수량 (qty)", 45, 16, "", FALSE, FALSE, NULL, 0},
    {L"주문가 (price)", 61, 13, "", FALSE, FALSE, NULL, 0},
    {L"매매구분 (memegb)", 74, 1, "1", FALSE, FALSE, BSCD, _countof(BSCD)},
    {L"호가유형코드 (hogagb)", 75, 2, "00", FALSE, FALSE, PB_ORDTYPE, _countof(PB_ORDTYPE)},
    {L"프로그램호가유형코드 (pgmtype)", 77, 2, "00", FALSE, FALSE, NULL, 0},
    {L"공매도가능여부 (gongtype)", 79, 1, "0", FALSE, FALSE, NULL, 0},
    {L"공매도호가구분 (gonghoga)", 80, 1, "0", FALSE, FALSE, NULL, 0},
    {L"통신매체코드 (tongsingb)", 81, 2, "11", FALSE, FALSE, NULL, 0},
    {L"신용거래코드 (sinmemecode)", 83, 3, "000", FALSE, FALSE, NULL, 0},
    {L"대출일 (loandt)", 86, 8, "00000000", FALSE, FALSE, NULL, 0},
    {L"회원번호 (memnumber)", 94, 3, "   ", FALSE, FALSE, NULL, 0},
    {L"주문조건구분 (ordcondgb)", 97, 1, "0", FALSE, FALSE, NULL, 0},
    {L"전략코드 (stragb)", 98, 6, "      ", FALSE, FALSE, NULL, 0},
    {L"그룹ID (groupid)", 104, 20, "                    ", FALSE, FALSE, NULL, 0},
    {L"주문회차 (ordernum)", 124, 10, "0000000000", FALSE, FALSE, NULL, 0},
    {L"포트폴리오번호 (portnum)", 134, 10, "0000000000", FALSE, FALSE, NULL, 0},
    {L"바스켓번호 (basketnum)", 144, 10, "0000000000", FALSE, FALSE, NULL, 0},
    {L"트렌치번호 (tranchnum)", 154, 10, "0000000000", FALSE, FALSE, NULL, 0},
    {L"아이템번호 (itemnum)", 164, 10, "0000000000", FALSE, FALSE, NULL, 0},
    {L"운용지시번호 (operordnum)", 174, 12, "            ", FALSE, FALSE, NULL, 0},
    {L"유동성공급자여부 (flowsupgb)", 186, 1, "0", FALSE, FALSE, NULL, 0},
    {L"반대매매구분 (oppbuygb)", 187, 1, "0", FALSE, FALSE, NULL, 0},
    {L"목표주문시장코드 (TrgtOrdMktCode)", 188, 2, "  ", FALSE, FALSE, NULL, 0},
    {L"자사주신고서ID (TrsrstkStmtDocID)", 190, 5, "     ", FALSE, FALSE, NULL, 0},
    {L"위탁사번호 (CsgnFirmNo)", 195, 5, "     ", FALSE, FALSE, NULL, 0},
    {L"알고리즘전략구분코드 (AlgoStrtgCode)", 200, 1, " ", FALSE, FALSE, NULL, 0},
    {L"거래자ID (TrdrID)", 201, 6, "      ", FALSE, FALSE, NULL, 0},
    {L"호가그룹번호 (OrdGrpNo)", 207, 2, "  ", FALSE, FALSE, NULL, 0},
    {L"자전거래방지코드 (SmpCode)", 209, 1, " ", FALSE, FALSE, NULL, 0},
    {L"CFD투자자분류코드 (CfdInvstrClssCode)", 210, 4, "    ", FALSE, FALSE, NULL, 0},
    {L"조건가격 (CndiPrc)", 214, 13, "0000000000.00", FALSE, FALSE, NULL, 0},
    {L"공매도ID (StslID)", 227, 10, "          ", FALSE, FALSE, NULL, 0},
    {L"SOR처리방법 (SorTrxMthd)",
     237,
     40,
     "                                        ",
     FALSE,
     FALSE,
     NULL,
     0},
    {L"PowerBASE내부영역 (PbArea)",
     277,
     50,
     "                                                  ",
     FALSE,
     TRUE,
     NULL,
     0}};
HWND g_inputs[_countof(INPUT_FIELDS)], g_labels[_countof(INPUT_FIELDS)];
HWND g_panel, g_order, g_prepare, g_retry, g_output;
int g_requestId = -1, g_scroll = 0;
BOOL g_submitting = FALSE, g_closing = FALSE, g_draining = FALSE;
BOOL g_closeRequested = FALSE;
std::string g_requestSecret;
BOOL g_armed = TRUE, g_uncertain = FALSE, g_received = FALSE, g_error = FALSE;
struct Notification
{
    UINT message;
    WPARAM kind;
    int id;
    BOOL malformed, systemError;
    RP_DATA_INFO response;
    std::vector<unsigned char> data;
    std::string code, text;
    Notification() : message(0), kind(0), id(-1), malformed(FALSE), systemError(FALSE)
    {
    }
};
std::deque<Notification> g_pending;
std::string Field(const void *data, int width)
{
    std::string s((const char *)data, width);
    while (!s.empty() && (s[s.size() - 1] == ' ' || s[s.size() - 1] == 0))
        s.erase(s.size() - 1);
    // 필드 중간의 NUL·제어문자가 뒤의 표시 내용을 숨기지 않도록 치환합니다.
    for (size_t i = 0; i < s.size(); ++i)
        if ((unsigned char)s[i] < 32)
            s[i] = '?';
    return s;
}
void Log(const char *text)
{
    if (!g_output)
        return;
    if (GetWindowTextLengthA(g_output) > 200000)
        SetWindowTextA(g_output, "");
    SendMessage(g_output, EM_SETSEL, (WPARAM)-1, (LPARAM)-1);
    SendMessageA(g_output, EM_REPLACESEL, FALSE, (LPARAM)text);
    SendMessageA(g_output, EM_REPLACESEL, FALSE, (LPARAM) "\r\n");
}
void UpdateControls()
{
    BOOL idle = !g_closing && !g_submitting && !Common::IsBusy() && g_requestId < 0;
    EnableWindow(g_order, idle && Common::IsReady() && g_armed && !g_uncertain);
    EnableWindow(g_prepare, idle && !g_armed);
    EnableWindow(g_retry, idle && !Common::IsReady());
    for (int i = 0; i < _countof(INPUT_FIELDS); ++i)
        EnableWindow(g_inputs[i], idle);
}

// 입력 전문: 화면 값과 실제 코드값을 구조체 멤버에 설정합니다.
BOOL BuildInput(OrderInput &input)
{
    // API_MFCDlg::Requestt3086처럼 실제 입력 구조체 전체를 0x20으로 초기화합니다.
    memset(&input, 0x20, sizeof(b8001InBlock));
    // 실제 구조체 멤버에 입력을 대응시키며 런타임 XML 파싱이나 배치 변경은 하지 않습니다.
    char *fields[] = {input.reccnt,
                      input.accno,
                      input.passwd,
                      input.expcode,
                      input.qty,
                      input.price,
                      input.memegb,
                      input.hogagb,
                      input.pgmtype,
                      input.gongtype,
                      input.gonghoga,
                      input.tongsingb,
                      input.sinmemecode,
                      input.loandt,
                      input.memnumber,
                      input.ordcondgb,
                      input.stragb,
                      input.groupid,
                      input.ordernum,
                      input.portnum,
                      input.basketnum,
                      input.tranchnum,
                      input.itemnum,
                      input.operordnum,
                      input.flowsupgb,
                      input.oppbuygb,
                      input.TrgtOrdMktCode,
                      input.TrsrstkStmtDocID,
                      input.CsgnFirmNo,
                      input.AlgoStrtgCode,
                      input.TrdrID,
                      input.OrdGrpNo,
                      input.SmpCode,
                      input.CfdInvstrClssCode,
                      input.CndiPrc,
                      input.StslID,
                      input.SorTrxMthd,
                      input.PbArea};
    for (int i = 0; i < _countof(INPUT_FIELDS); ++i)
    {
        const InputField &f = INPUT_FIELDS[i];
        char value[256] = {0};
        int length = 0;
        if (f.choiceCount)
        {
            int selected = (int)SendMessage(g_inputs[i], CB_GETCURSEL, 0, 0);
            if (selected < 0 || selected >= f.choiceCount)
            {
                Common::SetResult(L"주문 입력의 선택 항목을 선택하십시오.");
                return FALSE;
            }
            const char *code = f.choices[selected].code;
            length = (int)strlen(code);
            memcpy(value, code, length);
        }
        else
        {
            if (GetWindowTextLengthA(g_inputs[i]) >= sizeof(value))
            {
                Common::SetResult(L"입력값이 너무 깁니다.");
                return FALSE;
            }
            length = GetWindowTextA(g_inputs[i], value, sizeof(value));
        }
        const BOOL priceField = fields[i] == input.price || fields[i] == input.CndiPrc;
        if (fields[i] == input.qty || priceField)
        {
            // 수량·주문가격·조건가격은 앞뒤 ASCII 공백을 제거하고 숫자를 오른쪽에 배치합니다.
            int first = 0, end = length;
            while (first < end && strchr(" \t\r\n\v\f", value[first]))
                ++first;
            while (end > first && strchr(" \t\r\n\v\f", value[end - 1]))
                --end;
            length = end - first;
            const int width = (int)(fields[i] == input.qty ? sizeof(input.qty)
                                      : fields[i] == input.price ? sizeof(input.price)
                                                                : sizeof(input.CndiPrc));
            // CndiPrc의 빈 입력은 0으로 처리하고 XML의 13개 0 기본값도 0으로 변환합니다.
            if (fields[i] == input.CndiPrc &&
                (length == 0 || (length == width && memcmp(value + first, f.initial, width) == 0)))
            {
                value[0] = '0';
                first = 0;
                end = 1;
                length = 1;
            }
            // 가격은 마지막 3바이트를 .00으로 사용하고 수량은 전체 길이를 사용합니다.
            const int integerWidth = priceField ? width - 3 : width;
            if (integerWidth <= 0 || length == 0 || length > integerWidth)
            {
                SecureZeroMemory(value, sizeof(value));
                Common::SetResult(
                    L"수량·가격은 공백을 제외한 숫자를 필드 길이 이내로 입력하십시오.");
                return FALSE;
            }
            for (int j = first; j < end; ++j)
                if (value[j] < '0' || value[j] > '9')
                {
                    SecureZeroMemory(value, sizeof(value));
                    Common::SetResult(
                        L"수량·가격은 음수나 기타 문자 없이 숫자로 입력하십시오.");
                    return FALSE;
                }
            // 고정 길이 필드에 종료 NUL을 넣지 않으며 숫자 0도 허용합니다.
            memset(fields[i], '0', width);
            memcpy(fields[i] + integerWidth - length, value + first, length);
            if (priceField)
                memcpy(fields[i] + integerWidth, ".00", 3);
        }
        else
        {
            if (length > f.width)
            {
                SecureZeroMemory(value, sizeof(value));
                Common::SetResult(L"입력값의 바이트 길이가 XML 필드 길이를 초과했습니다.");
                return FALSE;
            }
            memcpy(fields[i], value, length);
        }
        SecureZeroMemory(value, sizeof(value));
    }
    return TRUE;
}
void ClearPasswords()
{
    for (int i = 0; i < _countof(INPUT_FIELDS); ++i)
        if (INPUT_FIELDS[i].password)
            SetWindowTextA(g_inputs[i], "");
}

// 주문 응답: 검증한 두 고정 블록에 구조체 포인터로 접근합니다.
BOOL Receiveb8001(const RP_DATA_INFO *pdi, std::string &text)
{
    if (!pdi || !pdi->lpData || pdi->nDataLength != 627 ||
        Field(pdi->Header.szTrCode, 6) != SERVICE_CODE || !Field(pdi->szBlockName, 17).empty())
        return FALSE;
    // B형이며 block/attr/occurs가 없으므로 고정 출력 블록이 XML 순서로 이어집니다.
    // API_MFCDlg::Receivet3086처럼 lpData를 제공된 전문 구조체로 접근합니다.
    // 이 서비스는 t3086의 반복 단일 블록과 달리 고정 블록 2개로 정의되어 있습니다.
    // 포인터 접근 전에 길이와 서비스를 검사하며 포인터는 이 함수 안에서만 사용합니다.
    const b8001OutBlock *pOut = (const b8001OutBlock *)pdi->lpData;
    const b8001OutBlock1 *pOut1 = (const b8001OutBlock1 *)(pOut + 1);
    text = "b8001OutBlock\r\n";
    text += "reccnt=";
    text += Field(pOut->reccnt, sizeof(pOut->reccnt));
    text += "\r\n";
    text += "accno=";
    text += Field(pOut->accno, sizeof(pOut->accno));
    text += "\r\n";
    text += "passwd=";
    text += "[redacted]";
    text += "\r\n";
    text += "expcode=";
    text += Field(pOut->expcode, sizeof(pOut->expcode));
    text += "\r\n";
    text += "qty=";
    text += Field(pOut->qty, sizeof(pOut->qty));
    text += "\r\n";
    text += "price=";
    text += Field(pOut->price, sizeof(pOut->price));
    text += "\r\n";
    text += "memegb=";
    text += Field(pOut->memegb, sizeof(pOut->memegb));
    text += "\r\n";
    text += "hogagb=";
    text += Field(pOut->hogagb, sizeof(pOut->hogagb));
    text += "\r\n";
    text += "pgmtype=";
    text += Field(pOut->pgmtype, sizeof(pOut->pgmtype));
    text += "\r\n";
    text += "gongtype=";
    text += Field(pOut->gongtype, sizeof(pOut->gongtype));
    text += "\r\n";
    text += "gonghoga=";
    text += Field(pOut->gonghoga, sizeof(pOut->gonghoga));
    text += "\r\n";
    text += "tongsingb=";
    text += Field(pOut->tongsingb, sizeof(pOut->tongsingb));
    text += "\r\n";
    text += "sinmemecode=";
    text += Field(pOut->sinmemecode, sizeof(pOut->sinmemecode));
    text += "\r\n";
    text += "loandt=";
    text += Field(pOut->loandt, sizeof(pOut->loandt));
    text += "\r\n";
    text += "memnumber=";
    text += Field(pOut->memnumber, sizeof(pOut->memnumber));
    text += "\r\n";
    text += "ordcondgb=";
    text += Field(pOut->ordcondgb, sizeof(pOut->ordcondgb));
    text += "\r\n";
    text += "stragb=";
    text += Field(pOut->stragb, sizeof(pOut->stragb));
    text += "\r\n";
    text += "groupid=";
    text += Field(pOut->groupid, sizeof(pOut->groupid));
    text += "\r\n";
    text += "ordernum=";
    text += Field(pOut->ordernum, sizeof(pOut->ordernum));
    text += "\r\n";
    text += "portnum=";
    text += Field(pOut->portnum, sizeof(pOut->portnum));
    text += "\r\n";
    text += "basketnum=";
    text += Field(pOut->basketnum, sizeof(pOut->basketnum));
    text += "\r\n";
    text += "tranchnum=";
    text += Field(pOut->tranchnum, sizeof(pOut->tranchnum));
    text += "\r\n";
    text += "itemnum=";
    text += Field(pOut->itemnum, sizeof(pOut->itemnum));
    text += "\r\n";
    text += "operordnum=";
    text += Field(pOut->operordnum, sizeof(pOut->operordnum));
    text += "\r\n";
    text += "flowsupgb=";
    text += Field(pOut->flowsupgb, sizeof(pOut->flowsupgb));
    text += "\r\n";
    text += "oppbuygb=";
    text += Field(pOut->oppbuygb, sizeof(pOut->oppbuygb));
    text += "\r\n";
    text += "TrgtOrdMktCode=";
    text += Field(pOut->TrgtOrdMktCode, sizeof(pOut->TrgtOrdMktCode));
    text += "\r\n";
    text += "TrsrstkStmtDocID=";
    text += Field(pOut->TrsrstkStmtDocID, sizeof(pOut->TrsrstkStmtDocID));
    text += "\r\n";
    text += "CsgnFirmNo=";
    text += Field(pOut->CsgnFirmNo, sizeof(pOut->CsgnFirmNo));
    text += "\r\n";
    text += "AlgoStrtgCode=";
    text += Field(pOut->AlgoStrtgCode, sizeof(pOut->AlgoStrtgCode));
    text += "\r\n";
    text += "TrdrID=";
    text += Field(pOut->TrdrID, sizeof(pOut->TrdrID));
    text += "\r\n";
    text += "OrdGrpNo=";
    text += Field(pOut->OrdGrpNo, sizeof(pOut->OrdGrpNo));
    text += "\r\n";
    text += "SmpCode=";
    text += Field(pOut->SmpCode, sizeof(pOut->SmpCode));
    text += "\r\n";
    text += "CfdInvstrClssCode=";
    text += Field(pOut->CfdInvstrClssCode, sizeof(pOut->CfdInvstrClssCode));
    text += "\r\n";
    text += "CndiPrc=";
    text += Field(pOut->CndiPrc, sizeof(pOut->CndiPrc));
    text += "\r\n";
    text += "StslID=";
    text += Field(pOut->StslID, sizeof(pOut->StslID));
    text += "\r\n";
    text += "SorTrxMthd=";
    text += Field(pOut->SorTrxMthd, sizeof(pOut->SorTrxMthd));
    text += "\r\n";
    text += "PbArea=";
    text += Field(pOut->PbArea, sizeof(pOut->PbArea));
    text += "\r\n";
    text += "b8001OutBlock1\r\n";
    text += "reccnt=";
    text += Field(pOut1->reccnt, sizeof(pOut1->reccnt));
    text += "\r\n";
    text += "OrdMktCode0=";
    text += Field(pOut1->OrdMktCode0, sizeof(pOut1->OrdMktCode0));
    text += "\r\n";
    text += "OrdNo0=";
    text += Field(pOut1->OrdNo0, sizeof(pOut1->OrdNo0));
    text += "\r\n";
    text += "OrdMktCode1=";
    text += Field(pOut1->OrdMktCode1, sizeof(pOut1->OrdMktCode1));
    text += "\r\n";
    text += "OrdNo1=";
    text += Field(pOut1->OrdNo1, sizeof(pOut1->OrdNo1));
    text += "\r\n";
    text += "OrdMktCode2=";
    text += Field(pOut1->OrdMktCode2, sizeof(pOut1->OrdMktCode2));
    text += "\r\n";
    text += "OrdNo2=";
    text += Field(pOut1->OrdNo2, sizeof(pOut1->OrdNo2));
    text += "\r\n";
    text += "OrdMktCode3=";
    text += Field(pOut1->OrdMktCode3, sizeof(pOut1->OrdMktCode3));
    text += "\r\n";
    text += "OrdNo3=";
    text += Field(pOut1->OrdNo3, sizeof(pOut1->OrdNo3));
    text += "\r\n";
    text += "OrdMktCode4=";
    text += Field(pOut1->OrdMktCode4, sizeof(pOut1->OrdMktCode4));
    text += "\r\n";
    text += "OrdNo4=";
    text += Field(pOut1->OrdNo4, sizeof(pOut1->OrdNo4));
    text += "\r\n";
    text += "ordtime=";
    text += Field(pOut1->ordtime, sizeof(pOut1->ordtime));
    text += "\r\n";
    text += "ordcode=";
    text += Field(pOut1->ordcode, sizeof(pOut1->ordcode));
    text += "\r\n";
    text += "expcode=";
    text += Field(pOut1->expcode, sizeof(pOut1->expcode));
    text += "\r\n";
    text += "operno=";
    text += Field(pOut1->operno, sizeof(pOut1->operno));
    text += "\r\n";
    text += "ordamt=";
    text += Field(pOut1->ordamt, sizeof(pOut1->ordamt));
    text += "\r\n";
    text += "fillermemegb=";
    text += Field(pOut1->fillermemegb, sizeof(pOut1->fillermemegb));
    text += "\r\n";
    text += "oppbuyseqno=";
    text += Field(pOut1->oppbuyseqno, sizeof(pOut1->oppbuyseqno));
    text += "\r\n";
    text += "preeordno=";
    text += Field(pOut1->preeordno, sizeof(pOut1->preeordno));
    text += "\r\n";
    text += "realordcnt=";
    text += Field(pOut1->realordcnt, sizeof(pOut1->realordcnt));
    text += "\r\n";
    text += "recoordcnt=";
    text += Field(pOut1->recoordcnt, sizeof(pOut1->recoordcnt));
    text += "\r\n";
    text += "cashordamt=";
    text += Field(pOut1->cashordamt, sizeof(pOut1->cashordamt));
    text += "\r\n";
    text += "daeordamt=";
    text += Field(pOut1->daeordamt, sizeof(pOut1->daeordamt));
    text += "\r\n";
    text += "recordamt=";
    text += Field(pOut1->recordamt, sizeof(pOut1->recordamt));
    text += "\r\n";
    text += "accno=";
    text += Field(pOut1->accno, sizeof(pOut1->accno));
    text += "\r\n";
    text += "hname=";
    text += Field(pOut1->hname, sizeof(pOut1->hname));
    text += "\r\n";
    return TRUE;
}
void ReleaseActive()
{
    int id = g_requestId;
    g_requestId = -1;
    if (!g_requestSecret.empty())
        SecureZeroMemory(&g_requestSecret[0], g_requestSecret.size());
    g_requestSecret.clear();
    if (id >= 0 && Common::Api())
        Common::Api()->ReleaseRequestData(id);
}
void Redact(std::string &text);
void ProcessNotification(Notification &n)
{
    if (g_closing || g_requestId < 0 || n.id != g_requestId)
        return;
    if (n.message == Common::API_MESSAGE_BASE + WM_TIMEOUT_DATA)
    {
        g_uncertain = TRUE;
        ReleaseActive();
        Common::SetResult(
            L"응답 시간 초과: 최종 주문 결과 미확인. 재주문 전에 주문 내역을 확인하십시오.");
        Log("Timeout: outcome unknown; no automatic resend.");
    }
    else if (n.kind == REQUEST_DATA)
    {
        n.response.lpData = n.data.empty() ? NULL : &n.data[0];
        std::string text;
        if (n.malformed || !Receiveb8001(&n.response, text))
        {
            g_error = TRUE;
            g_uncertain = TRUE;
            Log("Invalid response service/block/length; outcome unknown.");
            Common::SetResult(
                L"주문 응답 형식이 맞지 않습니다. 실제 주문 결과를 별도로 확인하십시오.");
        }
        else
        {
            std::string header = "Header.szMsgCode=" + Field(n.response.Header.szMsgCode, 5) +
                                 "\r\nHeader.szMsgTxt=" + Field(n.response.Header.szMsgTxt, 81);
            Redact(header);
            Redact(text);
            Log(header.c_str());
            Log(text.c_str());
            g_received = TRUE;
            Common::SetResult(
                g_error
                    ? L"응답 데이터와 오류 통지를 확인하십시오. 자동 재주문하지 않습니다."
                    : L"주문 응답 수신. 표시된 실제 주문번호 및 서버 메시지를 확인하십시오. 체결 완료를 의미하지 않습니다.");
        }
    }
    else if (n.kind == MESSAGE_DATA || n.kind == SYSTEM_ERROR_DATA)
    {
        Common::SetServerResult(n.code.c_str(), n.text.c_str());
        // 서버 메시지에 비밀번호가 포함될 수 있으므로 수신 시 가린 뒤 표시합니다.
        Log((n.code + ": " + n.text).c_str());
        if (n.systemError || n.kind == SYSTEM_ERROR_DATA)
            g_error = TRUE;
        if (n.systemError == 1)
        {
            // 시스템 오류이면 제품의 ReleaseMessageData가 이미 해당 요청도 해제한 상태입니다.
            g_requestId = -1;
            if (!g_received)
                g_uncertain = TRUE;
            if (!g_requestSecret.empty())
                SecureZeroMemory(&g_requestSecret[0], g_requestSecret.size());
            g_requestSecret.clear();
        }
    }
    else if (n.kind == RELEASE_DATA)
    {
        ReleaseActive();
        if (!g_received)
        {
            g_uncertain = TRUE;
            Log("Request released without a validated order response; outcome unknown.");
            Common::SetResult(L"?? ??: ??? ?? ??? ?? ?? ??? ??? ? ????. ?? ??? ??? ??????.");
        }
    }
    UpdateControls();
}
void Redact(std::string &text)
{
    if (!g_requestSecret.empty())
    {
        size_t pos = 0;
        while ((pos = text.find(g_requestSecret, pos)) != std::string::npos)
        {
            text.replace(pos, g_requestSecret.size(), "[redacted]");
            pos += 10;
        }
    }
}

// API 통지: 콜백의 유효한 버퍼를 복사하고 재진입 통지는 순서대로 보관합니다.
void CaptureNotification(UINT message, WPARAM kind, LPARAM parameter)
{
    // REQUEST_DATA는 활성 요청 소유이며 정리 뒤에는 해제됐을 수 있으므로 접근하지 않습니다.
    if (kind == REQUEST_DATA && (g_closing || (g_requestId < 0 && !g_submitting)))
        return;
    Notification n;
    n.message = message;
    n.kind = kind;
    if (message == Common::API_MESSAGE_BASE + WM_TIMEOUT_DATA || kind == RELEASE_DATA)
        n.id = (int)parameter;
    else if (kind == REQUEST_DATA)
    {
        const RP_DATA_INFO *p = (const RP_DATA_INFO *)parameter;
        if (!p)
            return;
        n.id = p->nRqID;
        n.response = *p;
        n.response.lpData = NULL;
        n.malformed = p->nDataLength != 627 || !p->lpData;
        if (!n.malformed)
            n.data.assign(p->lpData, p->lpData + p->nDataLength);
    }
    else if (kind == MESSAGE_DATA || kind == SYSTEM_ERROR_DATA)
    {
        const MSG_PACKET *p = (const MSG_PACKET *)parameter;
        if (!p)
            return;
        n.id = p->nRqID;
        n.systemError = p->nIsSystemError;
        n.code = Field(p->szCode, 5);
        if (p->nMsgLength >= 0 && p->nMsgLength <= 4096 &&
            (p->nMsgLength == 0 || p->lpszMessageData))
        {
            if (p->nMsgLength)
                n.text.assign((const char *)p->lpszMessageData, p->nMsgLength);
            Redact(n.text);
        }
        else
        {
            n.text = "Invalid server message length";
            n.systemError = TRUE;
        }
        if (Common::Api())
            Common::Api()->ReleaseMessageData(parameter);
    }
    else
        return;
    if (g_submitting || g_draining)
        g_pending.push_back(n);
    else
    {
        g_draining = TRUE;
        ProcessNotification(n);
        g_draining = FALSE;
    }
}
void DrainNotifications()
{
    if (g_submitting || g_draining)
        return;
    g_draining = TRUE;
    while (!g_pending.empty())
    {
        Notification n = g_pending.front();
        g_pending.pop_front();
        ProcessNotification(n);
    }
    g_draining = FALSE;
    if (g_closeRequested)
        PostMessage(GetParent(g_panel), WM_CLOSE, 0, 0);
}

// 주문 요청: 입력 검증 → 사용자 확인 → RequestData. 자동 재전송하지 않습니다.
void RequestOrder(HWND window)
{
    if (g_closing || g_submitting || Common::IsBusy() || g_requestId >= 0 || !g_armed ||
        g_uncertain || !Common::IsReady())
        return;
    b8001InBlock input;
    if (!BuildInput(input))
    {
        SecureZeroMemory(&input, sizeof(input));
        return;
    }
    g_submitting = TRUE;
    UpdateControls();
    std::string summary = SERVICE_CODE;
    summary += "\n종목번호 (expcode)=" + Field(input.expcode, sizeof(input.expcode));
    summary += "\n주문수량 (qty)=" + Field(input.qty, sizeof(input.qty));
    summary += "\n주문가 (price)=" + Field(input.price, sizeof(input.price));
    summary += "\nmemegb=" + Field(input.memegb, sizeof(input.memegb));
    summary += "\nhogagb=" + Field(input.hogagb, sizeof(input.hogagb));
    wchar_t details[512] = {0};
    MultiByteToWideChar(CP_ACP, 0, summary.c_str(), -1, details, _countof(details));
    wchar_t confirmation[768];
    swprintf_s(confirmation,
               _countof(confirmation),
               L"%s\n%s\n입력 내용을 확인하고 주문하시겠습니까?",
               SERVICE_NAME,
               details);
    int answer = MessageBoxW(window,
                             confirmation,
                             L"주문 전송 확인",
                             MB_YESNO | MB_DEFBUTTON2 | MB_ICONWARNING);
    if (answer != IDYES || g_closing || g_closeRequested)
    {
        SecureZeroMemory(&input, sizeof(input));
        g_submitting = FALSE;
        DrainNotifications();
        UpdateControls();
        if (g_closeRequested)
            PostMessage(window, WM_CLOSE, 0, 0);
        return;
    }
    Common::BeginCall();
    if (!Common::CheckSession() || g_closeRequested)
    {
        SecureZeroMemory(&input, sizeof(input));
        g_submitting = FALSE;
        Common::EndCall(window);
        DrainNotifications();
        UpdateControls();
        if (g_closeRequested)
            PostMessage(window, WM_CLOSE, 0, 0);
        return;
    }
    g_armed = FALSE;
    g_received = FALSE;
    g_error = FALSE;
    g_requestSecret.assign(input.passwd, sizeof(input.passwd));
    while (!g_requestSecret.empty() && g_requestSecret[g_requestSecret.size() - 1] == ' ')
        g_requestSecret.erase(g_requestSecret.size() - 1);
    // RequestData는 공개 서비스 요청 API이며 별도 주문 전용 함수를 가정하지 않습니다.
    // 0~255 반환은 요청 접수와 요청 ID이며 최종 주문 결과가 아닙니다. 연속조회하지 않습니다.
    // API_MFCDlg::Requestt3086과 같은 4인자 호출이며 생략한 다음 키는 NULL입니다.
    int result = Common::Api()->RequestData(window, NAME_b8001, &input, sizeof(b8001InBlock));
    SecureZeroMemory(&input, sizeof(input));
    g_requestId = result >= 0 && result <= 255 ? result : -1;
    g_submitting = FALSE;
    if (g_requestId >= 0)
    {
        Common::SetResult(L"주문 요청 접수. 최종 결과는 비동기 응답을 확인하십시오.");
        Log("Request accepted; awaiting asynchronous order response.");
    }
    else
    {
        Common::ShowCallResult(L"주문 요청 API 반환", result);
        Log("Request call returned an error; no automatic retry.");
    }
    Common::EndCall(window);
    DrainNotifications();
    ClearPasswords();
    if (g_requestId < 0)
    {
        if (!g_requestSecret.empty())
            SecureZeroMemory(&g_requestSecret[0], g_requestSecret.size());
        g_requestSecret.clear();
    }
    UpdateControls();
    if (g_closeRequested)
        PostMessage(window, WM_CLOSE, 0, 0);
}
void PositionInputs()
{
    for (int i = 0; i < _countof(INPUT_FIELDS); ++i)
    {
        SetWindowPos(g_labels[i], NULL, 6, 8 + i * 30 - g_scroll, 245, 22, SWP_NOZORDER);
        SetWindowPos(g_inputs[i],
                     NULL,
                     255,
                     6 + i * 30 - g_scroll,
                     240,
                     INPUT_FIELDS[i].choiceCount ? 150 : 24,
                     SWP_NOZORDER);
    }
}
LRESULT CALLBACK InputProc(HWND window, UINT message, WPARAM wParam, LPARAM lParam)
{
    if (message == WM_COMMAND && (HIWORD(wParam) == EN_SETFOCUS || HIWORD(wParam) == CBN_SETFOCUS))
    {
        for (int i = 0; i < _countof(INPUT_FIELDS); ++i)
            if (g_inputs[i] == (HWND)lParam)
            {
                int top = 6 + i * 30;
                if (top < g_scroll || top + 24 > g_scroll + 440)
                {
                    SetScrollPos(window, SB_VERT, top, TRUE);
                    g_scroll = GetScrollPos(window, SB_VERT);
                    PositionInputs();
                }
            }
    }
    if (message == WM_VSCROLL || message == WM_MOUSEWHEEL)
    {
        SCROLLINFO si = {sizeof(si), SIF_ALL};
        GetScrollInfo(window, SB_VERT, &si);
        int pos = si.nPos;
        if (message == WM_MOUSEWHEEL)
            pos -= GET_WHEEL_DELTA_WPARAM(wParam) / WHEEL_DELTA * 90;
        else
            switch (LOWORD(wParam))
            {
            case SB_LINEUP:
                pos -= 30;
                break;
            case SB_LINEDOWN:
                pos += 30;
                break;
            case SB_PAGEUP:
                pos -= 300;
                break;
            case SB_PAGEDOWN:
                pos += 300;
                break;
            case SB_THUMBTRACK:
                pos = si.nTrackPos;
                break;
            }
        si.fMask = SIF_POS;
        si.nPos = pos;
        SetScrollInfo(window, SB_VERT, &si, TRUE);
        g_scroll = GetScrollPos(window, SB_VERT);
        PositionInputs();
        return 0;
    }
    return DefWindowProcW(window, message, wParam, lParam);
}
void DiscardQueuedNotifications(HWND window)
{
    MSG pending;
    while (PeekMessage(&pending,
                       window,
                       Common::API_MESSAGE_BASE + WM_RECEIVE_DATA,
                       Common::API_MESSAGE_BASE + WM_RECEIVE_DATA,
                       PM_REMOVE))
    {
        if ((pending.wParam == MESSAGE_DATA || pending.wParam == SYSTEM_ERROR_DATA) &&
            Common::Api())
        {
            const MSG_PACKET *packet = (const MSG_PACKET *)pending.lParam;
            if (packet && packet->nIsSystemError == 1 && packet->nRqID == g_requestId)
                g_requestId = -1;
            Common::Api()->ReleaseMessageData(pending.lParam);
        }
    }
    while (PeekMessage(&pending,
                       window,
                       Common::API_MESSAGE_BASE + WM_TIMEOUT_DATA,
                       Common::API_MESSAGE_BASE + WM_TIMEOUT_DATA,
                       PM_REMOVE))
    {
    }
}

// 재로그인: 요청이 없는 상태에서 정리한 뒤 같은 화면의 세션을 초기화합니다.
void RetryLogin(HWND window)
{
    if (g_closing || g_submitting || g_draining || Common::IsBusy() || g_requestId >= 0 ||
        Common::IsReady())
        return;
    if (!Common::BeginClose())
        return;
    g_closing = TRUE;
    DiscardQueuedNotifications(window);
    Common::Cleanup();
    g_pending.clear();
    ClearPasswords();
    if (!Common::ResetAfterCleanup(window))
    {
        DestroyWindow(window);
        return;
    }
    g_closing = FALSE;
    Common::BeginCall();
    Common::Initialize();
    Common::EndCall(window);
    Log("Login session reset; server/ID retained and passwords cleared.");
    UpdateControls();
}

// 메인 메시지 처리: API 통지와 사용자 입력을 분리하고 종료 수명을 보호합니다.
LRESULT CALLBACK WindowProc(HWND window, UINT message, WPARAM wParam, LPARAM lParam)
{
    if (message == Common::API_MESSAGE_BASE + WM_RECEIVE_DATA ||
        message == Common::API_MESSAGE_BASE + WM_TIMEOUT_DATA)
    {
        CaptureNotification(message, wParam, lParam);
        DrainNotifications();
        return 0;
    }
    if (!g_closing && Common::HandleMessage(window, message, wParam, lParam))
    {
        if ((message == Common::API_MESSAGE_BASE + WM_DISCONNECT ||
             message == Common::API_MESSAGE_BASE + WM_LOGOUT) &&
            g_requestId >= 0)
        {
            g_uncertain = TRUE;
            g_armed = FALSE;
            Log("Session lost during request; order outcome unknown.");
        }
        UpdateControls();
        return 0;
    }
    switch (message)
    {
    case WM_CREATE:
    {
        Common::CreateLoginControls(window);
        g_retry = Common::AddControl(window,
                                     L"BUTTON",
                                     L"다시 로그인",
                                     WS_TABSTOP,
                                     400,
                                     490,
                                     130,
                                     28,
                                     302);
        WNDCLASSW wc = {0};
        wc.lpfnWndProc = InputProc;
        wc.hInstance = GetModuleHandle(NULL);
        wc.lpszClassName = L"OrdaOrderInputs";
        wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
        RegisterClassW(&wc);
        g_panel = CreateWindowExW(WS_EX_CONTROLPARENT,
                                  wc.lpszClassName,
                                  L"",
                                  WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_CLIPCHILDREN,
                                  580,
                                  20,
                                  530,
                                  440,
                                  window,
                                  NULL,
                                  wc.hInstance,
                                  NULL);
        for (int i = 0; i < _countof(INPUT_FIELDS); ++i)
        {
            const InputField &f = INPUT_FIELDS[i];
            g_labels[i] = Common::AddControl(g_panel, L"STATIC", f.label, 0, 0, 0, 245, 22, 0);
            if (f.choiceCount)
            {
                g_inputs[i] = Common::AddControl(g_panel,
                                                 L"COMBOBOX",
                                                 L"",
                                                 CBS_DROPDOWNLIST | WS_TABSTOP | WS_VSCROLL,
                                                 0,
                                                 0,
                                                 240,
                                                 150,
                                                 0);
                int initial = -1;
                for (int j = 0; j < f.choiceCount; ++j)
                {
                    SendMessageW(g_inputs[i], CB_ADDSTRING, 0, (LPARAM)f.choices[j].label);
                    if (strcmp(f.initial, f.choices[j].code) == 0)
                        initial = j;
                }
                SendMessage(g_inputs[i], CB_SETCURSEL, initial, 0);
            }
            else
            {
                g_inputs[i] = Common::AddControl(g_panel,
                                                 L"EDIT",
                                                 L"",
                                                 ES_AUTOHSCROLL | WS_TABSTOP |
                                                     (f.password ? ES_PASSWORD : 0) |
                                                     (f.readOnly ? ES_READONLY : 0),
                                                 0,
                                                 0,
                                                 240,
                                                 24,
                                                 0);
                SendMessage(g_inputs[i], EM_SETLIMITTEXT, f.width, 0);
                SetWindowTextA(g_inputs[i], f.initial);
            }
        }
        SCROLLINFO si = {sizeof(si),
                         SIF_RANGE | SIF_PAGE | SIF_POS,
                         0,
                         8 + _countof(INPUT_FIELDS) * 30,
                         440,
                         0,
                         0};
        SetScrollInfo(g_panel, SB_VERT, &si, TRUE);
        PositionInputs();
        g_order =
            Common::AddControl(window, L"BUTTON", L"주문 전송", WS_TABSTOP, 580, 480, 140, 30, 300);
        g_prepare = Common::AddControl(window,
                                       L"BUTTON",
                                       L"새 주문 준비",
                                       WS_TABSTOP,
                                       740,
                                       480,
                                       140,
                                       30,
                                       301);
        Common::AddControl(window,
                           L"STATIC",
                           L"주문 응답 및 처리 로그 (비밀번호 제외)",
                           0,
                           580,
                           525,
                           530,
                           22,
                           0);
        g_output = Common::AddControl(window,
                                      L"EDIT",
                                      L"",
                                      ES_READONLY | ES_MULTILINE | ES_AUTOVSCROLL | ES_AUTOHSCROLL |
                                          WS_VSCROLL | WS_HSCROLL,
                                      580,
                                      550,
                                      530,
                                      230,
                                      0);
        SendMessage(g_output, EM_SETLIMITTEXT, 256000, 0);
        UpdateControls();
        return 0;
    }
    case WM_COMMAND:
        if (HIWORD(wParam) == BN_CLICKED)
        {
            if (LOWORD(wParam) == 300)
                RequestOrder(window);
            else if (LOWORD(wParam) == 302)
                RetryLogin(window);
            else if (LOWORD(wParam) == 301 && !g_submitting && !g_draining && !g_closing &&
                     !Common::IsBusy() && g_requestId < 0 && !g_armed)
            {
                // 별도의 준비 확인을 거쳐야 다시 전송할 수 있어 연타·대기 중 클릭의 중복 주문을 막습니다.
                g_submitting = TRUE;
                UpdateControls();
                int answer = MessageBoxW(
                    window,
                    L"이전 주문의 실제 처리 결과를 확인했습니까?\n미확인 주문을 다시 전송하면 중복 주문이 발생할 수 있습니다.",
                    L"새 주문 준비",
                    MB_YESNO | MB_DEFBUTTON2 | MB_ICONWARNING);
                if (answer == IDYES)
                {
                    g_armed = TRUE;
                    g_uncertain = FALSE;
                    Log("User prepared a new order after checking the prior outcome.");
                }
                g_submitting = FALSE;
                DrainNotifications();
                UpdateControls();
                if (g_closeRequested)
                    PostMessage(window, WM_CLOSE, 0, 0);
            }
        }
        return 0;
    case WM_CLOSE:
        if (g_submitting || g_draining)
        {
            g_closeRequested = TRUE;
            return 0;
        }
        if (!Common::BeginClose())
            return 0;
        g_closing = TRUE;
        UpdateControls();
        DiscardQueuedNotifications(window);
        ReleaseActive();
        g_pending.clear();
        ClearPasswords();
        Common::Cleanup();
        DestroyWindow(window);
        return 0;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProcW(window, message, wParam, lParam);
}

// 시작과 종료: 화면 생성 → Common 초기화 → 메시지 루프 → 종료 처리.
int APIENTRY WinMain(HINSTANCE instance, HINSTANCE, LPSTR, int show)
{
    WNDCLASSW wc = {0};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = instance;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
    wc.lpszClassName = L"OrdaOrderSample";
    if (!RegisterClassW(&wc))
        return 1;
    HWND window = CreateWindowW(wc.lpszClassName,
                                SERVICE_NAME,
                                WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
                                CW_USEDEFAULT,
                                CW_USEDEFAULT,
                                1140,
                                840,
                                NULL,
                                NULL,
                                instance,
                                NULL);
    if (!window)
        return 1;
    ShowWindow(window, show);
    UpdateWindow(window);
    Common::BeginCall();
    Common::Initialize();
    Common::EndCall(window);
    UpdateControls();
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
