// 오프라인 전용 DLL. 네트워크·인증서·제품 Runtime을 사용하지 않습니다.
#include <windows.h>
#include <ordalib.h>
#include <b8509.h>
#include <string>
#include <vector>

static HWND notificationWindow;
static UINT messageBase;
static int connected, loggedIn, loginFailure;
static int responseMode = 1;
static int requestCount, requestReleases, messageReleases, logoutCount, disconnectCount;
static std::vector<unsigned char> capturedRequest;
static std::string capturedService;

#define EXPORT extern "C" __declspec(dllexport)

EXPORT int OrdaAPI_Initialize() { return 1; }
EXPORT int OrdaAPI_Connect(HWND window, LPCTSTR, int, UINT base)
{
    notificationWindow = window;
    messageBase = base;
    connected = 1;
    return 1;
}
EXPORT int OrdaAPI_IsConnected() { return connected; }
EXPORT int OrdaAPI_IsLogin() { return loggedIn; }
EXPORT int OrdaAPI_Login(LPCTSTR, LPCTSTR, LPCTSTR)
{
    loggedIn = loginFailure ? 0 : 1;
    SendMessage(notificationWindow, messageBase + WM_LOGIN,
                (WPARAM)(loggedIn ? "0000" : "9999"),
                (LPARAM)(loggedIn ? "offline login" : "offline failure"));
    return 1;
}
EXPORT void OrdaAPI_Logout() { ++logoutCount; loggedIn = 0; }
EXPORT int OrdaAPI_Disconnect() { ++disconnectCount; connected = 0; return 1; }
EXPORT void OrdaAPI_ReleaseRequestData(int) { ++requestReleases; }
EXPORT void OrdaAPI_ReleaseMessageData(LPARAM parameter)
{
    MSG_PACKET *packet = (MSG_PACKET *)parameter;
    ++messageReleases;
    // 제품의 시스템 오류 시 메시지 해제가 요청도 해제하는 동작을 재현합니다.
    if (packet->nIsSystemError == 1)
        OrdaAPI_ReleaseRequestData(packet->nRqID);
    delete[] packet->lpszMessageData;
    delete packet;
}
EXPORT void TestSetup(int mode) { responseMode = mode; }
EXPORT void TestLoginFailure(int fail) { loginFailure = fail; }
EXPORT int TestMetric(int metric)
{
    switch (metric)
    {
    case 0: return requestCount;
    case 1: return requestReleases;
    case 2: return messageReleases;
    case 3: return logoutCount;
    case 4: return disconnectCount;
    }
    return -1;
}
EXPORT int TestCopy(void *buffer, int size)
{
    if (size != (int)capturedRequest.size())
        return 0;
    memcpy(buffer, &capturedRequest[0], size);
    return size;
}
EXPORT const char *TestCode() { return capturedService.c_str(); }

static void SendServerMessage(HWND window, BOOL systemError)
{
    MSG_PACKET *packet = new MSG_PACKET;
    memset(packet, 0, sizeof(*packet));
    packet->nRqID = 0;
    packet->nIsSystemError = systemError ? 1 : 0;
    memcpy(packet->szCode, systemError ? "E0001" : "00000", 5);
    const char *text = "offline message secret";
    packet->nMsgLength = (int)strlen(text);
    packet->lpszMessageData = new unsigned char[packet->nMsgLength];
    memcpy(packet->lpszMessageData, text, packet->nMsgLength);
    SendMessage(window, messageBase + WM_RECEIVE_DATA,
                systemError ? SYSTEM_ERROR_DATA : MESSAGE_DATA, (LPARAM)packet);
}

EXPORT int OrdaAPI_RequestData(HWND window, LPCTSTR service, LPVOID data, int size, LPCTSTR next)
{
    ++requestCount;
    if (next || size != sizeof(b8509InBlock))
        return -900;
    capturedService = service;
    capturedRequest.assign((unsigned char *)data, (unsigned char *)data + size);
    // API 반환 전에 재진입한 주문 클릭도 중복 전송되면 안 됩니다.
    SendMessage(window, WM_COMMAND, MAKEWPARAM(300, BN_CLICKED), 0);
    if (responseMode == 5)
        return -5;
    if (responseMode == 4)
        return 0; // 활성 요청을 유지해 대기 중 클릭·종료를 검증합니다.
    if (responseMode == 2)
    {
        SendMessage(window, messageBase + WM_TIMEOUT_DATA, 0, 0);
        return 0;
    }
    if (responseMode == 1 || responseMode == 3 || responseMode == 9)
    {
        SendServerMessage(window, responseMode == 3);
        if (responseMode == 3)
            return 0;
    }
    if (responseMode != 8 && responseMode != 9)
    {
        std::vector<unsigned char> body(sizeof(b8509OutBlock) + sizeof(b8509OutBlock1), ' ');
        memcpy(&body[0], data, size);
        b8509OutBlock1 order;
        memset(&order, ' ', sizeof(order));
        memcpy(order.reccnt, "00001", 5);
        memcpy(order.dummyordno, "314", 3);
        memcpy(order.ordno, "1234567", 7);
        memcpy(order.isunm, "Offline futures", 15);
        memcpy(&body[sizeof(b8509OutBlock)], &order, sizeof(order));
        RP_DATA_INFO response;
        response.nRqID = responseMode == 10 ? 1 : 0;
        response.nDataLength = (int)body.size() - (responseMode == 6 ? 1 : 0);
        response.lpData = &body[0];
        memcpy(response.Header.szTrCode, responseMode == 7 ? "wrong" : service, 5);
        if (responseMode == 11)
            memcpy(response.szBlockName, "wrong", 5);
        if (responseMode == 12)
            response.Header.szRetFlag[0] = '1';
        memcpy(response.Header.szMsgCode, "00000", 5);
        memcpy(response.Header.szMsgTxt, "header secret", 13);
        SendMessage(window, messageBase + WM_RECEIVE_DATA, REQUEST_DATA, (LPARAM)&response);
        // 빌린 버퍼의 유효 기간 이후에 접근하는 오류를 검출합니다.
        memset(&body[0], 'X', body.size());
    }
    SendMessage(window, messageBase + WM_RECEIVE_DATA, RELEASE_DATA, 0);
    return 0;
}
