#pragma once

///////////////////////////////////////////////////////////////////////////////////////////////////
// 주식챠트(종합) ( attr,block,headtype=A ) 
#pragma pack( push, 1 )

#define NAME_s4100     "s4100"

// 입력
typedef struct _s4100InBlock
{
    char    shcode              [   6];    char    _shcode              ;    // [string,    6] 단축코드                       StartPos 0, Length 6
    char    gubun               [   1];    char    _gubun               ;    // [string,    1] 주기구분                       StartPos 7, Length 1
    char    ncnt                [   4];    char    _ncnt                ;    // [string,    4] 틱개수                         StartPos 9, Length 4
    char    qrycnt              [   4];    char    _qrycnt              ;    // [string,    4] 건수                           StartPos 14, Length 4
    char    tdgb                [   1];    char    _tdgb                ;    // [string,    1] 당일구분                       StartPos 19, Length 1
    char    sdate               [   8];    char    _sdate               ;    // [string,    8] 시작일자                       StartPos 21, Length 8
    char    edate               [   8];    char    _edate               ;    // [string,    8] 종료일자                       StartPos 30, Length 8
    char    cts_date            [   8];    char    _cts_date            ;    // [string,    8] 연속일자                       StartPos 39, Length 8
    char    cts_time            [  10];    char    _cts_time            ;    // [string,   10] 연속시간                       StartPos 48, Length 10
    char    cts_daygb           [   1];    char    _cts_daygb           ;    // [string,    1] 연속당일구분                   StartPos 59, Length 1
} s4100InBlock, *LPs4100InBlock;
#define NAME_s4100InBlock     "s4100InBlock"

// 출력
typedef struct _s4100OutBlock
{
    char    shcode              [   6];    char    _shcode              ;    // [string,    6] 단축코드                       StartPos 0, Length 6
    char    jisiga              [   8];    char    _jisiga              ;    // [string,    8] 전일시가                       StartPos 7, Length 8
    char    jihigh              [   8];    char    _jihigh              ;    // [string,    8] 전일고가                       StartPos 16, Length 8
    char    jilow               [   8];    char    _jilow               ;    // [string,    8] 전일저가                       StartPos 25, Length 8
    char    jiclose             [   8];    char    _jiclose             ;    // [string,    8] 전일종가                       StartPos 34, Length 8
    char    jivolume            [  12];    char    _jivolume            ;    // [string,   12] 전일거래량                     StartPos 43, Length 12
    char    disiga              [   8];    char    _disiga              ;    // [string,    8] 당일시가                       StartPos 56, Length 8
    char    dihigh              [   8];    char    _dihigh              ;    // [string,    8] 당일고가                       StartPos 65, Length 8
    char    dilow               [   8];    char    _dilow               ;    // [string,    8] 당일저가                       StartPos 74, Length 8
    char    diclose             [   8];    char    _diclose             ;    // [string,    8] 당일종가                       StartPos 83, Length 8
    char    highend             [   8];    char    _highend             ;    // [string,    8] 상한가                         StartPos 92, Length 8
    char    lowend              [   8];    char    _lowend              ;    // [string,    8] 하한가                         StartPos 101, Length 8
    char    cts_date            [   8];    char    _cts_date            ;    // [string,    8] 연속일자                       StartPos 110, Length 8
    char    cts_time            [  10];    char    _cts_time            ;    // [string,   10] 연속시간                       StartPos 119, Length 10
    char    cts_daygb           [   1];    char    _cts_daygb           ;    // [string,    1] 연속당일구분                   StartPos 130, Length 1
} s4100OutBlock, *LPs4100OutBlock;
#define NAME_s4100OutBlock     "s4100OutBlock"

typedef struct _s4100OutBlock1
{
    char    date                [   8];    char    _date                ;    // [string,    8] 날짜                           StartPos 0, Length 8
    char    time                [  10];    char    _time                ;    // [string,   10] 시간                           StartPos 9, Length 10
    char    open                [   8];    char    _open                ;    // [string,    8] 시가                           StartPos 20, Length 8
    char    high                [   8];    char    _high                ;    // [string,    8] 고가                           StartPos 29, Length 8
    char    low                 [   8];    char    _low                 ;    // [string,    8] 저가                           StartPos 38, Length 8
    char    close               [   8];    char    _close               ;    // [string,    8] 종가                           StartPos 47, Length 8
    char    jdiff_vol           [  12];    char    _jdiff_vol           ;    // [string,   12] 거래량                         StartPos 56, Length 12
    char    value               [  12];    char    _value               ;    // [string,   12] 거래대금                       StartPos 69, Length 12
    char    jongchk             [  13];    char    _jongchk             ;    // [string,   13] 수정구분                       StartPos 82, Length 13
    char    rate                [   6];    char    _rate                ;    // [string,    6] 수정비율                       StartPos 96, Length 6
    char    pricechk            [  13];    char    _pricechk            ;    // [string,   13] 수정주가반영항목               StartPos 103, Length 13
    char    ratevalue           [  12];    char    _ratevalue           ;    // [string,   12] 수정비율반영거래대금           StartPos 117, Length 12
} s4100OutBlock1, *LPs4100OutBlock1;
#define NAME_s4100OutBlock1     "s4100OutBlock1"


#pragma pack( pop )
///////////////////////////////////////////////////////////////////////////////////////////////////
