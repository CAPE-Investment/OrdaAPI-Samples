#pragma once

///////////////////////////////////////////////////////////////////////////////////////////////////
// 신용거래동향 ( attr,block,headtype=A ) 
#pragma pack( push, 1 )

#define NAME_t1361     "t1361"

// 입력
typedef struct _t1361InBlock
{
    char    shcode              [   6];    char    _shcode              ;    // [string,    6] 종목코드                       StartPos 0, Length 6
    char    gubun               [   1];    char    _gubun               ;    // [string,    1] 융자대주구분                   StartPos 7, Length 1
    char    date                [   8];    char    _date                ;    // [string,    8] 날짜                           StartPos 9, Length 8
    char    idx                 [   4];    char    _idx                 ;    // [string,    4] IDX                            StartPos 18, Length 4
    char    exchange            [   1];    char    _exchange            ;    // [string,    1] 거래소구분                     StartPos 23, Length 1
} t1361InBlock, *LPt1361InBlock;
#define NAME_t1361InBlock     "t1361InBlock"

// 출력
typedef struct _t1361OutBlock
{
    char    cnt                 [   4];    char    _cnt                 ;    // [string,    4] CNT                            StartPos 0, Length 4
    char    date                [   8];    char    _date                ;    // [string,    8] 날짜                           StartPos 5, Length 8
    char    idx                 [   4];    char    _idx                 ;    // [string,    4] IDX                            StartPos 14, Length 4
} t1361OutBlock, *LPt1361OutBlock;
#define NAME_t1361OutBlock     "t1361OutBlock"

typedef struct _t1361OutBlock1
{
    char    mmdate              [   8];    char    _mmdate              ;    // [string,    8] 날짜                           StartPos 0, Length 8
    char    close               [   8];    char    _close               ;    // [string,    8] 종가                           StartPos 9, Length 8
    char    sign                [   1];    char    _sign                ;    // [string,    1] 전일대비구분                   StartPos 18, Length 1
    char    jchange             [   8];    char    _jchange             ;    // [string,    8] 전일대비                       StartPos 20, Length 8
    char    diff                [   6];    char    _diff                ;    // [string,    6] 등락율                         StartPos 29, Length 6
    char    nvolume             [  10];    char    _nvolume             ;    // [string,   10] 신규                           StartPos 36, Length 10
    char    svolume             [  10];    char    _svolume             ;    // [string,   10] 상환                           StartPos 47, Length 10
    char    jvolume             [  10];    char    _jvolume             ;    // [string,   10] 잔고                           StartPos 58, Length 10
    char    price               [  10];    char    _price               ;    // [string,   10] 금액                           StartPos 69, Length 10
    char    change              [   8];    char    _change              ;    // [string,    8] 대비                           StartPos 80, Length 8
    char    gyrate              [   6];    char    _gyrate              ;    // [string,    6] 공여율                         StartPos 89, Length 6
    char    jkrate              [   6];    char    _jkrate              ;    // [string,    6] 잔고율                         StartPos 96, Length 6
    char    shcode              [   6];    char    _shcode              ;    // [string,    6] 종목코드                       StartPos 103, Length 6
} t1361OutBlock1, *LPt1361OutBlock1;
#define NAME_t1361OutBlock1     "t1361OutBlock1"


#pragma pack( pop )
///////////////////////////////////////////////////////////////////////////////////////////////////
