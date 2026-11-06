#pragma once

///////////////////////////////////////////////////////////////////////////////////////////////////
// 뉴스본문조회 ( block,headtype=A )
#pragma pack( push, 1 )

#define NAME_s2524     "s2524"

// 입력
typedef struct _s2524InBlock
{
    char    sNewsno             [  24];    // [string,   24] 뉴스번호                       StartPos 0, Length 24
} s2524InBlock, *LPs2524InBlock;
#define NAME_s2524InBlock     "s2524InBlock"

// 출력
typedef struct _s2524OutBlock
{
    char    sJongcode           [   6];    // [string,    6] 뉴스종목                       StartPos 0, Length 6
} s2524OutBlock, *LPs2524OutBlock;
#define NAME_s2524OutBlock     "s2524OutBlock"

typedef struct _s2524OutBlock1
{
    char    sBody               [ 100];    // [string,  100] 뉴스본문                       StartPos 0, Length 100
} s2524OutBlock1, *LPs2524OutBlock1;
#define NAME_s2524OutBlock1     "s2524OutBlock1"

typedef struct _s2524OutBlock2
{
    char    sTitle              [ 300];    // [string,  300] 뉴스타이틀                     StartPos 0, Length 300
} s2524OutBlock2, *LPs2524OutBlock2;
#define NAME_s2524OutBlock2     "s2524OutBlock2"


#pragma pack( pop )
///////////////////////////////////////////////////////////////////////////////////////////////////
