#pragma once

///////////////////////////////////////////////////////////////////////////////////////////////////
// 주식당일매매손익 ( attr,headtype=C ) 
#pragma pack( push, 1 )

#define NAME_d8306     "d8306"

// 입력
typedef struct _d8306InBlock
{
    char    accno               [  11];    char    _accno               ;    // [string,   11] 계좌번호                       StartPos 0, Length 11
    char    cts_expcode         [  12];    char    _cts_expcode         ;    // [string,   12] CTS_종목번호                   StartPos 12, Length 12
} d8306InBlock, *LPd8306InBlock;
#define NAME_d8306InBlock     "d8306InBlock"

// 출력
typedef struct _d8306OutBlock
{
    char    msamt               [  18];    char    _msamt               ;    // [string,   18] 매수체결금액                   StartPos 0, Length 18
    char    mdamt               [  18];    char    _mdamt               ;    // [string,   18] 매도체결금액                   StartPos 19, Length 18
    char    sunik               [  18];    char    _sunik               ;    // [string,   18] 금일손익금액                   StartPos 38, Length 18
    char    sunikrt             [  10];    char    _sunikrt             ;    // [string,   10] 금일수익률                     StartPos 57, Length 10
    char    cts_expcode         [  12];    char    _cts_expcode         ;    // [string,   12] CTS_종목번호                   StartPos 68, Length 12
} d8306OutBlock, *LPd8306OutBlock;
#define NAME_d8306OutBlock     "d8306OutBlock"

typedef struct _d8306OutBlock1
{
    char    expcode             [  12];    char    _expcode             ;    // [string,   12] 종목번호                       StartPos 0, Length 12
    char    janqty              [   9];    char    _janqty              ;    // [string,    9] 금일잔고                       StartPos 13, Length 9
    char    msqty               [   9];    char    _msqty               ;    // [string,    9] 매수수량                       StartPos 23, Length 9
    char    msprice             [   9];    char    _msprice             ;    // [string,    9] 매수가                         StartPos 33, Length 9
    char    mdqty               [   9];    char    _mdqty               ;    // [string,    9] 매도수량                       StartPos 43, Length 9
    char    mdprice             [   9];    char    _mdprice             ;    // [string,    9] 매도가                         StartPos 53, Length 9
    char    jansonik            [  18];    char    _jansonik            ;    // [string,   18] 금일잔량손익                   StartPos 63, Length 18
    char    price               [   9];    char    _price               ;    // [string,    9] 현재가                         StartPos 82, Length 9
    char    mdsonik             [  18];    char    _mdsonik             ;    // [string,   18] 매도실현손익                   StartPos 92, Length 18
    char    sonikrt             [  10];    char    _sonikrt             ;    // [string,   10] 잔고수익률                     StartPos 111, Length 10
    char    fee                 [  18];    char    _fee                 ;    // [string,   18] 수수료                         StartPos 122, Length 18
    char    tax                 [  18];    char    _tax                 ;    // [string,   18] 제세금                         StartPos 141, Length 18
} d8306OutBlock1, *LPd8306OutBlock1;
#define NAME_d8306OutBlock1     "d8306OutBlock1"


#pragma pack( pop )
///////////////////////////////////////////////////////////////////////////////////////////////////
