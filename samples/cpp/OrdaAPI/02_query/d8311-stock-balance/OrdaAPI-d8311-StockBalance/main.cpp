// 읽는 순서: WinMain 초기화 → WindowProc 로그인 통지 → 요청 함수 → 수신 함수 → 종료 처리.
// Load/Initialize와 Connect/Login은 기존 Common Helper를 사용합니다.
// 로그인 결과는 Common::HandleMessage가 처리하며 요청 API는 아래에서 직접 호출합니다.
// VS2010 v100 / Win32 / MultiByte. 조회 요청과 응답 처리는 이 파일에서 확인합니다.
#include "../../../common/OrdaSampleCommon.h"
#include "d8311.h"
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <wchar.h>
#include <string>
#include <deque>

namespace Common = OrdaSampleCommon;

const char SERVICE_CODE[] = "d8311";
const wchar_t WINDOW_TITLE[] = L"OrdaAPI 주식잔고 조회 (d8311 / D)";
const int INPUT_SIZE = 49;
const BOOL ALLOW_CONTINUATION = TRUE;
const char CONTINUATION_FLAG = '1'; // 공개 szPNFlag는 D형의 szTuxCont에서 전달됩니다.
const int CURSOR_INDEX =
    3; // 수신 d8311OutBlock.cts_expcode를 다음 요청의 d8311InBlock.cts_expcode에 전달합니다.
struct InputChoice
{
    const wchar_t *label;
    const char *code;
};
const InputChoice EXCHANGES[] = {{L"KRX (1)", "1"}, {L"NXT (2)", "2"}, {L"통합 (3)", "3"}};
struct InputField
{
    const wchar_t *label;
    int offset, width;
    const char *defaultValue;
    BOOL password;
    const InputChoice *choices;
    int choiceCount;
};
// d8311은 attr=1이며 각 필드 뒤에 공백 속성 바이트 1개가 있습니다.
const InputField INPUT_FIELDS[] = {
    {L"계좌번호 (accno)", 0, 11, "", FALSE},
    {L"계좌 비밀번호 (passwd)", 12, 8, "", TRUE},
    {L"종목번호 (expcode)", 21, 12, "            ", FALSE},
    {L"연속 종목번호 (cts_expcode)", 34, 12, "            ", FALSE},
    {L"거래소 (exchange: 1/2/3)", 47, 1, "3", FALSE, EXCHANGES, _countof(EXCHANGES)}};
HWND g_inputs[_countof(INPUT_FIELDS)], g_queryButton, g_nextButton, g_output;
int g_requestId = -1;
BOOL g_closing = FALSE, g_submitting = FALSE, g_received = FALSE, g_error = FALSE;
BOOL g_hasNext = FALSE, g_baseValid = FALSE;
char g_nextKey[19] = {0}; // 수신 키를 복사해 보관하며 18바이트 고정 길이의 공백도 유지합니다.
char g_cursor[13] = {0};
char g_baseInput[INPUT_SIZE] = {0}; // 조회 조건을 보관할 때 비밀번호 부분은 공백으로 지웁니다.
struct Notification
{
    UINT message;
    WPARAM wParam;
    LPARAM lParam;
};
std::deque<Notification> g_deferred;

std::string Field(const void *data, int length, int offset, int width)
{
    if (!data || offset < 0 || width < 0 || length < 0 || offset > length ||
        width > length - offset)
        return std::string();
    std::string value((const char *)data + offset, width);
    while (!value.empty() && (value[value.size() - 1] == ' ' || value[value.size() - 1] == 0))
        value.erase(value.size() - 1);
    size_t first = value.find_first_not_of(' ');
    return first == std::string::npos ? std::string() : value.substr(first);
}

// 비블록 반복 구간은 ASCII 건수 5바이트 뒤에 고정 길이 레코드가 이어집니다.
// 건수의 앞뒤 공백은 허용하되 부호·숫자 누락·그 외 문자는 거부합니다.
BOOL RecordCount(const unsigned char *data, int length, int position, int width, int &count)
{
    if (!data || position < 0 || position > length || length - position < 5 || width <= 0)
        return FALSE;
    count = 0;
    BOOL digit = FALSE, ended = FALSE;
    for (int i = 0; i < 5; ++i)
    {
        unsigned char c = data[position + i];
        if (c == ' ')
        {
            if (digit)
                ended = TRUE;
        }
        else if (c >= '0' && c <= '9' && !ended)
        {
            digit = TRUE;
            count = count * 10 + c - '0';
        }
        else
            return FALSE;
    }
    return digit && count <= (length - position - 5) / width;
}

std::string Ansi(const wchar_t *value)
{
    int length = WideCharToMultiByte(CP_ACP, 0, value, -1, NULL, 0, NULL, NULL);
    if (length <= 0)
        return std::string();
    std::string result(length, 0);
    WideCharToMultiByte(CP_ACP, 0, value, -1, &result[0], length, NULL, NULL);
    result.erase(result.size() - 1);
    return result;
}
std::string Rows(const unsigned char *records, int count)
{
    wchar_t heading[128];
    swprintf_s(heading, _countof(heading), L"레코드 수: %d (최대 200행 표시)\r\n", count);
    std::string text = Ansi(heading);
    int shown = count < 200 ? count : 200;
    for (int row = 0; row < shown; ++row)
    {
        char number[32];
        sprintf_s(number, "[%d] ", row + 1);
        text += number;
        // ParseService에서 고정 블록과 반복 레코드의 전체 경계를 검사한 상태입니다.
        d8311OutBlock1 output;
        memcpy(&output, records + row * sizeof(output), sizeof(output));
        text += "expcode=";
        text += Field(output.expcode, sizeof(output.expcode), 0, sizeof(output.expcode));
        text += " | janqty=";
        text += Field(output.janqty, sizeof(output.janqty), 0, sizeof(output.janqty));
        text += " | mdposqt=";
        text += Field(output.mdposqt, sizeof(output.mdposqt), 0, sizeof(output.mdposqt));
        text += " | pamt=";
        text += Field(output.pamt, sizeof(output.pamt), 0, sizeof(output.pamt));
        text += " | price=";
        text += Field(output.price, sizeof(output.price), 0, sizeof(output.price));
        text += " | sunikrt=";
        text += Field(output.sunikrt, sizeof(output.sunikrt), 0, sizeof(output.sunikrt));
        text += "\r\n";
    }
    return text;
}

// ------------------------------------------------------------
// D형 본문: d8311OutBlock 13바이트 + ASCII 건수 5바이트 + d8311OutBlock1 150바이트 × N.
// 첫 블록의 XML 커서는 Header.szContKey와 별도로 보관합니다.
// 통신 헤더는 OrdaAPI가 서버 TRINFO에 따라 만들므로 입력 본문에 추가하지 않습니다.
// ------------------------------------------------------------

// 출력 배치: XML에 정의된 고정 블록·반복 건수·전체 길이를 검사합니다.
BOOL ParseService(const RP_DATA_INFO *response, std::string &text)
{
    const unsigned char *data = response->lpData;
    const int fixedSize = sizeof(d8311OutBlock);
    const int recordSize = sizeof(d8311OutBlock1);
    const int recordsOffset = fixedSize + 5; // 비블록 반복 구간 앞의 ASCII 건수 길이입니다.
    int count = 0;
    if (!RecordCount(data, response->nDataLength, fixedSize, recordSize, count) ||
        response->nDataLength != recordsOffset + count * recordSize)
        return FALSE;
    d8311OutBlock fixed;
    memcpy(&fixed, data, sizeof(fixed));
    memcpy(g_cursor, fixed.cts_expcode, sizeof(fixed.cts_expcode));
    g_cursor[12] = 0;
    text = "d8311OutBlock.cts_expcode=" +
           Field(fixed.cts_expcode, sizeof(fixed.cts_expcode), 0, sizeof(fixed.cts_expcode)) +
           "\r\n";
    text += "d8311OutBlock1\r\n" + Rows(data + recordsOffset, count);
    return TRUE;
}

void UpdateButtons()
{
    BOOL ready =
        Common::IsReady() && !Common::IsBusy() && !g_closing && !g_submitting && g_requestId < 0;
    EnableWindow(g_queryButton, ready);
    if (g_nextButton)
        EnableWindow(g_nextButton, ready && g_hasNext && !g_error);
}

// 입력 전문: 화면 값과 실제 코드값을 구조체 멤버에 설정합니다.
BOOL BuildInput(d8311InBlock &input)
{
    // 화면 입력을 제공된 서비스 구조체의 실제 멤버에 대응시킵니다.
    char *fields[] = {input.accno, input.passwd, input.expcode, input.cts_expcode, input.exchange};
    for (int i = 0; i < _countof(INPUT_FIELDS); ++i)
    {
        char value[128] = {0};
        int length = 0;
        if (INPUT_FIELDS[i].choiceCount)
        {
            int selected = (int)SendMessage(g_inputs[i], CB_GETCURSEL, 0, 0);
            if (selected < 0 || selected >= INPUT_FIELDS[i].choiceCount)
            {
                Common::SetResult(L"?? ??? ?? ??? ??????.");
                return FALSE;
            }
            // 화면 설명이 아닌 XML의 실제 코드값만 복사합니다.
            const char *code = INPUT_FIELDS[i].choices[selected].code;
            length = (int)strlen(code);
            if (length < sizeof(value))
                memcpy(value, code, length);
        }
        else
            length = GetWindowTextA(g_inputs[i], value, sizeof(value));
        if (length > INPUT_FIELDS[i].width)
        {
            SecureZeroMemory(value, sizeof(value));
            Common::SetResult(L"입력값의 바이트 길이가 XML 필드 길이를 초과했습니다.");
            return FALSE;
        }
        memcpy(fields[i], value, length);
        SecureZeroMemory(value, sizeof(value));
    }
    return TRUE;
}

void RemovePasswords(char *input)
{
    for (int i = 0; i < _countof(INPUT_FIELDS); ++i)
        if (INPUT_FIELDS[i].password)
            memset(input + INPUT_FIELDS[i].offset, ' ', INPUT_FIELDS[i].width);
}

// 조회 요청: 입력 설정과 연속조회 상태를 확인한 뒤 RequestData를 호출합니다.
void RequestService(HWND window, BOOL next)
{
    if (!Common::IsReady())
    {
        Common::SetResult(L"로그인이 필요합니다.");
        return;
    }
    if (Common::IsBusy() || g_submitting || g_requestId >= 0 || g_closing)
        return;
    if (next && (!ALLOW_CONTINUATION || !g_hasNext || g_error))
        return;
    // 모든 XML 입력은 문자 필드이며 공백 패딩과 속성 바이트를 유지합니다.
    d8311InBlock input;
    memset(&input, ' ', sizeof(input));
    if (!BuildInput(input))
    {
        SecureZeroMemory(&input, sizeof(input));
        return;
    }
    char nonSecret[INPUT_SIZE];
    memcpy(nonSecret, &input, sizeof(input));
    RemovePasswords(nonSecret);
    // 커서는 페이지 상태이므로 사용자가 변경한 조회 조건과 구분합니다.
    if (CURSOR_INDEX >= 0)
        memset(nonSecret + INPUT_FIELDS[CURSOR_INDEX].offset,
               ' ',
               INPUT_FIELDS[CURSOR_INDEX].width);
    if (next && (!g_baseValid || memcmp(nonSecret, g_baseInput, INPUT_SIZE)))
    {
        SecureZeroMemory(&input, sizeof(input));
        Common::SetResult(L"조회 조건이 변경되었습니다. 처음 조회를 다시 실행하십시오.");
        return;
    }
    memcpy(g_baseInput, nonSecret, sizeof(nonSecret));
    g_baseValid = TRUE;
    SecureZeroMemory(nonSecret, sizeof(nonSecret));
    if (CURSOR_INDEX >= 0)
        memcpy(input.cts_expcode, next ? g_cursor : "            ", sizeof(input.cts_expcode));
    char nextKey[19] = {0};
    if (next)
        memcpy(nextKey, g_nextKey, sizeof(nextKey));
    g_hasNext = FALSE;
    g_received = FALSE;
    g_error = FALSE;
    if (!next)
    {
        memset(g_nextKey, 0, sizeof(g_nextKey));
        memset(g_cursor, 0, sizeof(g_cursor));
    }
    SetWindowTextA(g_output, "");
    Common::BeginCall();
    if (!Common::CheckSession())
    {
        SecureZeroMemory(&input, sizeof(input));
        Common::EndCall(window);
        Common::SetResult(L"로그인이 필요합니다. 프로그램을 다시 실행하십시오.");
        UpdateButtons();
        return;
    }
    g_submitting = TRUE;
    UpdateButtons();
    // D형 연속조회에는 입력 구조체의 수신 커서와 Header의 실제 키가 모두 필요합니다.
    // 제품의 공개 패킷에 없는 키를 임의로 만들지 않습니다.
    int result = Common::Api()->RequestData(window,
                                            SERVICE_CODE,
                                            &input,
                                            sizeof(input),
                                            next ? nextKey : NULL);
    SecureZeroMemory(&input, sizeof(input));
    SecureZeroMemory(nextKey, sizeof(nextKey));
    for (int i = 0; i < _countof(INPUT_FIELDS); ++i)
        if (INPUT_FIELDS[i].password)
            SetWindowTextA(g_inputs[i], "");
    g_requestId = result >= 0 && result <= 255 ? result : -1;
    g_submitting = FALSE;
    if (g_requestId >= 0)
        Common::SetResult(L"조회를 요청했습니다. 응답을 기다립니다.");
    else
    {
        g_error = TRUE;
        Common::ShowCallResult(L"조회 요청 실패", result);
    }
    Common::EndCall(window);
    UpdateButtons();
}

// 조회 응답: 검증한 출력과 연속조회 키를 화면 및 다음 요청 상태에 반영합니다.
void ReceiveService(const RP_DATA_INFO *response)
{
    if (g_closing || !response || g_requestId < 0 || response->nRqID != g_requestId)
        return;
    if (Field(response->Header.szTrCode, sizeof(response->Header.szTrCode), 0, 6) != SERVICE_CODE)
    {
        g_error = TRUE;
        Common::SetResult(L"다른 서비스 코드의 응답입니다.");
        return;
    }
    // XML에 block=1이 없으므로 B/C/D형 본문은 출력 블록이 XML 순서로 이어집니다.
    // 제품의 단일·다중 패킷 경로에 따라 nDataMode가 달라질 수 있으므로
    // 이 값만으로 배치를 판단하지 않으며 lpData를 통신 헤더로 해석하지 않습니다.
    if (!Field(response->szBlockName, sizeof(response->szBlockName), 0, 17).empty())
    {
        g_error = TRUE;
        Common::SetResult(L"예상하지 않은 블록별 응답입니다.");
        return;
    }
    std::string text;
    if (!ParseService(response, text))
    {
        g_error = TRUE;
        Common::SetResult(L"응답 길이 또는 반복 건수가 XML 구조와 맞지 않습니다.");
        return;
    }
    SetWindowTextA(g_output, text.c_str());
    g_received = TRUE;
    // 응답에 실제 연속조회 키가 있는 경우에만 수동 다음 조회를 허용합니다.
    // 공개 패킷에 키가 없으면 임의로 생성하지 않습니다.
    memcpy(g_nextKey, response->Header.szContKey, 18);
    g_nextKey[18] = 0;
    g_hasNext = ALLOW_CONTINUATION && response->Header.szPNFlag[0] == CONTINUATION_FLAG &&
                !Field(g_nextKey, 18, 0, 18).empty();
    if (CURSOR_INDEX >= 0 && Field(g_cursor, 12, 0, 12).empty())
        g_hasNext = FALSE;
    if (CURSOR_INDEX >= 0)
        SetWindowTextA(g_inputs[CURSOR_INDEX], Field(g_cursor, 12, 0, 12).c_str());
    if (!g_error)
    {
        if (response->Header.szPNFlag[0] == CONTINUATION_FLAG && !g_hasNext)
            Common::SetResult(
                L"응답은 수신했으나 연속 조회에 필요한 키 또는 매핑이 없어 다음 조회를 활성화하지 않았습니다.");
        else
            Common::SetResult(L"조회 결과를 수신했습니다. 반복 결과는 최대 200행까지 표시합니다.");
    }
}

// 요청 통지: 데이터·서버 메시지·오류·해제를 구분하고 소유권에 맞춰 정리합니다.
void HandleNotification(UINT message, WPARAM kind, LPARAM parameter)
{
    OrdaAPI *api = Common::Api();
    if (!api)
        return;
    if (message == Common::API_MESSAGE_BASE + WM_TIMEOUT_DATA)
    {
        if (g_requestId >= 0 && (int)parameter == g_requestId)
        {
            api->ReleaseRequestData(g_requestId);
            g_requestId = -1;
            g_error = TRUE;
            g_hasNext = FALSE;
            if (!g_closing)
                Common::SetResult(L"조회 응답 시간이 초과되었습니다.");
        }
    }
    else if (kind == REQUEST_DATA)
        ReceiveService((const RP_DATA_INFO *)parameter);
    else if (kind == MESSAGE_DATA || kind == SYSTEM_ERROR_DATA)
    {
        const MSG_PACKET *packet = (const MSG_PACKET *)parameter;
        if (packet)
        {
            if (!g_closing && g_requestId >= 0 && packet->nRqID == g_requestId)
            {
                std::string code = Field(packet->szCode, sizeof(packet->szCode), 0, 5);
                int size = packet->nMsgLength;
                if (size > 2047)
                    size = 2047;
                std::string text = size > 0 && packet->lpszMessageData
                                       ? std::string((const char *)packet->lpszMessageData, size)
                                       : std::string();
                Common::SetServerResult(code.c_str(), text.c_str());
                if (kind == SYSTEM_ERROR_DATA || packet->nIsSystemError)
                {
                    g_error = TRUE;
                    g_hasNext = FALSE;
                }
            }
            api->ReleaseMessageData(parameter);
        }
    }
    else if (kind == RELEASE_DATA && g_requestId >= 0 && (int)parameter == g_requestId)
    {
        api->ReleaseRequestData(g_requestId);
        g_requestId = -1;
        if (!g_received && !g_error && !g_closing)
            Common::SetResult(L"조회가 종료되었으나 데이터가 없습니다.");
        if (g_error || !g_received)
            g_hasNext = FALSE;
    }
    UpdateButtons();
}

void DrainNotifications()
{
    while (!g_submitting && !g_deferred.empty())
    {
        Notification item = g_deferred.front();
        g_deferred.pop_front();
        HandleNotification(item.message, item.wParam, item.lParam);
    }
}

// 메인 메시지 처리: API 통지와 사용자 입력을 분리하고 종료 수명을 보호합니다.
LRESULT CALLBACK WindowProc(HWND window, UINT message, WPARAM wParam, LPARAM lParam)
{
    if (message == Common::API_MESSAGE_BASE + WM_RECEIVE_DATA ||
        message == Common::API_MESSAGE_BASE + WM_TIMEOUT_DATA)
    {
        if (g_submitting)
        {
            Notification item = {message, wParam, lParam};
            g_deferred.push_back(item);
        }
        else
            HandleNotification(message, wParam, lParam);
        return 0;
    }
    if (Common::HandleMessage(window, message, wParam, lParam))
    {
        UpdateButtons();
        return 0;
    }
    switch (message)
    {
    case WM_CREATE:
        Common::CreateLoginControls(window);
        for (int i = 0; i < _countof(INPUT_FIELDS); ++i)
        {
            int y = 25 + i * 30;
            Common::AddControl(window, L"STATIC", INPUT_FIELDS[i].label, 0, 580, y, 245, 22, 0);
            if (INPUT_FIELDS[i].choiceCount)
            {
                g_inputs[i] = Common::AddControl(window,
                                                 L"COMBOBOX",
                                                 L"",
                                                 CBS_DROPDOWNLIST | WS_TABSTOP | WS_VSCROLL,
                                                 830,
                                                 y - 2,
                                                 260,
                                                 150,
                                                 0);
                int initial = -1;
                for (int j = 0; j < INPUT_FIELDS[i].choiceCount; ++j)
                {
                    SendMessageW(g_inputs[i],
                                 CB_ADDSTRING,
                                 0,
                                 (LPARAM)INPUT_FIELDS[i].choices[j].label);
                    if (strcmp(INPUT_FIELDS[i].defaultValue, INPUT_FIELDS[i].choices[j].code) == 0)
                        initial = j;
                }
                SendMessage(g_inputs[i], CB_SETCURSEL, initial, 0);
            }
            else
            {
                g_inputs[i] = Common::AddControl(window,
                                                 L"EDIT",
                                                 L"",
                                                 ES_AUTOHSCROLL | WS_TABSTOP |
                                                     (INPUT_FIELDS[i].password ? ES_PASSWORD : 0) |
                                                     (i == CURSOR_INDEX ? ES_READONLY : 0),
                                                 830,
                                                 y - 2,
                                                 260,
                                                 24,
                                                 0);
                SendMessage(g_inputs[i], EM_SETLIMITTEXT, INPUT_FIELDS[i].width, 0);
                // XML 기본값은 컨트롤 생성 시에만 적용하여 수신한 연속조회 커서를 보존합니다.
                SetWindowTextA(g_inputs[i], INPUT_FIELDS[i].defaultValue);
            }
        }
        {
            int y = 30 + _countof(INPUT_FIELDS) * 30;
            g_queryButton = Common::AddControl(window,
                                               L"BUTTON",
                                               L"처음 조회",
                                               WS_TABSTOP,
                                               830,
                                               y,
                                               120,
                                               28,
                                               300);
            if (ALLOW_CONTINUATION)
                g_nextButton = Common::AddControl(window,
                                                  L"BUTTON",
                                                  L"다음 조회",
                                                  WS_TABSTOP,
                                                  970,
                                                  y,
                                                  120,
                                                  28,
                                                  301);
            Common::AddControl(window,
                               L"STATIC",
                               L"조회 결과 (전체 건수 / 최대 200행 표시)",
                               0,
                               580,
                               y + 38,
                               510,
                               22,
                               0);
            g_output = Common::AddControl(window,
                                          L"EDIT",
                                          L"",
                                          ES_READONLY | ES_MULTILINE | ES_AUTOVSCROLL |
                                              ES_AUTOHSCROLL | WS_VSCROLL | WS_HSCROLL,
                                          580,
                                          y + 62,
                                          510,
                                          300,
                                          0);
            SendMessage(g_output, EM_SETLIMITTEXT, 1024 * 1024, 0);
        }
        UpdateButtons();
        return 0;
    case WM_COMMAND:
        if ((LOWORD(wParam) == 300 || LOWORD(wParam) == 301) && HIWORD(wParam) == BN_CLICKED)
        {
            RequestService(window, LOWORD(wParam) == 301);
            DrainNotifications();
        }
        return 0;
    case WM_CLOSE:
        if (!Common::BeginClose())
            return 0;
        if (!g_closing)
        {
            g_closing = TRUE;
            UpdateButtons();
            DrainNotifications();
            if (g_requestId >= 0 && Common::Api())
            {
                Common::Api()->ReleaseRequestData(g_requestId);
                g_requestId = -1;
            }
            SecureZeroMemory(g_baseInput, sizeof(g_baseInput));
            for (int i = 0; i < _countof(INPUT_FIELDS); ++i)
                if (INPUT_FIELDS[i].password)
                    SetWindowTextA(g_inputs[i], "");
            Common::Cleanup();
            DestroyWindow(window);
        }
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
    wc.lpszClassName = L"OrdaQueryHeaderSample";
    if (!RegisterClassW(&wc))
        return 1;
    HWND window = CreateWindowW(wc.lpszClassName,
                                WINDOW_TITLE,
                                WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
                                CW_USEDEFAULT,
                                CW_USEDEFAULT,
                                1140,
                                650,
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
    UpdateButtons();
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
