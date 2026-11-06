#pragma once

///////////////////////////////////////////////////////////////////////////////////////////////////
// 관심종목현재가 조회 ( attr,block,headtype=A ) 
#pragma pack( push, 1 )

#define NAME_s1032     "s1032"

// 입력
typedef struct _s1032InBlock
{
    char    nrec                [   4];    char    _nrec                ;    // [string,    4] 건수                           StartPos 0, Length 4
} s1032InBlock, *LPs1032InBlock;
#define NAME_s1032InBlock     "s1032InBlock"

typedef struct _s1032InBlock1
{
    char    smktgb              [   2];    char    _smktgb              ;    // [string,    2] 시장구분(KR:국내JP:일본HK:홍콩US:미국) StartPos 0, Length 2
    char    gubun               [   2];    char    _gubun               ;    // [string,    2] 종목구분(1:거주2:코주3:ELW4:선물5:옵션6:지수7:주선8.스선) StartPos 3, Length 2
    char    shcode              [  16];    char    _shcode              ;    // [string,   16] 종목코드                       StartPos 6, Length 16
    char    exchange            [   1];    char    _exchange            ;    // [string,    1] 거래소구분                     StartPos 23, Length 1
} s1032InBlock1, *LPs1032InBlock1;
#define NAME_s1032InBlock1     "s1032InBlock1"

// 출력
typedef struct _s1032OutBlock
{
    char    smktgb              [   2];    char    _smktgb              ;    // [string,    2] 시장구분                       StartPos 0, Length 2
    char    gubun               [   2];    char    _gubun               ;    // [string,    2] 구분                           StartPos 3, Length 2
    char    shcode              [  16];    char    _shcode              ;    // [string,   16] 종목코드                       StartPos 6, Length 16
    char    hname               [  40];    char    _hname               ;    // [string,   40] 종목명                         StartPos 23, Length 40
    char    price               [  10];    char    _price               ;    // [string,   10] 현재가                         StartPos 64, Length 10
    char    sign                [   1];    char    _sign                ;    // [string,    1] 전일대비구분                   StartPos 75, Length 1
    char    change              [  10];    char    _change              ;    // [string,   10] 전일대비                       StartPos 77, Length 10
    char    diff                [   6];    char    _diff                ;    // [string,    6] 등락율                         StartPos 88, Length 6
    char    volume              [  12];    char    _volume              ;    // [string,   12] 누적거래량                     StartPos 95, Length 12
    char    offerho             [  10];    char    _offerho             ;    // [string,   10] 매도호가                       StartPos 108, Length 10
    char    bidho               [  10];    char    _bidho               ;    // [string,   10] 매수호가                       StartPos 119, Length 10
    char    cvolume             [   8];    char    _cvolume             ;    // [string,    8] 체결수량                       StartPos 130, Length 8
    char    chdegree            [   9];    char    _chdegree            ;    // [string,    9] 체결강도                       StartPos 139, Length 9
    char    frnrate             [   6];    char    _frnrate             ;    // [string,    6] 외국인소진율                   StartPos 149, Length 6
    char    open                [  10];    char    _open                ;    // [string,   10] 시가                           StartPos 156, Length 10
    char    high                [  10];    char    _high                ;    // [string,   10] 고가                           StartPos 167, Length 10
    char    low                 [  10];    char    _low                 ;    // [string,   10] 저가                           StartPos 178, Length 10
    char    value               [  12];    char    _value               ;    // [string,   12] 거래대금(백만)                 StartPos 189, Length 12
    char    offerrem            [  12];    char    _offerrem            ;    // [string,   12] 매도잔량                       StartPos 202, Length 12
    char    bidrem              [  12];    char    _bidrem              ;    // [string,   12] 매수잔량                       StartPos 215, Length 12
    char    totofferrem         [  12];    char    _totofferrem         ;    // [string,   12] 총매도잔량                     StartPos 228, Length 12
    char    totbidrem           [  12];    char    _totbidrem           ;    // [string,   12] 총매수잔량                     StartPos 241, Length 12
    char    perx                [   8];    char    _perx                ;    // [string,    8] PER                            StartPos 254, Length 8
    char    epsx                [   8];    char    _epsx                ;    // [string,    8] EPS                            StartPos 263, Length 8
    char    pbrx                [   8];    char    _pbrx                ;    // [string,    8] PBR                            StartPos 272, Length 8
    char    elwexec             [   8];    char    _elwexec             ;    // [string,    8] 행사가                         StartPos 281, Length 8
    char    convrate            [   8];    char    _convrate            ;    // [string,    8] 전환비율                       StartPos 290, Length 8
    char    lastdate            [   8];    char    _lastdate            ;    // [string,    8] 최종거래일                     StartPos 299, Length 8
    char    jandatecnt          [   8];    char    _jandatecnt          ;    // [string,    8] 잔존일수                       StartPos 308, Length 8
    char    parity              [   6];    char    _parity              ;    // [string,    6] 패리티                         StartPos 317, Length 6
    char    preminum            [  10];    char    _preminum            ;    // [string,   10] 프리미엄                       StartPos 324, Length 10
    char    berate              [   6];    char    _berate              ;    // [string,    6] 손익분기율                     StartPos 335, Length 6
    char    capt                [   6];    char    _capt                ;    // [string,    6] 자본지지율                     StartPos 342, Length 6
    char    gearing             [   6];    char    _gearing             ;    // [string,    6] 기어링                         StartPos 349, Length 6
    char    egearing            [   6];    char    _egearing            ;    // [string,    6] e기어링                        StartPos 356, Length 6
    char    itemcode            [   6];    char    _itemcode            ;    // [string,    6] 기초자산코드                   StartPos 363, Length 6
    char    itemprice           [  10];    char    _itemprice           ;    // [string,   10] 기초자산현재가                 StartPos 370, Length 10
    char    itemsign            [   1];    char    _itemsign            ;    // [string,    1] 기초자산전일대비구분           StartPos 381, Length 1
    char    itemchange          [  10];    char    _itemchange          ;    // [string,   10] 기초자산전일대비               StartPos 383, Length 10
    char    itemdiff            [   6];    char    _itemdiff            ;    // [string,    6] 기초자산등락율                 StartPos 394, Length 6
    char    itemvolume          [  12];    char    _itemvolume          ;    // [string,   12] 기초자산거래량                 StartPos 401, Length 12
    char    theoryprice         [  10];    char    _theoryprice         ;    // [string,   10] 이론가                         StartPos 414, Length 10
    char    glyl                [   6];    char    _glyl                ;    // [string,    6] 괴리율                         StartPos 425, Length 6
    char    iv                  [   6];    char    _iv                  ;    // [string,    6] IV                             StartPos 432, Length 6
    char    mgjv                [  12];    char    _mgjv                ;    // [string,   12] 미결제약정                     StartPos 439, Length 12
    char    delt                [   6];    char    _delt                ;    // [string,    6] 델타                           StartPos 452, Length 6
    char    gama                [   6];    char    _gama                ;    // [string,    6] 감마                           StartPos 459, Length 6
    char    vega                [   6];    char    _vega                ;    // [string,    6] 베가                           StartPos 466, Length 6
    char    ceta                [   6];    char    _ceta                ;    // [string,    6] 쎄타                           StartPos 473, Length 6
    char    rhox                [   6];    char    _rhox                ;    // [string,    6] 로우                           StartPos 480, Length 6
    char    impv                [   6];    char    _impv                ;    // [string,    6] 내재가치                       StartPos 487, Length 6
    char    timevl              [   6];    char    _timevl              ;    // [string,    6] 시간가치                       StartPos 494, Length 6
    char    jniltimevolrate     [  10];    char    _jniltimevolrate     ;    // [string,   10] 전일동시간대비거래증가율       StartPos 501, Length 10
    char    offerho2            [  10];    char    _offerho2            ;    // [string,   10] 매도호가2                      StartPos 512, Length 10
    char    bidho2              [  10];    char    _bidho2              ;    // [string,   10] 매수호가2                      StartPos 523, Length 10
    char    offerho3            [  10];    char    _offerho3            ;    // [string,   10] 매도호가3                      StartPos 534, Length 10
    char    bidho3              [  10];    char    _bidho3              ;    // [string,   10] 매수호가3                      StartPos 545, Length 10
    char    uplmtprice          [  10];    char    _uplmtprice          ;    // [string,   10] 상한가                         StartPos 556, Length 10
    char    dnlmtprice          [  10];    char    _dnlmtprice          ;    // [string,   10] 하한가                         StartPos 567, Length 10
    char    yeprice             [  10];    char    _yeprice             ;    // [string,   10] 예상체결가                     StartPos 578, Length 10
    char    yesign              [   1];    char    _yesign              ;    // [string,    1] 예상전일대비구분               StartPos 589, Length 1
    char    yechange            [  10];    char    _yechange            ;    // [string,   10] 예상전일대비                   StartPos 591, Length 10
    char    yediff              [   6];    char    _yediff              ;    // [string,    6] 예상등락율                     StartPos 602, Length 6
    char    yevolume            [  12];    char    _yevolume            ;    // [string,   12] 예상체결량                     StartPos 609, Length 12
    char    yofferho0           [   8];    char    _yofferho0           ;    // [string,    8] 예상매도호가                   StartPos 622, Length 8
    char    ybidho0             [   8];    char    _ybidho0             ;    // [string,    8] 예상매수호가                   StartPos 631, Length 8
    char    yofferrem0          [  12];    char    _yofferrem0          ;    // [string,   12] 예상매도잔량                   StartPos 640, Length 12
    char    ybidrem0            [  12];    char    _ybidrem0            ;    // [string,   12] 예상매수잔량                   StartPos 653, Length 12
    char    sigatot             [  15];    char    _sigatot             ;    // [string,   15] 시가총액                       StartPos 666, Length 15
    char    jnilvolume          [  12];    char    _jnilvolume          ;    // [string,   12] 전일거래량                     StartPos 682, Length 12
    char    jvolrate            [   6];    char    _jvolrate            ;    // [string,    6] 전일대비거래비율               StartPos 695, Length 6
    char    openrate            [   6];    char    _openrate            ;    // [string,    6] 시가%                          StartPos 702, Length 6
    char    highrate            [   6];    char    _highrate            ;    // [string,    6] 고가%                          StartPos 709, Length 6
    char    lowrate             [   6];    char    _lowrate             ;    // [string,    6] 저가%                          StartPos 716, Length 6
    char    high52wrate         [   6];    char    _high52wrate         ;    // [string,    6] 52주최고가대비%                StartPos 723, Length 6
    char    low52wrate          [   6];    char    _low52wrate          ;    // [string,    6] 52주최저가대비%                StartPos 730, Length 6
    char    listrate            [   6];    char    _listrate            ;    // [string,    6] 회전율                         StartPos 737, Length 6
    char    yjrate              [   6];    char    _yjrate              ;    // [string,    6] 신용융자잔고비율               StartPos 744, Length 6
    char    jgubun              [   4];    char    _jgubun              ;    // [string,    4] 종목분류                       StartPos 751, Length 4
} s1032OutBlock, *LPs1032OutBlock;
#define NAME_s1032OutBlock     "s1032OutBlock"


#pragma pack( pop )
///////////////////////////////////////////////////////////////////////////////////////////////////
