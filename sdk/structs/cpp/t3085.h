#pragma once

///////////////////////////////////////////////////////////////////////////////////////////////////
// 옵션마스터조회(t3085) ( attr,block,headtype=A ) 
#pragma pack( push, 1 )

#define NAME_t3085     "t3085"

// 입력
typedef struct _t3085InBlock
{
    char    focode              [   8];    char    _focode              ;    // [string,    8] 단축코드                       StartPos 0, Length 8
} t3085InBlock, *LPt3085InBlock;
#define NAME_t3085InBlock     "t3085InBlock"

// 출력
typedef struct _t3085OutBlock
{
    char    shcode              [   8];    char    _shcode              ;    // [string,    8] 단축코드                       StartPos 0, Length 8
    char    expcode             [  12];    char    _expcode             ;    // [string,   12] 표준코드                       StartPos 9, Length 12
    char    hname               [  30];    char    _hname               ;    // [string,   30] 종목명                         StartPos 22, Length 30
} t3085OutBlock, *LPt3085OutBlock;
#define NAME_t3085OutBlock     "t3085OutBlock"


#pragma pack( pop )
///////////////////////////////////////////////////////////////////////////////////////////////////
