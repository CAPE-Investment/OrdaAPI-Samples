#pragma once

///////////////////////////////////////////////////////////////////////////////////////////////////
// 증거금율별종목조회 ( attr,block,headtype=A ) 
#pragma pack( push, 1 )

#define NAME_t1107     "t1107"

// 입력
typedef struct _t1107InBlock
{
    char    gubun               [   1];    char    _gubun               ;    // [string,    1] 시장구분                       StartPos 0, Length 1
    char    jongchk             [   1];    char    _jongchk             ;    // [string,    1] 위탁신용구분                   StartPos 2, Length 1
    char    jkrate              [   1];    char    _jkrate              ;    // [string,    1] 증거금율구분                   StartPos 4, Length 1
    char    shcode              [   6];    char    _shcode              ;    // [string,    6] 종목코드                       StartPos 6, Length 6
    char    idx                 [   4];    char    _idx                 ;    // [string,    4] IDX                            StartPos 13, Length 4
} t1107InBlock, *LPt1107InBlock;
#define NAME_t1107InBlock     "t1107InBlock"

// 출력
typedef struct _t1107OutBlock
{
    char    jkrate              [   3];    char    _jkrate              ;    // [string,    3] 위탁증거금율                   StartPos 0, Length 3
    char    sjkrate             [   3];    char    _sjkrate             ;    // [string,    3] 신용증거금율                   StartPos 4, Length 3
    char    idx                 [   4];    char    _idx                 ;    // [string,    4] IDX                            StartPos 8, Length 4
} t1107OutBlock, *LPt1107OutBlock;
#define NAME_t1107OutBlock     "t1107OutBlock"

typedef struct _t1107OutBlock1
{
    char    shcode              [   6];    char    _shcode              ;    // [string,    6] 종목코드                       StartPos 0, Length 6
    char    hname               [  20];    char    _hname               ;    // [string,   20] 종목명                         StartPos 7, Length 20
    char    jkrate              [   3];    char    _jkrate              ;    // [string,    3] 위탁증거금율                   StartPos 28, Length 3
    char    sjkrate             [   3];    char    _sjkrate             ;    // [string,    3] 신용증거금율                   StartPos 32, Length 3
    char    subprice            [   8];    char    _subprice            ;    // [string,    8] 대용가                         StartPos 36, Length 8
    char    recprice            [   8];    char    _recprice            ;    // [string,    8] 전일종가                       StartPos 45, Length 8
    char    price               [   8];    char    _price               ;    // [string,    8] 현재가                         StartPos 54, Length 8
    char    sign                [   1];    char    _sign                ;    // [string,    1] 전일대비구분                   StartPos 63, Length 1
    char    change              [   8];    char    _change              ;    // [string,    8] 전일대비                       StartPos 65, Length 8
    char    diff                [   6];    char    _diff                ;    // [string,    6] 등락율                         StartPos 74, Length 6
    char    volume              [  12];    char    _volume              ;    // [string,   12] 누적거래량                     StartPos 81, Length 12
} t1107OutBlock1, *LPt1107OutBlock1;
#define NAME_t1107OutBlock1     "t1107OutBlock1"


#pragma pack( pop )
///////////////////////////////////////////////////////////////////////////////////////////////////
