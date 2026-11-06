#pragma once

///////////////////////////////////////////////////////////////////////////////////////////////////
// 기간별주가 ( attr,block,headtype=A ) 
#pragma pack( push, 1 )

#define NAME_t1025     "t1025"

// 입력
typedef struct _t1025InBlock
{
    char    shcode              [   6];    char    _shcode              ;    // [string,    6] 단축코드                       StartPos 0, Length 6
    char    dwmcode             [   1];    char    _dwmcode             ;    // [string,    1] 일주월구분                     StartPos 7, Length 1
    char    date                [   8];    char    _date                ;    // [string,    8] 날짜                           StartPos 9, Length 8
    char    idx                 [   4];    char    _idx                 ;    // [string,    4] IDX                            StartPos 18, Length 4
    char    cnt                 [   4];    char    _cnt                 ;    // [string,    4] 건수                           StartPos 23, Length 4
    char    exchange            [   1];    char    _exchange            ;    // [string,    1] 거래소구분                     StartPos 28, Length 1
} t1025InBlock, *LPt1025InBlock;
#define NAME_t1025InBlock     "t1025InBlock"

// 출력
typedef struct _t1025OutBlock
{
    char    cnt                 [   4];    char    _cnt                 ;    // [string,    4] CNT                            StartPos 0, Length 4
    char    date                [   8];    char    _date                ;    // [string,    8] 날짜                           StartPos 5, Length 8
    char    idx                 [   4];    char    _idx                 ;    // [string,    4] IDX                            StartPos 14, Length 4
} t1025OutBlock, *LPt1025OutBlock;
#define NAME_t1025OutBlock     "t1025OutBlock"

typedef struct _t1025OutBlock1
{
    char    date                [   8];    char    _date                ;    // [string,    8] 날짜                           StartPos 0, Length 8
    char    open                [   8];    char    _open                ;    // [string,    8] 시가                           StartPos 9, Length 8
    char    high                [   8];    char    _high                ;    // [string,    8] 고가                           StartPos 18, Length 8
    char    low                 [   8];    char    _low                 ;    // [string,    8] 저가                           StartPos 27, Length 8
    char    close               [   8];    char    _close               ;    // [string,    8] 종가                           StartPos 36, Length 8
    char    sign                [   1];    char    _sign                ;    // [string,    1] 전일대비구분                   StartPos 45, Length 1
    char    change              [   8];    char    _change              ;    // [string,    8] 전일대비                       StartPos 47, Length 8
    char    diff                [   6];    char    _diff                ;    // [string,    6] 등락율                         StartPos 56, Length 6
    char    volume              [  12];    char    _volume              ;    // [string,    2] 누적거래량                     StartPos 63, Length 2
    char    diff_vol            [  10];    char    _diff_vol            ;    // [string,    0] 거래증가율                     StartPos 66, Length 0
    char    chdegree            [   6];    char    _chdegree            ;    // [string,    6] 체결강도                       StartPos 67, Length 6
    char    sojinrate           [   6];    char    _sojinrate           ;    // [string,    6] 소진율                         StartPos 74, Length 6
    char    changerate          [   6];    char    _changerate          ;    // [string,    6] 회전율                         StartPos 81, Length 6
    char    fpvolume            [  12];    char    _fpvolume            ;    // [string,    2] 외인순매수                     StartPos 88, Length 2
    char    covolume            [  12];    char    _covolume            ;    // [string,    2] 기관순매수                     StartPos 91, Length 2
    char    shcode              [   6];    char    _shcode              ;    // [string,    6] 종목코드                       StartPos 94, Length 6
    char    value               [  12];    char    _value               ;    // [string,    2] 누적거래대금                   StartPos 101, Length 2
    char    ppvolume            [  12];    char    _ppvolume            ;    // [string,    2] 개인순매수                     StartPos 104, Length 2
    char    o_sign              [   1];    char    _o_sign              ;    // [string,    1] 시가대비구분                   StartPos 107, Length 1
    char    o_change            [   8];    char    _o_change            ;    // [string,    8] 시가대비                       StartPos 109, Length 8
    char    o_diff              [   6];    char    _o_diff              ;    // [string,    6] 시가기준등락율                 StartPos 118, Length 6
    char    h_sign              [   1];    char    _h_sign              ;    // [string,    1] 고가대비구분                   StartPos 125, Length 1
    char    h_change            [   8];    char    _h_change            ;    // [string,    8] 고가대비                       StartPos 127, Length 8
    char    h_diff              [   6];    char    _h_diff              ;    // [string,    6] 고가기준등락율                 StartPos 136, Length 6
    char    l_sign              [   1];    char    _l_sign              ;    // [string,    1] 저가대비구분                   StartPos 143, Length 1
    char    l_change            [   8];    char    _l_change            ;    // [string,    8] 저가대비                       StartPos 145, Length 8
    char    l_diff              [   6];    char    _l_diff              ;    // [string,    6] 저가기준등락율                 StartPos 154, Length 6
    char    after_close         [   8];    char    _after_close         ;    // [string,    8] 애프터종가                     StartPos 161, Length 8
    char    after_sign          [   1];    char    _after_sign          ;    // [string,    1] 애프터전일대비구분             StartPos 170, Length 1
    char    after_change        [   8];    char    _after_change        ;    // [string,    8] 애프터전일대비                 StartPos 172, Length 8
    char    after_diff          [   6];    char    _after_diff          ;    // [string,    6] 애프터등락율                   StartPos 181, Length 6
} t1025OutBlock1, *LPt1025OutBlock1;
#define NAME_t1025OutBlock1     "t1025OutBlock1"


#pragma pack( pop )
///////////////////////////////////////////////////////////////////////////////////////////////////
