#pragma once

///////////////////////////////////////////////////////////////////////////////////////////////////
//  주식마스터조회(t3086) ( attr,block,headtype=A ) 
#pragma pack( push, 1 )

#define NAME_t3086     "t3086"

// 입력
typedef struct _t3086InBlock
{
    char    mgb                 [   1];    char    _mgb                 ;    // [string,    1] 시장구분                       StartPos 0, Length 1
} t3086InBlock, *LPt3086InBlock;
#define NAME_t3086InBlock     "t3086InBlock"

// 출력
typedef struct _t3086OutBlock
{
    char    shcode              [   6];    char    _shcode              ;    // [string,    6] 단축코드                       StartPos 0, Length 6
    char    expcode             [  12];    char    _expcode             ;    // [string,   12] 표준코드                       StartPos 7, Length 12
    char    hname               [  40];    char    _hname               ;    // [string,   40] 종목명                         StartPos 20, Length 40
    char    eKospi200           [   1];    char    _eKospi200           ;    // [string,    1] 코스피200                      StartPos 61, Length 1
    char    sPrePrice           [   7];    char    _sPrePrice           ;    // [string,    7] 전일종가                       StartPos 63, Length 7
    char    sDepositRate        [   3];    char    _sDepositRate        ;    // [string,    3] 증거금률                       StartPos 71, Length 3
    char    gETF                [   1];    char    _gETF                ;    // [string,    1] ETF여부                        StartPos 75, Length 1
    char    sSinDepositRate     [   3];    char    _sSinDepositRate     ;    // [string,    3] 신용증거금율                   StartPos 77, Length 3
    char    nxttrd              [   1];    char    _nxttrd              ;    // [string,    1] NXT거래여부                    StartPos 81, Length 1
} t3086OutBlock, *LPt3086OutBlock;
#define NAME_t3086OutBlock     "t3086OutBlock"


#pragma pack( pop )
///////////////////////////////////////////////////////////////////////////////////////////////////
