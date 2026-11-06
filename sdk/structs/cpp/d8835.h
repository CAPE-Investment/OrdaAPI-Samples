#pragma once

///////////////////////////////////////////////////////////////////////////////////////////////////
// 선물옵션 최종주문번호 조회 ( headtype=C )
#pragma pack( push, 1 )

#define NAME_d8835     "d8835"

// 입력
typedef struct _d8835InBlock
{
    char    accno               [  14];    // [string,   14] 계좌번호                       StartPos 0, Length 14
} d8835InBlock, *LPd8835InBlock;
#define NAME_d8835InBlock     "d8835InBlock"

// 출력
typedef struct _d8835OutBlock
{
    char    ordno               [   7];    // [string,    7] 주문번호                       StartPos 0, Length 7
} d8835OutBlock, *LPd8835OutBlock;
#define NAME_d8835OutBlock     "d8835OutBlock"


#pragma pack( pop )
///////////////////////////////////////////////////////////////////////////////////////////////////