#pragma once

///////////////////////////////////////////////////////////////////////////////////////////////////
// 주식잔고-매도가능수량조회 ( headtype=C )
#pragma pack( push, 1 )

#define NAME_d8310     "d8310"

// 입력
typedef struct _d8310InBlock
{
    char    accno               [  11];    // [string,   11] 계좌번호                       StartPos 0, Length 11
    char    expcode             [  12];    // [string,   12] 종목번호                       StartPos 11, Length 12
    char    jangb               [   1];    // [string,    1] 잔고구분(A전체0보통1대용2담보4신용) StartPos 23, Length 1
    char    loandt              [   8];    // [string,    8] 대출일자                       StartPos 24, Length 8
} d8310InBlock, *LPd8310InBlock;
#define NAME_d8310InBlock     "d8310InBlock"

// 출력
typedef struct _d8310OutBlock
{
    char    expcode             [  12];    // [string,   12] 종목번호                       StartPos 0, Length 12
    char    jangb               [  10];    // [string,   10] 잔고구분명                     StartPos 12, Length 10
    char    janqty              [  18];    // [string,   18] 잔고수량                       StartPos 22, Length 18
    char    mdposqt             [  18];    // [string,   18] 매도가능수량                   StartPos 40, Length 18
    char    mamt                [  18];    // [string,   18] 매입금액                       StartPos 58, Length 18
    char    sinamt              [  18];    // [string,   18] 대출금액                       StartPos 76, Length 18
    char    loandt              [   8];    // [string,    8] 대출일자                       StartPos 94, Length 8
    char    hname               [  20];    // [string,   20] 종목명                         StartPos 102, Length 20
} d8310OutBlock, *LPd8310OutBlock;
#define NAME_d8310OutBlock     "d8310OutBlock"


#pragma pack( pop )
///////////////////////////////////////////////////////////////////////////////////////////////////