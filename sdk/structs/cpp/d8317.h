#pragma once

///////////////////////////////////////////////////////////////////////////////////////////////////
// 주식체결/미체결 ( attr,headtype=D ) 
#pragma pack( push, 1 )

#define NAME_d8317     "d8317"

// 입력
typedef struct _d8317InBlock
{
    char    accno               [  11];    char    _accno               ;    // [string,   11] 계좌번호                       StartPos 0, Length 11
    char    passwd              [   8];    char    _passwd              ;    // [string,    8] 비밀번호                       StartPos 12, Length 8
    char    expcode             [  12];    char    _expcode             ;    // [string,   12] 종목번호                       StartPos 21, Length 12
    char    chegb               [   1];    char    _chegb               ;    // [string,    1] 체결구분                       StartPos 34, Length 1
    char    medosu              [   1];    char    _medosu              ;    // [string,    1] 매매구분                       StartPos 36, Length 1
    char    sortgb              [   1];    char    _sortgb              ;    // [string,    1] 정렬순서                       StartPos 38, Length 1
    char    cts_ordno           [  10];    char    _cts_ordno           ;    // [string,   10] 주문번호                       StartPos 40, Length 10
    char    exchange            [   1];    char    _exchange            ;    // [string,    1] 거래소구분                     StartPos 51, Length 1
} d8317InBlock, *LPd8317InBlock;
#define NAME_d8317InBlock     "d8317InBlock"

// 출력
typedef struct _d8317OutBlock
{
    char    tqty                [  18];    char    _tqty                ;    // [string,   18] 총주문수량                     StartPos 0, Length 18
    char    tcheqty             [  18];    char    _tcheqty             ;    // [string,   18] 총체결수량                     StartPos 19, Length 18
    char    tordrem             [  18];    char    _tordrem             ;    // [string,   18] 총미체결수량                   StartPos 38, Length 18
    char    cmss                [  18];    char    _cmss                ;    // [string,   18] 추정수수료                     StartPos 57, Length 18
    char    tamt                [  18];    char    _tamt                ;    // [string,   18] 총주문금액                     StartPos 76, Length 18
    char    tmdamt              [  18];    char    _tmdamt              ;    // [string,   18] 총매도체결금액                 StartPos 95, Length 18
    char    tmsamt              [  18];    char    _tmsamt              ;    // [string,   18] 총매수체결금액                 StartPos 114, Length 18
    char    tax                 [  18];    char    _tax                 ;    // [string,   18] 추정제세금                     StartPos 133, Length 18
    char    cts_ordno           [  10];    char    _cts_ordno           ;    // [string,   10] 주문번호                       StartPos 152, Length 10
} d8317OutBlock, *LPd8317OutBlock;
#define NAME_d8317OutBlock     "d8317OutBlock"

typedef struct _d8317OutBlock1
{
    char    ordno               [  10];    char    _ordno               ;    // [string,   10] 주문번호                       StartPos 0, Length 10
    char    expcode             [  12];    char    _expcode             ;    // [string,   12] 종목번호                       StartPos 11, Length 12
    char    marketgb            [   3];    char    _marketgb            ;    // [string,    3] 거래소                         StartPos 24, Length 3
    char    medosu              [  16];    char    _medosu              ;    // [string,   16] 구분                           StartPos 28, Length 16
    char    qty                 [   9];    char    _qty                 ;    // [string,    9] 주문수량                       StartPos 45, Length 9
    char    price               [   9];    char    _price               ;    // [string,    9] 주문가격                       StartPos 55, Length 9
    char    cheqty              [   9];    char    _cheqty              ;    // [string,    9] 체결수량                       StartPos 65, Length 9
    char    cheprice            [   9];    char    _cheprice            ;    // [string,    9] 체결가격                       StartPos 75, Length 9
    char    ordrem              [   9];    char    _ordrem              ;    // [string,    9] 미체결잔량                     StartPos 85, Length 9
    char    cfmqty              [   9];    char    _cfmqty              ;    // [string,    9] 확인수량                       StartPos 95, Length 9
    char    status              [  10];    char    _status              ;    // [string,   10] 상태                           StartPos 105, Length 10
    char    orgordno            [  10];    char    _orgordno            ;    // [string,   10] 원주문번호                     StartPos 116, Length 10
    char    ordgb               [  20];    char    _ordgb               ;    // [string,   20] 유형                           StartPos 127, Length 20
    char    ordtime             [   8];    char    _ordtime             ;    // [string,    8] 주문시간                       StartPos 148, Length 8
    char    ordermtd            [  10];    char    _ordermtd            ;    // [string,   10] 주문매체                       StartPos 157, Length 10
    char    sysprocseq          [  10];    char    _sysprocseq          ;    // [string,   10] 처리순번                       StartPos 168, Length 10
    char    hogagb              [   2];    char    _hogagb              ;    // [string,    2] 호가유형                       StartPos 179, Length 2
    char    price1              [   8];    char    _price1              ;    // [string,    8] 현재가                         StartPos 182, Length 8
    char    orggb               [   2];    char    _orggb               ;    // [string,    2] 주문구분                       StartPos 191, Length 2
    char    singb               [   2];    char    _singb               ;    // [string,    2] 신용구분                       StartPos 194, Length 2
    char    loandt              [   8];    char    _loandt              ;    // [string,    8] 대출일자                       StartPos 197, Length 8
    char    hname               [  40];    char    _hname               ;    // [string,   40] 종목명                         StartPos 206, Length 40
    char    chetime             [   8];    char    _chetime             ;    // [string,    8] 체결시간                       StartPos 247, Length 8
    char    cndiprc             [   9];    char    _cndiprc             ;    // [string,    9] 전환가격                       StartPos 256, Length 9
    char    cfmtime_stopprc     [   9];    char    _cfmtime_stopprc     ;    // [string,    9] 전환시간                       StartPos 266, Length 9
    char    diff                [   6];    char    _diff                ;    // [string,    6] 등락율                         StartPos 276, Length 6
} d8317OutBlock1, *LPd8317OutBlock1;
#define NAME_d8317OutBlock1     "d8317OutBlock1"


#pragma pack( pop )
///////////////////////////////////////////////////////////////////////////////////////////////////
