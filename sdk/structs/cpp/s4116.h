#pragma once

///////////////////////////////////////////////////////////////////////////////////////////////////
// 주식챠트(일/주/월) ( attr,block,headtype=A ) 
#pragma pack( push, 1 )

#define NAME_s4116     "s4116"

// 입력
typedef struct _s4116InBlock
{
    char    shcode              [   6];    char    _shcode              ;    // [string,    6] 단축코드                       StartPos 0, Length 6
    char    gubun               [   1];    char    _gubun               ;    // [string,    1] 주기구분(2:일3:주4:월)         StartPos 7, Length 1
    char    qrycnt              [   4];    char    _qrycnt              ;    // [string,    4] 요청건수(최대-압축:2000비압축:500) StartPos 9, Length 4
    char    sdate               [   8];    char    _sdate               ;    // [string,    8] 시작일자                       StartPos 14, Length 8
    char    edate               [   8];    char    _edate               ;    // [string,    8] 종료일자                       StartPos 23, Length 8
    char    cts_date            [   8];    char    _cts_date            ;    // [string,    8] 연속일자                       StartPos 32, Length 8
    char    comp_yn             [   1];    char    _comp_yn             ;    // [string,    1] 압축여부(Y:압축N:비압축)       StartPos 41, Length 1
} s4116InBlock, *LPs4116InBlock;
#define NAME_s4116InBlock     "s4116InBlock"

// 출력
typedef struct _s4116OutBlock
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
    char    s_time              [   6];    char    _s_time              ;    // [string,    6] 장시작시간(HHMMSS)             StartPos 119, Length 6
    char    e_time              [   6];    char    _e_time              ;    // [string,    6] 장종료시간(HHMMSS)             StartPos 126, Length 6
    char    dshmin              [   2];    char    _dshmin              ;    // [string,    2] 동시호가처리시간(MM:분)        StartPos 133, Length 2
    char    rec_count           [   7];    char    _rec_count           ;    // [string,    7] 레코드카운트                   StartPos 136, Length 7
} s4116OutBlock, *LPs4116OutBlock;
#define NAME_s4116OutBlock     "s4116OutBlock"

typedef struct _s4116OutBlock1
{
    char    date                [   8];    char    _date                ;    // [string,    8] 날짜                           StartPos 0, Length 8
    char    open                [   8];    char    _open                ;    // [string,    8] 시가                           StartPos 9, Length 8
    char    high                [   8];    char    _high                ;    // [string,    8] 고가                           StartPos 18, Length 8
    char    low                 [   8];    char    _low                 ;    // [string,    8] 저가                           StartPos 27, Length 8
    char    close               [   8];    char    _close               ;    // [string,    8] 종가                           StartPos 36, Length 8
    char    jdiff_vol           [  12];    char    _jdiff_vol           ;    // [string,   12] 거래량                         StartPos 45, Length 12
    char    value               [  12];    char    _value               ;    // [string,   12] 거래대금                       StartPos 58, Length 12
    char    jongchk             [  13];    char    _jongchk             ;    // [string,   13] 수정구분                       StartPos 71, Length 13
    char    rate                [   6];    char    _rate                ;    // [string,    6] 수정비율                       StartPos 85, Length 6
    char    pricechk            [  13];    char    _pricechk            ;    // [string,   13] 수정주가반영항목               StartPos 92, Length 13
    char    ratevalue           [  12];    char    _ratevalue           ;    // [string,   12] 수정비율반영거래대금           StartPos 106, Length 12
    char    sign                [   1];    char    _sign                ;    // [string,    1] 종가등락구분(1:상한2:상승3:보합4:하한5:하락주식일만사용) StartPos 119, Length 1
} s4116OutBlock1, *LPs4116OutBlock1;
#define NAME_s4116OutBlock1     "s4116OutBlock1"


#pragma pack( pop )
///////////////////////////////////////////////////////////////////////////////////////////////////
