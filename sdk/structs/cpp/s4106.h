#pragma once

///////////////////////////////////////////////////////////////////////////////////////////////////
// 업종차트(종합) ( attr,block,headtype=A ) 
#pragma pack( push, 1 )

#define NAME_s4106     "s4106"

// 입력
typedef struct _s4106InBlock
{
    char    shcode              [   3];    char    _shcode              ;    // [string,    3] 단축코드                       StartPos 0, Length 3
    char    gubun               [   1];    char    _gubun               ;    // [string,    1] 주기구분(1:분)                 StartPos 4, Length 1
    char    ncnt                [   4];    char    _ncnt                ;    // [string,    4] 단위(n분)                      StartPos 6, Length 4
    char    qrycnt              [   4];    char    _qrycnt              ;    // [string,    4] 건수(2000건이하)               StartPos 11, Length 4
    char    tdgb                [   1];    char    _tdgb                ;    // [string,    1] 당일구분(0:전체1:당일만)       StartPos 16, Length 1
    char    sdate               [   8];    char    _sdate               ;    // [string,    8] 시작일자                       StartPos 18, Length 8
    char    edate               [   8];    char    _edate               ;    // [string,    8] 종료일자                       StartPos 27, Length 8
    char    cts_date            [   8];    char    _cts_date            ;    // [string,    8] 연속일자                       StartPos 36, Length 8
    char    cts_time            [  10];    char    _cts_time            ;    // [string,   10] 연속시간                       StartPos 45, Length 10
    char    cts_daygb           [   1];    char    _cts_daygb           ;    // [string,    1] 연속당일구분(0:연속전체1:연속당일만2:연속전일만) StartPos 56, Length 1
} s4106InBlock, *LPs4106InBlock;
#define NAME_s4106InBlock     "s4106InBlock"

// 출력
typedef struct _s4106OutBlock
{
    char    shcode              [   3];    char    _shcode              ;    // [string,    3] 단축코드                       StartPos 0, Length 3
    char    jisiga              [   7];    char    _jisiga              ;    // [string,    7] 전일시가                       StartPos 4, Length 7
    char    jihigh              [   7];    char    _jihigh              ;    // [string,    7] 전일고가                       StartPos 12, Length 7
    char    jilow               [   7];    char    _jilow               ;    // [string,    7] 전일저가                       StartPos 20, Length 7
    char    jiclose             [   7];    char    _jiclose             ;    // [string,    7] 전일종가                       StartPos 28, Length 7
    char    jivolume            [  12];    char    _jivolume            ;    // [string,   12] 전일거래량                     StartPos 36, Length 12
    char    disiga              [   7];    char    _disiga              ;    // [string,    7] 당일시가                       StartPos 49, Length 7
    char    dihigh              [   7];    char    _dihigh              ;    // [string,    7] 당일고가                       StartPos 57, Length 7
    char    dilow               [   7];    char    _dilow               ;    // [string,    7] 당일저가                       StartPos 65, Length 7
    char    diclose             [   7];    char    _diclose             ;    // [string,    7] 당일종가                       StartPos 73, Length 7
    char    disvalue            [  12];    char    _disvalue            ;    // [string,   12] 당일거래대금                   StartPos 81, Length 12
    char    cts_date            [   8];    char    _cts_date            ;    // [string,    8] 연속일자                       StartPos 94, Length 8
    char    cts_time            [  10];    char    _cts_time            ;    // [string,   10] 연속시간                       StartPos 103, Length 10
    char    cts_daygb           [   1];    char    _cts_daygb           ;    // [string,    1] 연속당일구분                   StartPos 114, Length 1
    char    s_time              [   6];    char    _s_time              ;    // [string,    6] 업종시작시간                   StartPos 116, Length 6
    char    e_time              [   6];    char    _e_time              ;    // [string,    6] 업종종료시간                   StartPos 123, Length 6
    char    rec_count           [   7];    char    _rec_count           ;    // [string,    7] 레코드카운트                   StartPos 130, Length 7
} s4106OutBlock, *LPs4106OutBlock;
#define NAME_s4106OutBlock     "s4106OutBlock"

typedef struct _s4106OutBlock1
{
    char    date                [   8];    char    _date                ;    // [string,    8] 날짜                           StartPos 0, Length 8
    char    time                [   6];    char    _time                ;    // [string,    6] 시간                           StartPos 9, Length 6
    char    open                [   7];    char    _open                ;    // [string,    7] 시가                           StartPos 16, Length 7
    char    high                [   7];    char    _high                ;    // [string,    7] 고가                           StartPos 24, Length 7
    char    low                 [   7];    char    _low                 ;    // [string,    7] 저가                           StartPos 32, Length 7
    char    close               [   7];    char    _close               ;    // [string,    7] 종가                           StartPos 40, Length 7
    char    jdiff_vol           [  12];    char    _jdiff_vol           ;    // [string,   12] 거래량                         StartPos 48, Length 12
    char    value               [  12];    char    _value               ;    // [string,   12] 거래대금                       StartPos 61, Length 12
} s4106OutBlock1, *LPs4106OutBlock1;
#define NAME_s4106OutBlock1     "s4106OutBlock1"


#pragma pack( pop )
///////////////////////////////////////////////////////////////////////////////////////////////////
