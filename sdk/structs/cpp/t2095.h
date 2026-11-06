#pragma once

///////////////////////////////////////////////////////////////////////////////////////////////////
// 거래원별 매매상위 ( attr,block,headtype=A ) 
#pragma pack( push, 1 )

#define NAME_t2095     "t2095"

// 입력
typedef struct _t2095InBlock
{
    char    tradcode            [   3];    char    _tradcode            ;    // [string,    3] 거래원                         StartPos 0, Length 3
    char    market              [   1];    char    _market              ;    // [string,    1] 시장구분                       StartPos 4, Length 1
    char    mmgb                [   1];    char    _mmgb                ;    // [string,    1] 매수도구분                     StartPos 6, Length 1
    char    next                [   4];    char    _next                ;    // [string,    4] NEXTKEY                        StartPos 8, Length 4
} t2095InBlock, *LPt2095InBlock;
#define NAME_t2095InBlock     "t2095InBlock"

// 출력
typedef struct _t2095OutBlock
{
    char    next                [   4];    char    _next                ;    // [string,    4] NEXTKEY                        StartPos 0, Length 4
} t2095OutBlock, *LPt2095OutBlock;
#define NAME_t2095OutBlock     "t2095OutBlock"

typedef struct _t2095OutBlock1
{
    char    number              [   4];    char    _number              ;    // [string,    4] 순위                           StartPos 0, Length 4
    char    shcode              [   6];    char    _shcode              ;    // [string,    6] 종목코드                       StartPos 5, Length 6
    char    name                [  20];    char    _name                ;    // [string,   20] 종목명                         StartPos 12, Length 20
    char    mdvol               [   9];    char    _mdvol               ;    // [string,    9] 매도량                         StartPos 33, Length 9
    char    msvol               [   9];    char    _msvol               ;    // [string,    9] 매수량                         StartPos 43, Length 9
    char    ssvol               [   9];    char    _ssvol               ;    // [string,    9] 순매수량                       StartPos 53, Length 9
    char    volume              [  12];    char    _volume              ;    // [string,   12] 거래량                         StartPos 63, Length 12
} t2095OutBlock1, *LPt2095OutBlock1;
#define NAME_t2095OutBlock1     "t2095OutBlock1"


#pragma pack( pop )
///////////////////////////////////////////////////////////////////////////////////////////////////
