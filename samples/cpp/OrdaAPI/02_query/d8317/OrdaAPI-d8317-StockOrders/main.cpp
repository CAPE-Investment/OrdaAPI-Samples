// 읽는 순서: WinMain 초기화 → 공통 로그인 통지 → 조회 요청 → 수신 → 해제 → 다음 조회 또는 종료.
// VS2010 v100 / Win32 / MultiByte / C++03. 공통 Helper는 변경하지 않습니다.
#include "../../../common/OrdaSampleCommon.h"
#include "d8317.h"
#include <string.h>
#include <stdio.h>
#include <limits.h>
#include <errno.h>
#include <stdlib.h>
#include <float.h>
#include <string>
#include <vector>
#include <deque>

namespace Common = OrdaSampleCommon;
const char SERVICE_CODE[] = NAME_d8317;
const wchar_t SERVICE_TITLE[] = L"주식체결/미체결(d8317)";
const int QUERY_BUTTON = 300, NEXT_BUTTON = 301, DISPLAY_LIMIT = 200;
HWND g_inputs[7], g_queryButton, g_nextButton, g_output;
int g_requestId = -1, g_recordCount = 0;
BOOL g_submitting = FALSE, g_processing = FALSE, g_closing = FALSE;
BOOL g_closeRequested = FALSE, g_receivedData = FALSE, g_queryError = FALSE;
std::string g_rows;
BOOL g_moreFlag = FALSE, g_hasNext = FALSE, g_fixedReceived = FALSE, g_rowsReceived = FALSE;
std::string g_cursor, g_sentCursor, g_sdkKey, g_sentKey, g_summary;
const size_t MAX_RESPONSE_SIZE = INT_MAX;
int g_expectedCount = -1;
// 비밀번호를 저장하지 않고 조건 변경 통지로 연속 상태를 초기화합니다.
void ResetContinuation()
{
    g_hasNext = FALSE;
    g_cursor.clear();
    g_sdkKey.clear();
}


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

// 수량·금액은 64비트 정수로 변환하며 주문번호·코드의 선행 0은 보존합니다.
std::string NumberField(const void *data, int width, BOOL decimal = FALSE)
{
    std::string text = Field(data, width);
    if (text.empty()) return text;
    char *end = NULL;
    char converted[64];
    errno = 0;
    if (decimal)
    {
        double value = strtod(text.c_str(), &end);
        if (errno == ERANGE || end == text.c_str() || *end || !_finite(value))
            return "형식오류(" + text + ")";
        sprintf_s(converted, "%.10g", value);
    }
    else
    {
        __int64 value = _strtoi64(text.c_str(), &end, 10);
        if (errno == ERANGE || end == text.c_str() || *end)
            return "형식오류(" + text + ")";
        sprintf_s(converted, "%I64d", value);
    }
    return converted;
}

void UpdateQueryButton()
{
    BOOL idle = !g_closing && !g_submitting && !g_processing && !Common::IsBusy() &&
                g_requestId < 0;
    EnableWindow(g_queryButton, idle && Common::IsReady());
    EnableWindow(g_nextButton, idle && Common::IsReady() && g_hasNext);
    for (int i = 0; i < 7; ++i) EnableWindow(g_inputs[i], idle);
}

void ClearOutput()
{
    SetWindowTextA(g_output, "");
    g_rows.clear();
    g_recordCount = 0;
    g_summary.clear();
}

// 실제 SDK 멤버에 직접 복사하며 속성 바이트는 공백으로 유지합니다.
BOOL CopyInput(HWND control, char *field, int width, BOOL combo)
{
    char value[128] = {0};
    if (combo)
    {
        int selected = (int)SendMessage(control, CB_GETCURSEL, 0, 0);
        if (selected == CB_ERR) return FALSE;
        value[0] = (char)SendMessage(control, CB_GETITEMDATA, selected, 0);
    }
    else
    {
        if (GetWindowTextLengthA(control) > width) return FALSE;
        GetWindowTextA(control, value, sizeof(value));
    }
    memcpy(field, value, strlen(value));
    return TRUE;
}
BOOL Digits(const char *value, int width)
{
    for (int i = 0; i < width; ++i)
        if (value[i] < '0' || value[i] > '9') return FALSE;
    return TRUE;
}
BOOL BuildInput(d8317InBlock &input, BOOL next = FALSE)
{
    memset(&input, ' ', sizeof(input));
    if (!CopyInput(g_inputs[0], input.accno, sizeof(input.accno), FALSE)) return FALSE;
    if (Field(input.accno, sizeof(input.accno)).empty()) return FALSE;
    if (!CopyInput(g_inputs[1], input.passwd, sizeof(input.passwd), FALSE)) return FALSE;
    if (Field(input.passwd, sizeof(input.passwd)).empty()) return FALSE;
    if (!CopyInput(g_inputs[2], input.expcode, sizeof(input.expcode), FALSE)) return FALSE;
    if (!CopyInput(g_inputs[3], input.chegb, sizeof(input.chegb), TRUE)) return FALSE;
    if (!CopyInput(g_inputs[4], input.medosu, sizeof(input.medosu), TRUE)) return FALSE;
    if (!CopyInput(g_inputs[5], input.sortgb, sizeof(input.sortgb), TRUE)) return FALSE;
    if (!CopyInput(g_inputs[6], input.exchange, sizeof(input.exchange), TRUE)) return FALSE;
    if (next)
    {
        if (!g_hasNext) return FALSE;
        memcpy(input.cts_ordno, g_cursor.data() + 0, sizeof(input.cts_ordno));
    }
    return TRUE;
}

std::string Cursor(const d8317OutBlock &fixed)
{
    return std::string(fixed.cts_ordno, sizeof(fixed.cts_ordno));
}
void ReceiveFixed(const d8317OutBlock &fixed)
{
    std::string cursor = Cursor(fixed);
    if (g_fixedReceived && cursor != g_cursor) g_queryError = TRUE;
    g_fixedReceived = TRUE;
    g_cursor = cursor;
    g_summary.clear();
    g_summary += "반복건수(tqty)=" + NumberField(fixed.tqty, sizeof(fixed.tqty)) + " | ";
    g_summary += "총체결수량(tcheqty)=" + NumberField(fixed.tcheqty, sizeof(fixed.tcheqty)) + " | ";
    g_summary += "총미체결수량(tordrem)=" + NumberField(fixed.tordrem, sizeof(fixed.tordrem)) + " | ";
    g_summary += "추정수수료(cmss)=" + NumberField(fixed.cmss, sizeof(fixed.cmss)) + " | ";
    g_summary += "총주문금액(tamt)=" + NumberField(fixed.tamt, sizeof(fixed.tamt)) + " | ";
    g_summary += "총매도체결금액(tmdamt)=" + NumberField(fixed.tmdamt, sizeof(fixed.tmdamt)) + " | ";
    g_summary += "총매수체결금액(tmsamt)=" + NumberField(fixed.tmsamt, sizeof(fixed.tmsamt)) + " | ";
    g_summary += "추정제세금(tax)=" + NumberField(fixed.tax, sizeof(fixed.tax)) + " | ";
    g_summary += "연속주문번호(cts_ordno)=" + Field(fixed.cts_ordno, sizeof(fixed.cts_ordno)) + " | ";
}
void ShowOutput()
{
    char heading[128];
    sprintf_s(heading, "전체 수신 건수 %d (이 페이지 최대 %d행)\r\n", g_recordCount, DISPLAY_LIMIT);
    SetWindowTextA(g_output, (std::string(heading) + g_summary + "\r\n" + g_rows).c_str());
}
// tqty는 NUL 종료되지 않은 char[18]입니다. 공백 패딩을 허용하고
// 부호·비숫자·중간 공백·계산 범위 초과를 변환 전에 차단합니다.
BOOL ParseCount(const d8317OutBlock &fixed, int &count)
{
    const int maximum = (int)((MAX_RESPONSE_SIZE - sizeof(d8317OutBlock)) / sizeof(d8317OutBlock1));
    count = 0;
    BOOL digit = FALSE, ended = FALSE;
    for (int i = 0; i < (int)sizeof(fixed.tqty); ++i)
    {
        char c = fixed.tqty[i];
        if (c == ' ')
        {
            if (digit) ended = TRUE;
        }
        else if (c >= '0' && c <= '9' && !ended)
        {
            int value = c - '0';
            if (count > (maximum - value) / 10) return FALSE;
            digit = TRUE;
            count = count * 10 + value;
        }
        else return FALSE;
    }
    return digit;
}

// d8317은 비블록 전문이며 BlockName으로 출력 구조체를 구분하지 않습니다.
BOOL ValidBody(const RP_DATA_INFO *response)
{
    if (!response || response->nDataLength < 0 ||
        (!response->lpData && response->nDataLength > 0)) return FALSE;
    if (response->nTotalDataBufferSize > 0 && response->nDataLength > response->nTotalDataBufferSize) return FALSE;
    return (size_t)response->nDataLength <= MAX_RESPONSE_SIZE;
}

// Header Type D 참조 진입점. SDK가 조립한 본문에서 실제 구조체 포인터로 읽습니다.
BOOL ReceiveData(const RP_DATA_INFO *response)
{
    if (!response || response->nRqID != g_requestId || g_requestId < 0 || g_queryError) return FALSE;
    if (Field(response->Header.szTrCode, sizeof(response->Header.szTrCode)) != SERVICE_CODE || !ValidBody(response)) return FALSE;
    if (response->Header.szRetFlag[0] >= '1' && response->Header.szRetFlag[0] <= '9') return FALSE;
    if (g_fixedReceived || response->nDataLength < (int)sizeof(d8317OutBlock)) return FALSE;

    d8317OutBlock *pOutBlock = (d8317OutBlock *)response->lpData;
    int count = 0;
    if (!ParseCount(*pOutBlock, count)) return FALSE;
    size_t expectedSize = sizeof(d8317OutBlock) + 5 + (size_t)count * sizeof(d8317OutBlock1);
    if ((size_t)response->nDataLength != expectedSize) return FALSE;
    d8317OutBlock1 *pOutBlock1 = (d8317OutBlock1 *)(response->lpData + sizeof(d8317OutBlock));

    g_moreFlag = response->Header.szPNFlag[0] == '1';
    g_sdkKey.assign(response->Header.szContKey, 18); // 선행·후행 공백도 보존합니다.
    g_expectedCount = count;
    ReceiveFixed(*pOutBlock);
    for (int i = 0; i < count; i++)
    {
        // 전체 경계를 검증한 후 실제 SDK 멤버에서 각 필드를 추출합니다.
        if (i < DISPLAY_LIMIT)
        {
            char number[32];
            sprintf_s(number, "[%d] ", i + 1);
            g_rows += number;
            g_rows += "주문번호(ordno)=" + Field(pOutBlock1->ordno, sizeof(pOutBlock1->ordno)) + " | ";
            g_rows += "종목코드(expcode)=" + Field(pOutBlock1->expcode, sizeof(pOutBlock1->expcode)) + " | ";
            g_rows += "거래소(marketgb)=" + Field(pOutBlock1->marketgb, sizeof(pOutBlock1->marketgb)) + " | ";
            g_rows += "매매구분(medosu)=" + Field(pOutBlock1->medosu, sizeof(pOutBlock1->medosu)) + " | ";
            g_rows += "주문수량(qty)=" + NumberField(pOutBlock1->qty, sizeof(pOutBlock1->qty)) + " | ";
            g_rows += "주문가격(price)=" + NumberField(pOutBlock1->price, sizeof(pOutBlock1->price)) + " | ";
            g_rows += "체결수량(cheqty)=" + NumberField(pOutBlock1->cheqty, sizeof(pOutBlock1->cheqty)) + " | ";
            g_rows += "체결가격(cheprice)=" + NumberField(pOutBlock1->cheprice, sizeof(pOutBlock1->cheprice)) + " | ";
            g_rows += "미체결잔량(ordrem)=" + NumberField(pOutBlock1->ordrem, sizeof(pOutBlock1->ordrem)) + " | ";
            g_rows += "확인수량(cfmqty)=" + NumberField(pOutBlock1->cfmqty, sizeof(pOutBlock1->cfmqty)) + " | ";
            g_rows += "주문상태(status)=" + Field(pOutBlock1->status, sizeof(pOutBlock1->status)) + " | ";
            g_rows += "원주문번호(orgordno)=" + Field(pOutBlock1->orgordno, sizeof(pOutBlock1->orgordno)) + " | ";
            g_rows += "주문유형(ordgb)=" + Field(pOutBlock1->ordgb, sizeof(pOutBlock1->ordgb)) + " | ";
            g_rows += "주문시간(ordtime)=" + Field(pOutBlock1->ordtime, sizeof(pOutBlock1->ordtime)) + " | ";
            g_rows += "주문매체(ordermtd)=" + Field(pOutBlock1->ordermtd, sizeof(pOutBlock1->ordermtd)) + " | ";
            g_rows += "처리순번(sysprocseq)=" + Field(pOutBlock1->sysprocseq, sizeof(pOutBlock1->sysprocseq)) + " | ";
            g_rows += "호가유형(hogagb)=" + Field(pOutBlock1->hogagb, sizeof(pOutBlock1->hogagb)) + " | ";
            g_rows += "현재가(price1)=" + NumberField(pOutBlock1->price1, sizeof(pOutBlock1->price1)) + " | ";
            g_rows += "주문구분(orggb)=" + Field(pOutBlock1->orggb, sizeof(pOutBlock1->orggb)) + " | ";
            g_rows += "신용구분(singb)=" + Field(pOutBlock1->singb, sizeof(pOutBlock1->singb)) + " | ";
            g_rows += "대출일자(loandt)=" + Field(pOutBlock1->loandt, sizeof(pOutBlock1->loandt)) + " | ";
            g_rows += "종목명(hname)=" + Field(pOutBlock1->hname, sizeof(pOutBlock1->hname)) + " | ";
            g_rows += "체결시간(chetime)=" + Field(pOutBlock1->chetime, sizeof(pOutBlock1->chetime)) + " | ";
            g_rows += "전환가격(cndiprc)=" + NumberField(pOutBlock1->cndiprc, sizeof(pOutBlock1->cndiprc)) + " | ";
            g_rows += "전환시간(cfmtime_stopprc)=" + Field(pOutBlock1->cfmtime_stopprc, sizeof(pOutBlock1->cfmtime_stopprc)) + " | ";
            g_rows += "등락율(diff)=" + NumberField(pOutBlock1->diff, sizeof(pOutBlock1->diff), TRUE) + " | ";
            g_rows += "\r\n";
        }
        pOutBlock1++;
    }
    g_recordCount = count;
    g_rowsReceived = TRUE;
    return TRUE;
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
            Common::SetResult(L"응답 서비스·본문 길이 또는 서버 오류를 확인하십시오.");
            if (item.malformed)
                Common::SetResult(L"조회 응답 데이터가 없거나 길이가 맞지 않습니다.");
        }
        else
        {
            g_receivedData = TRUE;
            if (!g_queryError)
                Common::SetResult(L"조회 응답을 수신했습니다. 요청 해제 통지를 기다립니다.");
        }
    }
    else if (item.kind == MESSAGE_DATA || item.kind == SYSTEM_ERROR_DATA)
    {
        Common::SetServerResult(item.code.c_str(), item.text.c_str());
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
        if (!g_fixedReceived || !g_rowsReceived)
        {
            g_queryError = TRUE;
            Common::SetResult(L"고정 출력 또는 반복 출력이 누락되어 조회를 완료할 수 없습니다.");
        }
        g_hasNext = !g_queryError && g_receivedData && g_fixedReceived && g_moreFlag &&
                    !Field(g_cursor.data(), (int)g_cursor.size()).empty() && g_cursor != g_sentCursor;
        g_hasNext = g_hasNext && !Field(g_sdkKey.data(), (int)g_sdkKey.size()).empty() ;
        if (g_queryError) ClearOutput();
        ShowOutput();
        if (!g_receivedData && !g_queryError)
            Common::SetResult(L"조회가 종료되었으나 마스터 출력 블록이 없습니다.");
        else if (!g_queryError)
            Common::SetResult(g_hasNext ? L"조회 완료. 다음 조회가 가능합니다." : L"조회 완료. 마지막 페이지 또는 빈·반복 커서입니다.");
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
        if (!response)
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
void RequestData(HWND window, BOOL next = FALSE)
{
    if (g_closing || g_submitting || g_processing || Common::IsBusy() || g_requestId >= 0)
        return;
    if (!Common::IsReady())
    {
        Common::SetResult(L"로그인이 필요합니다.");
        return;
    }
    d8317InBlock input;
    if (!BuildInput(input, next))
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
    g_sentCursor = next ? g_cursor : std::string();
    g_sentKey = next ? g_sdkKey : std::string();
    ResetContinuation();
    g_fixedReceived = FALSE;
    g_rowsReceived = FALSE;
    g_expectedCount = -1;
    g_moreFlag = FALSE;
    ClearOutput();
    g_receivedData = FALSE;
    g_queryError = FALSE;
    // 최초는 4인자, 다음은 본문 커서와 공개 API의 5번째 인자를 전달합니다.
    int result = next
        ? Common::Api()->RequestData(window, NAME_d8317, &input, sizeof(input), g_sentKey.c_str())
        : Common::Api()->RequestData(window, NAME_d8317, &input, sizeof(input));
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
        Common::AddControl(window, L"STATIC", L"계좌번호 (accno)", 0, 580, 52, 205, 22, 0);
        g_inputs[0] = Common::AddControl(window, L"EDIT", L"", ES_AUTOHSCROLL | WS_TABSTOP, 790, 52, 300, 24, 400);
        SendMessage(g_inputs[0], EM_SETLIMITTEXT, 11, 0);
        SetWindowTextA(g_inputs[0], "");
        Common::AddControl(window, L"STATIC", L"비밀번호 (passwd)", 0, 580, 81, 205, 22, 0);
        g_inputs[1] = Common::AddControl(window, L"EDIT", L"", ES_AUTOHSCROLL | WS_TABSTOP | ES_PASSWORD, 790, 81, 300, 24, 401);
        SendMessage(g_inputs[1], EM_SETLIMITTEXT, 8, 0);
        SetWindowTextA(g_inputs[1], "");
        Common::AddControl(window, L"STATIC", L"종목번호 (expcode)", 0, 580, 110, 205, 22, 0);
        g_inputs[2] = Common::AddControl(window, L"EDIT", L"", ES_AUTOHSCROLL | WS_TABSTOP, 790, 110, 300, 24, 402);
        SendMessage(g_inputs[2], EM_SETLIMITTEXT, 12, 0);
        SetWindowTextA(g_inputs[2], "");
        Common::AddControl(window, L"STATIC", L"체결구분 (chegb)", 0, 580, 139, 205, 22, 0);
        g_inputs[3] = Common::AddControl(window, L"COMBOBOX", L"", CBS_DROPDOWNLIST | WS_TABSTOP | WS_VSCROLL, 790, 139, 300, 170, 403);
        SendMessageW(g_inputs[3], CB_ADDSTRING, 0, (LPARAM)L"0 전체");
        SendMessage(g_inputs[3], CB_SETITEMDATA, 0, '0');
        SendMessageW(g_inputs[3], CB_ADDSTRING, 0, (LPARAM)L"1 체결");
        SendMessage(g_inputs[3], CB_SETITEMDATA, 1, '1');
        SendMessageW(g_inputs[3], CB_ADDSTRING, 0, (LPARAM)L"2 미체결");
        SendMessage(g_inputs[3], CB_SETITEMDATA, 2, '2');
        SendMessage(g_inputs[3], CB_SETCURSEL, 0, 0);
        Common::AddControl(window, L"STATIC", L"매매구분 (medosu)", 0, 580, 168, 205, 22, 0);
        g_inputs[4] = Common::AddControl(window, L"COMBOBOX", L"", CBS_DROPDOWNLIST | WS_TABSTOP | WS_VSCROLL, 790, 168, 300, 170, 404);
        SendMessageW(g_inputs[4], CB_ADDSTRING, 0, (LPARAM)L"0 전체");
        SendMessage(g_inputs[4], CB_SETITEMDATA, 0, '0');
        SendMessageW(g_inputs[4], CB_ADDSTRING, 0, (LPARAM)L"1 매도");
        SendMessage(g_inputs[4], CB_SETITEMDATA, 1, '1');
        SendMessageW(g_inputs[4], CB_ADDSTRING, 0, (LPARAM)L"2 매수");
        SendMessage(g_inputs[4], CB_SETITEMDATA, 2, '2');
        SendMessage(g_inputs[4], CB_SETCURSEL, 0, 0);
        Common::AddControl(window, L"STATIC", L"정렬순서 (sortgb)", 0, 580, 197, 205, 22, 0);
        g_inputs[5] = Common::AddControl(window, L"COMBOBOX", L"", CBS_DROPDOWNLIST | WS_TABSTOP | WS_VSCROLL, 790, 197, 300, 170, 405);
        SendMessageW(g_inputs[5], CB_ADDSTRING, 0, (LPARAM)L"1 역순");
        SendMessage(g_inputs[5], CB_SETITEMDATA, 0, '1');
        SendMessageW(g_inputs[5], CB_ADDSTRING, 0, (LPARAM)L"2 순");
        SendMessage(g_inputs[5], CB_SETITEMDATA, 1, '2');
        SendMessage(g_inputs[5], CB_SETCURSEL, 0, 0);
        Common::AddControl(window, L"STATIC", L"거래소구분 (exchange)", 0, 580, 226, 205, 22, 0);
        g_inputs[6] = Common::AddControl(window, L"COMBOBOX", L"", CBS_DROPDOWNLIST | WS_TABSTOP | WS_VSCROLL, 790, 226, 300, 170, 406);
        SendMessageW(g_inputs[6], CB_ADDSTRING, 0, (LPARAM)L"1 KRX");
        SendMessage(g_inputs[6], CB_SETITEMDATA, 0, '1');
        SendMessageW(g_inputs[6], CB_ADDSTRING, 0, (LPARAM)L"2 NXT");
        SendMessage(g_inputs[6], CB_SETITEMDATA, 1, '2');
        SendMessageW(g_inputs[6], CB_ADDSTRING, 0, (LPARAM)L"3 통합");
        SendMessage(g_inputs[6], CB_SETITEMDATA, 2, '3');
        SendMessage(g_inputs[6], CB_SETCURSEL, 2, 0);
        g_queryButton = Common::AddControl(window, L"BUTTON", L"처음 조회", WS_TABSTOP, 790, 263, 140, 30, QUERY_BUTTON);
        g_nextButton = Common::AddControl(window, L"BUTTON", L"다음 조회", WS_TABSTOP, 940, 263, 140, 30, NEXT_BUTTON);
        g_output = Common::AddControl(window, L"EDIT", L"", ES_READONLY | ES_MULTILINE | ES_AUTOVSCROLL | ES_AUTOHSCROLL | WS_VSCROLL | WS_HSCROLL, 580, 303, 540, 307, 0);
        SendMessage(g_output, EM_SETLIMITTEXT, 1024 * 1024, 0);
        UpdateQueryButton();
        return 0;
    }
    case WM_COMMAND:
        if (LOWORD(wParam) == QUERY_BUTTON && HIWORD(wParam) == BN_CLICKED)
            RequestData(window);
        if (LOWORD(wParam) == NEXT_BUTTON && HIWORD(wParam) == BN_CLICKED)
            RequestData(window, TRUE);
        if (LOWORD(wParam) >= 400 && LOWORD(wParam) < 500 &&
            (HIWORD(wParam) == EN_CHANGE || HIWORD(wParam) == CBN_SELCHANGE))
        {
            ResetContinuation();
            UpdateQueryButton();
        }
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
    WNDCLASSW wc = {0};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = instance;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
    wc.lpszClassName = L"OrdaAPId8317StockOrders";
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
