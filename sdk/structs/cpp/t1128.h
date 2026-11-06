#pragma once

///////////////////////////////////////////////////////////////////////////////////////////////////
// 상하한가 직전 ( attr,block,headtype=A ) 
#pragma pack( push, 1 )

#define NAME_t1128     "t1128"

// 입력
typedef struct _t1128InBlock
{
    char    qrygb               [   1];    char    _qrygb               ;    // [string,    1] 조회구분                       StartPos 0, Length 1
    char    gubun               [   1];    char    _gubun               ;    // [string,    1] 구분                           StartPos 2, Length 1
    char    signgubun           [   1];    char    _signgubun           ;    // [string,    1] 상하한가구분                   StartPos 4, Length 1
    char    diff                [   3];    char    _diff                ;    // [string,    3] 등락율                         StartPos 6, Length 3
    char    jc_num              [  12];    char    _jc_num              ;    // [string,   12] 대상제외                       StartPos 10, Length 12
    char    sprice              [   8];    char    _sprice              ;    // [string,    8] 시작가격                       StartPos 23, Length 8
    char    eprice              [   8];    char    _eprice              ;    // [string,    8] 종료가격                       StartPos 32, Length 8
    char    volume              [  12];    char    _volume              ;    // [string,   12] 거래량                         StartPos 41, Length 12
    char    idx                 [   4];    char    _idx                 ;    // [string,    4] IDX                            StartPos 54, Length 4
} t1128InBlock, *LPt1128InBlock;
#define NAME_t1128InBlock     "t1128InBlock"

// 출력
typedef struct _t1128OutBlock
{
    char    cnt                 [   4];    char    _cnt                 ;    // [string,    4] CNT                            StartPos 0, Length 4
    char    idx                 [   4];    char    _idx                 ;    // [string,    4] IDX                            StartPos 5, Length 4
} t1128OutBlock, *LPt1128OutBlock;
#define NAME_t1128OutBlock     "t1128OutBlock"

typedef struct _t1128OutBlock1
{
    char    hname               [  20];    char    _hname               ;    // [string,   20] 한글명                         StartPos 0, Length 20
    char    price               [   8];    char    _price               ;    // [string,    8] 현재가                         StartPos 21, Length 8
    char    sign                [   1];    char    _sign                ;    // [string,    1] 전일대비구분                   StartPos 30, Length 1
    char    change              [   8];    char    _change              ;    // [string,    8] 전일대비                       StartPos 32, Length 8
    char    diff                [   6];    char    _diff                ;    // [string,    6] 등락율                         StartPos 41, Length 6
    char    volume              [  12];    char    _volume              ;    // [string,   12] 누적거래량                     StartPos 48, Length 12
    char    diff_vol            [  10];    char    _diff_vol            ;    // [string,   10] 거래증가율                     StartPos 61, Length 10
    char    lmtprice            [   8];    char    _lmtprice            ;    // [string,    8] 상한가/하한가                  StartPos 72, Length 8
    char    rate                [  12];    char    _rate                ;    // [string,   12] 대비율                         StartPos 81, Length 12
    char    shcode              [   6];    char    _shcode              ;    // [string,    6] 종목코드                       StartPos 94, Length 6
    char    jnilvolume          [  12];    char    _jnilvolume          ;    // [string,   12] 전일거래량                     StartPos 101, Length 12
} t1128OutBlock1, *LPt1128OutBlock1;
#define NAME_t1128OutBlock1     "t1128OutBlock1"


#pragma pack( pop )
///////////////////////////////////////////////////////////////////////////////////////////////////
