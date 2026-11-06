#pragma once

///////////////////////////////////////////////////////////////////////////////////////////////////
// 상품선물마스터조회 ( attr,block,headtype=A ) 
#pragma pack( push, 1 )

#define NAME_t3093     "t3093"

// 입력
typedef struct _t3093InBlock
{
    char    focode              [   8];    char    _focode              ;    // [string,    8] 단축코드                       StartPos 0, Length 8
} t3093InBlock, *LPt3093InBlock;
#define NAME_t3093InBlock     "t3093InBlock"

// 출력
typedef struct _t3093OutBlock
{
    char    shcode              [   8];    char    _shcode              ;    // [string,    8] 단축코드                       StartPos 0, Length 8
    char    expcode             [  12];    char    _expcode             ;    // [string,   12] 종목코드                       StartPos 9, Length 12
    char    hname               [  30];    char    _hname               ;    // [string,   30] 종목명                         StartPos 22, Length 30
} t3093OutBlock, *LPt3093OutBlock;
#define NAME_t3093OutBlock     "t3093OutBlock"


#pragma pack( pop )
///////////////////////////////////////////////////////////////////////////////////////////////////
