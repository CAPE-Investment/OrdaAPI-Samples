#pragma once

///////////////////////////////////////////////////////////////////////////////////////////////////
// 만기월 조회 ( block,headtype=A )
#pragma pack( push, 1 )

#define NAME_s1508     "s1508"

// 입력
typedef struct _s1508InBlock
{
    char    dummy               [   1];    // [string,    1] 공백                           StartPos 0, Length 1
} s1508InBlock, *LPs1508InBlock;
#define NAME_s1508InBlock     "s1508InBlock"

// 출력
typedef struct _s1508OutBlock
{
    char    lastym              [   6];    // [string,    6] 만기월                         StartPos 0, Length 6
    char    lastnm              [  10];    // [string,   10] 만기월명                       StartPos 6, Length 10
} s1508OutBlock, *LPs1508OutBlock;
#define NAME_s1508OutBlock     "s1508OutBlock"


#pragma pack( pop )
///////////////////////////////////////////////////////////////////////////////////////////////////