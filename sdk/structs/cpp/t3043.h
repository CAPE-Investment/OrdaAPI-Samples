#pragma once

///////////////////////////////////////////////////////////////////////////////////////////////////
// 옵션전광판 ( attr,block,headtype=A ) 
#pragma pack( push, 1 )

#define NAME_t3043     "t3043"

// 입력
typedef struct _t3043InBlock
{
    char    yyyymm              [   6];    char    _yyyymm              ;    // [string,    6] 월물                           StartPos 0, Length 6
} t3043InBlock, *LPt3043InBlock;
#define NAME_t3043InBlock     "t3043InBlock"

// 출력
typedef struct _t3043OutBlock
{
    char    histimpv            [   4];    char    _histimpv            ;    // [string,    4] 역사적변동성                   StartPos 0, Length 4
    char    jandatecnt          [   4];    char    _jandatecnt          ;    // [string,    4] 옵션잔존일                     StartPos 5, Length 4
    char    cimpv               [   6];    char    _cimpv               ;    // [string,    6] 콜옵션대표IV                   StartPos 10, Length 6
    char    pimpv               [   6];    char    _pimpv               ;    // [string,    6] 풋옵션대표IV                   StartPos 17, Length 6
    char    gmprice             [   6];    char    _gmprice             ;    // [string,    6] 근월물현재가                   StartPos 24, Length 6
    char    gmsign              [   1];    char    _gmsign              ;    // [string,    1] 근월물전일대비구분             StartPos 31, Length 1
    char    gmchange            [   6];    char    _gmchange            ;    // [string,    6] 근월물전일대비                 StartPos 33, Length 6
    char    gmdiff              [   6];    char    _gmdiff              ;    // [string,    6] 근월물등락율                   StartPos 40, Length 6
    char    gmvolume            [  12];    char    _gmvolume            ;    // [string,   12] 근월물거래량                   StartPos 47, Length 12
    char    gmshcode            [   8];    char    _gmshcode            ;    // [string,    8] 근월물선물코드                 StartPos 60, Length 8
} t3043OutBlock, *LPt3043OutBlock;
#define NAME_t3043OutBlock     "t3043OutBlock"

typedef struct _t3043OutBlock1
{
    char    actprice            [   6];    char    _actprice            ;    // [string,    6] 행사가                         StartPos 0, Length 6
    char    optcode             [   8];    char    _optcode             ;    // [string,    8] 콜옵션코드                     StartPos 7, Length 8
    char    price               [   6];    char    _price               ;    // [string,    6] 현재가                         StartPos 16, Length 6
    char    sign                [   1];    char    _sign                ;    // [string,    1] 전일대비구분                   StartPos 23, Length 1
    char    change              [   6];    char    _change              ;    // [string,    6] 전일대비                       StartPos 25, Length 6
    char    diff                [   6];    char    _diff                ;    // [string,    6] 등락율                         StartPos 32, Length 6
    char    volume              [  12];    char    _volume              ;    // [string,   12] 거래량                         StartPos 39, Length 12
    char    iv                  [   6];    char    _iv                  ;    // [string,    6] IV                             StartPos 52, Length 6
    char    mgjv                [  12];    char    _mgjv                ;    // [string,   12] 미결제약정                     StartPos 59, Length 12
    char    mgjvupdn            [  12];    char    _mgjvupdn            ;    // [string,   12] 미결제약정증감                 StartPos 72, Length 12
    char    offerho1            [   6];    char    _offerho1            ;    // [string,    6] 매도호가                       StartPos 85, Length 6
    char    bidho1              [   6];    char    _bidho1              ;    // [string,    6] 매수호가                       StartPos 92, Length 6
    char    cvolume             [  12];    char    _cvolume             ;    // [string,   12] 체결량                         StartPos 99, Length 12
    char    delt                [   6];    char    _delt                ;    // [string,    6] 델타                           StartPos 112, Length 6
    char    gama                [   6];    char    _gama                ;    // [string,    6] 감마                           StartPos 119, Length 6
    char    vega                [   6];    char    _vega                ;    // [string,    6] 베가                           StartPos 126, Length 6
    char    ceta                [   6];    char    _ceta                ;    // [string,    6] 쎄타                           StartPos 133, Length 6
    char    rhox                [   6];    char    _rhox                ;    // [string,    6] 로우                           StartPos 140, Length 6
    char    theoryprice         [   6];    char    _theoryprice         ;    // [string,    6] 이론가                         StartPos 147, Length 6
    char    impv                [   6];    char    _impv                ;    // [string,    6] 내재가치                       StartPos 154, Length 6
    char    timevl              [   6];    char    _timevl              ;    // [string,    6] 시간가치                       StartPos 161, Length 6
    char    jvolume             [  12];    char    _jvolume             ;    // [string,   12] 잔고수량                       StartPos 168, Length 12
    char    parpl               [  12];    char    _parpl               ;    // [string,   12] 평가손익                       StartPos 181, Length 12
    char    jngo                [   6];    char    _jngo                ;    // [string,    6] 청산가능수량                   StartPos 194, Length 6
    char    offerrem1           [  12];    char    _offerrem1           ;    // [string,   12] 매도잔량                       StartPos 201, Length 12
    char    bidrem1             [  12];    char    _bidrem1             ;    // [string,   12] 매수잔량                       StartPos 214, Length 12
    char    open                [   6];    char    _open                ;    // [string,    6] 시가                           StartPos 227, Length 6
    char    high                [   6];    char    _high                ;    // [string,    6] 고가                           StartPos 234, Length 6
    char    low                 [   6];    char    _low                 ;    // [string,    6] 저가                           StartPos 241, Length 6
    char    atmgubun            [   1];    char    _atmgubun            ;    // [string,    1] ATM구분                        StartPos 248, Length 1
    char    jisuconv            [  10];    char    _jisuconv            ;    // [string,   10] 지수환산                       StartPos 250, Length 10
    char    value               [  12];    char    _value               ;    // [string,   12] 거래대금                       StartPos 261, Length 12
    char    yeprice             [  12];    char    _yeprice             ;    // [string,   12] 예상체결가                     StartPos 274, Length 12
    char    yechange            [   6];    char    _yechange            ;    // [string,    6] 예상전일비                     StartPos 287, Length 6
    char    yediff              [   6];    char    _yediff              ;    // [string,    6] 예상등락율                     StartPos 294, Length 6
    char    yevolume            [  12];    char    _yevolume            ;    // [string,   12] 예상체결량                     StartPos 301, Length 12
} t3043OutBlock1, *LPt3043OutBlock1;
#define NAME_t3043OutBlock1     "t3043OutBlock1"

typedef struct _t3043OutBlock2
{
    char    actprice            [   6];    char    _actprice            ;    // [string,    6] 행사가                         StartPos 0, Length 6
    char    optcode             [   8];    char    _optcode             ;    // [string,    8] 풋옵션코드                     StartPos 7, Length 8
    char    price               [   6];    char    _price               ;    // [string,    6] 현재가                         StartPos 16, Length 6
    char    sign                [   1];    char    _sign                ;    // [string,    1] 전일대비구분                   StartPos 23, Length 1
    char    change              [   6];    char    _change              ;    // [string,    6] 전일대비                       StartPos 25, Length 6
    char    diff                [   6];    char    _diff                ;    // [string,    6] 등락율                         StartPos 32, Length 6
    char    volume              [  12];    char    _volume              ;    // [string,   12] 거래량                         StartPos 39, Length 12
    char    iv                  [   6];    char    _iv                  ;    // [string,    6] IV                             StartPos 52, Length 6
    char    mgjv                [  12];    char    _mgjv                ;    // [string,   12] 미결제약정                     StartPos 59, Length 12
    char    mgjvupdn            [  12];    char    _mgjvupdn            ;    // [string,   12] 미결제약정증감                 StartPos 72, Length 12
    char    offerho1            [   6];    char    _offerho1            ;    // [string,    6] 매도호가                       StartPos 85, Length 6
    char    bidho1              [   6];    char    _bidho1              ;    // [string,    6] 매수호가                       StartPos 92, Length 6
    char    cvolume             [  12];    char    _cvolume             ;    // [string,   12] 체결량                         StartPos 99, Length 12
    char    delt                [   6];    char    _delt                ;    // [string,    6] 델타                           StartPos 112, Length 6
    char    gama                [   6];    char    _gama                ;    // [string,    6] 감마                           StartPos 119, Length 6
    char    vega                [   6];    char    _vega                ;    // [string,    6] 베가                           StartPos 126, Length 6
    char    ceta                [   6];    char    _ceta                ;    // [string,    6] 쎄타                           StartPos 133, Length 6
    char    rhox                [   6];    char    _rhox                ;    // [string,    6] 로우                           StartPos 140, Length 6
    char    theoryprice         [   6];    char    _theoryprice         ;    // [string,    6] 이론가                         StartPos 147, Length 6
    char    impv                [   6];    char    _impv                ;    // [string,    6] 내재가치                       StartPos 154, Length 6
    char    timevl              [   6];    char    _timevl              ;    // [string,    6] 시간가치                       StartPos 161, Length 6
    char    jvolume             [  12];    char    _jvolume             ;    // [string,   12] 잔고수량                       StartPos 168, Length 12
    char    parpl               [  12];    char    _parpl               ;    // [string,   12] 평가손익                       StartPos 181, Length 12
    char    jngo                [   6];    char    _jngo                ;    // [string,    6] 청산가능수량                   StartPos 194, Length 6
    char    offerrem1           [  12];    char    _offerrem1           ;    // [string,   12] 매도잔량                       StartPos 201, Length 12
    char    bidrem1             [  12];    char    _bidrem1             ;    // [string,   12] 매수잔량                       StartPos 214, Length 12
    char    open                [   6];    char    _open                ;    // [string,    6] 시가                           StartPos 227, Length 6
    char    high                [   6];    char    _high                ;    // [string,    6] 고가                           StartPos 234, Length 6
    char    low                 [   6];    char    _low                 ;    // [string,    6] 저가                           StartPos 241, Length 6
    char    atmgubun            [   1];    char    _atmgubun            ;    // [string,    1] ATM구분                        StartPos 248, Length 1
    char    jisuconv            [  10];    char    _jisuconv            ;    // [string,   10] 지수환산                       StartPos 250, Length 10
    char    value               [  12];    char    _value               ;    // [string,   12] 거래대금                       StartPos 261, Length 12
    char    yeprice             [  12];    char    _yeprice             ;    // [string,   12] 예상체결가                     StartPos 274, Length 12
    char    yechange            [   6];    char    _yechange            ;    // [string,    6] 예상전일비                     StartPos 287, Length 6
    char    yediff              [   6];    char    _yediff              ;    // [string,    6] 예상등락율                     StartPos 294, Length 6
    char    yevolume            [  12];    char    _yevolume            ;    // [string,   12] 예상체결량                     StartPos 301, Length 12
} t3043OutBlock2, *LPt3043OutBlock2;
#define NAME_t3043OutBlock2     "t3043OutBlock2"


#pragma pack( pop )
///////////////////////////////////////////////////////////////////////////////////////////////////
