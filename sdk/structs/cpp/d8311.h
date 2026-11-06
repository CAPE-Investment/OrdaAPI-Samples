#pragma once

///////////////////////////////////////////////////////////////////////////////////////////////////
// 주식잔고 ( attr,headtype=D ) 
#pragma pack( push, 1 )

#define NAME_d8311     "d8311"

// 입력
typedef struct _d8311InBlock
{
    char    accno               [  11];    char    _accno               ;    // [string,   11] 계좌번호                       StartPos 0, Length 11
    char    passwd              [   8];    char    _passwd              ;    // [string,    8] 비밀번호                       StartPos 12, Length 8
    char    expcode             [  12];    char    _expcode             ;    // [string,   12] 종목번호                       StartPos 21, Length 12
    char    cts_expcode         [  12];    char    _cts_expcode         ;    // [string,   12] CTS_종목번호                   StartPos 34, Length 12
    char    exchange            [   1];    char    _exchange            ;    // [string,    1] 거래소구분                     StartPos 47, Length 1
} d8311InBlock, *LPd8311InBlock;
#define NAME_d8311InBlock     "d8311InBlock"

// 출력
typedef struct _d8311OutBlock
{
    char    cts_expcode         [  12];    char    _cts_expcode         ;    // [string,   12] CTS_종목번호                   StartPos 0, Length 12
} d8311OutBlock, *LPd8311OutBlock;
#define NAME_d8311OutBlock     "d8311OutBlock"

typedef struct _d8311OutBlock1
{
    char    expcode             [  12];    char    _expcode             ;    // [string,   12] 종목번호                       StartPos 0, Length 12
    char    janqty              [  18];    char    _janqty              ;    // [string,   18] 잔고수량                       StartPos 13, Length 18
    char    mdposqt             [  18];    char    _mdposqt             ;    // [string,   18] 매도가능수량                   StartPos 32, Length 18
    char    pamt                [  10];    char    _pamt                ;    // [string,   10] 평균단가                       StartPos 51, Length 10
    char    mamt                [  18];    char    _mamt                ;    // [string,   18] 매입금액                       StartPos 62, Length 18
    char    sysprocseq          [  10];    char    _sysprocseq          ;    // [string,   10] 처리순번                       StartPos 81, Length 10
    char    dtsunik1            [  18];    char    _dtsunik1            ;    // [string,   18] 매매손익                       StartPos 92, Length 18
    char    price               [   8];    char    _price               ;    // [string,    8] 현재가                         StartPos 111, Length 8
    char    dtsunik             [  18];    char    _dtsunik             ;    // [string,   18] 평가손익                       StartPos 120, Length 18
    char    sunikrt             [  10];    char    _sunikrt             ;    // [string,   10] 수익율                         StartPos 139, Length 10
} d8311OutBlock1, *LPd8311OutBlock1;
#define NAME_d8311OutBlock1     "d8311OutBlock1"


#pragma pack( pop )
///////////////////////////////////////////////////////////////////////////////////////////////////
