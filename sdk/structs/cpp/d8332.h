#pragma once

///////////////////////////////////////////////////////////////////////////////////////////////////
// 주식주문내역 최종주문번호 조회 ( headtype=C )
#pragma pack( push, 1 )

#define NAME_d8332     "d8332"

// 입력
typedef struct _d8332InBlock
{
    char    accno               [  14];    // [string,   14] 계좌번호                       StartPos 0, Length 14
} d8332InBlock, *LPd8332InBlock;
#define NAME_d8332InBlock     "d8332InBlock"

// 출력
typedef struct _d8332OutBlock
{
    char    ordno               [  10];    // [string,   10] 주문번호                       StartPos 0, Length 10
} d8332OutBlock, *LPd8332OutBlock;
#define NAME_d8332OutBlock     "d8332OutBlock"


#pragma pack( pop )
///////////////////////////////////////////////////////////////////////////////////////////////////