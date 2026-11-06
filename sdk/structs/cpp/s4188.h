#pragma once

///////////////////////////////////////////////////////////////////////////////////////////////////
// 선물옵션차트(틱/n틱) ( attr,block,headtype=A ) 
#pragma pack( push, 1 )

#define NAME_s4188     "s4188"

// 입력
typedef struct _s4188InBlock
{
    char    shcode              [   8];    char    _shcode              ;    // [string,    8] 단축코드                       StartPos 0, Length 8
    char    mkgb                [   1];    char    _mkgb                ;    // [string,    1] 시장구분                       StartPos 9, Length 1
    char    gubun               [   1];    char    _gubun               ;    // [string,    1] 주기구분(2:일3:주4:월)         StartPos 11, Length 1
    char    hsilgb              [   1];    char    _hsilgb              ;    // [string,    1] 봉구분(허봉실봉)               StartPos 13, Length 1
    char    ncnt                [   4];    char    _ncnt                ;    // [string,    4] 단위(n분)                      StartPos 15, Length 4
    char    qrycnt              [   4];    char    _qrycnt              ;    // [string,    4] 요청건수(최대-압축:2000비압축:500) StartPos 20, Length 4
    char    nday                [   1];    char    _nday                ;    // [string,    1] 조회영업일수(0:미사용1>=사용)  StartPos 25, Length 1
    char    sdate               [   8];    char    _sdate               ;    // [string,    8] 시작일자                       StartPos 27, Length 8
    char    edate               [   8];    char    _edate               ;    // [string,    8] 종료일자                       StartPos 36, Length 8
    char    cts_date            [   8];    char    _cts_date            ;    // [string,    8] 연속일자                       StartPos 45, Length 8
    char    cts_time            [  10];    char    _cts_time            ;    // [string,   10] 연속시간                       StartPos 54, Length 10
    char    comp_yn             [   1];    char    _comp_yn             ;    // [string,    1] 압축여부(Y:압축N:비압축)       StartPos 65, Length 1
} s4188InBlock, *LPs4188InBlock;
#define NAME_s4188InBlock     "s4188InBlock"

// 출력
typedef struct _s4188OutBlock
{
    char    shcode              [   8];    char    _shcode              ;    // [string,    8] 단축코드                       StartPos 0, Length 8
    char    mkgb                [   1];    char    _mkgb                ;    // [string,    1] 시장구분                       StartPos 9, Length 1
    char    gubun               [   1];    char    _gubun               ;    // [string,    1] 주기구분(2:일3:주4:월)         StartPos 11, Length 1
    char    hsilgb              [   1];    char    _hsilgb              ;    // [string,    1] 봉구분(허봉실봉)               StartPos 13, Length 1
    char    bday                [   8];    char    _bday                ;    // [string,    8] 영업일                         StartPos 15, Length 8
    char    stime               [  14];    char    _stime               ;    // [string,   14] 조회시간(YYYYMMDDHHMMSS)       StartPos 24, Length 14
    char    chegb               [   1];    char    _chegb               ;    // [string,    1] 장중체결여부                   StartPos 39, Length 1
    char    lasttm              [   6];    char    _lasttm              ;    // [string,    6] 마지막체결시간(HHMMSS)         StartPos 41, Length 6
    char    recprice            [   9];    char    _recprice            ;    // [string,    9] 기준가                         StartPos 48, Length 9
    char    jisiga              [   9];    char    _jisiga              ;    // [string,    9] 전일시가                       StartPos 58, Length 9
    char    jihigh              [   9];    char    _jihigh              ;    // [string,    9] 전일고가                       StartPos 68, Length 9
    char    jilow               [   9];    char    _jilow               ;    // [string,    9] 전일저가                       StartPos 78, Length 9
    char    jiclose             [   9];    char    _jiclose             ;    // [string,    9] 전일종가                       StartPos 88, Length 9
    char    jivolume            [  12];    char    _jivolume            ;    // [string,   12] 전일거래량                     StartPos 98, Length 12
    char    disiga              [   9];    char    _disiga              ;    // [string,    9] 당일시가                       StartPos 111, Length 9
    char    dihigh              [   9];    char    _dihigh              ;    // [string,    9] 당일고가                       StartPos 121, Length 9
    char    dilow               [   9];    char    _dilow               ;    // [string,    9] 당일저가                       StartPos 131, Length 9
    char    diclose             [   9];    char    _diclose             ;    // [string,    9] 당일종가                       StartPos 141, Length 9
    char    sign                [   1];    char    _sign                ;    // [string,    1] 전일대비부호                   StartPos 151, Length 1
    char    change              [   8];    char    _change              ;    // [string,    8] 전일대비                       StartPos 153, Length 8
    char    rate                [   6];    char    _rate                ;    // [string,    6] 대비등락률                     StartPos 162, Length 6
    char    volume              [  12];    char    _volume              ;    // [string,   12] 당일거래량                     StartPos 169, Length 12
    char    value               [  12];    char    _value               ;    // [string,   12] 누적거래대금                   StartPos 182, Length 12
    char    highend             [   9];    char    _highend             ;    // [string,    9] 상한가                         StartPos 195, Length 9
    char    lowend              [   9];    char    _lowend              ;    // [string,    9] 하한가                         StartPos 205, Length 9
    char    offerho1            [   9];    char    _offerho1            ;    // [string,    9] 매도호가                       StartPos 215, Length 9
    char    bidho1              [   9];    char    _bidho1              ;    // [string,    9] 매수호가                       StartPos 225, Length 9
    char    k200jisu            [  10];    char    _k200jisu            ;    // [string,   10] 코스피200현재가                StartPos 235, Length 10
    char    k200sign            [   1];    char    _k200sign            ;    // [string,    1] K200전일대비부호               StartPos 246, Length 1
    char    k200change          [   6];    char    _k200change          ;    // [string,    6] k200전일대비                   StartPos 248, Length 6
    char    k200rate            [   6];    char    _k200rate            ;    // [string,    6] k200대비등락률                 StartPos 255, Length 6
    char    openyak             [   8];    char    _openyak             ;    // [string,    8] 미결제약정                     StartPos 262, Length 8
    char    mgjvdiff            [   8];    char    _mgjvdiff            ;    // [string,    8] 미결제증감                     StartPos 271, Length 8
    char    sbasis              [   6];    char    _sbasis              ;    // [string,    6] 시장베이시스                   StartPos 280, Length 6
    char    chdegree            [   5];    char    _chdegree            ;    // [string,    5] 체결강도                       StartPos 287, Length 5
    char    theoryprice         [   9];    char    _theoryprice         ;    // [string,    9] 이론가                         StartPos 293, Length 9
    char    glyl                [   6];    char    _glyl                ;    // [string,    6] 괴리율                         StartPos 303, Length 6
    char    delt                [   6];    char    _delt                ;    // [string,    6] 델타                           StartPos 310, Length 6
    char    gama                [   6];    char    _gama                ;    // [string,    6] 감마                           StartPos 317, Length 6
    char    ceta                [   6];    char    _ceta                ;    // [string,    6] 세타                           StartPos 324, Length 6
    char    vega                [   6];    char    _vega                ;    // [string,    6] 베가                           StartPos 331, Length 6
    char    rhox                [   6];    char    _rhox                ;    // [string,    6] 로우                           StartPos 338, Length 6
    char    impv                [   6];    char    _impv                ;    // [string,    6] 내재변동성                     StartPos 345, Length 6
    char    tickcnt             [   4];    char    _tickcnt             ;    // [string,    4] N틱시마지막봉개수              StartPos 352, Length 4
    char    s_time              [   6];    char    _s_time              ;    // [string,    6] 장시작시간(HHMMSS)             StartPos 357, Length 6
    char    e_time              [   6];    char    _e_time              ;    // [string,    6] 장종료시간(HHMMSS)             StartPos 364, Length 6
    char    dshmin              [   2];    char    _dshmin              ;    // [string,    2] 동시호가처리시간(MM:분)        StartPos 371, Length 2
    char    cts_date            [   8];    char    _cts_date            ;    // [string,    8] 연속일자                       StartPos 374, Length 8
    char    cts_time            [  10];    char    _cts_time            ;    // [string,   10] 연속시간                       StartPos 383, Length 10
    char    rec_count           [   7];    char    _rec_count           ;    // [string,    7] 레코드카운트                   StartPos 394, Length 7
} s4188OutBlock, *LPs4188OutBlock;
#define NAME_s4188OutBlock     "s4188OutBlock"

typedef struct _s4188OutBlock1
{
    char    date                [   8];    char    _date                ;    // [string,    8] 날짜                           StartPos 0, Length 8
    char    time                [  10];    char    _time                ;    // [string,   10] 시간                           StartPos 9, Length 10
    char    open                [   9];    char    _open                ;    // [string,    9] 시가                           StartPos 20, Length 9
    char    high                [   9];    char    _high                ;    // [string,    9] 고가                           StartPos 30, Length 9
    char    low                 [   9];    char    _low                 ;    // [string,    9] 저가                           StartPos 40, Length 9
    char    close               [   9];    char    _close               ;    // [string,    9] 종가                           StartPos 50, Length 9
    char    jdiff_vol           [  12];    char    _jdiff_vol           ;    // [string,   12] 누적거래량                     StartPos 60, Length 12
    char    value               [  12];    char    _value               ;    // [string,   12] 거래대금                       StartPos 73, Length 12
    char    openyak             [   8];    char    _openyak             ;    // [string,    8] 미결제약정                     StartPos 86, Length 8
} s4188OutBlock1, *LPs4188OutBlock1;
#define NAME_s4188OutBlock1     "s4188OutBlock1"


#pragma pack( pop )
///////////////////////////////////////////////////////////////////////////////////////////////////
