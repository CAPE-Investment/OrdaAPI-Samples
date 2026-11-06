// VS2010 v100 / Win32 / MultiByte. All Query logic stays in this file.
#include "../../../common/OrdaSampleCommon.h"
#include "b8024.h"
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <wchar.h>
#include <string>
#include <deque>

namespace Common = OrdaSampleCommon;

// Keep the supplied packed layout identical to the existing XML request.
static_assert(sizeof(b8024InBlock) == 41, "Request length differs from XML");
static_assert(__alignof(b8024InBlock) == 1, "Request alignment differs from XML");
static_assert(offsetof(b8024InBlock, reccnt) == 0 && sizeof(((b8024InBlock*)0)->reccnt) == 5, "reccnt layout differs from XML");
static_assert(offsetof(b8024InBlock, lmtclasscd) == 5 && sizeof(((b8024InBlock*)0)->lmtclasscd) == 2, "lmtclasscd layout differs from XML");
static_assert(offsetof(b8024InBlock, expcode) == 7 && sizeof(((b8024InBlock*)0)->expcode) == 12, "expcode layout differs from XML");
static_assert(offsetof(b8024InBlock, AcntNo) == 19 && sizeof(((b8024InBlock*)0)->AcntNo) == 20, "AcntNo layout differs from XML");
static_assert(offsetof(b8024InBlock, jolmtgb) == 39 && sizeof(((b8024InBlock*)0)->jolmtgb) == 1, "jolmtgb layout differs from XML");
static_assert(offsetof(b8024InBlock, lmtvaluegb) == 40 && sizeof(((b8024InBlock*)0)->lmtvaluegb) == 1, "lmtvaluegb layout differs from XML");

// C++03-compatible checks for the supplied XML output layouts.
typedef char Check_b8024OutBlock_reccnt[(offsetof(b8024OutBlock, reccnt) == 0 && sizeof(((b8024OutBlock*)0)->reccnt) == 5) ? 1 : -1];
typedef char Check_b8024OutBlock_lmtclasscd[(offsetof(b8024OutBlock, lmtclasscd) == 5 && sizeof(((b8024OutBlock*)0)->lmtclasscd) == 2) ? 1 : -1];
typedef char Check_b8024OutBlock_expcode[(offsetof(b8024OutBlock, expcode) == 7 && sizeof(((b8024OutBlock*)0)->expcode) == 12) ? 1 : -1];
typedef char Check_b8024OutBlock_AcntNo[(offsetof(b8024OutBlock, AcntNo) == 19 && sizeof(((b8024OutBlock*)0)->AcntNo) == 20) ? 1 : -1];
typedef char Check_b8024OutBlock_jolmtgb[(offsetof(b8024OutBlock, jolmtgb) == 39 && sizeof(((b8024OutBlock*)0)->jolmtgb) == 1) ? 1 : -1];
typedef char Check_b8024OutBlock_lmtvaluegb[(offsetof(b8024OutBlock, lmtvaluegb) == 40 && sizeof(((b8024OutBlock*)0)->lmtvaluegb) == 1) ? 1 : -1];
typedef char Check_b8024OutBlock_size[(sizeof(b8024OutBlock) == 41 && __alignof(b8024OutBlock) == 1) ? 1 : -1];
typedef char Check_b8024OutBlock1_lmtclassnm[(offsetof(b8024OutBlock1, lmtclassnm) == 0 && sizeof(((b8024OutBlock1*)0)->lmtclassnm) == 40) ? 1 : -1];
typedef char Check_b8024OutBlock1_hname[(offsetof(b8024OutBlock1, hname) == 40 && sizeof(((b8024OutBlock1*)0)->hname) == 40) ? 1 : -1];
typedef char Check_b8024OutBlock1_lmtgbnm[(offsetof(b8024OutBlock1, lmtgbnm) == 80 && sizeof(((b8024OutBlock1*)0)->lmtgbnm) == 40) ? 1 : -1];
typedef char Check_b8024OutBlock1_lmtvalgbnm[(offsetof(b8024OutBlock1, lmtvalgbnm) == 120 && sizeof(((b8024OutBlock1*)0)->lmtvalgbnm) == 40) ? 1 : -1];
typedef char Check_b8024OutBlock1_lmtval[(offsetof(b8024OutBlock1, lmtval) == 160 && sizeof(((b8024OutBlock1*)0)->lmtval) == 18) ? 1 : -1];
typedef char Check_b8024OutBlock1_loanjanval[(offsetof(b8024OutBlock1, loanjanval) == 178 && sizeof(((b8024OutBlock1*)0)->loanjanval) == 18) ? 1 : -1];
typedef char Check_b8024OutBlock1_newordval[(offsetof(b8024OutBlock1, newordval) == 196 && sizeof(((b8024OutBlock1*)0)->newordval) == 18) ? 1 : -1];
typedef char Check_b8024OutBlock1_newcheval[(offsetof(b8024OutBlock1, newcheval) == 214 && sizeof(((b8024OutBlock1*)0)->newcheval) == 18) ? 1 : -1];
typedef char Check_b8024OutBlock1_sincheval[(offsetof(b8024OutBlock1, sincheval) == 232 && sizeof(((b8024OutBlock1*)0)->sincheval) == 18) ? 1 : -1];
typedef char Check_b8024OutBlock1_loandamt[(offsetof(b8024OutBlock1, loandamt) == 250 && sizeof(((b8024OutBlock1*)0)->loandamt) == 16) ? 1 : -1];
typedef char Check_b8024OutBlock1_useablelmtamt[(offsetof(b8024OutBlock1, useablelmtamt) == 266 && sizeof(((b8024OutBlock1*)0)->useablelmtamt) == 16) ? 1 : -1];
typedef char Check_b8024OutBlock1_lmtclasscode[(offsetof(b8024OutBlock1, lmtclasscode) == 282 && sizeof(((b8024OutBlock1*)0)->lmtclasscode) == 2) ? 1 : -1];
typedef char Check_b8024OutBlock1_expcode[(offsetof(b8024OutBlock1, expcode) == 284 && sizeof(((b8024OutBlock1*)0)->expcode) == 12) ? 1 : -1];
typedef char Check_b8024OutBlock1_jolmtclass[(offsetof(b8024OutBlock1, jolmtclass) == 296 && sizeof(((b8024OutBlock1*)0)->jolmtclass) == 1) ? 1 : -1];
typedef char Check_b8024OutBlock1_lmtvalgb[(offsetof(b8024OutBlock1, lmtvalgb) == 297 && sizeof(((b8024OutBlock1*)0)->lmtvalgb) == 1) ? 1 : -1];
typedef char Check_b8024OutBlock1_jogrplmtclass[(offsetof(b8024OutBlock1, jogrplmtclass) == 298 && sizeof(((b8024OutBlock1*)0)->jogrplmtclass) == 1) ? 1 : -1];
typedef char Check_b8024OutBlock1_gbnm1[(offsetof(b8024OutBlock1, gbnm1) == 299 && sizeof(((b8024OutBlock1*)0)->gbnm1) == 40) ? 1 : -1];
typedef char Check_b8024OutBlock1_jogrp[(offsetof(b8024OutBlock1, jogrp) == 339 && sizeof(((b8024OutBlock1*)0)->jogrp) == 2) ? 1 : -1];
typedef char Check_b8024OutBlock1_jogrpnm[(offsetof(b8024OutBlock1, jogrpnm) == 341 && sizeof(((b8024OutBlock1*)0)->jogrpnm) == 40) ? 1 : -1];
typedef char Check_b8024OutBlock1_zLoanInfoGrdCode[(offsetof(b8024OutBlock1, zLoanInfoGrdCode) == 381 && sizeof(((b8024OutBlock1*)0)->zLoanInfoGrdCode) == 2) ? 1 : -1];
typedef char Check_b8024OutBlock1_lLstdQty[(offsetof(b8024OutBlock1, lLstdQty) == 383 && sizeof(((b8024OutBlock1*)0)->lLstdQty) == 16) ? 1 : -1];
typedef char Check_b8024OutBlock1_size[(sizeof(b8024OutBlock1) == 399 && __alignof(b8024OutBlock1) == 1) ? 1 : -1];

const char SERVICE_CODE[] = "b8024";
const wchar_t WINDOW_TITLE[] = L"OrdaAPI 종목별 신용한도 조회 (b8024 / B)";
const int INPUT_SIZE = 41;
const BOOL ALLOW_CONTINUATION = TRUE;
const char CONTINUATION_FLAG = 'Y'; // B: BASE21_DATA_HEADER.szCont, not A's '1'.
const int CURSOR_INDEX = -1; // B uses Header.szContKey; no XML cursor in this service.
struct InputField
{
    const wchar_t* label;
    int offset, width;
    const char* defaultValue;
    BOOL password;
};
// b8024InBlock: no attr bytes. No invented code-list defaults.
const InputField INPUT_FIELDS[] = {
    {L"레코드갯수 (reccnt)", 0, 5, "", FALSE},
    {L"한도분류코드 (lmtclasscd)", 5, 2, "", FALSE},
    {L"종목번호 (expcode)", 7, 12, "", FALSE},
    {L"계좌번호 (AcntNo)", 19, 20, "", FALSE},
    {L"종목한도구분 (jolmtgb)", 39, 1, "", FALSE},
    {L"한도값구분 (lmtvaluegb)", 40, 1, "", FALSE}
};
HWND g_inputs[_countof(INPUT_FIELDS)], g_queryButton, g_nextButton, g_output;
int g_requestId = -1;
BOOL g_closing = FALSE, g_submitting = FALSE, g_received = FALSE, g_error = FALSE;
BOOL g_hasNext = FALSE, g_baseValid = FALSE;
char g_nextKey[19] = {0}; // Own copy; preserve fixed-width spaces in the 18-byte key.
char g_cursor[13] = {0};
char g_baseInput[INPUT_SIZE] = {0}; // Password bytes are replaced with spaces.
struct Notification { UINT message; WPARAM wParam; LPARAM lParam; };
std::deque<Notification> g_deferred;

std::string Field(const void* data, int length, int offset, int width)
{
    if (!data || offset < 0 || width < 0 || length < 0 || offset > length || width > length-offset)
        return std::string();
    std::string value((const char*)data+offset, width);
    while (!value.empty() && (value[value.size()-1]==' ' || value[value.size()-1]==0))
        value.erase(value.size()-1);
    size_t first=value.find_first_not_of(' ');
    return first==std::string::npos ? std::string() : value.substr(first);
}

// Non-block occurs prefix: exactly five ASCII columns, then N fixed-width records.
// Leading/trailing spaces are accepted, but signs, missing digits and junk are not.
BOOL RecordCount(const unsigned char* data, int length, int position, int width, int& count)
{
    if (!data || position<0 || position>length || length-position<5 || width<=0)
        return FALSE;
    count=0;
    BOOL digit=FALSE, ended=FALSE;
    for (int i=0; i<5; ++i)
    {
        unsigned char c=data[position+i];
        if (c==' ')
        {
            if (digit) ended=TRUE;
        }
        else if (c>='0' && c<='9' && !ended)
        {
            digit=TRUE;
            count=count*10+c-'0';
        }
        else return FALSE;
    }
    return digit && count <= (length-position-5)/width;
}

std::string Ansi(const wchar_t* value)
{
    int length=WideCharToMultiByte(CP_ACP,0,value,-1,NULL,0,NULL,NULL);
    if (length<=0) return std::string();
    std::string result(length,0);
    WideCharToMultiByte(CP_ACP,0,value,-1,&result[0],length,NULL,NULL);
    result.erase(result.size()-1);
    return result;
}
std::string Rows(const unsigned char* records, int count)
{
    wchar_t heading[128];
    swprintf_s(heading, _countof(heading), L"레코드 수: %d (최대 200행 표시)\r\n", count);
    std::string text=Ansi(heading);
    int shown=count<200 ? count : 200;
    for (int row=0; row<shown; ++row)
    {
        char number[32]; sprintf_s(number, "[%d] ", row+1); text+=number;
        // ParseService validated the complete fixed/record boundary.
        b8024OutBlock1 output;
        memcpy(&output,records+row*sizeof(output),sizeof(output));
        text+="lmtclassnm=";
        text+=Field(output.lmtclassnm,sizeof(output.lmtclassnm),0,sizeof(output.lmtclassnm));
        text+=" | hname=";
        text+=Field(output.hname,sizeof(output.hname),0,sizeof(output.hname));
        text+=" | lmtgbnm=";
        text+=Field(output.lmtgbnm,sizeof(output.lmtgbnm),0,sizeof(output.lmtgbnm));
        text+=" | lmtval=";
        text+=Field(output.lmtval,sizeof(output.lmtval),0,sizeof(output.lmtval));
        text+=" | loanjanval=";
        text+=Field(output.loanjanval,sizeof(output.loanjanval),0,sizeof(output.loanjanval));
        text+=" | useablelmtamt=";
        text+=Field(output.useablelmtamt,sizeof(output.useablelmtamt),0,sizeof(output.useablelmtamt));
        text+="\r\n";
    }
    return text;
}

// ------------------------------------------------------------
// B: [b8024OutBlock:41] [ASCII count:5] [b8024OutBlock1:399 * N].
// Echo block's reccnt is NOT used as the occurs-prefix record count.
// No wire Header is added here: OrdaAPI constructs it from server TRINFO.
// ------------------------------------------------------------
BOOL ParseService(const RP_DATA_INFO* response, std::string& text)
{
    const unsigned char* data=response->lpData;
    const int fixedSize=sizeof(b8024OutBlock);
    const int recordSize=sizeof(b8024OutBlock1);
    const int recordsOffset=fixedSize+5; // Non-block ASCII count prefix.
    int count=0;
    if (!RecordCount(data,response->nDataLength,fixedSize,recordSize,count) ||
        response->nDataLength != recordsOffset+count*recordSize)
        return FALSE;
    // The fixed echo block is not displayed; its reccnt is not the prefix count.
    text="b8024OutBlock1\r\n"+Rows(data+recordsOffset,count);
    return TRUE;
}

void UpdateButtons()
{
    BOOL ready=Common::IsReady() && !Common::IsBusy() && !g_closing &&
               !g_submitting && g_requestId<0;
    EnableWindow(g_queryButton,ready);
    if (g_nextButton) EnableWindow(g_nextButton,ready && g_hasNext && !g_error);
}

BOOL BuildInput(b8024InBlock& input)
{
    // Map UI fields directly to the supplied service structure members.
    char* fields[] = {input.reccnt, input.lmtclasscd, input.expcode, input.AcntNo, input.jolmtgb, input.lmtvaluegb};
    for (int i=0; i<_countof(INPUT_FIELDS); ++i)
    {
        char value[128]={0};
        int length=GetWindowTextA(g_inputs[i],value,sizeof(value));
        if (length>INPUT_FIELDS[i].width)
        {
            SecureZeroMemory(value,sizeof(value));
            Common::SetResult(L"입력값의 바이트 길이가 XML 필드 길이를 초과했습니다.");
            return FALSE;
        }
        memcpy(fields[i],value,length);
        SecureZeroMemory(value,sizeof(value));
    }
    return TRUE;
}

void RemovePasswords(char* input)
{
    for (int i=0; i<_countof(INPUT_FIELDS); ++i)
        if (INPUT_FIELDS[i].password)
            memset(input+INPUT_FIELDS[i].offset,' ',INPUT_FIELDS[i].width);
}

void RequestService(HWND window, BOOL next)
{
    if (!Common::IsReady()) { Common::SetResult(L"로그인이 필요합니다."); return; }
    if (Common::IsBusy() || g_submitting || g_requestId>=0 || g_closing) return;
    if (next && (!ALLOW_CONTINUATION || !g_hasNext || g_error)) return;
    // XML char fields have no attr bytes; preserve space padding without NULs.
    b8024InBlock input;
    memset(&input,' ',sizeof(input));
    if (!BuildInput(input)) { SecureZeroMemory(&input,sizeof(input)); return; }
    char nonSecret[INPUT_SIZE]; memcpy(nonSecret,&input,sizeof(input)); RemovePasswords(nonSecret);
    // Cursor is part of the page state, not a user-changed search condition.
    if (CURSOR_INDEX>=0)
        memset(nonSecret+INPUT_FIELDS[CURSOR_INDEX].offset,' ',INPUT_FIELDS[CURSOR_INDEX].width);
    if (next && (!g_baseValid || memcmp(nonSecret,g_baseInput,INPUT_SIZE)))
    {
        SecureZeroMemory(&input,sizeof(input));
        Common::SetResult(L"조회 조건이 변경되었습니다. 처음 조회를 다시 실행하십시오.");
        return;
    }
    memcpy(g_baseInput,nonSecret,sizeof(nonSecret)); g_baseValid=TRUE;
    SecureZeroMemory(nonSecret,sizeof(nonSecret));
    char nextKey[19]={0};
    if (next) memcpy(nextKey,g_nextKey,sizeof(nextKey));
    g_hasNext=FALSE; g_received=FALSE; g_error=FALSE;
    if (!next) { memset(g_nextKey,0,sizeof(g_nextKey)); memset(g_cursor,0,sizeof(g_cursor)); }
    SetWindowTextA(g_output,"");
    Common::BeginCall();
    if (!Common::CheckSession())
    {
        SecureZeroMemory(&input,sizeof(input)); Common::EndCall(window);
        Common::SetResult(L"로그인이 필요합니다. 프로그램을 다시 실행하십시오.");
        UpdateButtons(); return;
    }
    g_submitting=TRUE; UpdateButtons();
    // Actual Public API remains visible. For B, pass only the received cont-key.
    int result=Common::Api()->RequestData(window,SERVICE_CODE,&input,sizeof(input),next ? nextKey : NULL);
    SecureZeroMemory(&input,sizeof(input)); SecureZeroMemory(nextKey,sizeof(nextKey));
    for (int i=0; i<_countof(INPUT_FIELDS); ++i)
        if (INPUT_FIELDS[i].password) SetWindowTextA(g_inputs[i],"");
    g_requestId=result>=0 && result<=255 ? result : -1;
    g_submitting=FALSE;
    if (g_requestId>=0) Common::SetResult(L"조회를 요청했습니다. 응답을 기다립니다.");
    else { g_error=TRUE; Common::ShowCallResult(L"조회 요청 실패",result); }
    Common::EndCall(window); UpdateButtons();
}

void ReceiveService(const RP_DATA_INFO* response)
{
    if (g_closing || !response || g_requestId<0 || response->nRqID!=g_requestId) return;
    if (Field(response->Header.szTrCode,sizeof(response->Header.szTrCode),0,6)!=SERVICE_CODE)
    { g_error=TRUE; Common::SetResult(L"다른 서비스 코드의 응답입니다."); return; }
    // XML has no block="1": B/C/D body contains all OUTBLOCKs in XML order.
    // Product nDataMode differs between single-/multi-packet branches; it alone
    // cannot determine this layout. Never cast the wire Header onto lpData.
    if (!Field(response->szBlockName,sizeof(response->szBlockName),0,17).empty())
    { g_error=TRUE; Common::SetResult(L"예상하지 않은 블록별 응답입니다."); return; }
    std::string text;
    if (!ParseService(response,text))
    { g_error=TRUE; Common::SetResult(L"응답 길이 또는 반복 건수가 XML 구조와 맞지 않습니다."); return; }
    SetWindowTextA(g_output,text.c_str()); g_received=TRUE;
    // Only enable a real, manually requested continuation when the public packet
    // actually supplied a key. Do not invent one if a product path omits it.
    memcpy(g_nextKey,response->Header.szContKey,18); g_nextKey[18]=0;
    g_hasNext=ALLOW_CONTINUATION && response->Header.szPNFlag[0]==CONTINUATION_FLAG &&
              !Field(g_nextKey,18,0,18).empty();
    if (CURSOR_INDEX>=0 && Field(g_cursor,12,0,12).empty()) g_hasNext=FALSE;
    if (CURSOR_INDEX>=0) SetWindowTextA(g_inputs[CURSOR_INDEX],Field(g_cursor,12,0,12).c_str());
    if (!g_error)
    {
        if (response->Header.szPNFlag[0]==CONTINUATION_FLAG && !g_hasNext)
            Common::SetResult(L"응답은 수신했으나 연속 조회에 필요한 키 또는 매핑이 없어 다음 조회를 활성화하지 않았습니다.");
        else
            Common::SetResult(L"조회 결과를 수신했습니다. 반복 결과는 최대 200행까지 표시합니다.");
    }
}

void HandleNotification(UINT message, WPARAM kind, LPARAM parameter)
{
    OrdaAPI* api=Common::Api(); if (!api) return;
    if (message==Common::API_MESSAGE_BASE+WM_TIMEOUT_DATA)
    {
        if (g_requestId>=0 && (int)parameter==g_requestId)
        {
            api->ReleaseRequestData(g_requestId); g_requestId=-1; g_error=TRUE; g_hasNext=FALSE;
            if (!g_closing) Common::SetResult(L"조회 응답 시간이 초과되었습니다.");
        }
    }
    else if (kind==REQUEST_DATA) ReceiveService((const RP_DATA_INFO*)parameter);
    else if (kind==MESSAGE_DATA || kind==SYSTEM_ERROR_DATA)
    {
        const MSG_PACKET* packet=(const MSG_PACKET*)parameter;
        if (packet)
        {
            if (!g_closing && g_requestId>=0 && packet->nRqID==g_requestId)
            {
                std::string code=Field(packet->szCode,sizeof(packet->szCode),0,5);
                int size=packet->nMsgLength;
                if (size>2047) size=2047;
                std::string text=size>0 && packet->lpszMessageData ?
                    std::string((const char*)packet->lpszMessageData,size) : std::string();
                Common::SetServerResult(code.c_str(),text.c_str());
                if (kind==SYSTEM_ERROR_DATA || packet->nIsSystemError)
                { g_error=TRUE; g_hasNext=FALSE; }
            }
            api->ReleaseMessageData(parameter);
        }
    }
    else if (kind==RELEASE_DATA && g_requestId>=0 && (int)parameter==g_requestId)
    {
        api->ReleaseRequestData(g_requestId); g_requestId=-1;
        if (!g_received && !g_error && !g_closing) Common::SetResult(L"조회가 종료되었으나 데이터가 없습니다.");
        if (g_error || !g_received) g_hasNext=FALSE;
    }
    UpdateButtons();
}

void DrainNotifications()
{
    while (!g_submitting && !g_deferred.empty())
    {
        Notification item=g_deferred.front(); g_deferred.pop_front();
        HandleNotification(item.message,item.wParam,item.lParam);
    }
}

LRESULT CALLBACK WindowProc(HWND window, UINT message, WPARAM wParam, LPARAM lParam)
{
    if (message==Common::API_MESSAGE_BASE+WM_RECEIVE_DATA || message==Common::API_MESSAGE_BASE+WM_TIMEOUT_DATA)
    {
        if (g_submitting) { Notification item={message,wParam,lParam}; g_deferred.push_back(item); }
        else HandleNotification(message,wParam,lParam);
        return 0;
    }
    if (Common::HandleMessage(window,message,wParam,lParam))
    { UpdateButtons(); return 0; }
    switch (message)
    {
    case WM_CREATE:
        Common::CreateLoginControls(window);
        for (int i=0; i<_countof(INPUT_FIELDS); ++i)
        {
            int y=25+i*30;
            Common::AddControl(window,L"STATIC",INPUT_FIELDS[i].label,0,580,y,245,22,0);
            g_inputs[i]=Common::AddControl(window,L"EDIT",L"",ES_AUTOHSCROLL | WS_TABSTOP |
                (INPUT_FIELDS[i].password ? ES_PASSWORD : 0) | (i==CURSOR_INDEX ? ES_READONLY : 0),
                830,y-2,260,24,0);
            SendMessage(g_inputs[i],EM_SETLIMITTEXT,INPUT_FIELDS[i].width,0);
            SetWindowTextA(g_inputs[i],INPUT_FIELDS[i].defaultValue);
        }
        {
            int y=30+_countof(INPUT_FIELDS)*30;
            g_queryButton=Common::AddControl(window,L"BUTTON",L"처음 조회",WS_TABSTOP,830,y,120,28,300);
            if (ALLOW_CONTINUATION)
                g_nextButton=Common::AddControl(window,L"BUTTON",L"다음 조회",WS_TABSTOP,970,y,120,28,301);
            Common::AddControl(window,L"STATIC",L"조회 결과 (전체 건수 / 최대 200행 표시)",0,580,y+38,510,22,0);
            g_output=Common::AddControl(window,L"EDIT",L"",ES_READONLY | ES_MULTILINE | ES_AUTOVSCROLL |
                ES_AUTOHSCROLL | WS_VSCROLL | WS_HSCROLL,580,y+62,510,300,0);
            SendMessage(g_output,EM_SETLIMITTEXT,1024*1024,0);
        }
        UpdateButtons(); return 0;
    case WM_COMMAND:
        if ((LOWORD(wParam)==300 || LOWORD(wParam)==301) && HIWORD(wParam)==BN_CLICKED)
        { RequestService(window,LOWORD(wParam)==301); DrainNotifications(); }
        return 0;
    case WM_CLOSE:
        if (!Common::BeginClose()) return 0;
        if (!g_closing)
        {
            g_closing=TRUE; UpdateButtons(); DrainNotifications();
            if (g_requestId>=0 && Common::Api())
            { Common::Api()->ReleaseRequestData(g_requestId); g_requestId=-1; }
            SecureZeroMemory(g_baseInput,sizeof(g_baseInput));
            for (int i=0; i<_countof(INPUT_FIELDS); ++i)
                if (INPUT_FIELDS[i].password) SetWindowTextA(g_inputs[i],"");
            Common::Cleanup(); DestroyWindow(window);
        }
        return 0;
    case WM_DESTROY: PostQuitMessage(0); return 0;
    }
    return DefWindowProcW(window,message,wParam,lParam);
}

int APIENTRY WinMain(HINSTANCE instance,HINSTANCE,LPSTR,int show)
{
    WNDCLASSW wc={0}; wc.lpfnWndProc=WindowProc; wc.hInstance=instance;
    wc.hCursor=LoadCursor(NULL,IDC_ARROW); wc.hbrBackground=(HBRUSH)(COLOR_BTNFACE+1);
    wc.lpszClassName=L"OrdaQueryHeaderSample";
    if (!RegisterClassW(&wc)) return 1;
    HWND window=CreateWindowW(wc.lpszClassName,WINDOW_TITLE,
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
        CW_USEDEFAULT,CW_USEDEFAULT,1140,650,NULL,NULL,instance,NULL);
    if (!window) return 1;
    ShowWindow(window,show); UpdateWindow(window);
    Common::BeginCall(); Common::Initialize(); Common::EndCall(window); UpdateButtons();
    MSG message; int status;
    while ((status=GetMessageW(&message,NULL,0,0))>0)
        if (!IsDialogMessageW(window,&message)) { TranslateMessage(&message); DispatchMessageW(&message); }
    if (status==-1 && IsWindow(window)) SendMessage(window,WM_CLOSE,0,0);
    return status==-1 ? 1 : (int)message.wParam;
}
