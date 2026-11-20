// 읽는 순서: WinMain 초기화 → 공통 로그인 통지 → 조회 요청 → 수신 → 해제 → 다음 조회 또는 종료.
// VS2010 v100 / Win32 / MultiByte / C++03. 공통 Helper는 변경하지 않습니다.
#include "../../../common/OrdaSampleCommon.h"
// 수신 블록명은 서비스 XML의 이름을 사용합니다.
#include "s2524.h"
#include <string.h>
#include <stdio.h>
#include <new>
#include <string>
#include <vector>
#include <deque>

namespace Common = OrdaSampleCommon;
const char SERVICE_CODE[] = NAME_s2524;
const wchar_t SERVICE_TITLE[] = L"뉴스본문조회(s2524)";
const int QUERY_BUTTON = 300;
const size_t BLOCK_LIMIT = 8u * 1024u * 1024u;
HWND g_queryButton, g_output, g_code;
int g_requestId = -1;

struct OutputBlock
{
    const char *name;
    HWND edit;
    std::string bytes;
    size_t queuedBytes;
    explicit OutputBlock(const char *blockName) : name(blockName), edit(NULL), queuedBytes(0) {}
};

OutputBlock g_related(NAME_s2524OutBlock);
OutputBlock g_body(NAME_s2524OutBlock1);
OutputBlock g_title(NAME_s2524OutBlock2);
OutputBlock *const g_blocks[] = {&g_related, &g_body, &g_title};
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
    OutputBlock *block;
    RP_DATA_INFO response;
    std::vector<unsigned char> data;
    std::string code, text;
    Notification() : message(0), kind(0), id(-1), systemError(0), malformed(FALSE), block(NULL) {}
};
std::deque<Notification> g_pending;

// SDK 통지의 메타데이터에만 사용합니다. 출력 블록 내용은 별도로 보존합니다.
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

// 문자열 코드는 변환 없이 공백으로 초기화된 요청 필드에 복사합니다.
BOOL ReadInput(HWND edit, char *field, size_t width, BOOL required)
{
    char value[32];
    int length = GetWindowTextA(edit, value, sizeof(value));
    if (GetWindowTextLengthA(edit) > (int)width || length > (int)width ||
        (required && strspn(value, " ") == (size_t)length))
        return FALSE;
    memcpy(field, value, length);
    return TRUE;
}

void ClearOutput()
{
    SetWindowTextA(g_output, "");
    for (size_t i = 0; i < _countof(g_blocks); ++i)
    {
        g_blocks[i]->bytes.clear();
        SetWindowTextA(g_blocks[i]->edit, "");
    }
}

BOOL BuildInput(s2524InBlock &input)
{
    // 미입력 문자 필드와 속성 바이트의 초기값은 공백입니다.
    memset(&input, ' ', sizeof(input));
    if (!ReadInput(g_code, input.sNewsno, sizeof(input.sNewsno), TRUE)) return FALSE;
    return TRUE;
}

OutputBlock *FindOutputBlock(const RP_DATA_INFO &response)
{
    std::string name = Field(response.szBlockName, sizeof(response.szBlockName));
    for (size_t i = 0; i < _countof(g_blocks); ++i)
        if (name == g_blocks[i]->name) return g_blocks[i];
    return NULL;
}

BOOL ValidBuffer(const RP_DATA_INFO &response)
{
    return response.nDataLength >= 0 &&
           response.nDataLength <= response.nTotalDataBufferSize &&
           (response.lpData || response.nDataLength == 0);
}

void ShowOutput()
{
    char heading[128];
    sprintf_s(heading, "수신 바이트   관련 종목 %u / 본문 %u / 제목 %u",
              (unsigned)g_related.bytes.size(), (unsigned)g_body.bytes.size(),
              (unsigned)g_title.bytes.size());
    SetWindowTextA(g_output, heading);
}

BOOL ReceiveData(const RP_DATA_INFO *response, OutputBlock *block)
{
    if (!response || !block || response->nRqID != g_requestId ||
        g_requestId < 0 || !ValidBuffer(*response))
        return FALSE;
    if (Field(response->Header.szTrCode, sizeof(response->Header.szTrCode)) != SERVICE_CODE)
        return FALSE;
    if (response->Header.szRetFlag[0] >= '1' && response->Header.szRetFlag[0] <= '9')
        return FALSE;

    size_t length = response->nDataLength;
    if (length > BLOCK_LIMIT - block->bytes.size()) return FALSE;
    if (length != 0)
        block->bytes.append(reinterpret_cast<const char *>(response->lpData), length);

    // 이전 조각과 연결한 ANSI 바이트를 표시합니다. c_str()은 끝에 NUL을 제공합니다.
    if (!SetWindowTextA(block->edit, block->bytes.c_str())) return FALSE;
    ShowOutput();
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
        if (item.malformed || !ReceiveData(&item.response, item.block))
        {
            g_queryError = TRUE;
            Common::SetResult(item.malformed ?
                L"응답 블록명, 버퍼 길이 또는 블록별 8 MiB 한도를 확인하십시오." :
                L"응답 서비스, 서버 오류 또는 Edit 출력 상태를 확인하십시오.");
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
        item.block = FindOutputBlock(*response);
        item.malformed = !item.block || !ValidBuffer(*response);
        if (!item.malformed && response->nDataLength > 0)
        {
            size_t bytes = response->nDataLength;
            if (bytes > BLOCK_LIMIT - item.block->bytes.size() - item.block->queuedBytes)
                item.malformed = TRUE;
            else
                item.data.assign(response->lpData, response->lpData + bytes);
        }
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
    std::vector<unsigned char> bytes;
    bytes.swap(item.data);
    g_pending.push_back(item);
    Notification &saved = g_pending.back();
    saved.data.swap(bytes);
    if (saved.block) saved.block->queuedBytes += saved.data.size();
}

void DrainNotifications(HWND window)
{
    if (g_submitting || g_processing)
        return;
    g_processing = TRUE;
    while (!g_pending.empty())
    {
        Notification &item = g_pending.front();
        if (item.block) item.block->queuedBytes -= item.data.size();
        try
        {
            ProcessNotification(item);
        }
        catch (const std::bad_alloc &)
        {
            g_queryError = TRUE;
            Common::SetResult(L"수신 데이터를 표시할 메모리가 부족합니다.");
        }
        g_pending.pop_front();
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
    s2524InBlock input;
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
    {
        if (!g_queryError)
            Common::SetResult(L"조회 요청 접수. 최종 결과는 비동기 응답을 확인하십시오.");
    }
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
        try
        {
            CaptureNotification(message, wParam, lParam);
        }
        catch (const std::bad_alloc &)
        {
            if (message == Common::API_MESSAGE_BASE + WM_RECEIVE_DATA &&
                (wParam == MESSAGE_DATA || wParam == SYSTEM_ERROR_DATA) && lParam && Common::Api())
            {
                const MSG_PACKET *packet = reinterpret_cast<const MSG_PACKET *>(lParam);
                if (packet->nIsSystemError == 1 && packet->nRqID == g_requestId)
                    g_requestId = -1;
                Common::Api()->ReleaseMessageData(lParam);
            }
            g_queryError = TRUE;
            Common::SetResult(L"수신 데이터를 저장할 메모리가 부족합니다.");
        }
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
        Common::AddControl(window, L"STATIC", L"뉴스번호", 0, 580, 53, 100, 22, 0);
        g_code = Common::AddControl(window, L"EDIT", L"", WS_BORDER | WS_TABSTOP | ES_AUTOHSCROLL, 685, 49, 245, 24, 303);
        SendMessage(g_code, EM_SETLIMITTEXT, 24, 0);
        const DWORD readOnly = ES_READONLY | WS_BORDER | WS_TABSTOP;
        const DWORD multiline = readOnly | ES_MULTILINE | ES_AUTOVSCROLL | WS_VSCROLL | ES_AUTOHSCROLL | WS_HSCROLL;
        g_title.edit = Common::AddControl(window, L"EDIT", L"", readOnly | ES_AUTOHSCROLL,
                                          580, 116, 540, 26, 304);
        g_body.edit = Common::AddControl(window, L"EDIT", L"", multiline, 580, 150, 540, 270, 305);
        Common::AddControl(window, L"STATIC", L"관련 종목", 0, 580, 424, 540, 22, 0);
        g_related.edit = Common::AddControl(window, L"EDIT", L"", multiline, 580, 448, 540, 132, 302);
        g_queryButton = Common::AddControl(window, L"BUTTON", L"조회", WS_TABSTOP, 950, 48, 140, 29, QUERY_BUTTON);
        g_output = Common::AddControl(window, L"STATIC", L"", 0, 580, 91, 540, 22, 0);
        for (size_t i = 0; i < _countof(g_blocks); ++i)
        {
            if (!g_blocks[i]->edit) return -1;
            SendMessage(g_blocks[i]->edit, EM_SETLIMITTEXT, BLOCK_LIMIT, 0);
        }
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
    WNDCLASSW wc = {0};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = instance;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
    wc.lpszClassName = L"OrdaAPIs2524NewsContent";
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
