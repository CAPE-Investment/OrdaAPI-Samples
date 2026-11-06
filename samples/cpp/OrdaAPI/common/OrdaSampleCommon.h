#pragma once
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <ordalib.h>

// Source helper: compile OrdaSampleCommon.cpp in each sample (no extra DLL/lib).
// One window/session and one ID-login attempt per process, as in 01_connection/login.
namespace OrdaSampleCommon
{
    const UINT API_MESSAGE_BASE = WM_APP + 100;
    HWND AddControl(HWND parent, const wchar_t* type, const wchar_t* text,
                    DWORD style, int x, int y, int width, int height, int id);
    void CreateLoginControls(HWND window);
    BOOL Initialize();
    // Handles login controls/notifications only; query messages remain in main.cpp.
    BOOL HandleMessage(HWND window, UINT message, WPARAM wParam, LPARAM lParam);
    OrdaAPI* Api();
    BOOL IsReady();
    BOOL CheckSession();
    BOOL IsBusy();
    void BeginCall();
    void EndCall(HWND window);
    // Returns FALSE and defers WM_CLOSE if called from a nested API notification.
    BOOL BeginClose();
    void Cleanup();
    void SetResult(const wchar_t* message);
    void ShowCallResult(const wchar_t* stage, int result);
    void SetServerResult(const char* code, const char* message);
}
