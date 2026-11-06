#pragma once

///////////////////////////////////////////////////////////////////////////////////////////////////
// 등락율상위 ( attr,block,headtype=A ) 
#pragma pack( push, 1 )

#define NAME_t1151     "t1151"

// 입력
typedef struct _t1151InBlock
{
    char    gubun1              [   1];    char    _gubun1              ;    // [string,    1] 구분                           StartPos 0, Length 1
    char    gubun2              [   1];    char    _gubun2              ;    // [string,    1] 상승하락                       StartPos 2, Length 1
    char    gubun3              [   1];    char    _gubun3              ;    // [string,    1] 당일전일                       StartPos 4, Length 1
    char    jc_num              [  12];    char    _jc_num              ;    // [string,   12] 대상제외                       StartPos 6, Length 12
    char    sprice              [   8];    char    _sprice              ;    // [string,    8] 시작가격                       StartPos 19, Length 8
    char    eprice              [   8];    char    _eprice              ;    // [string,    8] 종료가격                       StartPos 28, Length 8
    char    volume              [  12];    char    _volume              ;    // [string,   12] 거래량                         StartPos 37, Length 12
    char    idx                 [   4];    char    _idx                 ;    // [string,    4] IDX                            StartPos 50, Length 4
    char    exchange            [   1];    char    _exchange            ;    // [string,    1] 거래소구분                     StartPos 55, Length 1
} t1151InBlock, *LPt1151InBlock;
#define NAME_t1151InBlock     "t1151InBlock"

// 출력
typedef struct _t1151OutBlock
{
    char    idx                 [   4];    char    _idx                 ;    // [string,    4] IDX                            StartPos 0, Length 4
} t1151OutBlock, *LPt1151OutBlock;
#define NAME_t1151OutBlock     "t1151OutBlock"

typedef struct _t1151OutBlock1
{
    char    hname               [  40];    char    _hname               ;    // [string,   40] 한글명                         StartPos 0, Length 40
    char    price               [   8];    char    _price               ;    // [string,    8] 현재가                         StartPos 41, Length 8
    char    sign                [   1];    char    _sign                ;    // [string,    1] 전일대비구분                   StartPos 50, Length 1
    char    change              [   8];    char    _change              ;    // [string,    8] 전일대비                       StartPos 52, Length 8
    char    diff                [   6];    char    _diff                ;    // [string,    6] 등락율                         StartPos 61, Length 6
    char    volume              [  12];    char    _volume              ;    // [string,   12] 누적거래량                     StartPos 68, Length 12
    char    offerrem1           [  12];    char    _offerrem1           ;    // [string,   12] 매도잔량                       StartPos 81, Length 12
    char    offerho1            [  12];    char    _offerho1            ;    // [string,   12] 매도호가                       StartPos 94, Length 12
    char    bidho1              [  12];    char    _bidho1              ;    // [string,   12] 매수호가                       StartPos 107, Length 12
    char    bidrem1             [  12];    char    _bidrem1             ;    // [string,   12] 매수잔량                       StartPos 120, Length 12
    char    updaycnt            [   4];    char    _updaycnt            ;    // [string,    4] 연속                           StartPos 133, Length 4
    char    jnildiff            [   6];    char    _jnildiff            ;    // [string,    6] 전일등락율                     StartPos 138, Length 6
    char    shcode              [   6];    char    _shcode              ;    // [string,    6] 종목코드                       StartPos 145, Length 6
    char    open                [   8];    char    _open                ;    // [string,    8] 시가                           StartPos 152, Length 8
    char    high                [   8];    char    _high                ;    // [string,    8] 고가                           StartPos 161, Length 8
    char    low                 [   8];    char    _low                 ;    // [string,    8] 저가                           StartPos 170, Length 8
} t1151OutBlock1, *LPt1151OutBlock1;
#define NAME_t1151OutBlock1     "t1151OutBlock1"


#pragma pack( pop )
///////////////////////////////////////////////////////////////////////////////////////////////////
