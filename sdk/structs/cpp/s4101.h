#pragma once

///////////////////////////////////////////////////////////////////////////////////////////////////
// 선물/옵션챠트(종합) ( attr,block,headtype=A ) 
#pragma pack( push, 1 )

#define NAME_s4101     "s4101"

// 입력
typedef struct _s4101InBlock
{
    char    shcode              [   8];    char    _shcode              ;    // [string,    8] 단축코드                       StartPos 0, Length 8
    char    gubun               [   1];    char    _gubun               ;    // [string,    1] 주기구분                       StartPos 9, Length 1
    char    ncnt                [   4];    char    _ncnt                ;    // [string,    4] 틱개수                         StartPos 11, Length 4
    char    qrycnt              [   4];    char    _qrycnt              ;    // [string,    4] 건수                           StartPos 16, Length 4
    char    tdgb                [   1];    char    _tdgb                ;    // [string,    1] 당일구분                       StartPos 21, Length 1
    char    sdate               [   8];    char    _sdate               ;    // [string,    8] 시작일자                       StartPos 23, Length 8
    char    edate               [   8];    char    _edate               ;    // [string,    8] 종료일자                       StartPos 32, Length 8
    char    cts_date            [   8];    char    _cts_date            ;    // [string,    8] 연속일자                       StartPos 41, Length 8
    char    cts_time            [  10];    char    _cts_time            ;    // [string,   10] 연속시간                       StartPos 50, Length 10
    char    cts_daygb           [   1];    char    _cts_daygb           ;    // [string,    1] 연속당일구분                   StartPos 61, Length 1
} s4101InBlock, *LPs4101InBlock;
#define NAME_s4101InBlock     "s4101InBlock"

// 출력
typedef struct _s4101OutBlock
{
    char    shcode              [   8];    char    _shcode              ;    // [string,    8] 단축코드                       StartPos 0, Length 8
    char    jisiga              [   6];    char    _jisiga              ;    // [string,    6] 전일시가                       StartPos 9, Length 6
    char    jihigh              [   6];    char    _jihigh              ;    // [string,    6] 전일고가                       StartPos 16, Length 6
    char    jilow               [   6];    char    _jilow               ;    // [string,    6] 전일저가                       StartPos 23, Length 6
    char    jiclose             [   6];    char    _jiclose             ;    // [string,    6] 전일종가                       StartPos 30, Length 6
    char    jivolume            [  12];    char    _jivolume            ;    // [string,   12] 전일거래량                     StartPos 37, Length 12
    char    disiga              [   6];    char    _disiga              ;    // [string,    6] 당일시가                       StartPos 50, Length 6
    char    dihigh              [   6];    char    _dihigh              ;    // [string,    6] 당일고가                       StartPos 57, Length 6
    char    dilow               [   6];    char    _dilow               ;    // [string,    6] 당일저가                       StartPos 64, Length 6
    char    diclose             [   6];    char    _diclose             ;    // [string,    6] 당일종가                       StartPos 71, Length 6
    char    highend             [   6];    char    _highend             ;    // [string,    6] 상한가                         StartPos 78, Length 6
    char    lowend              [   6];    char    _lowend              ;    // [string,    6] 하한가                         StartPos 85, Length 6
    char    cts_date            [   8];    char    _cts_date            ;    // [string,    8] 연속일자                       StartPos 92, Length 8
    char    cts_time            [  10];    char    _cts_time            ;    // [string,   10] 연속시간                       StartPos 101, Length 10
    char    cts_daygb           [   1];    char    _cts_daygb           ;    // [string,    1] 연속당일구분                   StartPos 112, Length 1
} s4101OutBlock, *LPs4101OutBlock;
#define NAME_s4101OutBlock     "s4101OutBlock"

typedef struct _s4101OutBlock1
{
    char    date                [   8];    char    _date                ;    // [string,    8] 날짜                           StartPos 0, Length 8
    char    time                [  10];    char    _time                ;    // [string,   10] 시간                           StartPos 9, Length 10
    char    open                [   6];    char    _open                ;    // [string,    6] 시가                           StartPos 20, Length 6
    char    high                [   6];    char    _high                ;    // [string,    6] 고가                           StartPos 27, Length 6
    char    low                 [   6];    char    _low                 ;    // [string,    6] 저가                           StartPos 34, Length 6
    char    close               [   6];    char    _close               ;    // [string,    6] 종가                           StartPos 41, Length 6
    char    jdiff_vol           [  12];    char    _jdiff_vol           ;    // [string,   12] 누적거래량                     StartPos 48, Length 12
    char    value               [  12];    char    _value               ;    // [string,   12] 거래대금                       StartPos 61, Length 12
    char    openyak             [  12];    char    _openyak             ;    // [string,   12] 미결제약정                     StartPos 74, Length 12
} s4101OutBlock1, *LPs4101OutBlock1;
#define NAME_s4101OutBlock1     "s4101OutBlock1"


#pragma pack( pop )
///////////////////////////////////////////////////////////////////////////////////////////////////
