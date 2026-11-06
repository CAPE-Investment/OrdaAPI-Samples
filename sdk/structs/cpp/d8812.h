#pragma once

///////////////////////////////////////////////////////////////////////////////////////////////////
// 선물/옵션 체결/미체결 ( headtype=D )
#pragma pack( push, 1 )

#define NAME_d8812     "d8812"

// 입력
typedef struct _d8812InBlock
{
    char    accno               [  11];    // [string,   11] 계좌번호                       StartPos 0, Length 11
    char    passwd              [   8];    // [string,    8] 비밀번호                       StartPos 11, Length 8
    char    expcode             [   8];    // [string,    8] 종목번호                       StartPos 19, Length 8
    char    chegb               [   1];    // [string,    1] 체결구분                       StartPos 27, Length 1
    char    sortgb              [   1];    // [string,    1] 정렬순서                       StartPos 28, Length 1
    char    cts_ordno           [   7];    // [string,    7] CTS_주문번호                   StartPos 29, Length 7
} d8812InBlock, *LPd8812InBlock;
#define NAME_d8812InBlock     "d8812InBlock"

// 출력
typedef struct _d8812OutBlock
{
    char    cts_ordno           [   7];    // [string,    7] CTS_주문번호                   StartPos 0, Length 7
} d8812OutBlock, *LPd8812OutBlock;
#define NAME_d8812OutBlock     "d8812OutBlock"

typedef struct _d8812OutBlock1
{
    char    ordno               [   7];    // [string,    7] 주문번호                       StartPos 0, Length 7
    char    orgordno            [   7];    // [string,    7] 원주문번호                     StartPos 7, Length 7
    char    medosu              [  10];    // [string,   10] 구분                           StartPos 14, Length 10
    char    ordgb               [  20];    // [string,   20] 유형                           StartPos 24, Length 20
    char    qty                 [   9];    // [string,    9] 주문수량                       StartPos 44, Length 9
    char    price               [   9];    // [string,    9] 주문가격                       StartPos 53, Length 9
    char    cheqty              [   9];    // [string,    9] 체결수량                       StartPos 62, Length 9
    char    cheprice            [   9];    // [string,    9] 체결가격                       StartPos 71, Length 9
    char    ordrem              [   9];    // [string,    9] 미체결잔량                     StartPos 80, Length 9
    char    status              [  10];    // [string,   10] 상태                           StartPos 89, Length 10
    char    ordtime             [   8];    // [string,    8] 주문시간                       StartPos 99, Length 8
    char    ordermtd            [  10];    // [string,   10] 주문매체                       StartPos 107, Length 10
    char    expcode             [   8];    // [string,    8] 종목번호                       StartPos 117, Length 8
    char    rtcode              [   3];    // [string,    3] 사유코드                       StartPos 125, Length 3
    char    sysprocseq          [  10];    // [string,   10] 처리순번                       StartPos 128, Length 10
    char    hogatype            [   1];    // [string,    1] 호가타입                       StartPos 138, Length 1
} d8812OutBlock1, *LPd8812OutBlock1;
#define NAME_d8812OutBlock1     "d8812OutBlock1"


#pragma pack( pop )
///////////////////////////////////////////////////////////////////////////////////////////////////