#pragma once

///////////////////////////////////////////////////////////////////////////////////////////////////
// 국내외금융시장동향 ( attr,block,headtype=A ) 
#pragma pack( push, 1 )

#define NAME_t1222     "t1222"

// 입력
typedef struct _t1222InBlock
{
    char    gubun               [   1];    char    _gubun               ;    // [string,    1] 금액수량구분                   StartPos 0, Length 1
} t1222InBlock, *LPt1222InBlock;
#define NAME_t1222InBlock     "t1222InBlock"

// 출력
typedef struct _t1222OutBlock1
{
    char    upcode              [   8];    char    _upcode              ;    // [string,    8] 지수코드                       StartPos 0, Length 8
    char    hname               [  20];    char    _hname               ;    // [string,   20] 지수명                         StartPos 9, Length 20
    char    pricejisu           [  12];    char    _pricejisu           ;    // [string,   12] 지수                           StartPos 30, Length 12
    char    sign                [   1];    char    _sign                ;    // [string,    1] 대비구분                       StartPos 43, Length 1
    char    change              [   6];    char    _change              ;    // [string,    6] 대비                           StartPos 45, Length 6
    char    diff                [   6];    char    _diff                ;    // [string,    6] 등락율                         StartPos 52, Length 6
    char    volume              [  12];    char    _volume              ;    // [string,   12] 거래량                         StartPos 59, Length 12
    char    value               [  12];    char    _value               ;    // [string,   12] 거래대금                       StartPos 72, Length 12
} t1222OutBlock1, *LPt1222OutBlock1;
#define NAME_t1222OutBlock1     "t1222OutBlock1"

typedef struct _t1222OutBlock2
{
    char    hname               [  20];    char    _hname               ;    // [string,   20] 지수명                         StartPos 0, Length 20
    char    jisu                [  12];    char    _jisu                ;    // [string,   12] 지수                           StartPos 21, Length 12
    char    sign                [   1];    char    _sign                ;    // [string,    1] 대비구분                       StartPos 34, Length 1
    char    change              [   7];    char    _change              ;    // [string,    7] 대비                           StartPos 36, Length 7
    char    diff                [   6];    char    _diff                ;    // [string,    6] 등락율                         StartPos 44, Length 6
    char    volume              [  12];    char    _volume              ;    // [string,   12] 거래량                         StartPos 51, Length 12
    char    date                [   8];    char    _date                ;    // [string,    8] 일자                           StartPos 64, Length 8
} t1222OutBlock2, *LPt1222OutBlock2;
#define NAME_t1222OutBlock2     "t1222OutBlock2"

typedef struct _t1222OutBlock3
{
    char    hname               [  20];    char    _hname               ;    // [string,   20] 환율명                         StartPos 0, Length 20
    char    jisu                [  12];    char    _jisu                ;    // [string,   12] 지수                           StartPos 21, Length 12
    char    sign                [   1];    char    _sign                ;    // [string,    1] 대비구분                       StartPos 34, Length 1
    char    change              [   6];    char    _change              ;    // [string,    6] 대비                           StartPos 36, Length 6
    char    hname1              [  20];    char    _hname1              ;    // [string,   20] 환율명                         StartPos 43, Length 20
    char    jisu1               [  12];    char    _jisu1               ;    // [string,   12] 지수                           StartPos 64, Length 12
    char    sign1               [   1];    char    _sign1               ;    // [string,    1] 대비구분                       StartPos 77, Length 1
    char    change1             [   6];    char    _change1             ;    // [string,    6] 대비                           StartPos 79, Length 6
} t1222OutBlock3, *LPt1222OutBlock3;
#define NAME_t1222OutBlock3     "t1222OutBlock3"

typedef struct _t1222OutBlock4
{
    char    hname               [  20];    char    _hname               ;    // [string,   20] 투자주체명                     StartPos 0, Length 20
    char    val18               [  12];    char    _val18               ;    // [string,   12] 외국인                         StartPos 21, Length 12
    char    val17               [  12];    char    _val17               ;    // [string,   12] 기관계                         StartPos 34, Length 12
    char    val8                [  12];    char    _val8                ;    // [string,   12] 개인                           StartPos 47, Length 12
} t1222OutBlock4, *LPt1222OutBlock4;
#define NAME_t1222OutBlock4     "t1222OutBlock4"

typedef struct _t1222OutBlock5
{
    char    date                [   8];    char    _date                ;    // [string,    8] 일자                           StartPos 0, Length 8
    char    hname               [  20];    char    _hname               ;    // [string,   20] 증시명                         StartPos 9, Length 20
    char    jkval1              [  12];    char    _jkval1              ;    // [string,   12] 수치1                          StartPos 30, Length 12
    char    hname1              [  20];    char    _hname1              ;    // [string,   20] 증시명                         StartPos 43, Length 20
    char    jkval2              [   8];    char    _jkval2              ;    // [string,    8] 수치2                          StartPos 64, Length 8
} t1222OutBlock5, *LPt1222OutBlock5;
#define NAME_t1222OutBlock5     "t1222OutBlock5"


#pragma pack( pop )
///////////////////////////////////////////////////////////////////////////////////////////////////
