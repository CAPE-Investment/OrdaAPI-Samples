#pragma once

///////////////////////////////////////////////////////////////////////////////////////////////////
// 위클리옵션마스터조회(t3087) ( attr,block,headtype=A ) 
#pragma pack( push, 1 )

#define NAME_t3087     "t3087"

// 입력
typedef struct _t3087InBlock
{
    char    dummy               [   4];    char    _dummy               ;    // [string,    4]                                StartPos 0, Length 4
} t3087InBlock, *LPt3087InBlock;
#define NAME_t3087InBlock     "t3087InBlock"

// 출력
typedef struct _t3087OutBlock
{
    char    shcode              [   8];    char    _shcode              ;    // [string,    8] 단축코드                       StartPos 0, Length 8
    char    expcode             [  12];    char    _expcode             ;    // [string,   12] 표준코드                       StartPos 9, Length 12
    char    hname               [  20];    char    _hname               ;    // [string,   20] 종목명                         StartPos 22, Length 20
    char    atmgubun            [   1];    char    _atmgubun            ;    // [string,    1] ATM구분                        StartPos 43, Length 1
    char    expweek             [   6];    char    _expweek             ;    // [string,    6] 만기주                         StartPos 45, Length 6
} t3087OutBlock, *LPt3087OutBlock;
#define NAME_t3087OutBlock     "t3087OutBlock"


#pragma pack( pop )
///////////////////////////////////////////////////////////////////////////////////////////////////
