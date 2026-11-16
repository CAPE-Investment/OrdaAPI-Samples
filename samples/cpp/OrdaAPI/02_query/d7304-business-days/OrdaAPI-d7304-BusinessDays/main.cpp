// 읽는 순서: WinMain 초기화 → WindowProc 로그인 통지 → 요청 함수 → 수신 함수 → 종료 처리.
// Load/Initialize와 Connect/Login은 기존 Common Helper를 사용합니다.
// 로그인 결과는 Common::HandleMessage가 처리하며 요청 API는 아래에서 직접 호출합니다.
// VS2010 v100 / Win32 / MultiByte. 조회 요청과 응답 처리는 이 파일에서 확인합니다.
#include "../../../common/OrdaSampleCommon.h"
#include "d7304.h"
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <wchar.h>
#include <string>
#include <deque>

namespace Common = OrdaSampleCommon;

const char SERVICE_CODE[] = "d7304";
const wchar_t WINDOW_TITLE[] = L"OrdaAPI 영업일정보 조회 (d7304 / C)";
const int INPUT_SIZE = 21;
const BOOL ALLOW_CONTINUATION = FALSE; // 이 XML에는 연속조회 필드가 없습니다.
const char CONTINUATION_FLAG =
    'Y'; // C형 내부 연속조회 값은 Y/N이지만 이 서비스는 처음 조회만 요청합니다.
const int CURSOR_INDEX = -1;
struct InputChoice
{
    const wchar_t *label;
    const char *code;
};
// 이 필드의 두 선택값만 XML desc에 명시되어 있습니다. 일반 설명 파서는 사용하지 않습니다.
const InputChoice DAY_KINDS[] = {{L"N일전 (1)", "1"}, {L"N영업일전 (5)", "5"}};
struct InputField
{
    const wchar_t *label;
    int offset, width;
    const char *defaultValue;
    BOOL password;
    const InputChoice *choices;
    int choiceCount;
};
// d7304 입력에는 속성 바이트와 XML 기본값이 없으므로 사용자가 입력합니다.
const InputField INPUT_FIELDS[] = {
    {L"구분 (gubun: 1 또는 5)", 0, 1, "", FALSE, DAY_KINDS, _countof(DAY_KINDS)},
    {L"시작일자 (fromdt)", 1, 8, "", FALSE},
    {L"종료일자 (todt)", 9, 8, "", FALSE},
    {L"일수 (cnt)", 17, 4, "", FALSE}};
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
        d7304OutBlock output;
        memcpy(&output, records + row * sizeof(output), sizeof(output));
        text += "bizdt=";
        text += Field(output.bizdt, sizeof(output.bizdt), 0, sizeof(output.bizdt));
        text += " | bfbizdt=";
        text += Field(output.bfbizdt, sizeof(output.bfbizdt), 0, sizeof(output.bfbizdt));
        text += " | afbizdt=";
        text += Field(output.afbizdt, sizeof(output.afbizdt), 0, sizeof(output.afbizdt));
        text += " | afbizdt2=";
        text += Field(output.afbizdt2, sizeof(output.afbizdt2), 0, sizeof(output.afbizdt2));
        text += " | day_cnt=";
        text += Field(output.day_cnt, sizeof(output.day_cnt), 0, sizeof(output.day_cnt));
        text += "\r\n";
    }
    return text;
}

// ------------------------------------------------------------
// C형 본문: d7304OutBlock 36바이트이며 속성 바이트나 반복 건수 접두부가 없습니다.
// 통신 헤더는 OrdaAPI가 서버 TRINFO에 따라 만들므로 입력 본문에 추가하지 않습니다.
// ------------------------------------------------------------

// 출력 배치: XML에 정의된 고정 블록·반복 건수·전체 길이를 검사합니다.
BOOL ParseService(const RP_DATA_INFO *response, std::string &text)
{
    if (!response->lpData || response->nDataLength != sizeof(d7304OutBlock))
        return FALSE;
    text = "d7304OutBlock\r\n" + Rows(response->lpData, 1);
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
BOOL BuildInput(d7304InBlock &input)
{
    // 화면 입력을 제공된 서비스 구조체의 실제 멤버에 대응시킵니다.
    char *fields[] = {input.gubun, input.fromdt, input.todt, input.cnt};
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
    // 속성 바이트가 없는 고정 길이 문자 필드이며 종료 NUL 없이 공백을 유지합니다.
    d7304InBlock input;
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
    // C형 통신 헤더는 OrdaAPI가 만들며 이 서비스의 연속조회 인자는 항상 NULL입니다.
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
