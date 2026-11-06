#pragma once

///////////////////////////////////////////////////////////////////////////////////////////////////
// 업종현재가 ( attr,block,headtype=A ) 
#pragma pack( push, 1 )

#define NAME_t1253     "t1253"

// 입력
typedef struct _t1253InBlock
{
    char    upcode              [   3];    char    _upcode              ;    // [string,    3] 업종코드                       StartPos 0, Length 3
} t1253InBlock, *LPt1253InBlock;
#define NAME_t1253InBlock     "t1253InBlock"

// 출력
typedef struct _t1253OutBlock
{
    char    gubun               [   1];    char    _gubun               ;    // [string,    1] 업종구분                       StartPos 0, Length 1
    char    hname               [  20];    char    _hname               ;    // [string,   20] 업종명                         StartPos 2, Length 20
    char    pricejisu           [   7];    char    _pricejisu           ;    // [string,    7] 현재지수                       StartPos 23, Length 7
    char    jniljisu            [   7];    char    _jniljisu            ;    // [string,    7] 전일지수                       StartPos 31, Length 7
    char    sign                [   1];    char    _sign                ;    // [string,    1] 전일대비구분                   StartPos 39, Length 1
    char    change              [   7];    char    _change              ;    // [string,    7] 전일대비                       StartPos 41, Length 7
    char    diffjisu            [   6];    char    _diffjisu            ;    // [string,    6] 지수등락율                     StartPos 49, Length 6
    char    jnilvolume          [  12];    char    _jnilvolume          ;    // [string,   12] 전일거래량                     StartPos 56, Length 12
    char    volume              [  12];    char    _volume              ;    // [string,   12] 당일거래량                     StartPos 69, Length 12
    char    volumechange        [  12];    char    _volumechange        ;    // [string,   12] 거래량전일대비                 StartPos 82, Length 12
    char    volumerate          [   6];    char    _volumerate          ;    // [string,    6] 거래량비율                     StartPos 95, Length 6
    char    jnilvalue           [  12];    char    _jnilvalue           ;    // [string,   12] 전일거래대금                   StartPos 102, Length 12
    char    value               [  12];    char    _value               ;    // [string,   12] 당일거래대금                   StartPos 115, Length 12
    char    valuechange         [  12];    char    _valuechange         ;    // [string,   12] 거래대금전일대비               StartPos 128, Length 12
    char    valuerate           [   6];    char    _valuerate           ;    // [string,    6] 거래대금비율                   StartPos 141, Length 6
    char    openjisu            [   7];    char    _openjisu            ;    // [string,    7] 시가지수                       StartPos 148, Length 7
    char    opendiff            [   6];    char    _opendiff            ;    // [string,    6] 시가등락율                     StartPos 156, Length 6
    char    opentime            [   6];    char    _opentime            ;    // [string,    6] 시가시간                       StartPos 163, Length 6
    char    highjisu            [   7];    char    _highjisu            ;    // [string,    7] 고가지수                       StartPos 170, Length 7
    char    highdiff            [   6];    char    _highdiff            ;    // [string,    6] 고가등락율                     StartPos 178, Length 6
    char    hightime            [   6];    char    _hightime            ;    // [string,    6] 고가시간                       StartPos 185, Length 6
    char    lowjisu             [   7];    char    _lowjisu             ;    // [string,    7] 저가지수                       StartPos 192, Length 7
    char    lowdiff             [   6];    char    _lowdiff             ;    // [string,    6] 저가등락율                     StartPos 200, Length 6
    char    lowtime             [   6];    char    _lowtime             ;    // [string,    6] 저가시간                       StartPos 207, Length 6
    char    whjisu              [   7];    char    _whjisu              ;    // [string,    7] 52주최고지수                   StartPos 214, Length 7
    char    whchange            [   7];    char    _whchange            ;    // [string,    7] 52주최고현재가대비             StartPos 222, Length 7
    char    whjday              [   8];    char    _whjday              ;    // [string,    8] 52주최고지수일자               StartPos 230, Length 8
    char    wljisu              [   7];    char    _wljisu              ;    // [string,    7] 52주최저지수                   StartPos 239, Length 7
    char    wlchange            [   7];    char    _wlchange            ;    // [string,    7] 52주최저현재가대비             StartPos 247, Length 7
    char    wljday              [   8];    char    _wljday              ;    // [string,    8] 52주최저지수일자               StartPos 255, Length 8
    char    yhjisu              [   7];    char    _yhjisu              ;    // [string,    7] 연중최고지수                   StartPos 264, Length 7
    char    yhchange            [   7];    char    _yhchange            ;    // [string,    7] 연중최고현재가대비             StartPos 272, Length 7
    char    yhjday              [   8];    char    _yhjday              ;    // [string,    8] 연중최고지수일자               StartPos 280, Length 8
    char    yljisu              [   7];    char    _yljisu              ;    // [string,    7] 연중최저지수                   StartPos 289, Length 7
    char    ylchange            [   7];    char    _ylchange            ;    // [string,    7] 연중최저현재가대비             StartPos 297, Length 7
    char    yljday              [   8];    char    _yljday              ;    // [string,    8] 연중최저지수일자               StartPos 305, Length 8
    char    firstjcode          [   3];    char    _firstjcode          ;    // [string,    3] 첫번째지수코드                 StartPos 314, Length 3
    char    firstjname          [  20];    char    _firstjname          ;    // [string,   20] 첫번째지수명                   StartPos 318, Length 20
    char    firstjisu           [   7];    char    _firstjisu           ;    // [string,    7] 첫번째지수                     StartPos 339, Length 7
    char    firsign             [   1];    char    _firsign             ;    // [string,    1] 첫번째대비구분                 StartPos 347, Length 1
    char    firchange           [   7];    char    _firchange           ;    // [string,    7] 첫번째전일대비                 StartPos 349, Length 7
    char    firdiff             [   6];    char    _firdiff             ;    // [string,    6] 첫번째등락율                   StartPos 357, Length 6
    char    secondjcode         [   3];    char    _secondjcode         ;    // [string,    3] 두번째지수코드                 StartPos 364, Length 3
    char    secondjname         [  20];    char    _secondjname         ;    // [string,   20] 두번째지수명                   StartPos 368, Length 20
    char    secondjisu          [   7];    char    _secondjisu          ;    // [string,    7] 두번째지수                     StartPos 389, Length 7
    char    secsign             [   1];    char    _secsign             ;    // [string,    1] 두번째대비구분                 StartPos 397, Length 1
    char    secchange           [   7];    char    _secchange           ;    // [string,    7] 두번째전일대비                 StartPos 399, Length 7
    char    secdiff             [   6];    char    _secdiff             ;    // [string,    6] 두번째등락율                   StartPos 407, Length 6
    char    thirdjcode          [   3];    char    _thirdjcode          ;    // [string,    3] 세번째지수코드                 StartPos 414, Length 3
    char    thirdjname          [  20];    char    _thirdjname          ;    // [string,   20] 세번째지수명                   StartPos 418, Length 20
    char    thirdjisu           [   7];    char    _thirdjisu           ;    // [string,    7] 세번째지수                     StartPos 439, Length 7
    char    thrsign             [   1];    char    _thrsign             ;    // [string,    1] 세번째대비구분                 StartPos 447, Length 1
    char    thrchange           [   7];    char    _thrchange           ;    // [string,    7] 세번째전일대비                 StartPos 449, Length 7
    char    thrdiff             [   6];    char    _thrdiff             ;    // [string,    6] 세번째등락율                   StartPos 457, Length 6
    char    fourthjcode         [   3];    char    _fourthjcode         ;    // [string,    3] 네번째지수코드                 StartPos 464, Length 3
    char    fourthjname         [  20];    char    _fourthjname         ;    // [string,   20] 네번째지수명                   StartPos 468, Length 20
    char    fourthjisu          [   7];    char    _fourthjisu          ;    // [string,    7] 네번째지수                     StartPos 489, Length 7
    char    forsign             [   1];    char    _forsign             ;    // [string,    1] 네번째대비구분                 StartPos 497, Length 1
    char    forchange           [   7];    char    _forchange           ;    // [string,    7] 네번째전일대비                 StartPos 499, Length 7
    char    fordiff             [   6];    char    _fordiff             ;    // [string,    6] 네번째등락율                   StartPos 507, Length 6
    char    highjo              [   4];    char    _highjo              ;    // [string,    4] 상승종목수                     StartPos 514, Length 4
    char    upjo                [   4];    char    _upjo                ;    // [string,    4] 상한종목수                     StartPos 519, Length 4
    char    unchgjo             [   4];    char    _unchgjo             ;    // [string,    4] 보합종목수                     StartPos 524, Length 4
    char    lowjo               [   4];    char    _lowjo               ;    // [string,    4] 하락종목수                     StartPos 529, Length 4
    char    downjo              [   4];    char    _downjo              ;    // [string,    4] 하한종목수                     StartPos 534, Length 4
} t1253OutBlock, *LPt1253OutBlock;
#define NAME_t1253OutBlock     "t1253OutBlock"


#pragma pack( pop )
///////////////////////////////////////////////////////////////////////////////////////////////////
