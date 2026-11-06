#pragma once

///////////////////////////////////////////////////////////////////////////////////////////////////
// 초저유동성종목조회 ( attr,block,headtype=A ) 
#pragma pack( push, 1 )

#define NAME_t1205     "t1205"

// 입력
typedef struct _t1205InBlock
{
    char    gubun1              [   1];    char    _gubun1              ;    // [string,    1] 구분                           StartPos 0, Length 1
    char    custno              [   6];    char    _custno              ;    // [string,    6] 종목번호                       StartPos 2, Length 6
    char    idx                 [   4];    char    _idx                 ;    // [string,    4] IDX                            StartPos 9, Length 4
} t1205InBlock, *LPt1205InBlock;
#define NAME_t1205InBlock     "t1205InBlock"

// 출력
typedef struct _t1205OutBlock
{
    char    idx                 [   4];    char    _idx                 ;    // [string,    4] IDX                            StartPos 0, Length 4
} t1205OutBlock, *LPt1205OutBlock;
#define NAME_t1205OutBlock     "t1205OutBlock"

typedef struct _t1205OutBlock1
{
    char    hname               [  20];    char    _hname               ;    // [string,   20] 한글명                         StartPos 0, Length 20
    char    price               [   8];    char    _price               ;    // [string,    8] 현재가                         StartPos 21, Length 8
    char    sign                [   1];    char    _sign                ;    // [string,    1] 대비구분                       StartPos 30, Length 1
    char    change              [   8];    char    _change              ;    // [string,    8] 대비                           StartPos 32, Length 8
    char    diff                [   6];    char    _diff                ;    // [string,    6] 등락율                         StartPos 41, Length 6
    char    yeprice             [   8];    char    _yeprice             ;    // [string,    8] 예상체결가                     StartPos 48, Length 8
    char    ysign               [   1];    char    _ysign               ;    // [string,    1] 예상전일대비구분               StartPos 57, Length 1
    char    ychange             [   8];    char    _ychange             ;    // [string,    8] 예상전일대비                   StartPos 59, Length 8
    char    ydiff               [   6];    char    _ydiff               ;    // [string,    6] 예상등락율                     StartPos 68, Length 6
    char    yevolume            [  12];    char    _yevolume            ;    // [string,   12] 예상체결량                     StartPos 75, Length 12
    char    volume              [  12];    char    _volume              ;    // [string,   12] 누적거래량                     StartPos 88, Length 12
    char    date                [   8];    char    _date                ;    // [string,    8] 지정일                         StartPos 101, Length 8
    char    shcode              [   6];    char    _shcode              ;    // [string,    6] 종목코드                       StartPos 110, Length 6
} t1205OutBlock1, *LPt1205OutBlock1;
#define NAME_t1205OutBlock1     "t1205OutBlock1"


#pragma pack( pop )
///////////////////////////////////////////////////////////////////////////////////////////////////
