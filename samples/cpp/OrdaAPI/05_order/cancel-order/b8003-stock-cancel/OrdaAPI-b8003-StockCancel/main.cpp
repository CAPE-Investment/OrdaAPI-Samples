// 읽는 순서: WinMain 초기화 → WindowProc 로그인 통지 → 요청 함수 → 수신 함수 → 종료 처리.
// Load/Initialize와 Connect/Login은 기존 Common Helper를 사용합니다.
// 로그인 결과는 Common::HandleMessage가 처리하며 요청 API는 아래에서 직접 호출합니다.
// VS2010 v100 / Win32 / MultiByte / C++03. 주문을 자동 전송하거나 재시도하지 않습니다.
#include "../../../../common/OrdaSampleCommon.h"
#include "b8003.h"
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <string>
#include <vector>
#include <deque>
namespace Common = OrdaSampleCommon;
const char SERVICE_CODE[] = NAME_b8003;
const wchar_t SERVICE_NAME[] = L"현금취소주문";
typedef b8003InBlock OrderInput;
typedef char Check_b8003InBlock_reccnt
    [(offsetof(b8003InBlock, reccnt) == 0 && sizeof(((b8003InBlock *)0)->reccnt) == 5) ? 1 : -1];
typedef char Check_b8003InBlock_orgordno[(offsetof(b8003InBlock, orgordno) == 5 &&
                                          sizeof(((b8003InBlock *)0)->orgordno) == 10)
                                             ? 1
                                             : -1];
typedef char Check_b8003InBlock_accno
    [(offsetof(b8003InBlock, accno) == 15 && sizeof(((b8003InBlock *)0)->accno) == 20) ? 1 : -1];
typedef char Check_b8003InBlock_passwd
    [(offsetof(b8003InBlock, passwd) == 35 && sizeof(((b8003InBlock *)0)->passwd) == 8) ? 1 : -1];
typedef char Check_b8003InBlock_expcode[(offsetof(b8003InBlock, expcode) == 43 &&
                                         sizeof(((b8003InBlock *)0)->expcode) == 12)
                                            ? 1
                                            : -1];
typedef char Check_b8003InBlock_qty
    [(offsetof(b8003InBlock, qty) == 55 && sizeof(((b8003InBlock *)0)->qty) == 16) ? 1 : -1];
typedef char Check_b8003InBlock_tongsingb[(offsetof(b8003InBlock, tongsingb) == 71 &&
                                           sizeof(((b8003InBlock *)0)->tongsingb) == 2)
                                              ? 1
                                              : -1];
typedef char Check_b8003InBlock_groupid[(offsetof(b8003InBlock, groupid) == 73 &&
                                         sizeof(((b8003InBlock *)0)->groupid) == 20)
                                            ? 1
                                            : -1];
typedef char Check_b8003InBlock_stragb
    [(offsetof(b8003InBlock, stragb) == 93 && sizeof(((b8003InBlock *)0)->stragb) == 6) ? 1 : -1];
typedef char Check_b8003InBlock_ordernum[(offsetof(b8003InBlock, ordernum) == 99 &&
                                          sizeof(((b8003InBlock *)0)->ordernum) == 10)
                                             ? 1
                                             : -1];
typedef char Check_b8003InBlock_portnum[(offsetof(b8003InBlock, portnum) == 109 &&
                                         sizeof(((b8003InBlock *)0)->portnum) == 10)
                                            ? 1
                                            : -1];
typedef char Check_b8003InBlock_basketnum[(offsetof(b8003InBlock, basketnum) == 119 &&
                                           sizeof(((b8003InBlock *)0)->basketnum) == 10)
                                              ? 1
                                              : -1];
typedef char Check_b8003InBlock_tranchnum[(offsetof(b8003InBlock, tranchnum) == 129 &&
                                           sizeof(((b8003InBlock *)0)->tranchnum) == 10)
                                              ? 1
                                              : -1];
typedef char Check_b8003InBlock_itemnum[(offsetof(b8003InBlock, itemnum) == 139 &&
                                         sizeof(((b8003InBlock *)0)->itemnum) == 10)
                                            ? 1
                                            : -1];
typedef char Check_b8003InBlock_TrgtOrdMktCode[(offsetof(b8003InBlock, TrgtOrdMktCode) == 149 &&
                                                sizeof(((b8003InBlock *)0)->TrgtOrdMktCode) == 2)
                                                   ? 1
                                                   : -1];
typedef char Check_b8003InBlock_SorChldOrdYn[(offsetof(b8003InBlock, SorChldOrdYn) == 151 &&
                                              sizeof(((b8003InBlock *)0)->SorChldOrdYn) == 1)
                                                 ? 1
                                                 : -1];
typedef char Check_b8003InBlock_SorTrxMthd[(offsetof(b8003InBlock, SorTrxMthd) == 152 &&
                                            sizeof(((b8003InBlock *)0)->SorTrxMthd) == 40)
                                               ? 1
                                               : -1];
typedef char Check_b8003InBlock_PbArea
    [(offsetof(b8003InBlock, PbArea) == 192 && sizeof(((b8003InBlock *)0)->PbArea) == 50) ? 1 : -1];
typedef char
    Check_b8003InBlock_size[(sizeof(b8003InBlock) == 242 && __alignof(b8003InBlock) == 1) ? 1 : -1];
typedef char Check_b8003OutBlock_reccnt
    [(offsetof(b8003OutBlock, reccnt) == 0 && sizeof(((b8003OutBlock *)0)->reccnt) == 5) ? 1 : -1];
typedef char Check_b8003OutBlock_orgordno[(offsetof(b8003OutBlock, orgordno) == 5 &&
                                           sizeof(((b8003OutBlock *)0)->orgordno) == 10)
                                              ? 1
                                              : -1];
typedef char Check_b8003OutBlock_accno
    [(offsetof(b8003OutBlock, accno) == 15 && sizeof(((b8003OutBlock *)0)->accno) == 20) ? 1 : -1];
typedef char Check_b8003OutBlock_passwd
    [(offsetof(b8003OutBlock, passwd) == 35 && sizeof(((b8003OutBlock *)0)->passwd) == 8) ? 1 : -1];
typedef char Check_b8003OutBlock_expcode[(offsetof(b8003OutBlock, expcode) == 43 &&
                                          sizeof(((b8003OutBlock *)0)->expcode) == 12)
                                             ? 1
                                             : -1];
typedef char Check_b8003OutBlock_qty
    [(offsetof(b8003OutBlock, qty) == 55 && sizeof(((b8003OutBlock *)0)->qty) == 16) ? 1 : -1];
typedef char Check_b8003OutBlock_tongsingb[(offsetof(b8003OutBlock, tongsingb) == 71 &&
                                            sizeof(((b8003OutBlock *)0)->tongsingb) == 2)
                                               ? 1
                                               : -1];
typedef char Check_b8003OutBlock_groupid[(offsetof(b8003OutBlock, groupid) == 73 &&
                                          sizeof(((b8003OutBlock *)0)->groupid) == 20)
                                             ? 1
                                             : -1];
typedef char Check_b8003OutBlock_stragb
    [(offsetof(b8003OutBlock, stragb) == 93 && sizeof(((b8003OutBlock *)0)->stragb) == 6) ? 1 : -1];
typedef char Check_b8003OutBlock_ordernum[(offsetof(b8003OutBlock, ordernum) == 99 &&
                                           sizeof(((b8003OutBlock *)0)->ordernum) == 10)
                                              ? 1
                                              : -1];
typedef char Check_b8003OutBlock_portnum[(offsetof(b8003OutBlock, portnum) == 109 &&
                                          sizeof(((b8003OutBlock *)0)->portnum) == 10)
                                             ? 1
                                             : -1];
typedef char Check_b8003OutBlock_basketnum[(offsetof(b8003OutBlock, basketnum) == 119 &&
                                            sizeof(((b8003OutBlock *)0)->basketnum) == 10)
                                               ? 1
                                               : -1];
typedef char Check_b8003OutBlock_tranchnum[(offsetof(b8003OutBlock, tranchnum) == 129 &&
                                            sizeof(((b8003OutBlock *)0)->tranchnum) == 10)
                                               ? 1
                                               : -1];
typedef char Check_b8003OutBlock_itemnum[(offsetof(b8003OutBlock, itemnum) == 139 &&
                                          sizeof(((b8003OutBlock *)0)->itemnum) == 10)
                                             ? 1
                                             : -1];
typedef char Check_b8003OutBlock_TrgtOrdMktCode[(offsetof(b8003OutBlock, TrgtOrdMktCode) == 149 &&
                                                 sizeof(((b8003OutBlock *)0)->TrgtOrdMktCode) == 2)
                                                    ? 1
                                                    : -1];
typedef char Check_b8003OutBlock_SorChldOrdYn[(offsetof(b8003OutBlock, SorChldOrdYn) == 151 &&
                                               sizeof(((b8003OutBlock *)0)->SorChldOrdYn) == 1)
                                                  ? 1
                                                  : -1];
typedef char Check_b8003OutBlock_SorTrxMthd[(offsetof(b8003OutBlock, SorTrxMthd) == 152 &&
                                             sizeof(((b8003OutBlock *)0)->SorTrxMthd) == 40)
                                                ? 1
                                                : -1];
typedef char Check_b8003OutBlock_PbArea[(offsetof(b8003OutBlock, PbArea) == 192 &&
                                         sizeof(((b8003OutBlock *)0)->PbArea) == 50)
                                            ? 1
                                            : -1];
typedef char
    Check_b8003OutBlock_size[(sizeof(b8003OutBlock) == 242 && __alignof(b8003OutBlock) == 1) ? 1
                                                                                             : -1];
typedef char Check_b8003OutBlock1_reccnt[(offsetof(b8003OutBlock1, reccnt) == 0 &&
                                          sizeof(((b8003OutBlock1 *)0)->reccnt) == 5)
                                             ? 1
                                             : -1];
typedef char Check_b8003OutBlock1_OrdMktCode0[(offsetof(b8003OutBlock1, OrdMktCode0) == 5 &&
                                               sizeof(((b8003OutBlock1 *)0)->OrdMktCode0) == 2)
                                                  ? 1
                                                  : -1];
typedef char Check_b8003OutBlock1_OrdNo0[(offsetof(b8003OutBlock1, OrdNo0) == 7 &&
                                          sizeof(((b8003OutBlock1 *)0)->OrdNo0) == 10)
                                             ? 1
                                             : -1];
typedef char Check_b8003OutBlock1_OrdMktCode1[(offsetof(b8003OutBlock1, OrdMktCode1) == 17 &&
                                               sizeof(((b8003OutBlock1 *)0)->OrdMktCode1) == 2)
                                                  ? 1
                                                  : -1];
typedef char Check_b8003OutBlock1_OrdNo1[(offsetof(b8003OutBlock1, OrdNo1) == 19 &&
                                          sizeof(((b8003OutBlock1 *)0)->OrdNo1) == 10)
                                             ? 1
                                             : -1];
typedef char Check_b8003OutBlock1_OrdMktCode2[(offsetof(b8003OutBlock1, OrdMktCode2) == 29 &&
                                               sizeof(((b8003OutBlock1 *)0)->OrdMktCode2) == 2)
                                                  ? 1
                                                  : -1];
typedef char Check_b8003OutBlock1_OrdNo2[(offsetof(b8003OutBlock1, OrdNo2) == 31 &&
                                          sizeof(((b8003OutBlock1 *)0)->OrdNo2) == 10)
                                             ? 1
                                             : -1];
typedef char Check_b8003OutBlock1_OrdMktCode3[(offsetof(b8003OutBlock1, OrdMktCode3) == 41 &&
                                               sizeof(((b8003OutBlock1 *)0)->OrdMktCode3) == 2)
                                                  ? 1
                                                  : -1];
typedef char Check_b8003OutBlock1_OrdNo3[(offsetof(b8003OutBlock1, OrdNo3) == 43 &&
                                          sizeof(((b8003OutBlock1 *)0)->OrdNo3) == 10)
                                             ? 1
                                             : -1];
typedef char Check_b8003OutBlock1_OrdMktCode4[(offsetof(b8003OutBlock1, OrdMktCode4) == 53 &&
                                               sizeof(((b8003OutBlock1 *)0)->OrdMktCode4) == 2)
                                                  ? 1
                                                  : -1];
typedef char Check_b8003OutBlock1_OrdNo4[(offsetof(b8003OutBlock1, OrdNo4) == 55 &&
                                          sizeof(((b8003OutBlock1 *)0)->OrdNo4) == 10)
                                             ? 1
                                             : -1];
typedef char Check_b8003OutBlock1_mordno[(offsetof(b8003OutBlock1, mordno) == 65 &&
                                          sizeof(((b8003OutBlock1 *)0)->mordno) == 10)
                                             ? 1
                                             : -1];
typedef char Check_b8003OutBlock1_ordtime[(offsetof(b8003OutBlock1, ordtime) == 75 &&
                                           sizeof(((b8003OutBlock1 *)0)->ordtime) == 9)
                                              ? 1
                                              : -1];
typedef char Check_b8003OutBlock1_ordcode[(offsetof(b8003OutBlock1, ordcode) == 84 &&
                                           sizeof(((b8003OutBlock1 *)0)->ordcode) == 2)
                                              ? 1
                                              : -1];
typedef char Check_b8003OutBlock1_expcode[(offsetof(b8003OutBlock1, expcode) == 86 &&
                                           sizeof(((b8003OutBlock1 *)0)->expcode) == 9)
                                              ? 1
                                              : -1];
typedef char Check_b8003OutBlock1_pgmtype[(offsetof(b8003OutBlock1, pgmtype) == 95 &&
                                           sizeof(((b8003OutBlock1 *)0)->pgmtype) == 2)
                                              ? 1
                                              : -1];
typedef char Check_b8003OutBlock1_gongtype[(offsetof(b8003OutBlock1, gongtype) == 97 &&
                                            sizeof(((b8003OutBlock1 *)0)->gongtype) == 1)
                                               ? 1
                                               : -1];
typedef char Check_b8003OutBlock1_gonghoga[(offsetof(b8003OutBlock1, gonghoga) == 98 &&
                                            sizeof(((b8003OutBlock1 *)0)->gonghoga) == 1)
                                               ? 1
                                               : -1];
typedef char Check_b8003OutBlock1_sinmemecode[(offsetof(b8003OutBlock1, sinmemecode) == 99 &&
                                               sizeof(((b8003OutBlock1 *)0)->sinmemecode) == 3)
                                                  ? 1
                                                  : -1];
typedef char Check_b8003OutBlock1_loandt[(offsetof(b8003OutBlock1, loandt) == 102 &&
                                          sizeof(((b8003OutBlock1 *)0)->loandt) == 8)
                                             ? 1
                                             : -1];
typedef char Check_b8003OutBlock1_oppbuygb[(offsetof(b8003OutBlock1, oppbuygb) == 110 &&
                                            sizeof(((b8003OutBlock1 *)0)->oppbuygb) == 1)
                                               ? 1
                                               : -1];
typedef char Check_b8003OutBlock1_fowsupgb[(offsetof(b8003OutBlock1, fowsupgb) == 111 &&
                                            sizeof(((b8003OutBlock1 *)0)->fowsupgb) == 1)
                                               ? 1
                                               : -1];
typedef char Check_b8003OutBlock1_operno[(offsetof(b8003OutBlock1, operno) == 112 &&
                                          sizeof(((b8003OutBlock1 *)0)->operno) == 9)
                                             ? 1
                                             : -1];
typedef char Check_b8003OutBlock1_memegb[(offsetof(b8003OutBlock1, memegb) == 121 &&
                                          sizeof(((b8003OutBlock1 *)0)->memegb) == 1)
                                             ? 1
                                             : -1];
typedef char Check_b8003OutBlock1_fillermemegb[(offsetof(b8003OutBlock1, fillermemegb) == 122 &&
                                                sizeof(((b8003OutBlock1 *)0)->fillermemegb) == 10)
                                                   ? 1
                                                   : -1];
typedef char Check_b8003OutBlock1_oppbuyseqno[(offsetof(b8003OutBlock1, oppbuyseqno) == 132 &&
                                               sizeof(((b8003OutBlock1 *)0)->oppbuyseqno) == 10)
                                                  ? 1
                                                  : -1];
typedef char Check_b8003OutBlock1_preeordno[(offsetof(b8003OutBlock1, preeordno) == 142 &&
                                             sizeof(((b8003OutBlock1 *)0)->preeordno) == 10)
                                                ? 1
                                                : -1];
typedef char Check_b8003OutBlock1_accno[(offsetof(b8003OutBlock1, accno) == 152 &&
                                         sizeof(((b8003OutBlock1 *)0)->accno) == 40)
                                            ? 1
                                            : -1];
typedef char Check_b8003OutBlock1_hname[(offsetof(b8003OutBlock1, hname) == 192 &&
                                         sizeof(((b8003OutBlock1 *)0)->hname) == 40)
                                            ? 1
                                            : -1];
typedef char Check_b8003OutBlock1_size
    [(sizeof(b8003OutBlock1) == 232 && __alignof(b8003OutBlock1) == 1) ? 1 : -1];
struct Choice
{
    const wchar_t *label;
    const char *code;
};

struct InputField
{
    const wchar_t *label;
    int offset, width;
    const char *initial;
    BOOL password, readOnly;
    const Choice *choices;
    int choiceCount;
};
const InputField INPUT_FIELDS[] = {
    {L"레코드갯수 (reccnt)", 0, 5, "00001", FALSE, FALSE, NULL, 0},
    {L"원주문번호 (orgordno)", 5, 10, "", FALSE, FALSE, NULL, 0},
    {L"계좌번호 (accno)", 15, 20, "", FALSE, FALSE, NULL, 0},
    {L"계좌비밀번호 (passwd)", 35, 8, "", TRUE, FALSE, NULL, 0},
    {L"종목번호 (expcode)", 43, 12, "", FALSE, FALSE, NULL, 0},
    {L"주문수량 (qty)", 55, 16, "", FALSE, FALSE, NULL, 0},
    {L"통신매체코드 (tongsingb)", 71, 2, "11", FALSE, FALSE, NULL, 0},
    {L"그룹ID (groupid)", 73, 20, "                    ", FALSE, FALSE, NULL, 0},
    {L"전략코드 (stragb)", 93, 6, "      ", FALSE, FALSE, NULL, 0},
    {L"주문회차 (ordernum)", 99, 10, "0000000000", FALSE, FALSE, NULL, 0},
    {L"포트폴리오번호 (portnum)", 109, 10, "0000000000", FALSE, FALSE, NULL, 0},
    {L"바스켓번호 (basketnum)", 119, 10, "0000000000", FALSE, FALSE, NULL, 0},
    {L"트렌치번호 (tranchnum)", 129, 10, "0000000000", FALSE, FALSE, NULL, 0},
    {L"아이템번호 (itemnum)", 139, 10, "0000000000", FALSE, FALSE, NULL, 0},
    {L"목표주문시장코드 (TrgtOrdMktCode)", 149, 2, "  ", FALSE, FALSE, NULL, 0},
    {L"SOR자주문여부 (SorChldOrdYn)", 151, 1, " ", FALSE, FALSE, NULL, 0},
    {L"SOR처리방법 (SorTrxMthd)",
     152,
     40,
     "                                        ",
     FALSE,
     FALSE,
     NULL,
     0},
    {L"PowerBASE내부영역 (PbArea)",
     192,
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
    memset(&input, 0x20, sizeof(b8003InBlock));
    // 실제 구조체 멤버에 입력을 대응시키며 런타임 XML 파싱이나 배치 변경은 하지 않습니다.
    char *fields[] = {input.reccnt,
                      input.orgordno,
                      input.accno,
                      input.passwd,
                      input.expcode,
                      input.qty,
                      input.tongsingb,
                      input.groupid,
                      input.stragb,
                      input.ordernum,
                      input.portnum,
                      input.basketnum,
                      input.tranchnum,
                      input.itemnum,
                      input.TrgtOrdMktCode,
                      input.SorChldOrdYn,
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
        if (fields[i] == input.orgordno)
        {
            // 원주문번호는 앞뒤 ASCII 공백을 제거한 숫자 문자열로 검증합니다.
            int first = 0, end = length;
            while (first < end && strchr(" \t\r\n\v\f", value[first]))
                ++first;
            while (end > first && strchr(" \t\r\n\v\f", value[end - 1]))
                --end;
            length = end - first;
            const size_t fieldSize = sizeof(input.orgordno);
            if (length == 0 || (size_t)length > fieldSize)
            {
                SecureZeroMemory(value, sizeof(value));
                Common::SetResult(L"원주문번호는 공백을 제외한 숫자를 필드 길이 이내로 입력하십시오.");
                return FALSE;
            }
            for (int j = first; j < end; ++j)
                if (value[j] < '0' || value[j] > '9')
                {
                    SecureZeroMemory(value, sizeof(value));
                    Common::SetResult(L"원주문번호는 음수나 기타 문자 없이 숫자로 입력하십시오.");
                    return FALSE;
                }
            // 필드 전체를 왼쪽 Zero Padding하며 종료 NUL은 전송하지 않습니다.
            memset(input.orgordno, '0', fieldSize);
            memcpy(input.orgordno + fieldSize - length, value + first, length);
        }
        else if (fields[i] == input.qty)
        {
            // 주문수량·주문가격만 앞뒤 ASCII 공백을 제거하고 숫자를 오른쪽에 배치합니다.
            int first = 0, end = length;
            while (first < end && strchr(" \t\r\n\v\f", value[first]))
                ++first;
            while (end > first && strchr(" \t\r\n\v\f", value[end - 1]))
                --end;
            length = end - first;
            const int width = (int)(sizeof(input.qty));
            if (length == 0 || length > width)
            {
                SecureZeroMemory(value, sizeof(value));
                Common::SetResult(
                    L"주문수량·주문가격은 공백을 제외한 숫자를 필드 길이 이내로 입력하십시오.");
                return FALSE;
            }
            for (int j = first; j < end; ++j)
                if (value[j] < '0' || value[j] > '9')
                {
                    SecureZeroMemory(value, sizeof(value));
                    Common::SetResult(
                        L"주문수량·주문가격은 음수나 기타 문자 없이 숫자로 입력하십시오.");
                    return FALSE;
                }
            // 고정 길이 필드에 종료 NUL을 넣지 않으며 숫자 0도 허용합니다.
            memset(fields[i], '0', width);
            memcpy(fields[i] + width - length, value + first, length);
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
BOOL Receiveb8003(const RP_DATA_INFO *pdi, std::string &text)
{
    if (!pdi || !pdi->lpData || pdi->nDataLength != 474 ||
        Field(pdi->Header.szTrCode, 6) != SERVICE_CODE || !Field(pdi->szBlockName, 17).empty())
        return FALSE;
    // B형이며 block/attr/occurs가 없으므로 고정 출력 블록이 XML 순서로 이어집니다.
    // API_MFCDlg::Receivet3086처럼 lpData를 제공된 전문 구조체로 접근합니다.
    // 이 서비스는 t3086의 반복 단일 블록과 달리 고정 블록 2개로 정의되어 있습니다.
    // 포인터 접근 전에 길이와 서비스를 검사하며 포인터는 이 함수 안에서만 사용합니다.
    const b8003OutBlock *pOut = (const b8003OutBlock *)pdi->lpData;
    const b8003OutBlock1 *pOut1 = (const b8003OutBlock1 *)(pOut + 1);
    text = "b8003OutBlock\r\n";
    text += "reccnt=";
    text += Field(pOut->reccnt, sizeof(pOut->reccnt));
    text += "\r\n";
    text += "orgordno=";
    text += Field(pOut->orgordno, sizeof(pOut->orgordno));
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
    text += "tongsingb=";
    text += Field(pOut->tongsingb, sizeof(pOut->tongsingb));
    text += "\r\n";
    text += "groupid=";
    text += Field(pOut->groupid, sizeof(pOut->groupid));
    text += "\r\n";
    text += "stragb=";
    text += Field(pOut->stragb, sizeof(pOut->stragb));
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
    text += "TrgtOrdMktCode=";
    text += Field(pOut->TrgtOrdMktCode, sizeof(pOut->TrgtOrdMktCode));
    text += "\r\n";
    text += "SorChldOrdYn=";
    text += Field(pOut->SorChldOrdYn, sizeof(pOut->SorChldOrdYn));
    text += "\r\n";
    text += "SorTrxMthd=";
    text += Field(pOut->SorTrxMthd, sizeof(pOut->SorTrxMthd));
    text += "\r\n";
    text += "PbArea=";
    text += Field(pOut->PbArea, sizeof(pOut->PbArea));
    text += "\r\n";
    text += "b8003OutBlock1\r\n";
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
    text += "mordno=";
    text += Field(pOut1->mordno, sizeof(pOut1->mordno));
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
    text += "pgmtype=";
    text += Field(pOut1->pgmtype, sizeof(pOut1->pgmtype));
    text += "\r\n";
    text += "gongtype=";
    text += Field(pOut1->gongtype, sizeof(pOut1->gongtype));
    text += "\r\n";
    text += "gonghoga=";
    text += Field(pOut1->gonghoga, sizeof(pOut1->gonghoga));
    text += "\r\n";
    text += "sinmemecode=";
    text += Field(pOut1->sinmemecode, sizeof(pOut1->sinmemecode));
    text += "\r\n";
    text += "loandt=";
    text += Field(pOut1->loandt, sizeof(pOut1->loandt));
    text += "\r\n";
    text += "oppbuygb=";
    text += Field(pOut1->oppbuygb, sizeof(pOut1->oppbuygb));
    text += "\r\n";
    text += "fowsupgb=";
    text += Field(pOut1->fowsupgb, sizeof(pOut1->fowsupgb));
    text += "\r\n";
    text += "operno=";
    text += Field(pOut1->operno, sizeof(pOut1->operno));
    text += "\r\n";
    text += "memegb=";
    text += Field(pOut1->memegb, sizeof(pOut1->memegb));
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
        if (n.malformed || !Receiveb8003(&n.response, text))
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
        n.malformed = p->nDataLength != 474 || !p->lpData;
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
    b8003InBlock input;
    if (!BuildInput(input))
    {
        SecureZeroMemory(&input, sizeof(input));
        return;
    }
    g_submitting = TRUE;
    UpdateControls();
    std::string summary = SERVICE_CODE;
    summary += "\n원주문번호 (orgordno)=" + Field(input.orgordno, sizeof(input.orgordno));
    summary += "\n종목번호 (expcode)=" + Field(input.expcode, sizeof(input.expcode));
    summary += "\n주문수량 (qty)=" + Field(input.qty, sizeof(input.qty));
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
    int result = Common::Api()->RequestData(window, NAME_b8003, &input, sizeof(b8003InBlock));
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
