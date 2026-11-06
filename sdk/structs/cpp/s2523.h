#pragma once

///////////////////////////////////////////////////////////////////////////////////////////////////
// 회원사 리스트 조회 ( block,headtype=A )
#pragma pack( push, 1 )

#define NAME_s2523     "s2523"

// 입력
typedef struct _s2523InBlock
{
    char    dummy               [   1];    // [string,    1] 공백                           StartPos 0, Length 1
} s2523InBlock, *LPs2523InBlock;
#define NAME_s2523InBlock     "s2523InBlock"

// 출력
typedef struct _s2523OutBlock1
{
    char    tradcode            [   3];    // [string,    3] 거래원코드                     StartPos 0, Length 3
    char    tradname            [  20];    // [string,   20] 거래원명                       StartPos 3, Length 20
    char    wtradgubun          [   1];    // [string,    1] 구분                           StartPos 23, Length 1
} s2523OutBlock1, *LPs2523OutBlock1;
#define NAME_s2523OutBlock1     "s2523OutBlock1"


#pragma pack( pop )
///////////////////////////////////////////////////////////////////////////////////////////////////