#pragma once

///////////////////////////////////////////////////////////////////////////////////////////////////
// 주식선물 기초자산 리스트 ( attr,block,headtype=A ) 
#pragma pack( push, 1 )

#define NAME_s2105     "s2105"

// 입력
typedef struct _s2105InBlock
{
    char    dummy               [   4];    char    _dummy               ;    // [string,    4] 공백                           StartPos 0, Length 4
} s2105InBlock, *LPs2105InBlock;
#define NAME_s2105InBlock     "s2105InBlock"

// 출력
typedef struct _s2105OutBlock
{
    char    name                [  20];    char    _name                ;    // [string,   20] 기초자산명                     StartPos 0, Length 20
    char    eitem               [  10];    char    _eitem               ;    // [string,   10] 기초자산코드                   StartPos 21, Length 10
} s2105OutBlock, *LPs2105OutBlock;
#define NAME_s2105OutBlock     "s2105OutBlock"


#pragma pack( pop )
///////////////////////////////////////////////////////////////////////////////////////////////////
