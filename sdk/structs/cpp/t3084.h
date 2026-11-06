#pragma once

///////////////////////////////////////////////////////////////////////////////////////////////////
// 선물마스터조회(t3084) ( attr,block,headtype=A ) 
#pragma pack( push, 1 )

#define NAME_t3084     "t3084"

// 입력
typedef struct _t3084InBlock
{
    char    focode              [   8];    char    _focode              ;    // [string,    8] 단축코드                       StartPos 0, Length 8
} t3084InBlock, *LPt3084InBlock;
#define NAME_t3084InBlock     "t3084InBlock"

// 출력
typedef struct _t3084OutBlock
{
    char    shcode              [   8];    char    _shcode              ;    // [string,    8] 단축코드                       StartPos 0, Length 8
    char    expcode             [  12];    char    _expcode             ;    // [string,   12] 표준코드                       StartPos 9, Length 12
    char    hname               [  30];    char    _hname               ;    // [string,   30] 종목명                         StartPos 22, Length 30
} t3084OutBlock, *LPt3084OutBlock;
#define NAME_t3084OutBlock     "t3084OutBlock"


#pragma pack( pop )
///////////////////////////////////////////////////////////////////////////////////////////////////
