#pragma once

///////////////////////////////////////////////////////////////////////////////////////////////////
// 해외챠트(일/주/월) ( attr,block,headtype=A ) 
#pragma pack( push, 1 )

#define NAME_s4112     "s4112"

// 입력
typedef struct _s4112InBlock
{
    char    kind                [   1];    char    _kind                ;    // [string,    1] 종목종류(S:업종F:선물R:환율)   StartPos 0, Length 1
    char    symbol              [  16];    char    _symbol              ;    // [string,   16] SYMBOL                         StartPos 2, Length 16
    char    exid                [   4];    char    _exid                ;    // [string,    4] EXID(시장코드)                 StartPos 19, Length 4
    char    tgubun              [   1];    char    _tgubun              ;    // [string,    1] 주기구분(2:일3:주4:월)         StartPos 24, Length 1
    char    qrycnt              [   4];    char    _qrycnt              ;    // [string,    4] 요청건수(최대-압축:2000비압축:500) StartPos 26, Length 4
    char    sdate               [   8];    char    _sdate               ;    // [string,    8] 시작일자                       StartPos 31, Length 8
    char    edate               [   8];    char    _edate               ;    // [string,    8] 종료일자                       StartPos 40, Length 8
    char    cts                 [   8];    char    _cts                 ;    // [string,    8] 연속일자                       StartPos 49, Length 8
    char    comp_yn             [   1];    char    _comp_yn             ;    // [string,    1] 압축여부(Y:압축N:비압축)       StartPos 58, Length 1
} s4112InBlock, *LPs4112InBlock;
#define NAME_s4112InBlock     "s4112InBlock"

// 출력
typedef struct _s4112OutBlock
{
    char    kind                [   1];    char    _kind                ;    // [string,    1] 종목종류                       StartPos 0, Length 1
    char    symbol              [  16];    char    _symbol              ;    // [string,   16] SYMBOL                         StartPos 2, Length 16
    char    exid                [   4];    char    _exid                ;    // [string,    4] EXID                           StartPos 19, Length 4
    char    jopen               [  10];    char    _jopen               ;    // [string,   10] 전일시가                       StartPos 24, Length 10
    char    jhigh               [  10];    char    _jhigh               ;    // [string,   10] 전일고가                       StartPos 35, Length 10
    char    jlow                [  10];    char    _jlow                ;    // [string,   10] 전일저가                       StartPos 46, Length 10
    char    jclose              [  10];    char    _jclose              ;    // [string,   10] 전일종가                       StartPos 57, Length 10
    char    open                [  10];    char    _open                ;    // [string,   10] 당일시가                       StartPos 68, Length 10
    char    high                [  10];    char    _high                ;    // [string,   10] 당일고가                       StartPos 79, Length 10
    char    low                 [  10];    char    _low                 ;    // [string,   10] 당일저가                       StartPos 90, Length 10
    char    close               [  10];    char    _close               ;    // [string,   10] 당일종가                       StartPos 101, Length 10
    char    uplmtprice          [  10];    char    _uplmtprice          ;    // [string,   10] 상한가                         StartPos 112, Length 10
    char    dnlmtprice          [  10];    char    _dnlmtprice          ;    // [string,   10] 하한가                         StartPos 123, Length 10
    char    cts                 [   8];    char    _cts                 ;    // [string,    8] 연속일자                       StartPos 134, Length 8
    char    rec_count           [   7];    char    _rec_count           ;    // [string,    7] 레코드카운트                   StartPos 143, Length 7
} s4112OutBlock, *LPs4112OutBlock;
#define NAME_s4112OutBlock     "s4112OutBlock"

typedef struct _s4112OutBlock1
{
    char    date                [   8];    char    _date                ;    // [string,    8] 날짜                           StartPos 0, Length 8
    char    open                [  10];    char    _open                ;    // [string,   10] 시가                           StartPos 9, Length 10
    char    high                [  10];    char    _high                ;    // [string,   10] 고가                           StartPos 20, Length 10
    char    low                 [  10];    char    _low                 ;    // [string,   10] 저가                           StartPos 31, Length 10
    char    close               [  10];    char    _close               ;    // [string,   10] 종가                           StartPos 42, Length 10
    char    volume              [  15];    char    _volume              ;    // [string,   15] 거래량                         StartPos 53, Length 15
} s4112OutBlock1, *LPs4112OutBlock1;
#define NAME_s4112OutBlock1     "s4112OutBlock1"


#pragma pack( pop )
///////////////////////////////////////////////////////////////////////////////////////////////////
