#pragma once

///////////////////////////////////////////////////////////////////////////////////////////////////
// ELW 현재가 조회 ( attr,block,headtype=A ) 
#pragma pack( push, 1 )

#define NAME_s1512     "s1512"

// 입력
typedef struct _s1512InBlock
{
    char    shcode              [   6];    char    _shcode              ;    // [string,    6] ELW 단축코드                   StartPos 0, Length 6
} s1512InBlock, *LPs1512InBlock;
#define NAME_s1512InBlock     "s1512InBlock"

// 출력
typedef struct _s1512OutBlock
{
    char    stdcode             [  12];    char    _stdcode             ;    // [string,   12] 표준코드                       StartPos 0, Length 12
    char    shcode              [   6];    char    _shcode              ;    // [string,    6] 단축코드                       StartPos 13, Length 6
    char    hname               [  40];    char    _hname               ;    // [string,   40] 한글명                         StartPos 20, Length 40
    char    chetime             [  10];    char    _chetime             ;    // [string,   10] 체결시간                       StartPos 61, Length 10
    char    price               [   8];    char    _price               ;    // [string,    8] 현재가                         StartPos 72, Length 8
    char    sign                [   1];    char    _sign                ;    // [string,    1] 전일대비구분                   StartPos 81, Length 1
    char    change              [   8];    char    _change              ;    // [string,    8] 전일대비                       StartPos 83, Length 8
    char    diff                [   6];    char    _diff                ;    // [string,    6] 등락율                         StartPos 92, Length 6
    char    cvolume             [  10];    char    _cvolume             ;    // [string,   10] 체결량                         StartPos 99, Length 10
    char    volume              [  12];    char    _volume              ;    // [string,   12] 누적거래량                     StartPos 110, Length 12
    char    recprice            [   8];    char    _recprice            ;    // [string,    8] 기준가                         StartPos 123, Length 8
    char    avg                 [   8];    char    _avg                 ;    // [string,    8] 가중평균                       StartPos 132, Length 8
    char    jnilvolume          [  12];    char    _jnilvolume          ;    // [string,   12] 전일거래량                     StartPos 141, Length 12
    char    jvolume             [  12];    char    _jvolume             ;    // [string,   12] 전일동시간거래량               StartPos 154, Length 12
    char    jnilclose           [   8];    char    _jnilclose           ;    // [string,    8] 전일종가                       StartPos 167, Length 8
    char    volumechg           [  12];    char    _volumechg           ;    // [string,   12] 거래량차                       StartPos 176, Length 12
    char    volumediff          [   6];    char    _volumediff          ;    // [string,    6] 거래량차등락율                 StartPos 189, Length 6
    char    open                [   8];    char    _open                ;    // [string,    8] 시가                           StartPos 196, Length 8
    char    odiff               [   6];    char    _odiff               ;    // [string,    6] 시가등락율                     StartPos 205, Length 6
    char    opentime            [   6];    char    _opentime            ;    // [string,    6] 시가시간                       StartPos 212, Length 6
    char    high                [   8];    char    _high                ;    // [string,    8] 고가                           StartPos 219, Length 8
    char    hdiff               [   6];    char    _hdiff               ;    // [string,    6] 고가등락율                     StartPos 228, Length 6
    char    hightime            [   6];    char    _hightime            ;    // [string,    6] 고가시간                       StartPos 235, Length 6
    char    low                 [   8];    char    _low                 ;    // [string,    8] 저가                           StartPos 242, Length 8
    char    ldiff               [   6];    char    _ldiff               ;    // [string,    6] 저가등락율                     StartPos 251, Length 6
    char    lowtime             [   6];    char    _lowtime             ;    // [string,    6] 저가시간                       StartPos 258, Length 6
    char    high52w             [   8];    char    _high52w             ;    // [string,    8] 52최고가                       StartPos 265, Length 8
    char    high52wdiff         [   6];    char    _high52wdiff         ;    // [string,    6] 52최고가등락율                 StartPos 274, Length 6
    char    high52wdate         [   8];    char    _high52wdate         ;    // [string,    8] 52최고가일                     StartPos 281, Length 8
    char    low52w              [   8];    char    _low52w              ;    // [string,    8] 52최저가                       StartPos 290, Length 8
    char    low52wdiff          [   6];    char    _low52wdiff          ;    // [string,    6] 52최저가등락율                 StartPos 299, Length 6
    char    low52wdate          [   8];    char    _low52wdate          ;    // [string,    8] 52최저가일                     StartPos 306, Length 8
    char    exhratio            [   6];    char    _exhratio            ;    // [string,    6] 소진율                         StartPos 315, Length 6
    char    listing             [  12];    char    _listing             ;    // [string,   12] 상장주식수(천)                 StartPos 322, Length 12
    char    memedan             [   5];    char    _memedan             ;    // [string,    5] 수량단위                       StartPos 335, Length 5
    char    vol                 [   6];    char    _vol                 ;    // [string,    6] 회전율                         StartPos 341, Length 6
    char    parity              [   8];    char    _parity              ;    // [string,    8] 패리티                         StartPos 348, Length 8
    char    berate              [   8];    char    _berate              ;    // [string,    8] 손익분기                       StartPos 357, Length 8
    char    gearing             [   8];    char    _gearing             ;    // [string,    8] 기어링                         StartPos 366, Length 8
    char    elwexec             [   8];    char    _elwexec             ;    // [string,    8] 행사가                         StartPos 375, Length 8
    char    issueprice          [   8];    char    _issueprice          ;    // [string,    8] 발행가                         StartPos 384, Length 8
    char    convrate            [  12];    char    _convrate            ;    // [string,   12] 전환비율                       StartPos 393, Length 12
    char    lastdate            [   8];    char    _lastdate            ;    // [string,    8] 최종거래일                     StartPos 406, Length 8
    char    capt                [   8];    char    _capt                ;    // [string,    8] 자본지지                       StartPos 415, Length 8
    char    egearing            [   8];    char    _egearing            ;    // [string,    8] e.기어링                       StartPos 424, Length 8
    char    premium             [   8];    char    _premium             ;    // [string,    8] 프리미엄                       StartPos 433, Length 8
    char    spread              [   6];    char    _spread              ;    // [string,    6] 스프레드                       StartPos 442, Length 6
    char    espread             [   6];    char    _espread             ;    // [string,    6] 최대스프레드                   StartPos 449, Length 6
    char    theoryprice         [  10];    char    _theoryprice         ;    // [string,   10] 이론가                         StartPos 456, Length 10
    char    impv                [   6];    char    _impv                ;    // [string,    6] 내재변동성                     StartPos 467, Length 6
    char    moneyness           [   1];    char    _moneyness           ;    // [string,    1] 상태                           StartPos 474, Length 1
    char    delt                [   8];    char    _delt                ;    // [string,    8] 델타                           StartPos 476, Length 8
    char    gama                [   8];    char    _gama                ;    // [string,    8] 감마                           StartPos 485, Length 8
    char    vega                [  13];    char    _vega                ;    // [string,   13] 베가                           StartPos 494, Length 13
    char    ceta                [  13];    char    _ceta                ;    // [string,   13] 쎄타                           StartPos 508, Length 13
    char    rhox                [  13];    char    _rhox                ;    // [string,   13] 로                             StartPos 522, Length 13
    char    bjandatecnt         [   4];    char    _bjandatecnt         ;    // [string,    4] 잔존일수                       StartPos 536, Length 4
    char    mmsdate             [   8];    char    _mmsdate             ;    // [string,    8] 행사개시일                     StartPos 541, Length 8
    char    mmedate             [   8];    char    _mmedate             ;    // [string,    8] 행사종료일                     StartPos 550, Length 8
    char    payday              [   8];    char    _payday              ;    // [string,    8] 지급일                         StartPos 559, Length 8
    char    listdate            [   8];    char    _listdate            ;    // [string,    8] 발행일                         StartPos 568, Length 8
    char    lpmem               [  20];    char    _lpmem               ;    // [string,   20] LP회원사                       StartPos 577, Length 20
    char    lp_holdvol          [  12];    char    _lp_holdvol          ;    // [string,   12] LP보유수량                     StartPos 598, Length 12
    char    bcode               [   6];    char    _bcode               ;    // [string,    6] 기초자산코드                   StartPos 611, Length 6
    char    bgubun              [   1];    char    _bgubun              ;    // [string,    1] 기초자산구분                   StartPos 618, Length 1
    char    bprice              [   8];    char    _bprice              ;    // [string,    8] 기초자산현재가                 StartPos 620, Length 8
    char    bsign               [   1];    char    _bsign               ;    // [string,    1] 기초자산전일비구분             StartPos 629, Length 1
    char    bchange             [   8];    char    _bchange             ;    // [string,    8] 기초자산전일비                 StartPos 631, Length 8
    char    bdiff               [   6];    char    _bdiff               ;    // [string,    6] 기초자산등락율                 StartPos 640, Length 6
    char    bvolume             [  12];    char    _bvolume             ;    // [string,   12] 기초자산거래량                 StartPos 647, Length 12
    char    info1               [  10];    char    _info1               ;    // [string,   10] 락구분                         StartPos 660, Length 10
    char    info2               [  10];    char    _info2               ;    // [string,   10] 관리/급등구분                  StartPos 671, Length 10
    char    info3               [  10];    char    _info3               ;    // [string,   10] 정지/연장구분                  StartPos 682, Length 10
    char    info4               [  12];    char    _info4               ;    // [string,   12] 투자/불성실구분                StartPos 693, Length 12
    char    janginfo            [  10];    char    _janginfo            ;    // [string,   10] 장구분                         StartPos 706, Length 10
    char    basketgb            [   1];    char    _basketgb            ;    // [string,    1] 바스켓구분                     StartPos 717, Length 1
    char    basketcnt           [   3];    char    _basketcnt           ;    // [string,    3] 바스켓갯수                     StartPos 719, Length 3
    char    elwtype             [   2];    char    _elwtype             ;    // [string,    2] ELW권리행사방식                StartPos 723, Length 2
    char    settletype          [   2];    char    _settletype          ;    // [string,    2] ELW결제방법                    StartPos 726, Length 2
    char    lpord               [   2];    char    _lpord               ;    // [string,    2] LP사주문가능여부               StartPos 729, Length 2
    char    elwdetail           [ 100];    char    _elwdetail           ;    // [string,    0] 권리내용                       StartPos 732, Length 0
    char    valuation           [ 100];    char    _valuation           ;    // [string,    0] 만기평가가격방식               StartPos 733, Length 0
    char    info5               [  10];    char    _info5               ;    // [string,   10] 배분정보구분                   StartPos 734, Length 10
} s1512OutBlock, *LPs1512OutBlock;
#define NAME_s1512OutBlock     "s1512OutBlock"

typedef struct _s1512OutBlock1
{
    char    hname               [  40];    char    _hname               ;    // [string,   40] 한글명                         StartPos 0, Length 40
    char    price               [   8];    char    _price               ;    // [string,    8] 현재가                         StartPos 41, Length 8
    char    sign                [   1];    char    _sign                ;    // [string,    1] 전일대비구분                   StartPos 50, Length 1
    char    change              [   8];    char    _change              ;    // [string,    8] 전일대비                       StartPos 52, Length 8
    char    diff                [   6];    char    _diff                ;    // [string,    6] 등락율                         StartPos 61, Length 6
    char    volume              [  12];    char    _volume              ;    // [string,   12] 누적거래량                     StartPos 68, Length 12
    char    jnilclose           [   8];    char    _jnilclose           ;    // [string,    8] 전일종가                       StartPos 81, Length 8
    char    offerho1            [   8];    char    _offerho1            ;    // [string,    8] 매도호가1                      StartPos 90, Length 8
    char    bidho1              [   8];    char    _bidho1              ;    // [string,    8] 매수호가1                      StartPos 99, Length 8
    char    offerrem1           [  12];    char    _offerrem1           ;    // [string,   12] 매도호가수량1                  StartPos 108, Length 12
    char    lp_offerrem1        [  12];    char    _lp_offerrem1        ;    // [string,   12] LP매도호가수량1                StartPos 121, Length 12
    char    bidrem1             [  12];    char    _bidrem1             ;    // [string,   12] 매수호가수량1                  StartPos 134, Length 12
    char    lp_bidrem1          [  12];    char    _lp_bidrem1          ;    // [string,   12] LP매수호가수량1                StartPos 147, Length 12
    char    preoffercha1        [  12];    char    _preoffercha1        ;    // [string,   12] 직전매도대비수량1              StartPos 160, Length 12
    char    prebidcha1          [  12];    char    _prebidcha1          ;    // [string,   12] 직전매수대비수량1              StartPos 173, Length 12
    char    offerho2            [   8];    char    _offerho2            ;    // [string,    8] 매도호가2                      StartPos 186, Length 8
    char    bidho2              [   8];    char    _bidho2              ;    // [string,    8] 매수호가2                      StartPos 195, Length 8
    char    offerrem2           [  12];    char    _offerrem2           ;    // [string,   12] 매도호가수량2                  StartPos 204, Length 12
    char    lp_offerrem2        [  12];    char    _lp_offerrem2        ;    // [string,   12] LP매도호가수량2                StartPos 217, Length 12
    char    bidrem2             [  12];    char    _bidrem2             ;    // [string,   12] 매수호가수량2                  StartPos 230, Length 12
    char    lp_bidrem2          [  12];    char    _lp_bidrem2          ;    // [string,   12] LP매수호가수량2                StartPos 243, Length 12
    char    preoffercha2        [  12];    char    _preoffercha2        ;    // [string,   12] 직전매도대비수량2              StartPos 256, Length 12
    char    prebidcha2          [  12];    char    _prebidcha2          ;    // [string,   12] 직전매수대비수량2              StartPos 269, Length 12
    char    offerho3            [   8];    char    _offerho3            ;    // [string,    8] 매도호가3                      StartPos 282, Length 8
    char    bidho3              [   8];    char    _bidho3              ;    // [string,    8] 매수호가3                      StartPos 291, Length 8
    char    offerrem3           [  12];    char    _offerrem3           ;    // [string,   12] 매도호가수량3                  StartPos 300, Length 12
    char    lp_offerrem3        [  12];    char    _lp_offerrem3        ;    // [string,   12] LP매도호가수량3                StartPos 313, Length 12
    char    bidrem3             [  12];    char    _bidrem3             ;    // [string,   12] 매수호가수량3                  StartPos 326, Length 12
    char    lp_bidrem3          [  12];    char    _lp_bidrem3          ;    // [string,   12] LP매수호가수량3                StartPos 339, Length 12
    char    preoffercha3        [  12];    char    _preoffercha3        ;    // [string,   12] 직전매도대비수량3              StartPos 352, Length 12
    char    prebidcha3          [  12];    char    _prebidcha3          ;    // [string,   12] 직전매수대비수량3              StartPos 365, Length 12
    char    offerho4            [   8];    char    _offerho4            ;    // [string,    8] 매도호가4                      StartPos 378, Length 8
    char    bidho4              [   8];    char    _bidho4              ;    // [string,    8] 매수호가4                      StartPos 387, Length 8
    char    offerrem4           [  12];    char    _offerrem4           ;    // [string,   12] 매도호가수량4                  StartPos 396, Length 12
    char    lp_offerrem4        [  12];    char    _lp_offerrem4        ;    // [string,   12] LP매도호가수량4                StartPos 409, Length 12
    char    bidrem4             [  12];    char    _bidrem4             ;    // [string,   12] 매수호가수량4                  StartPos 422, Length 12
    char    lp_bidrem4          [  12];    char    _lp_bidrem4          ;    // [string,   12] LP매수호가수량4                StartPos 435, Length 12
    char    preoffercha4        [  12];    char    _preoffercha4        ;    // [string,   12] 직전매도대비수량4              StartPos 448, Length 12
    char    prebidcha4          [  12];    char    _prebidcha4          ;    // [string,   12] 직전매수대비수량4              StartPos 461, Length 12
    char    offerho5            [   8];    char    _offerho5            ;    // [string,    8] 매도호가5                      StartPos 474, Length 8
    char    bidho5              [   8];    char    _bidho5              ;    // [string,    8] 매수호가5                      StartPos 483, Length 8
    char    offerrem5           [  12];    char    _offerrem5           ;    // [string,   12] 매도호가수량5                  StartPos 492, Length 12
    char    lp_offerrem5        [  12];    char    _lp_offerrem5        ;    // [string,   12] LP매도호가수량5                StartPos 505, Length 12
    char    bidrem5             [  12];    char    _bidrem5             ;    // [string,   12] 매수호가수량5                  StartPos 518, Length 12
    char    lp_bidrem5          [  12];    char    _lp_bidrem5          ;    // [string,   12] LP매수호가수량5                StartPos 531, Length 12
    char    preoffercha5        [  12];    char    _preoffercha5        ;    // [string,   12] 직전매도대비수량5              StartPos 544, Length 12
    char    prebidcha5          [  12];    char    _prebidcha5          ;    // [string,   12] 직전매수대비수량5              StartPos 557, Length 12
    char    offerho6            [   8];    char    _offerho6            ;    // [string,    8] 매도호가6                      StartPos 570, Length 8
    char    bidho6              [   8];    char    _bidho6              ;    // [string,    8] 매수호가6                      StartPos 579, Length 8
    char    offerrem6           [  12];    char    _offerrem6           ;    // [string,   12] 매도호가수량6                  StartPos 588, Length 12
    char    lp_offerrem6        [  12];    char    _lp_offerrem6        ;    // [string,   12] LP매도호가수량6                StartPos 601, Length 12
    char    bidrem6             [  12];    char    _bidrem6             ;    // [string,   12] 매수호가수량6                  StartPos 614, Length 12
    char    lp_bidrem6          [  12];    char    _lp_bidrem6          ;    // [string,   12] LP매수호가수량6                StartPos 627, Length 12
    char    preoffercha6        [  12];    char    _preoffercha6        ;    // [string,   12] 직전매도대비수량6              StartPos 640, Length 12
    char    prebidcha6          [  12];    char    _prebidcha6          ;    // [string,   12] 직전매수대비수량6              StartPos 653, Length 12
    char    offerho7            [   8];    char    _offerho7            ;    // [string,    8] 매도호가7                      StartPos 666, Length 8
    char    bidho7              [   8];    char    _bidho7              ;    // [string,    8] 매수호가7                      StartPos 675, Length 8
    char    offerrem7           [  12];    char    _offerrem7           ;    // [string,   12] 매도호가수량7                  StartPos 684, Length 12
    char    lp_offerrem7        [  12];    char    _lp_offerrem7        ;    // [string,   12] LP매도호가수량7                StartPos 697, Length 12
    char    bidrem7             [  12];    char    _bidrem7             ;    // [string,   12] 매수호가수량7                  StartPos 710, Length 12
    char    lp_bidrem7          [  12];    char    _lp_bidrem7          ;    // [string,   12] LP매수호가수량7                StartPos 723, Length 12
    char    preoffercha7        [  12];    char    _preoffercha7        ;    // [string,   12] 직전매도대비수량7              StartPos 736, Length 12
    char    prebidcha7          [  12];    char    _prebidcha7          ;    // [string,   12] 직전매수대비수량7              StartPos 749, Length 12
    char    offerho8            [   8];    char    _offerho8            ;    // [string,    8] 매도호가8                      StartPos 762, Length 8
    char    bidho8              [   8];    char    _bidho8              ;    // [string,    8] 매수호가8                      StartPos 771, Length 8
    char    offerrem8           [  12];    char    _offerrem8           ;    // [string,   12] 매도호가수량8                  StartPos 780, Length 12
    char    lp_offerrem8        [  12];    char    _lp_offerrem8        ;    // [string,   12] LP매도호가수량8                StartPos 793, Length 12
    char    bidrem8             [  12];    char    _bidrem8             ;    // [string,   12] 매수호가수량8                  StartPos 806, Length 12
    char    lp_bidrem8          [  12];    char    _lp_bidrem8          ;    // [string,   12] LP매수호가수량8                StartPos 819, Length 12
    char    preoffercha8        [  12];    char    _preoffercha8        ;    // [string,   12] 직전매도대비수량8              StartPos 832, Length 12
    char    prebidcha8          [  12];    char    _prebidcha8          ;    // [string,   12] 직전매수대비수량8              StartPos 845, Length 12
    char    offerho9            [   8];    char    _offerho9            ;    // [string,    8] 매도호가9                      StartPos 858, Length 8
    char    bidho9              [   8];    char    _bidho9              ;    // [string,    8] 매수호가9                      StartPos 867, Length 8
    char    offerrem9           [  12];    char    _offerrem9           ;    // [string,   12] 매도호가수량9                  StartPos 876, Length 12
    char    lp_offerrem9        [  12];    char    _lp_offerrem9        ;    // [string,   12] LP매도호가수량9                StartPos 889, Length 12
    char    bidrem9             [  12];    char    _bidrem9             ;    // [string,   12] 매수호가수량9                  StartPos 902, Length 12
    char    lp_bidrem9          [  12];    char    _lp_bidrem9          ;    // [string,   12] LP매수호가수량9                StartPos 915, Length 12
    char    preoffercha9        [  12];    char    _preoffercha9        ;    // [string,   12] 직전매도대비수량9              StartPos 928, Length 12
    char    prebidcha9          [  12];    char    _prebidcha9          ;    // [string,   12] 직전매수대비수량9              StartPos 941, Length 12
    char    offerho10           [   8];    char    _offerho10           ;    // [string,    8] 매도호가10                     StartPos 954, Length 8
    char    bidho10             [   8];    char    _bidho10             ;    // [string,    8] 매수호가10                     StartPos 963, Length 8
    char    offerrem10          [  12];    char    _offerrem10          ;    // [string,   12] 매도호가수량10                 StartPos 972, Length 12
    char    lp_offerrem10       [  12];    char    _lp_offerrem10       ;    // [string,   12] LP매도호가수량10               StartPos 985, Length 12
    char    bidrem10            [  12];    char    _bidrem10            ;    // [string,   12] 매수호가수량10                 StartPos 998, Length 12
    char    lp_bidrem10         [  12];    char    _lp_bidrem10         ;    // [string,   12] LP매수호가수량10               StartPos 1011, Length 12
    char    preoffercha10       [  12];    char    _preoffercha10       ;    // [string,   12] 직전매도대비수량10             StartPos 1024, Length 12
    char    prebidcha10         [  12];    char    _prebidcha10         ;    // [string,   12] 직전매수대비수량10             StartPos 1037, Length 12
    char    offer               [  12];    char    _offer               ;    // [string,   12] 매도호가수량합                 StartPos 1050, Length 12
    char    bid                 [  12];    char    _bid                 ;    // [string,   12] 매수호가수량합                 StartPos 1063, Length 12
    char    preoffercha         [  12];    char    _preoffercha         ;    // [string,   12] 직전매도대비수량합             StartPos 1076, Length 12
    char    prebidcha           [  12];    char    _prebidcha           ;    // [string,   12] 직전매수대비수량합             StartPos 1089, Length 12
    char    hotime              [   8];    char    _hotime              ;    // [string,    8] 수신시간                       StartPos 1102, Length 8
    char    yeprice             [   8];    char    _yeprice             ;    // [string,    8] 예상체결가격                   StartPos 1111, Length 8
    char    yevolume            [  12];    char    _yevolume            ;    // [string,   12] 예상체결수량                   StartPos 1120, Length 12
    char    yesign              [   1];    char    _yesign              ;    // [string,    1] 예상체결전일구분               StartPos 1133, Length 1
    char    yechange            [   8];    char    _yechange            ;    // [string,    8] 예상체결전일대비               StartPos 1135, Length 8
    char    yediff              [   6];    char    _yediff              ;    // [string,    6] 예상체결등락율                 StartPos 1144, Length 6
    char    tmoffer             [  12];    char    _tmoffer             ;    // [string,   12] 시간외매도잔량                 StartPos 1151, Length 12
    char    tmbid               [  12];    char    _tmbid               ;    // [string,   12] 시간외매수잔량                 StartPos 1164, Length 12
    char    ho_status           [   1];    char    _ho_status           ;    // [string,    1] 동시구분                       StartPos 1177, Length 1
    char    open                [   8];    char    _open                ;    // [string,    8] 시가                           StartPos 1179, Length 8
    char    high                [   8];    char    _high                ;    // [string,    8] 고가                           StartPos 1188, Length 8
    char    low                 [   8];    char    _low                 ;    // [string,    8] 저가                           StartPos 1197, Length 8
} s1512OutBlock1, *LPs1512OutBlock1;
#define NAME_s1512OutBlock1     "s1512OutBlock1"

typedef struct _s1512OutBlock2
{
    char    bskstdcode          [  12];    char    _bskstdcode          ;    // [string,   12] 기초자산표준코드               StartPos 0, Length 12
    char    bskcode             [   6];    char    _bskcode             ;    // [string,    6] 기초자산코드                   StartPos 13, Length 6
    char    bskbno              [   3];    char    _bskbno              ;    // [string,    3] 기초자산비율                   StartPos 20, Length 3
    char    bskprice            [   8];    char    _bskprice            ;    // [string,    8] 기초자산현재가                 StartPos 24, Length 8
    char    bsksign             [   1];    char    _bsksign             ;    // [string,    1] 기초자산전일비구분             StartPos 33, Length 1
    char    bskchange           [   8];    char    _bskchange           ;    // [string,    8] 기초자산전일비                 StartPos 35, Length 8
    char    bskdiff             [   6];    char    _bskdiff             ;    // [string,    6] 기초자산등락율                 StartPos 44, Length 6
    char    bskvolume           [  12];    char    _bskvolume           ;    // [string,   12] 기초자산거래량                 StartPos 51, Length 12
    char    bskjnilclose        [   8];    char    _bskjnilclose        ;    // [string,    8] 기초자산전일종가               StartPos 64, Length 8
} s1512OutBlock2, *LPs1512OutBlock2;
#define NAME_s1512OutBlock2     "s1512OutBlock2"

typedef struct _s1512OutBlock3
{
    char    hname               [  20];    char    _hname               ;    // [string,   20] 한글명                         StartPos 0, Length 20
    char    price               [   8];    char    _price               ;    // [string,    8] 현재가                         StartPos 21, Length 8
    char    sign                [   1];    char    _sign                ;    // [string,    1] 전일대비구분                   StartPos 30, Length 1
    char    change              [   8];    char    _change              ;    // [string,    8] 전일대비                       StartPos 32, Length 8
    char    diff                [   6];    char    _diff                ;    // [string,    6] 등락율                         StartPos 41, Length 6
    char    volume              [  12];    char    _volume              ;    // [string,   12] 누적거래량                     StartPos 48, Length 12
    char    jnilclose           [   8];    char    _jnilclose           ;    // [string,    8] 전일종가                       StartPos 61, Length 8
    char    offerho1            [   8];    char    _offerho1            ;    // [string,    8] 매도호가1                      StartPos 70, Length 8
    char    bidho1              [   8];    char    _bidho1              ;    // [string,    8] 매수호가1                      StartPos 79, Length 8
    char    offerrem1           [  12];    char    _offerrem1           ;    // [string,   12] 매도호가수량1                  StartPos 88, Length 12
    char    bidrem1             [  12];    char    _bidrem1             ;    // [string,   12] 매수호가수량1                  StartPos 101, Length 12
    char    preoffercha1        [  12];    char    _preoffercha1        ;    // [string,   12] 직전매도대비수량1              StartPos 114, Length 12
    char    prebidcha1          [  12];    char    _prebidcha1          ;    // [string,   12] 직전매수대비수량1              StartPos 127, Length 12
    char    offerho2            [   8];    char    _offerho2            ;    // [string,    8] 매도호가2                      StartPos 140, Length 8
    char    bidho2              [   8];    char    _bidho2              ;    // [string,    8] 매수호가2                      StartPos 149, Length 8
    char    offerrem2           [  12];    char    _offerrem2           ;    // [string,   12] 매도호가수량2                  StartPos 158, Length 12
    char    bidrem2             [  12];    char    _bidrem2             ;    // [string,   12] 매수호가수량2                  StartPos 171, Length 12
    char    preoffercha2        [  12];    char    _preoffercha2        ;    // [string,   12] 직전매도대비수량2              StartPos 184, Length 12
    char    prebidcha2          [  12];    char    _prebidcha2          ;    // [string,   12] 직전매수대비수량2              StartPos 197, Length 12
    char    offerho3            [   8];    char    _offerho3            ;    // [string,    8] 매도호가3                      StartPos 210, Length 8
    char    bidho3              [   8];    char    _bidho3              ;    // [string,    8] 매수호가3                      StartPos 219, Length 8
    char    offerrem3           [  12];    char    _offerrem3           ;    // [string,   12] 매도호가수량3                  StartPos 228, Length 12
    char    bidrem3             [  12];    char    _bidrem3             ;    // [string,   12] 매수호가수량3                  StartPos 241, Length 12
    char    preoffercha3        [  12];    char    _preoffercha3        ;    // [string,   12] 직전매도대비수량3              StartPos 254, Length 12
    char    prebidcha3          [  12];    char    _prebidcha3          ;    // [string,   12] 직전매수대비수량3              StartPos 267, Length 12
    char    offerho4            [   8];    char    _offerho4            ;    // [string,    8] 매도호가4                      StartPos 280, Length 8
    char    bidho4              [   8];    char    _bidho4              ;    // [string,    8] 매수호가4                      StartPos 289, Length 8
    char    offerrem4           [  12];    char    _offerrem4           ;    // [string,   12] 매도호가수량4                  StartPos 298, Length 12
    char    bidrem4             [  12];    char    _bidrem4             ;    // [string,   12] 매수호가수량4                  StartPos 311, Length 12
    char    preoffercha4        [  12];    char    _preoffercha4        ;    // [string,   12] 직전매도대비수량4              StartPos 324, Length 12
    char    prebidcha4          [  12];    char    _prebidcha4          ;    // [string,   12] 직전매수대비수량4              StartPos 337, Length 12
    char    offerho5            [   8];    char    _offerho5            ;    // [string,    8] 매도호가5                      StartPos 350, Length 8
    char    bidho5              [   8];    char    _bidho5              ;    // [string,    8] 매수호가5                      StartPos 359, Length 8
    char    offerrem5           [  12];    char    _offerrem5           ;    // [string,   12] 매도호가수량5                  StartPos 368, Length 12
    char    bidrem5             [  12];    char    _bidrem5             ;    // [string,   12] 매수호가수량5                  StartPos 381, Length 12
    char    preoffercha5        [  12];    char    _preoffercha5        ;    // [string,   12] 직전매도대비수량5              StartPos 394, Length 12
    char    prebidcha5          [  12];    char    _prebidcha5          ;    // [string,   12] 직전매수대비수량5              StartPos 407, Length 12
    char    offerho6            [   8];    char    _offerho6            ;    // [string,    8] 매도호가6                      StartPos 420, Length 8
    char    bidho6              [   8];    char    _bidho6              ;    // [string,    8] 매수호가6                      StartPos 429, Length 8
    char    offerrem6           [  12];    char    _offerrem6           ;    // [string,   12] 매도호가수량6                  StartPos 438, Length 12
    char    bidrem6             [  12];    char    _bidrem6             ;    // [string,   12] 매수호가수량6                  StartPos 451, Length 12
    char    preoffercha6        [  12];    char    _preoffercha6        ;    // [string,   12] 직전매도대비수량6              StartPos 464, Length 12
    char    prebidcha6          [  12];    char    _prebidcha6          ;    // [string,   12] 직전매수대비수량6              StartPos 477, Length 12
    char    offerho7            [   8];    char    _offerho7            ;    // [string,    8] 매도호가7                      StartPos 490, Length 8
    char    bidho7              [   8];    char    _bidho7              ;    // [string,    8] 매수호가7                      StartPos 499, Length 8
    char    offerrem7           [  12];    char    _offerrem7           ;    // [string,   12] 매도호가수량7                  StartPos 508, Length 12
    char    bidrem7             [  12];    char    _bidrem7             ;    // [string,   12] 매수호가수량7                  StartPos 521, Length 12
    char    preoffercha7        [  12];    char    _preoffercha7        ;    // [string,   12] 직전매도대비수량7              StartPos 534, Length 12
    char    prebidcha7          [  12];    char    _prebidcha7          ;    // [string,   12] 직전매수대비수량7              StartPos 547, Length 12
    char    offerho8            [   8];    char    _offerho8            ;    // [string,    8] 매도호가8                      StartPos 560, Length 8
    char    bidho8              [   8];    char    _bidho8              ;    // [string,    8] 매수호가8                      StartPos 569, Length 8
    char    offerrem8           [  12];    char    _offerrem8           ;    // [string,   12] 매도호가수량8                  StartPos 578, Length 12
    char    bidrem8             [  12];    char    _bidrem8             ;    // [string,   12] 매수호가수량8                  StartPos 591, Length 12
    char    preoffercha8        [  12];    char    _preoffercha8        ;    // [string,   12] 직전매도대비수량8              StartPos 604, Length 12
    char    prebidcha8          [  12];    char    _prebidcha8          ;    // [string,   12] 직전매수대비수량8              StartPos 617, Length 12
    char    offerho9            [   8];    char    _offerho9            ;    // [string,    8] 매도호가9                      StartPos 630, Length 8
    char    bidho9              [   8];    char    _bidho9              ;    // [string,    8] 매수호가9                      StartPos 639, Length 8
    char    offerrem9           [  12];    char    _offerrem9           ;    // [string,   12] 매도호가수량9                  StartPos 648, Length 12
    char    bidrem9             [  12];    char    _bidrem9             ;    // [string,   12] 매수호가수량9                  StartPos 661, Length 12
    char    preoffercha9        [  12];    char    _preoffercha9        ;    // [string,   12] 직전매도대비수량9              StartPos 674, Length 12
    char    prebidcha9          [  12];    char    _prebidcha9          ;    // [string,   12] 직전매수대비수량9              StartPos 687, Length 12
    char    offerho10           [   8];    char    _offerho10           ;    // [string,    8] 매도호가10                     StartPos 700, Length 8
    char    bidho10             [   8];    char    _bidho10             ;    // [string,    8] 매수호가10                     StartPos 709, Length 8
    char    offerrem10          [  12];    char    _offerrem10          ;    // [string,   12] 매도호가수량10                 StartPos 718, Length 12
    char    bidrem10            [  12];    char    _bidrem10            ;    // [string,   12] 매수호가수량10                 StartPos 731, Length 12
    char    preoffercha10       [  12];    char    _preoffercha10       ;    // [string,   12] 직전매도대비수량10             StartPos 744, Length 12
    char    prebidcha10         [  12];    char    _prebidcha10         ;    // [string,   12] 직전매수대비수량10             StartPos 757, Length 12
    char    offer               [  12];    char    _offer               ;    // [string,   12] 매도호가수량합                 StartPos 770, Length 12
    char    bid                 [  12];    char    _bid                 ;    // [string,   12] 매수호가수량합                 StartPos 783, Length 12
    char    preoffercha         [  12];    char    _preoffercha         ;    // [string,   12] 직전매도대비수량합             StartPos 796, Length 12
    char    prebidcha           [  12];    char    _prebidcha           ;    // [string,   12] 직전매수대비수량합             StartPos 809, Length 12
    char    hotime              [   8];    char    _hotime              ;    // [string,    8] 수신시간                       StartPos 822, Length 8
    char    yeprice             [   8];    char    _yeprice             ;    // [string,    8] 예상체결가격                   StartPos 831, Length 8
    char    yevolume            [  12];    char    _yevolume            ;    // [string,   12] 예상체결수량                   StartPos 840, Length 12
    char    yesign              [   1];    char    _yesign              ;    // [string,    1] 예상체결전일구분               StartPos 853, Length 1
    char    yechange            [   8];    char    _yechange            ;    // [string,    8] 예상체결전일대비               StartPos 855, Length 8
    char    yediff              [   6];    char    _yediff              ;    // [string,    6] 예상체결등락율                 StartPos 864, Length 6
    char    tmoffer             [  12];    char    _tmoffer             ;    // [string,   12] 시간외매도잔량                 StartPos 871, Length 12
    char    tmbid               [  12];    char    _tmbid               ;    // [string,   12] 시간외매수잔량                 StartPos 884, Length 12
    char    ho_status           [   1];    char    _ho_status           ;    // [string,    1] 동시구분                       StartPos 897, Length 1
    char    shcode              [   6];    char    _shcode              ;    // [string,    6] 단축코드                       StartPos 899, Length 6
    char    uplmtprice          [   8];    char    _uplmtprice          ;    // [string,    8] 상한가                         StartPos 906, Length 8
    char    dnlmtprice          [   8];    char    _dnlmtprice          ;    // [string,    8] 하한가                         StartPos 915, Length 8
    char    open                [   8];    char    _open                ;    // [string,    8] 시가                           StartPos 924, Length 8
    char    high                [   8];    char    _high                ;    // [string,    8] 고가                           StartPos 933, Length 8
    char    low                 [   8];    char    _low                 ;    // [string,    8] 저가                           StartPos 942, Length 8
} s1512OutBlock3, *LPs1512OutBlock3;
#define NAME_s1512OutBlock3     "s1512OutBlock3"


#pragma pack( pop )
///////////////////////////////////////////////////////////////////////////////////////////////////
