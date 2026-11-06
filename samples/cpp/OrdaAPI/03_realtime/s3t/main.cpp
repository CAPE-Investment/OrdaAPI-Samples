// VS2010 v100 / C++03 / Win32 / MultiByte. Save as UTF-8 with BOM.
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include "../../common/OrdaSampleCommon.h"
namespace Common = OrdaSampleCommon;
#include <S3T.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <new>
#include <string>

typedef char CheckInput[(sizeof(InBlock) == 6) ? 1 : -1];
typedef char CheckOutput[(sizeof(OutBlock) == 223) ? 1 : -1];
typedef char CheckAlignment[(__alignof(OutBlock) == 1) ? 1 : -1];
typedef char CheckSymbol[(offsetof(OutBlock, shcode) == 216) ? 1 : -1];
typedef char CheckPrice[(offsetof(OutBlock, price) == 25) ? 1 : -1];

const UINT API_BASE = WM_APP + 100;
const UINT WORK_COMMAND = WM_APP + 500;
const UINT WORK_DONE = WM_APP + 501;
const int ID_REQUEST = 300, ID_RETRY = 301;
enum Operation { OP_ADVISE, OP_CLOSE };

// The API key-list contract needs a NUL, although InBlock itself is exactly 6 bytes.
struct Job
{
    Operation operation;
    OrdaAPI* api; // borrowed from the unchanged common helper until OP_CLOSE
    char key[sizeof(InBlock) + 1], oldKey[sizeof(InBlock) + 1];
    int result, connected, loggedIn, cleanupResult, keyCleanupResult;
    BOOL hadKey, needsWindowCleanup, oldReleased, adviseAttempted, exception;
    int unadviseResult;
};
struct WorkerContext { HWND window; HANDLE ready, done; };
WorkerContext g_worker = {0};
HANDLE g_thread = NULL;
DWORD g_threadId = 0;
Job* g_job = NULL; // UI owns it except between PostThreadMessage and done.SetEvent.
HWND g_window, g_symbol, g_current, g_retry;
HWND g_requestButton, g_status, g_data, g_log;
BOOL g_ready = FALSE, g_restart = FALSE, g_cleanupComplete = FALSE;
BOOL g_closing = FALSE, g_sessionLost = FALSE, g_registered = FALSE;
BOOL g_uncertain = FALSE, g_accept = FALSE;
BOOL g_haveTick = FALSE;
char g_key[7] = {0};
OutBlock g_latest;
unsigned long g_tickCount = 0;

void Log(const char* text)
{
    // Bound the UI history even for a long-running stream or repeated bad packets.
    if (!g_log) return;
    int length = GetWindowTextLengthA(g_log);
    if (length > 28000) SetWindowTextA(g_log, "[Earlier log entries cleared]\r\n");
    SendMessageA(g_log, EM_SETSEL, (WPARAM)-1, (LPARAM)-1);
    SendMessageA(g_log, EM_REPLACESEL, FALSE, (LPARAM)text);
    SendMessageA(g_log, EM_REPLACESEL, FALSE, (LPARAM)"\r\n");
}
void LogResult(const char* stage, int result)
{
    char text[200]; sprintf_s(text, sizeof(text), "%s: %d", stage, result); Log(text);
}
void UpdateControls()
{
    if (!g_closing) g_ready = Common::IsReady() && !g_sessionLost;
    BOOL idle = !g_job && !g_closing && !Common::IsBusy();
    EnableWindow(g_requestButton, idle && g_thread && Common::IsReady() && g_ready && !g_uncertain);
    EnableWindow(g_symbol, idle && !g_uncertain);
    EnableWindow(g_retry, idle);
    if (g_current) SetWindowTextA(g_current, g_registered ? g_key : "(none)");
}
void FinishWindow();
void CreateRealtimeControls();
// S3T requests run on one persistent worker. Login uses the unchanged common
// helper; the GUI continues to service synchronous DLL notifications.
// Never invoke the API from a receive notification (dispatcher iterators are live).
DWORD WINAPI ApiWorker(void* argument)
{
    WorkerContext* context = (WorkerContext*)argument;

    MSG message;
    PeekMessage(&message, NULL, WM_USER, WM_USER, PM_NOREMOVE);
    SetEvent(context->ready);
    while (GetMessage(&message, NULL, 0, 0) > 0)
    {
        if (message.hwnd || message.message != WORK_COMMAND)
        {
            TranslateMessage(&message); DispatchMessage(&message); continue;
        }
        Job* job = (Job*)message.lParam;
        BOOL finish = job->operation == OP_CLOSE;
        OrdaAPI* api = job->api;
        try
        {
            switch (job->operation)
            {
            case OP_ADVISE:
                job->connected = api->IsConnected(); job->loggedIn = api->IsLogin();
                if (job->connected != 1 || job->loggedIn != 1)
                { job->result = ErrCode::LoginRequired; break; }
                if (job->hadKey)
                {
                    job->unadviseResult = api->UnadviseRealData(context->window, NAME_S3T, job->oldKey, sizeof(InBlock));
                    // Preserve the previous registration if its explicit unadvise fails.
                    // Do not substitute UnadviseWindow or proceed to a new Advise here.
                    if (job->unadviseResult != ErrCode::Success)
                    { job->result = job->unadviseResult; break; }
                    job->oldReleased = TRUE;
                }
                job->adviseAttempted = TRUE;
                job->result = api->AdviseRealData(context->window, NAME_S3T, job->key, sizeof(InBlock));
                if (job->result != ErrCode::Success)
                    job->cleanupResult = api->UnadviseWindow(context->window);
                break;
            case OP_CLOSE:
                if (api && (job->hadKey || job->needsWindowCleanup))
                {
                    if (job->hadKey)
                        job->keyCleanupResult = api->UnadviseRealData(context->window, NAME_S3T, job->key, sizeof(InBlock));
                    // Window-wide cleanup is additional to the explicit key unadvise.
                    job->cleanupResult = api->UnadviseWindow(context->window);
                }
                Common::Cleanup(); // existing Logout -> Disconnect -> FreeLibrary
                job->result = ErrCode::Success;
                break;
            }
        }
        catch (...)
        {
            job->exception = TRUE; job->result = ErrCode::Failed;
            // Keep HWND and DLL alive if cleanup itself did not finish.
            finish = FALSE;
        }
        SetEvent(context->done); // publishes the job; never touch it again here
        PostMessage(context->window, WORK_DONE, 0, 0); // timer is a fallback
        if (finish) return 0;
    }
    return 0;
}

Job* NewJob(Operation operation)
{
    Job* job = new(std::nothrow) Job;
    if (job) { memset(job, 0, sizeof(*job)); job->operation = operation; job->api = Common::Api(); }
    else Log("Unable to allocate API command.");
    return job;
}
BOOL Submit(Job* job)
{
    if (!job) return FALSE;
    if (g_job) { SecureZeroMemory(job, sizeof(*job)); delete job; return FALSE; }
    ResetEvent(g_worker.done);
    g_job = job;
    if (!PostThreadMessage(g_threadId, WORK_COMMAND, 0, (LPARAM)job))
    {
        g_job = NULL; SecureZeroMemory(job, sizeof(*job)); delete job;
        Log("Unable to queue API command. No request was submitted.");
        UpdateControls(); return FALSE;
    }
    UpdateControls(); return TRUE;
}
void QueueClose()
{
    if (g_job) return; // completion will submit close; never block the UI
    if (!g_thread) { Common::Cleanup(); g_cleanupComplete = TRUE; FinishWindow(); return; }
    Job* job = NewJob(OP_CLOSE);
    if (!job) return;
    memcpy(job->key, g_key, sizeof(g_key));
    job->hadKey = g_registered;
    job->needsWindowCleanup = g_uncertain;
    Submit(job);
}
void CompleteJob()
{
    if (!g_job || WaitForSingleObject(g_worker.done, 0) != WAIT_OBJECT_0) return;
    Job* job = g_job; g_job = NULL;
    if (job->exception)
    {
        g_ready = FALSE; g_accept = FALSE; g_uncertain = TRUE;
        Log("API raised a C++ exception. Session is disabled; close the sample.");
        // Avoid an automatic retry loop if OP_CLOSE threw.
        if (job->operation == OP_CLOSE)
        {
            g_closing = FALSE;
            SecureZeroMemory(job, sizeof(*job)); delete job; UpdateControls(); return;
        }
    }
    else switch (job->operation)
    {
    case OP_ADVISE:
        if (job->connected != 1 || job->loggedIn != 1) { g_ready = FALSE; g_sessionLost = TRUE; }
        if (job->hadKey) LogResult("Previous key: UnadviseRealData", job->unadviseResult);
        if (job->hadKey && !job->oldReleased)
        {
            Log("Previous registration retained; no new Advise was called.");
            g_accept = g_registered && g_ready && !g_sessionLost && !g_closing;
            SetWindowTextW(g_status, L"기존 종목 해제 실패 — 기존 등록을 유지합니다");
        }
        else
        {
            LogResult("AdviseRealData (local API result, not a server acknowledgement)", job->result);
            g_registered = job->adviseAttempted && job->result == 1;
            g_uncertain = job->adviseAttempted && !g_registered && job->cleanupResult != 1;
            if (g_registered)
            {
                memcpy(g_key, job->key, sizeof(g_key));
                g_tickCount = 0; g_haveTick = FALSE; SetWindowTextA(g_data, "");
            }
            else
            {
                memset(g_key, 0, sizeof(g_key));
                g_tickCount = 0; g_haveTick = FALSE; SetWindowTextA(g_data, "");
            }
            g_accept = g_registered && g_ready && !g_sessionLost && !g_closing;
            if (job->connected != 1 || job->loggedIn != 1) { g_ready = FALSE; g_sessionLost = TRUE; }
            if (job->adviseAttempted && !g_registered) LogResult("Registration failure cleanup: UnadviseWindow", job->cleanupResult);
            SetWindowTextW(g_status, g_accept ? L"등록 API 성공 — 실시간 데이터 대기 중" : L"등록 실패 — 현재 등록 종목 없음");
        }
        break;
    case OP_CLOSE:
        LogResult("Close: UnadviseRealData (0 also means skipped)", job->keyCleanupResult);
        LogResult("Close: UnadviseWindow (0 also means skipped)", job->cleanupResult);
        Log("Common helper cleanup completed: Logout / Disconnect / DLL release.");
        // API is unloaded; the thread has no more SendMessage calls to make.
        WaitForSingleObject(g_thread, INFINITE); CloseHandle(g_thread); g_thread = NULL;
        CloseHandle(g_worker.ready); CloseHandle(g_worker.done);
        g_worker.ready = g_worker.done = NULL;
        SecureZeroMemory(job, sizeof(*job)); delete job;
        g_registered = FALSE; memset(g_key, 0, sizeof(g_key));
        g_cleanupComplete = TRUE; FinishWindow(); return;
    }
    SecureZeroMemory(job, sizeof(*job)); delete job;
    if (g_closing) QueueClose();
    UpdateControls();
}

void Advise()
{
    if (!Common::IsReady() || !g_ready || g_uncertain || g_job || Common::IsBusy() || g_closing) return;
    char symbol[32]; GetWindowTextA(g_symbol, symbol, sizeof(symbol));
    if (strlen(symbol) != sizeof(InBlock)) { Log("Enter a six-byte alphanumeric short code."); return; }
    for (int i = 0; i < 6; ++i)
        if (!((symbol[i] >= '0' && symbol[i] <= '9') || (symbol[i] >= 'A' && symbol[i] <= 'Z') || (symbol[i] >= 'a' && symbol[i] <= 'z')))
        { Log("The short code must contain six ASCII letters/digits."); return; }
    if (g_registered && memcmp(symbol, g_key, sizeof(InBlock)) == 0)
    { Log("Already registered: duplicate Advise was not called."); return; }
    Job* job = NewJob(OP_ADVISE); if (!job) return;
    job->hadKey = g_registered;
    memcpy(job->oldKey, g_key, sizeof(g_key));
    InBlock input; memcpy(input.shcode, symbol, sizeof(input.shcode));
    memcpy(job->key, &input, sizeof(input)); job->key[sizeof(input)] = 0;
    g_accept = FALSE; // ignore the old stream while the switch is pending
    g_haveTick = FALSE;
    if (!Submit(job)) g_accept = g_registered && g_ready && !g_sessionLost;
}
// Length-bounded conversion: never call strlen/printf("%s") on a wire field.
std::string Field(const char* field, size_t width)
{
    std::string text(field, width);
    while (!text.empty() && (text[text.size()-1] == ' ' || text[text.size()-1] == '\0')) text.erase(text.size()-1);
    size_t start = text.find_first_not_of(' ');
    if (start == std::string::npos) return "";
    text.erase(0, start);
    for (size_t i = 0; i < text.size(); ++i)
        if ((unsigned char)text[i] < 32 || (unsigned char)text[i] > 126) text[i] = '?';
    return text;
}
void DisplayTick();
void ReceiveRealData(LPARAM parameter)
{
    if (!g_accept || g_closing) return; // late packets after unadvise/disconnect
    const BC_ITEM_INFO* packet = (const BC_ITEM_INFO*)parameter;
    if (!packet) { Log("Rejected NULL realtime notification."); return; }
    if (memcmp(packet->szBcCode, NAME_S3T, MAX_BC_CODE_LENGTH) != 0 ||
        (packet->szBcCode[MAX_BC_CODE_LENGTH] != 0 && packet->szBcCode[MAX_BC_CODE_LENGTH] != ' '))
    { Log("Rejected notification: service is not S3T."); return; }
    if (packet->nDataLength != sizeof(OutBlock) || packet->nDataLength > MAX_PACKET_SIZE)
    { LogResult("Rejected S3T payload length (expected 223)", packet->nDataLength); return; }
    if (packet->nKeyLength != sizeof(InBlock) || memcmp(packet->szRegKey, g_key, sizeof(InBlock)) != 0)
    { Log("Rejected S3T notification: registration key differs."); return; }
    OutBlock output;
    memcpy(&output, packet->szBcData, sizeof(output));
    if (memcmp(output.shcode, g_key, sizeof(output.shcode)) != 0)
    { Log("Rejected S3T notification: payload shcode differs."); return; }
    // BC_ITEM_INFO is a dispatcher stack object. No borrowed pointer escapes.
    g_latest = output; g_haveTick = TRUE; ++g_tickCount;
    DisplayTick(); // already on the GUI thread; refresh once per valid notification
}
void DisplayTick()
{
    if (!g_haveTick || !g_accept || g_closing) return;
    std::string text;
#define ADD_FIELD(member, label) text += label; text += Field(g_latest.member, sizeof(g_latest.member)); text += "\r\n"
    ADD_FIELD(shcode, "shcode / short code: ");
    ADD_FIELD(chetime, "chetime / trade time: ");
    ADD_FIELD(price, "price / current price: ");
    ADD_FIELD(sign, "sign / change direction: ");
    ADD_FIELD(change, "change / price change: ");
    ADD_FIELD(drate, "drate / change rate: ");
    ADD_FIELD(cgubun, "cgubun / trade category: ");
    ADD_FIELD(cvolume, "cvolume / trade quantity: ");
    ADD_FIELD(volume, "volume / cumulative quantity: ");
    ADD_FIELD(value, "value / cumulative value: ");
    ADD_FIELD(cpower, "cpower / trade strength: ");
    ADD_FIELD(offerho, "offerho / offer: ");
    ADD_FIELD(bidho, "bidho / bid: ");
    ADD_FIELD(status, "status / market status: ");
#undef ADD_FIELD
    char count[80]; sprintf_s(count, sizeof(count), "Validated notifications: %lu\r\n", g_tickCount);
    text += count;
    SetWindowTextA(g_data, text.c_str());
}


// Reuse the existing query samples' controls, positions, fonts and login logic.
BOOL CALLBACK ClearPassword(HWND control, LPARAM)
{
    if ((GetWindowLong(control, GWL_STYLE) & ES_PASSWORD) != 0)
    {
        wchar_t type[32]; GetClassNameW(control, type, 32);
        if (wcscmp(type, L"Edit") == 0) SetWindowTextW(control, L"");
    }
    return TRUE;
}
void CreateRealtimeControls()
{
    // Same side-by-side layout as the five query samples: login stays at x=20,
    // request/output area begins at x=580. Create once, before any login.
    Common::AddControl(g_window, L"STATIC", L"실시간 통합체결 (S3T)", 0, 580, 20, 510, 22, 0);
    Common::AddControl(g_window, L"STATIC", L"종목코드", 0, 580, 62, 240, 22, 0);
    g_symbol = Common::AddControl(g_window, L"EDIT", L"", ES_AUTOHSCROLL | WS_TABSTOP, 830, 60, 140, 24, 0);
    SendMessage(g_symbol, EM_SETLIMITTEXT, 6, 0);
    g_requestButton = Common::AddControl(g_window, L"BUTTON", L"요청", WS_TABSTOP, 980, 60, 110, 30, ID_REQUEST);
    Common::AddControl(g_window, L"STATIC", L"현재 등록 종목", 0, 580, 102, 240, 22, 0);
    g_current = Common::AddControl(g_window, L"EDIT", L"(none)", ES_READONLY | ES_AUTOHSCROLL, 830, 100, 260, 24, 0);
    Common::AddControl(g_window, L"STATIC", L"실시간 수신 데이터", 0, 580, 142, 510, 22, 0);
    DWORD style = ES_READONLY | ES_MULTILINE | ES_AUTOVSCROLL | WS_VSCROLL;
    g_data = Common::AddControl(g_window, L"EDIT", L"", style, 580, 170, 510, 280, 0);
    Common::AddControl(g_window, L"STATIC", L"처리 / 오류 로그", 0, 580, 472, 510, 22, 0);
    g_log = Common::AddControl(g_window, L"EDIT", L"", style, 580, 500, 510, 180, 0);
    SendMessage(g_log, EM_SETLIMITTEXT, 32000, 0);
    g_status = Common::AddControl(g_window, L"STATIC", L"로그인 후 종목 요청이 가능합니다.", 0, 580, 700, 510, 40, 0);
    Log("Login and realtime controls share this main window.");
    UpdateControls();
}
BOOL StartWorker(HWND window)
{
    g_worker.window = window;
    g_worker.ready = CreateEvent(NULL, TRUE, FALSE, NULL);
    g_worker.done = CreateEvent(NULL, TRUE, FALSE, NULL);
    if (g_worker.ready && g_worker.done)
        g_thread = CreateThread(NULL, 0, ApiWorker, &g_worker, 0, &g_threadId);
    if (!g_thread)
    {
        if (g_worker.ready) CloseHandle(g_worker.ready);
        if (g_worker.done) CloseHandle(g_worker.done);
        g_worker.ready = g_worker.done = NULL;
        Common::SetResult(L"실시간 처리 스레드를 생성하지 못했습니다."); return FALSE;
    }
    WaitForSingleObject(g_worker.ready, INFINITE); // signals before any API call
    return TRUE;
}
void FinishWindow()
{
    if (g_restart)
    {
        // Cleanup and worker termination have completed. Reuse the main window
        // with fresh DLL/session state; no new process or credential transfer.
        if (!Common::ResetAfterCleanup(g_window))
        {
            MessageBoxW(g_window, L"로그인 상태를 초기화하지 못했습니다.", L"다시 로그인", MB_OK | MB_ICONERROR);
            DestroyWindow(g_window); return;
        }
        g_restart = FALSE; g_closing = FALSE; g_cleanupComplete = FALSE;
        g_sessionLost = FALSE; g_ready = FALSE; g_uncertain = FALSE;
        g_accept = FALSE; g_haveTick = FALSE; g_tickCount = 0;
        SetWindowTextA(g_data, "");
        SetWindowTextW(g_status, L"로그인 정보를 확인하고 다시 로그인하세요.");
        Log("Session reset completed in the same main window.");
        StartWorker(g_window);
        Common::BeginCall(); Common::Initialize(); Common::EndCall(g_window);
        UpdateControls();
        return;
    }
    DestroyWindow(g_window);
}
LRESULT CALLBACK WindowProc(HWND window, UINT message, WPARAM wParam, LPARAM lParam)
{
    if (message == API_BASE + WM_RECEIVE_REAL_DATA) { ReceiveRealData(lParam); return 0; }
    // During asynchronous cleanup the helper already has BeginClose applied.
    // Do not access its API/state until the worker publishes completion.
    if (!g_closing && Common::HandleMessage(window, message, wParam, lParam))
    {
        if (message == WM_COMMAND && (LOWORD(wParam) == 101 || LOWORD(wParam) == 102))
            EnumChildWindows(window, ClearPassword, 0);
        if (message == API_BASE + WM_DISCONNECT || message == API_BASE + WM_LOGOUT)
        {
            g_sessionLost = TRUE; g_ready = FALSE; g_accept = FALSE; g_haveTick = FALSE;
            Log("Session ended; incoming realtime data is now ignored.");
            SetWindowTextW(g_status, L"세션이 종료되었습니다. Sample을 다시 실행하세요.");
        }
        if (Common::IsReady() && !g_sessionLost && !g_ready)
        {
            Log("Login completed through the common helper.");
            SetWindowTextW(g_status, L"로그인 성공 — 종목코드를 입력하고 요청하세요.");
        }
        UpdateControls(); return 0;
    }
    switch (message)
    {
    case WM_CREATE:
        g_window = window;
        Common::CreateLoginControls(window);
        // Use helper behavior unchanged, but never leave its preset password in UI.
        EnumChildWindows(window, ClearPassword, 0);
        g_retry = Common::AddControl(window, L"BUTTON", L"다시 로그인", WS_TABSTOP, 420, 490, 110, 30, ID_RETRY);
        CreateRealtimeControls();
        if (!SetTimer(window, 1, 100, NULL)) return -1;
        StartWorker(window); return 0;
    case WORK_DONE: CompleteJob(); return 0;
    case WM_TIMER:
        CompleteJob();
        UpdateControls(); return 0;
    case WM_COMMAND:
        if (HIWORD(wParam) == BN_CLICKED && !g_closing)
        {
            if (LOWORD(wParam) == ID_REQUEST) Advise();
            if (LOWORD(wParam) == ID_RETRY && !Common::IsBusy())
            { g_restart = TRUE; PostMessage(window, WM_CLOSE, 0, 0); }
        }
        return 0;
    case WM_CLOSE:
        if (g_closing) { g_restart = FALSE; return 0; }
        if (!Common::BeginClose()) return 0; // helper defers close during login
        g_closing = TRUE; g_ready = FALSE; g_accept = FALSE; g_haveTick = FALSE;
        EnumChildWindows(window, ClearPassword, 0);
        SetWindowTextW(g_status, L"종료 중 — 등록 해제 및 세션 정리를 기다립니다");
        UpdateControls(); QueueClose(); return 0;
    case WM_DESTROY:
        KillTimer(window, 1); PostQuitMessage(0); return 0;
    }
    return DefWindowProcW(window, message, wParam, lParam);
}
int WINAPI WinMain(HINSTANCE instance, HINSTANCE, LPSTR, int show)
{
    WNDCLASSW cls = {0}; cls.lpfnWndProc = WindowProc; cls.hInstance = instance;
    cls.lpszClassName = L"OrdaAPI.S3T.Sample";
    cls.hCursor = LoadCursor(NULL, IDC_ARROW); cls.hbrBackground = (HBRUSH)(COLOR_BTNFACE+1);
    if (!RegisterClassW(&cls)) return 1;
    HWND window = CreateWindowW(cls.lpszClassName, L"OrdaAPI 실시간 통합체결 예제 (S3T)",
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
        CW_USEDEFAULT, CW_USEDEFAULT, 1120, 800, NULL, NULL, instance, NULL);
    if (!window) return 1;
    ShowWindow(window, show); UpdateWindow(window);
    Common::BeginCall(); Common::Initialize(); Common::EndCall(window);
    UpdateControls();
    MSG message; int result;
    while ((result = GetMessageW(&message, NULL, 0, 0)) > 0)
    {
        if (!IsDialogMessageW(window, &message)) { TranslateMessage(&message); DispatchMessageW(&message); }
    }
    if (result == -1 && IsWindow(window)) SendMessage(window, WM_CLOSE, 0, 0);
    return result == -1 ? 1 : (int)message.wParam;
}
