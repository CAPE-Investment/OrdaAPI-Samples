#pragma once

///////////////////////////////////////////////////////////////////////////////////////////////////
// 주식챠트(종합) ( attr,block,headtype=A ) 
#pragma pack( push, 1 )

#define NAME_s4180     "s4180"

// 입력
typedef struct _s4180InBlock
{
    char    shcode              [   6];    char    _shcode              ;    // [string,    6] 단축코드                       StartPos 0, Length 6
    char    mkgb                [   1];    char    _mkgb                ;    // [string,    1] 시장구분(주식ELWETN)           StartPos 7, Length 1
    char    ilgb                [   1];    char    _ilgb                ;    // [string,    1] 주기구분(일주월분틱)           StartPos 9, Length 1
    char    hsilgb              [   1];    char    _hsilgb              ;    // [string,    1] 봉구분(허봉실봉)               StartPos 11, Length 1
    char    ncnt                [   4];    char    _ncnt                ;    // [string,    4] 단위(n분)                      StartPos 13, Length 4
    char    qrycnt              [   4];    char    _qrycnt              ;    // [string,    4] 요청건수(최대-압축:2000비압축:500) StartPos 18, Length 4
    char    nday                [   1];    char    _nday                ;    // [string,    1] 조회영업일수(0:미사용1>=사용)  StartPos 23, Length 1
    char    sdate               [   8];    char    _sdate               ;    // [string,    8] 시작일자                       StartPos 25, Length 8
    char    edate               [   8];    char    _edate               ;    // [string,    8] 종료일자                       StartPos 34, Length 8
    char    cts_date            [   8];    char    _cts_date            ;    // [string,    8] 연속일자                       StartPos 43, Length 8
    char    cts_time            [  10];    char    _cts_time            ;    // [string,   10] 연속시간                       StartPos 52, Length 10
    char    comp_yn             [   1];    char    _comp_yn             ;    // [string,    1] 압축여부(Y:압축N:비압축)       StartPos 63, Length 1
    char    modigb              [   1];    char    _modigb              ;    // [string,    1] 수정주가여부                   StartPos 65, Length 1
    char    exchange            [   1];    char    _exchange            ;    // [string,    1] 거래소구분(1:KRX2:NXT3:통합)   StartPos 67, Length 1
} s4180InBlock, *LPs4180InBlock;
#define NAME_s4180InBlock     "s4180InBlock"

// 출력
typedef struct _s4180OutBlock
{
    char    shcode              [   6];    char    _shcode              ;    // [string,    6] 단축코드                       StartPos 0, Length 6
    char    mkgb                [   1];    char    _mkgb                ;    // [string,    1] 시장구분(주식ELWETN)           StartPos 7, Length 1
    char    ilgb                [   1];    char    _ilgb                ;    // [string,    1] 주기구분(일주월분틱)           StartPos 9, Length 1
    char    hsilgb              [   1];    char    _hsilgb              ;    // [string,    1] 봉구분(허봉실봉)               StartPos 11, Length 1
    char    bday                [   8];    char    _bday                ;    // [string,    8] 영업일                         StartPos 13, Length 8
    char    stime               [  14];    char    _stime               ;    // [string,   14] 조회시간(YYYYMMDDHHMMSS)       StartPos 22, Length 14
    char    chegb               [   1];    char    _chegb               ;    // [string,    1] 장중체결여부                   StartPos 37, Length 1
    char    lasttm              [   6];    char    _lasttm              ;    // [string,    6] 마지막체결시간(HHMMSS)         StartPos 39, Length 6
    char    basprc              [   8];    char    _basprc              ;    // [string,    8] 기준가                         StartPos 46, Length 8
    char    jisiga              [   8];    char    _jisiga              ;    // [string,    8] 전일시가                       StartPos 55, Length 8
    char    jihigh              [   8];    char    _jihigh              ;    // [string,    8] 전일고가                       StartPos 64, Length 8
    char    jilow               [   8];    char    _jilow               ;    // [string,    8] 전일저가                       StartPos 73, Length 8
    char    jiclose             [   8];    char    _jiclose             ;    // [string,    8] 전일종가                       StartPos 82, Length 8
    char    jivolume            [  12];    char    _jivolume            ;    // [string,   12] 전일거래량                     StartPos 91, Length 12
    char    jitamt              [  13];    char    _jitamt              ;    // [string,   13] 전일거래대금                   StartPos 104, Length 13
    char    jivolrate           [   8];    char    _jivolrate           ;    // [string,    8] 전일거래량대비율               StartPos 118, Length 8
    char    disiga              [   8];    char    _disiga              ;    // [string,    8] 당일시가                       StartPos 127, Length 8
    char    dihigh              [   8];    char    _dihigh              ;    // [string,    8] 당일고가                       StartPos 136, Length 8
    char    dilow               [   8];    char    _dilow               ;    // [string,    8] 당일저가                       StartPos 145, Length 8
    char    diclose             [   8];    char    _diclose             ;    // [string,    8] 당일종가                       StartPos 154, Length 8
    char    sign                [   1];    char    _sign                ;    // [string,    1] 전일대비부호                   StartPos 163, Length 1
    char    change              [   8];    char    _change              ;    // [string,    8] 전일대비                       StartPos 165, Length 8
    char    rate                [   6];    char    _rate                ;    // [string,    6] 대비등락률                     StartPos 174, Length 6
    char    volume              [  12];    char    _volume              ;    // [string,   12] 당일거래량                     StartPos 181, Length 12
    char    value               [  13];    char    _value               ;    // [string,   13] 누적거래대금                   StartPos 194, Length 13
    char    volrate             [   8];    char    _volrate             ;    // [string,    8] 회전율                         StartPos 208, Length 8
    char    highend             [   8];    char    _highend             ;    // [string,    8] 상한가                         StartPos 217, Length 8
    char    lowend              [   8];    char    _lowend              ;    // [string,    8] 하한가                         StartPos 226, Length 8
    char    offerho1            [   8];    char    _offerho1            ;    // [string,    8] 매도호가                       StartPos 235, Length 8
    char    bidho1              [   8];    char    _bidho1              ;    // [string,    8] 매수호가                       StartPos 244, Length 8
    char    offerrem1           [  12];    char    _offerrem1           ;    // [string,   12] 매도호가수량                   StartPos 253, Length 12
    char    bidrem1             [  12];    char    _bidrem1             ;    // [string,   12] 매수호가수량                   StartPos 266, Length 12
    char    listing             [  12];    char    _listing             ;    // [string,   12] 상장주식수(천)                 StartPos 279, Length 12
    char    total               [  12];    char    _total               ;    // [string,   12] 시가총액(억)                   StartPos 292, Length 12
    char    per                 [  12];    char    _per                 ;    // [string,   12] PER                            StartPos 305, Length 12
    char    pbr                 [  12];    char    _pbr                 ;    // [string,   12] PBR                            StartPos 318, Length 12
    char    eps                 [  12];    char    _eps                 ;    // [string,   12] EPS                            StartPos 331, Length 12
    char    bps                 [  12];    char    _bps                 ;    // [string,   12] BPS                            StartPos 344, Length 12
    char    surate              [   9];    char    _surate              ;    // [string,    9] 누적수정비율(과거일자조회시)   StartPos 357, Length 9
    char    tickcnt             [   4];    char    _tickcnt             ;    // [string,    4] N틱시마지막봉개수              StartPos 367, Length 4
    char    s_time1             [   6];    char    _s_time1             ;    // [string,    6] 장시작시간1(HHMMSS)            StartPos 372, Length 6
    char    e_time1             [   6];    char    _e_time1             ;    // [string,    6] 장종료시간1(HHMMSS)            StartPos 379, Length 6
    char    s_time2             [   6];    char    _s_time2             ;    // [string,    6] 장시작시간2(HHMMSS)            StartPos 386, Length 6
    char    e_time2             [   6];    char    _e_time2             ;    // [string,    6] 장종료시간2(HHMMSS)            StartPos 393, Length 6
    char    s_time3             [   6];    char    _s_time3             ;    // [string,    6] 장시작시간3(HHMMSS)            StartPos 400, Length 6
    char    e_time3             [   6];    char    _e_time3             ;    // [string,    6] 장종료시간3(HHMMSS)            StartPos 407, Length 6
    char    s_time4             [   6];    char    _s_time4             ;    // [string,    6] 장시작시간4(HHMMSS)            StartPos 414, Length 6
    char    e_time4             [   6];    char    _e_time4             ;    // [string,    6] 장종료시간4(HHMMSS)            StartPos 421, Length 6
    char    s_time5             [   6];    char    _s_time5             ;    // [string,    6] 장시작시간5(HHMMSS)            StartPos 428, Length 6
    char    e_time5             [   6];    char    _e_time5             ;    // [string,    6] 장종료시간5(HHMMSS)            StartPos 435, Length 6
    char    cts_date            [   8];    char    _cts_date            ;    // [string,    8] 연속일자                       StartPos 442, Length 8
    char    cts_time            [  10];    char    _cts_time            ;    // [string,   10] 연속시간                       StartPos 451, Length 10
    char    rec_count           [   7];    char    _rec_count           ;    // [string,    7] 레코드카운트                   StartPos 462, Length 7
} s4180OutBlock, *LPs4180OutBlock;
#define NAME_s4180OutBlock     "s4180OutBlock"

typedef struct _s4180OutBlock1
{
    char    date                [   8];    char    _date                ;    // [string,    8] 날짜                           StartPos 0, Length 8
    char    time                [  10];    char    _time                ;    // [string,   10] 시간                           StartPos 9, Length 10
    char    recprice            [   8];    char    _recprice            ;    // [string,    8] 기준가                         StartPos 20, Length 8
    char    open                [   8];    char    _open                ;    // [string,    8] 시가                           StartPos 29, Length 8
    char    high                [   8];    char    _high                ;    // [string,    8] 고가                           StartPos 38, Length 8
    char    low                 [   8];    char    _low                 ;    // [string,    8] 저가                           StartPos 47, Length 8
    char    close               [   8];    char    _close               ;    // [string,    8] 종가                           StartPos 56, Length 8
    char    jdiff_vol           [  12];    char    _jdiff_vol           ;    // [string,   12] 거래량                         StartPos 65, Length 12
    char    value               [  13];    char    _value               ;    // [string,   13] 거래대금                       StartPos 78, Length 13
    char    jongchk             [  13];    char    _jongchk             ;    // [string,   13] 수정구분                       StartPos 92, Length 13
    char    rate                [   6];    char    _rate                ;    // [string,    6] 수정비율                       StartPos 106, Length 6
    char    pricechk            [  13];    char    _pricechk            ;    // [string,   13] 수정주가반영항목               StartPos 113, Length 13
    char    ratevalue           [  13];    char    _ratevalue           ;    // [string,   13] 수정비율거래대금               StartPos 127, Length 13
    char    sign                [   1];    char    _sign                ;    // [string,    1] 종가등락구분(1:상한2:상승3:보합4:하한5:하락) StartPos 141, Length 1
} s4180OutBlock1, *LPs4180OutBlock1;
#define NAME_s4180OutBlock1     "s4180OutBlock1"


#pragma pack( pop )
///////////////////////////////////////////////////////////////////////////////////////////////////
