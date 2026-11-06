#pragma once

///////////////////////////////////////////////////////////////////////////////////////////////////
// 상하한 ( attr,block,headtype=A ) 
#pragma pack( push, 1 )

#define NAME_t1122     "t1122"

// 입력
typedef struct _t1122InBlock
{
    char    qrygb               [   1];    char    _qrygb               ;    // [string,    1] 조회구분                       StartPos 0, Length 1
    char    gubun               [   1];    char    _gubun               ;    // [string,    1] 구분                           StartPos 2, Length 1
    char    jnilgubun           [   1];    char    _jnilgubun           ;    // [string,    1] 전일구분                       StartPos 4, Length 1
    char    sign                [   1];    char    _sign                ;    // [string,    1] 상하한구분                     StartPos 6, Length 1
    char    jc_num              [  12];    char    _jc_num              ;    // [string,   12] 대상제외                       StartPos 8, Length 12
    char    sprice              [   8];    char    _sprice              ;    // [string,    8] 시작가격                       StartPos 21, Length 8
    char    eprice              [   8];    char    _eprice              ;    // [string,    8] 종료가격                       StartPos 30, Length 8
    char    volume              [  12];    char    _volume              ;    // [string,   12] 거래량                         StartPos 39, Length 12
    char    idx                 [   4];    char    _idx                 ;    // [string,    4] IDX                            StartPos 52, Length 4
    char    exchange            [   1];    char    _exchange            ;    // [string,    1] 거래소구분                     StartPos 57, Length 1
} t1122InBlock, *LPt1122InBlock;
#define NAME_t1122InBlock     "t1122InBlock"

// 출력
typedef struct _t1122OutBlock
{
    char    cnt                 [   4];    char    _cnt                 ;    // [string,    4] CNT                            StartPos 0, Length 4
    char    idx                 [   4];    char    _idx                 ;    // [string,    4] IDX                            StartPos 5, Length 4
} t1122OutBlock, *LPt1122OutBlock;
#define NAME_t1122OutBlock     "t1122OutBlock"

typedef struct _t1122OutBlock1
{
    char    hname               [  20];    char    _hname               ;    // [string,   20] 한글명                         StartPos 0, Length 20
    char    price               [   8];    char    _price               ;    // [string,    8] 현재가                         StartPos 21, Length 8
    char    sign                [   1];    char    _sign                ;    // [string,    1] 전일대비구분                   StartPos 30, Length 1
    char    change              [   8];    char    _change              ;    // [string,    8] 전일대비                       StartPos 32, Length 8
    char    diff                [   6];    char    _diff                ;    // [string,    6] 등락율                         StartPos 41, Length 6
    char    volume              [  12];    char    _volume              ;    // [string,   12] 누적거래량                     StartPos 48, Length 12
    char    diff_vol            [  10];    char    _diff_vol            ;    // [string,   10] 거래증가율                     StartPos 61, Length 10
    char    offerrem1           [  12];    char    _offerrem1           ;    // [string,   12] 매도잔량                       StartPos 72, Length 12
    char    bidrem1             [  12];    char    _bidrem1             ;    // [string,   12] 매수잔량                       StartPos 85, Length 12
    char    last                [   6];    char    _last                ;    // [string,    6] 최종진입                       StartPos 98, Length 6
    char    lmtdaycnt           [   8];    char    _lmtdaycnt           ;    // [string,    8] 연속                           StartPos 105, Length 8
    char    jnilvolume          [  12];    char    _jnilvolume          ;    // [string,   12] 전일거래량                     StartPos 114, Length 12
    char    shcode              [   6];    char    _shcode              ;    // [string,    6] 종목코드                       StartPos 127, Length 6
} t1122OutBlock1, *LPt1122OutBlock1;
#define NAME_t1122OutBlock1     "t1122OutBlock1"


#pragma pack( pop )
///////////////////////////////////////////////////////////////////////////////////////////////////
