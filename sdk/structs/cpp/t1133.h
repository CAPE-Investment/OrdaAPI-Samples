#pragma once

///////////////////////////////////////////////////////////////////////////////////////////////////
// API_MASTER ( attr,block,headtype=A ) 
#pragma pack( push, 1 )

#define NAME_t1133     "t1133"

// 입력
typedef struct _t1133InBlock
{
    char    gubun               [   1];    char    _gubun               ;    // [string,    1] 구분                           StartPos 0, Length 1
    char    cts                 [   7];    char    _cts                 ;    // [string,    7] CTS                            StartPos 2, Length 7
} t1133InBlock, *LPt1133InBlock;
#define NAME_t1133InBlock     "t1133InBlock"

// 출력
typedef struct _t1133OutBlock
{
    char    cts                 [   7];    char    _cts                 ;    // [string,    7] CTS                            StartPos 0, Length 7
    char    cnt                 [   4];    char    _cnt                 ;    // [string,    4] CNT                            StartPos 8, Length 4
} t1133OutBlock, *LPt1133OutBlock;
#define NAME_t1133OutBlock     "t1133OutBlock"

typedef struct _t1133OutBlock1
{
    char    mkgb                [   1];    char    _mkgb                ;    // [string,    1] 시장구분                       StartPos 0, Length 1
    char    shcode              [   6];    char    _shcode              ;    // [string,    6] 종목코드                       StartPos 2, Length 6
    char    expcode             [  12];    char    _expcode             ;    // [string,   12] 표준종목코드                   StartPos 9, Length 12
    char    hname               [  40];    char    _hname               ;    // [string,   40] 한글명                         StartPos 22, Length 40
    char    bu12gubun           [   2];    char    _bu12gubun           ;    // [string,    2] 종목분류코드                   StartPos 63, Length 2
} t1133OutBlock1, *LPt1133OutBlock1;
#define NAME_t1133OutBlock1     "t1133OutBlock1"


#pragma pack( pop )
///////////////////////////////////////////////////////////////////////////////////////////////////
