// 읽는 순서: WinMain 초기화 → WindowProc 로그인 통지 → 요청 함수 → 수신 함수 → 종료 처리.
// Load/Initialize와 Connect/Login은 기존 Common Helper를 사용합니다.
// 로그인 결과는 Common::HandleMessage가 처리하며 요청 API는 아래에서 직접 호출합니다.
// VS2010 v100 / Win32 / MultiByte / C++03. 주문을 자동 전송하거나 재시도하지 않습니다.
#include "../../../../common/OrdaSampleCommon.h"
#include "b8510.h"
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <string>
#include <vector>
#include <deque>
namespace Common = OrdaSampleCommon;
const char SERVICE_CODE[] = NAME_b8510;
const wchar_t SERVICE_NAME[] = L"옵션취소주문";
typedef b8510InBlock OrderInput;
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
    {L"주문시장코드 (ordmarketcode)", 5, 2, "40", FALSE, FALSE, NULL, 0},
    {L"계좌번호 (accno)", 7, 20, "", FALSE, FALSE, NULL, 0},
    {L"입력비밀번호 (passwd)", 27, 8, "", TRUE, FALSE, NULL, 0},
    {L"옵션종목번호 (expcode)", 35, 32, "", FALSE, FALSE, NULL, 0},
    {L"선물옵션주문유형코드 (fnoordptncode)", 67, 2, "00", FALSE, FALSE, NULL, 0},
    {L"원주문번호 (orgordno)", 69, 10, "", FALSE, FALSE, NULL, 0},
    {L"취소수량 (qty)", 79, 16, "", FALSE, FALSE, NULL, 0},
    {L"통신매체코드 (tongsingb)", 95, 2, "11", FALSE, FALSE, NULL, 0},
    {L"전략코드 (stragb)", 97, 6, "      ", FALSE, FALSE, NULL, 0},
    {L"그룹ID (groupid)", 103, 20, "                    ", FALSE, FALSE, NULL, 0},
    {L"주문회차 (ordernum)", 123, 10, "0000000000", FALSE, FALSE, NULL, 0},
    {L"포트폴리오번호 (portnum)", 133, 10, "0000000000", FALSE, FALSE, NULL, 0},
    {L"바스켓번호 (basketnum)", 143, 10, "0000000000", FALSE, FALSE, NULL, 0},
    {L"트렌치번호 (tranchnum)", 153, 10, "0000000000", FALSE, FALSE, NULL, 0},
    {L"아이템번호 (itemnum)", 163, 10, "0000000000", FALSE, FALSE, NULL, 0},
    {L"관리사번 (mgempno)", 173, 9, "         ", FALSE, FALSE, NULL, 0},
    {L"펀드ID (fundid)", 182, 12, "            ", FALSE, FALSE, NULL, 0},
    {L"dummy펀드원주문번호 (dummyfundorgordno)", 194, 3, "000", FALSE, FALSE, NULL, 0},
    {L"펀드원주문번호 (fundorgordno)", 197, 7, "0000000", FALSE, FALSE, NULL, 0},
    {L"dummy펀드주문번호 (dummyfundordno)", 204, 3, "000", FALSE, FALSE, NULL, 0},
    {L"펀드주문번호 (fundordno)", 207, 7, "0000000", FALSE, FALSE, NULL, 0}
};
HWND g_inputs[_countof(INPUT_FIELDS)], g_labels[_countof(INPUT_FIELDS)];
HWND g_panel, g_order, g_prepare, g_retry, g_logout, g_output;
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
    EnableWindow(g_logout, idle && Common::IsReady());
    for (int i = 0; i < _countof(INPUT_FIELDS); ++i)
        EnableWindow(g_inputs[i], idle);
}

// 입력 전문: XML 기본값과 화면 입력을 실제 b8510 구조체 멤버에 설정합니다.
BOOL BuildInput(OrderInput &input)
{
    memset(&input, 0x20, sizeof(input));
    char *fields[] = {
        input.reccnt,
        input.ordmarketcode,
        input.accno,
        input.passwd,
        input.expcode,
        input.fnoordptncode,
        input.orgordno,
        input.qty,
        input.tongsingb,
        input.stragb,
        input.groupid,
        input.ordernum,
        input.portnum,
        input.basketnum,
        input.tranchnum,
        input.itemnum,
        input.mgempno,
        input.fundid,
        input.dummyfundorgordno,
        input.fundorgordno,
        input.dummyfundordno,
        input.fundordno
    };
    for (int i = 0; i < _countof(INPUT_FIELDS); ++i)
    {
        const InputField &field = INPUT_FIELDS[i];
        char value[256] = {0};
        int length;
        if (field.choiceCount)
        {
            int selected = (int)SendMessage(g_inputs[i], CB_GETCURSEL, 0, 0);
            if (selected < 0 || selected >= field.choiceCount)
            {
                Common::SetResult(L"호가유형을 선택하십시오.");
                return FALSE;
            }
            const char *code = field.choices[selected].code;
            length = (int)strlen(code);
            memcpy(value, code, length);
        }
        else
        {
            if (GetWindowTextLengthA(g_inputs[i]) >= (int)sizeof(value))
            {
                Common::SetResult(L"입력값이 너무 깁니다.");
                return FALSE;
            }
            length = GetWindowTextA(g_inputs[i], value, sizeof(value));
        }
        if (fields[i] == input.orgordno)
        {
            // 화면에는 XML의 원주문번호 10바이트를 한 항목으로 입력받습니다.
            int first = 0, end = length;
            while (first < end && strchr(" \t\r\n\v\f", value[first]))
                ++first;
            while (end > first && strchr(" \t\r\n\v\f", value[end - 1]))
                --end;
            length = end - first;
            char originalOrderNumber[sizeof(input.dummyorgordno) + sizeof(input.orgordno)];
            if (length == 0 || length > (int)sizeof(originalOrderNumber))
            {
                SecureZeroMemory(value, sizeof(value));
                Common::SetResult(L"원주문번호는 공백을 제외한 1~10자리 숫자로 입력하십시오.");
                return FALSE;
            }
            for (int j = first; j < end; ++j)
                if (value[j] < '0' || value[j] > '9')
                {
                    SecureZeroMemory(value, sizeof(value));
                    Common::SetResult(L"원주문번호는 음수나 기타 문자 없이 숫자로 입력하십시오.");
                    return FALSE;
                }
            // 전체 10자리를 Zero Padding한 뒤 앞 3자리와 뒤 7자리로 나눕니다.
            memset(originalOrderNumber, '0', sizeof(originalOrderNumber));
            memcpy(originalOrderNumber + sizeof(originalOrderNumber) - length, value + first, length);
            memcpy(input.dummyorgordno, originalOrderNumber, sizeof(input.dummyorgordno));
            memcpy(input.orgordno, originalOrderNumber + sizeof(input.dummyorgordno), sizeof(input.orgordno));
        }
        else if (fields[i] == input.qty)
        {
            int first = 0, end = length;
            while (first < end && strchr(" \t\r\n\v\f", value[first]))
                ++first;
            while (end > first && strchr(" \t\r\n\v\f", value[end - 1]))
                --end;
            length = end - first;
            if (length == 0 || length > (int)sizeof(input.qty))
            {
                SecureZeroMemory(value, sizeof(value));
                Common::SetResult(L"취소수량은 1~16자리 숫자로 입력하십시오.");
                return FALSE;
            }
            for (int j = first; j < end; ++j)
                if (value[j] < '0' || value[j] > '9')
                {
                    SecureZeroMemory(value, sizeof(value));
                    Common::SetResult(L"취소수량은 음수나 소수점 없이 숫자로 입력하십시오.");
                    return FALSE;
                }
            // 고정 길이 수량 필드에 종료 NUL을 넣지 않습니다.
            memset(input.qty, '0', sizeof(input.qty));
            memcpy(input.qty + sizeof(input.qty) - length, value + first, length);
        }
        else
        {
            if (length > field.width)
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
BOOL Receiveb8510(const RP_DATA_INFO *response, std::string &text)
{
    const int responseSize = sizeof(b8510OutBlock) + sizeof(b8510OutBlock1);
    if (!response || !response->lpData || response->nDataLength != responseSize ||
        Field(response->Header.szTrCode, 6) != SERVICE_CODE ||
        !Field(response->szBlockName, 17).empty())
        return FALSE;
    // 길이를 검사한 뒤 두 고정 블록을 실제 서비스 구조체로 읽습니다.
    const b8510OutBlock *output = (const b8510OutBlock *)response->lpData;
    const b8510OutBlock1 *order = (const b8510OutBlock1 *)(output + 1);
    text = "b8510OutBlock\r\n";
    text += "reccnt=";
    text += Field(output->reccnt, sizeof(output->reccnt));
    text += "\r\n";
    text += "ordmarketcode=";
    text += Field(output->ordmarketcode, sizeof(output->ordmarketcode));
    text += "\r\n";
    text += "accno=";
    text += Field(output->accno, sizeof(output->accno));
    text += "\r\n";
    text += "passwd=";
    text += "[redacted]";
    text += "\r\n";
    text += "expcode=";
    text += Field(output->expcode, sizeof(output->expcode));
    text += "\r\n";
    text += "fnoordptncode=";
    text += Field(output->fnoordptncode, sizeof(output->fnoordptncode));
    text += "\r\n";
    // 분리된 3+7바이트를 하나의 10자리 번호로 표시합니다.
    std::string originalOrderNumber(output->dummyorgordno, sizeof(output->dummyorgordno));
    originalOrderNumber.append(output->orgordno, sizeof(output->orgordno));
    text += "originalOrderNumber=" + Field(originalOrderNumber.data(), (int)originalOrderNumber.size()) + "\r\n";
    text += "qty=";
    text += Field(output->qty, sizeof(output->qty));
    text += "\r\n";
    text += "tongsingb=";
    text += Field(output->tongsingb, sizeof(output->tongsingb));
    text += "\r\n";
    text += "stragb=";
    text += Field(output->stragb, sizeof(output->stragb));
    text += "\r\n";
    text += "groupid=";
    text += Field(output->groupid, sizeof(output->groupid));
    text += "\r\n";
    text += "ordernum=";
    text += Field(output->ordernum, sizeof(output->ordernum));
    text += "\r\n";
    text += "portnum=";
    text += Field(output->portnum, sizeof(output->portnum));
    text += "\r\n";
    text += "basketnum=";
    text += Field(output->basketnum, sizeof(output->basketnum));
    text += "\r\n";
    text += "tranchnum=";
    text += Field(output->tranchnum, sizeof(output->tranchnum));
    text += "\r\n";
    text += "itemnum=";
    text += Field(output->itemnum, sizeof(output->itemnum));
    text += "\r\n";
    text += "mgempno=";
    text += Field(output->mgempno, sizeof(output->mgempno));
    text += "\r\n";
    text += "fundid=";
    text += Field(output->fundid, sizeof(output->fundid));
    text += "\r\n";
    text += "dummyfundorgordno=";
    text += Field(output->dummyfundorgordno, sizeof(output->dummyfundorgordno));
    text += "\r\n";
    text += "fundorgordno=";
    text += Field(output->fundorgordno, sizeof(output->fundorgordno));
    text += "\r\n";
    text += "dummyfundordno=";
    text += Field(output->dummyfundordno, sizeof(output->dummyfundordno));
    text += "\r\n";
    text += "fundordno=";
    text += Field(output->fundordno, sizeof(output->fundordno));
    text += "\r\n";
    text += "b8510OutBlock1\r\n";
    text += "reccnt=";
    text += Field(order->reccnt, sizeof(order->reccnt));
    text += "\r\n";
    // 분리된 3+7바이트를 하나의 10자리 번호로 표시합니다.
    std::string orderNumber(order->dummyordno, sizeof(order->dummyordno));
    orderNumber.append(order->ordno, sizeof(order->ordno));
    text += "orderNumber=" + Field(orderNumber.data(), (int)orderNumber.size()) + "\r\n";
    text += "brnnm=";
    text += Field(order->brnnm, sizeof(order->brnnm));
    text += "\r\n";
    text += "accno=";
    text += Field(order->accno, sizeof(order->accno));
    text += "\r\n";
    text += "isunm=";
    text += Field(order->isunm, sizeof(order->isunm));
    text += "\r\n";
    text += "ordableamt=";
    text += Field(order->ordableamt, sizeof(order->ordableamt));
    text += "\r\n";
    text += "mnyordableamt=";
    text += Field(order->mnyordableamt, sizeof(order->mnyordableamt));
    text += "\r\n";
    text += "ordmgn=";
    text += Field(order->ordmgn, sizeof(order->ordmgn));
    text += "\r\n";
    text += "mnyordmgn=";
    text += Field(order->mnyordmgn, sizeof(order->mnyordmgn));
    text += "\r\n";
    text += "ordableqty=";
    text += Field(order->ordableqty, sizeof(order->ordableqty));
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
        if (n.malformed || !Receiveb8510(&n.response, text))
        {
            g_error = TRUE;
            g_uncertain = TRUE;
            Log("Invalid response service/block/length; outcome unknown.");
            Common::SetResult(
                L"주문 응답 형식이 맞지 않습니다. 실제 주문 결과를 별도로 확인하십시오.");
        }
        else
        {
            // SDK RP_HEADER의 '1'~'9'는 오류이며 수신 자체를 성공으로 판정하지 않습니다.
            if (n.response.Header.szRetFlag[0] >= '1' && n.response.Header.szRetFlag[0] <= '9')
                g_error = TRUE;
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
        n.malformed = p->nDataLength != sizeof(b8510OutBlock) + sizeof(b8510OutBlock1) || !p->lpData;
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
    b8510InBlock input;
    if (!BuildInput(input))
    {
        SecureZeroMemory(&input, sizeof(input));
        return;
    }
    g_submitting = TRUE;
    UpdateControls();
    std::string summary = SERVICE_CODE;
    summary += "\n계좌번호 (accno)=" + Field(input.accno, sizeof(input.accno));
    summary += "\n선물옵션종목번호 (expcode)=" + Field(input.expcode, sizeof(input.expcode));
    std::string originalOrderNumber(input.dummyorgordno, sizeof(input.dummyorgordno));
    originalOrderNumber.append(input.orgordno, sizeof(input.orgordno));
    summary += "\n원주문번호=" + originalOrderNumber;
    summary += "\n취소수량 (qty)=" + Field(input.qty, sizeof(input.qty));
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
    int result = Common::Api()->RequestData(window, NAME_b8510, &input, sizeof(b8510InBlock));
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

// 로그아웃·재로그인 준비: 요청이 없을 때 세션을 정리하고 로그인 화면을 초기화합니다.
void ResetLoginSession(HWND window)
{
    if (g_closing || g_submitting || g_draining || Common::IsBusy() || g_requestId >= 0)
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
        g_logout = Common::AddControl(window, L"BUTTON", L"로그아웃", WS_TABSTOP,
                                      400, 530, 130, 28, 303);
        WNDCLASSW wc = {0};
        wc.lpfnWndProc = InputProc;
        wc.hInstance = GetModuleHandle(NULL);
        wc.lpszClassName = L"OrdaOptionsCancelInputs";
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
            else if ((LOWORD(wParam) == 302 && !Common::IsReady()) ||
                     (LOWORD(wParam) == 303 && Common::IsReady()))
                ResetLoginSession(window);
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
    wc.lpszClassName = L"OrdaOptionsCancelSample";
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
