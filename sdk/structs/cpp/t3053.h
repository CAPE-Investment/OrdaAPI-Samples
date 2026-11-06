#pragma once

///////////////////////////////////////////////////////////////////////////////////////////////////
// 미결제약정분석 ( attr,block,headtype=A ) 
#pragma pack( push, 1 )

#define NAME_t3053     "t3053"

// 입력
typedef struct _t3053InBlock
{
    char    focode              [   8];    char    _focode              ;    // [string,    8] 선옵단축코드                   StartPos 0, Length 8
    char    termgbn             [   2];    char    _termgbn             ;    // [string,    2] 구간구분(1:0.12:0.23:0.54:1.0) StartPos 9, Length 2
    char    fromprc             [   6];    char    _fromprc             ;    // [string,    6] 시작가격구간(선물만사용)       StartPos 12, Length 6
    char    toprc               [   6];    char    _toprc               ;    // [string,    6] 종료가격구간(선물만사용)       StartPos 19, Length 6
} t3053InBlock, *LPt3053InBlock;
#define NAME_t3053InBlock     "t3053InBlock"

// 출력
typedef struct _t3053OutBlock
{
    char    topenyak_tot        [   8];    char    _topenyak_tot        ;    // [string,    8] 당일총미결제약정수량           StartPos 0, Length 8
    char    jopenyak_tot        [   8];    char    _jopenyak_tot        ;    // [string,    8] 누적총미결제약정수량           StartPos 9, Length 8
    char    fromprc             [   6];    char    _fromprc             ;    // [string,    6] 출력시작가격구간(선물만사용)   StartPos 18, Length 6
    char    toprc               [   6];    char    _toprc               ;    // [string,    6] 출력종료가격구간(선물만사용)   StartPos 25, Length 6
} t3053OutBlock, *LPt3053OutBlock;
#define NAME_t3053OutBlock     "t3053OutBlock"

typedef struct _t3053OutBlock1
{
    char    price               [   6];    char    _price               ;    // [string,    6] 선물옵션가격                   StartPos 0, Length 6
    char    topenyak            [   8];    char    _topenyak            ;    // [string,    8] 당일미결제약정수량             StartPos 7, Length 8
    char    jopenyak            [   8];    char    _jopenyak            ;    // [string,    8] 누적일미결제약정수량           StartPos 16, Length 8
} t3053OutBlock1, *LPt3053OutBlock1;
#define NAME_t3053OutBlock1     "t3053OutBlock1"


#pragma pack( pop )
///////////////////////////////////////////////////////////////////////////////////////////////////
