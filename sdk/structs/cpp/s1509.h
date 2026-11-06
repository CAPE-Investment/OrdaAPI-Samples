#pragma once

///////////////////////////////////////////////////////////////////////////////////////////////////
// ETF LP호가 ( attr,block,headtype=A ) 
#pragma pack( push, 1 )

#define NAME_s1509     "s1509"

// 입력
typedef struct _s1509InBlock
{
    char    shcode              [   6];    char    _shcode              ;    // [string,    6] 단축코드                       StartPos 0, Length 6
} s1509InBlock, *LPs1509InBlock;
#define NAME_s1509InBlock     "s1509InBlock"

// 출력
typedef struct _s1509OutBlock
{
    char    hname               [  20];    char    _hname               ;    // [string,   20] 한글명                         StartPos 0, Length 20
    char    price               [   8];    char    _price               ;    // [string,    8] 현재가                         StartPos 21, Length 8
    char    sign                [   1];    char    _sign                ;    // [string,    1] 전일대비구분                   StartPos 30, Length 1
    char    change              [   8];    char    _change              ;    // [string,    8] 전일대비                       StartPos 32, Length 8
    char    diff                [   6];    char    _diff                ;    // [string,    6] 등락율                         StartPos 41, Length 6
    char    volume              [  12];    char    _volume              ;    // [string,   12] 누적거래량                     StartPos 48, Length 12
    char    lp_offerrem1        [  12];    char    _lp_offerrem1        ;    // [string,   12] LP매도호가수량1                StartPos 61, Length 12
    char    lp_bidrem1          [  12];    char    _lp_bidrem1          ;    // [string,   12] LP매수호가수량1                StartPos 74, Length 12
    char    lp_offerrem2        [  12];    char    _lp_offerrem2        ;    // [string,   12] LP매도호가수량2                StartPos 87, Length 12
    char    lp_bidrem2          [  12];    char    _lp_bidrem2          ;    // [string,   12] LP매수호가수량2                StartPos 100, Length 12
    char    lp_offerrem3        [  12];    char    _lp_offerrem3        ;    // [string,   12] LP매도호가수량3                StartPos 113, Length 12
    char    lp_bidrem3          [  12];    char    _lp_bidrem3          ;    // [string,   12] LP매수호가수량3                StartPos 126, Length 12
    char    lp_offerrem4        [  12];    char    _lp_offerrem4        ;    // [string,   12] LP매도호가수량4                StartPos 139, Length 12
    char    lp_bidrem4          [  12];    char    _lp_bidrem4          ;    // [string,   12] LP매수호가수량4                StartPos 152, Length 12
    char    lp_offerrem5        [  12];    char    _lp_offerrem5        ;    // [string,   12] LP매도호가수량5                StartPos 165, Length 12
    char    lp_bidrem5          [  12];    char    _lp_bidrem5          ;    // [string,   12] LP매수호가수량5                StartPos 178, Length 12
    char    lp_offerrem6        [  12];    char    _lp_offerrem6        ;    // [string,   12] LP매도호가수량6                StartPos 191, Length 12
    char    lp_bidrem6          [  12];    char    _lp_bidrem6          ;    // [string,   12] LP매수호가수량6                StartPos 204, Length 12
    char    lp_offerrem7        [  12];    char    _lp_offerrem7        ;    // [string,   12] LP매도호가수량7                StartPos 217, Length 12
    char    lp_bidrem7          [  12];    char    _lp_bidrem7          ;    // [string,   12] LP매수호가수량7                StartPos 230, Length 12
    char    lp_offerrem8        [  12];    char    _lp_offerrem8        ;    // [string,   12] LP매도호가수량8                StartPos 243, Length 12
    char    lp_bidrem8          [  12];    char    _lp_bidrem8          ;    // [string,   12] LP매수호가수량8                StartPos 256, Length 12
    char    lp_offerrem9        [  12];    char    _lp_offerrem9        ;    // [string,   12] LP매도호가수량9                StartPos 269, Length 12
    char    lp_bidrem9          [  12];    char    _lp_bidrem9          ;    // [string,   12] LP매수호가수량9                StartPos 282, Length 12
    char    lp_offerrem10       [  12];    char    _lp_offerrem10       ;    // [string,   12] LP매도호가수량10               StartPos 295, Length 12
    char    lp_bidrem10         [  12];    char    _lp_bidrem10         ;    // [string,   12] LP매수호가수량10               StartPos 308, Length 12
} s1509OutBlock, *LPs1509OutBlock;
#define NAME_s1509OutBlock     "s1509OutBlock"


#pragma pack( pop )
///////////////////////////////////////////////////////////////////////////////////////////////////
