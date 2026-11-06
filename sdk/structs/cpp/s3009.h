#pragma once

///////////////////////////////////////////////////////////////////////////////////////////////////
// 주식선물 현재가 조회 ( attr,block,headtype=A ) 
#pragma pack( push, 1 )

#define NAME_s3009     "s3009"

// 입력
typedef struct _s3009InBlock
{
    char    focode              [   8];    char    _focode              ;    // [string,    8] 단축코드                       StartPos 0, Length 8
} s3009InBlock, *LPs3009InBlock;
#define NAME_s3009InBlock     "s3009InBlock"

// 출력
typedef struct _s3009OutBlock
{
    char    hname               [  20];    char    _hname               ;    // [string,   20] 한글명                         StartPos 0, Length 20
    char    price               [   8];    char    _price               ;    // [string,    8] 현재가                         StartPos 21, Length 8
    char    sign                [   1];    char    _sign                ;    // [string,    1] 전일대비구분                   StartPos 30, Length 1
    char    change              [   8];    char    _change              ;    // [string,    8] 전일대비                       StartPos 32, Length 8
    char    jnilclose           [   8];    char    _jnilclose           ;    // [string,    8] 전일종가                       StartPos 41, Length 8
    char    diff                [   6];    char    _diff                ;    // [string,    6] 등락율                         StartPos 50, Length 6
    char    volume              [  12];    char    _volume              ;    // [string,   12] 거래량                         StartPos 57, Length 12
    char    stimeqrt            [   6];    char    _stimeqrt            ;    // [string,    6] 거래량전일동시간비율           StartPos 70, Length 6
    char    value               [  12];    char    _value               ;    // [string,   12] 거래대금                       StartPos 77, Length 12
    char    mgjv                [   8];    char    _mgjv                ;    // [string,    8] 미결제량                       StartPos 90, Length 8
    char    mgjvdiff            [   8];    char    _mgjvdiff            ;    // [string,    8] 미결제증감                     StartPos 99, Length 8
    char    open                [   8];    char    _open                ;    // [string,    8] 시가                           StartPos 108, Length 8
    char    high                [   8];    char    _high                ;    // [string,    8] 고가                           StartPos 117, Length 8
    char    low                 [   8];    char    _low                 ;    // [string,    8] 저가                           StartPos 126, Length 8
    char    high52w             [   8];    char    _high52w             ;    // [string,    8] 52최고가                       StartPos 135, Length 8
    char    low52w              [   8];    char    _low52w              ;    // [string,    8] 52최저가                       StartPos 144, Length 8
    char    basis               [   6];    char    _basis               ;    // [string,    6] 베이시스                       StartPos 153, Length 6
    char    recprice            [   8];    char    _recprice            ;    // [string,    8] 기준가                         StartPos 160, Length 8
    char    theoryprice         [   8];    char    _theoryprice         ;    // [string,    8] 이론가                         StartPos 169, Length 8
    char    glyl                [   6];    char    _glyl                ;    // [string,    6] 괴리율                         StartPos 178, Length 6
    char    lastmonth           [   8];    char    _lastmonth           ;    // [string,    8] 만기일                         StartPos 185, Length 8
    char    jandatecnt          [   8];    char    _jandatecnt          ;    // [string,    8] 잔여일                         StartPos 194, Length 8
    char    pricejisu           [  10];    char    _pricejisu           ;    // [string,   10] 종합지수                       StartPos 203, Length 10
    char    jisusign            [   1];    char    _jisusign            ;    // [string,    1] 종합지수전일대비구분           StartPos 214, Length 1
    char    jisuchange          [   6];    char    _jisuchange          ;    // [string,    6] 종합지수전일대비               StartPos 216, Length 6
    char    jisudiff            [   6];    char    _jisudiff            ;    // [string,    6] 종합지수등락율                 StartPos 223, Length 6
    char    kospijisu           [  10];    char    _kospijisu           ;    // [string,   10] KOSPI200지수                   StartPos 230, Length 10
    char    kospisign           [   1];    char    _kospisign           ;    // [string,    1] KOSPI200전일대비구분           StartPos 241, Length 1
    char    kospichange         [   6];    char    _kospichange         ;    // [string,    6] KOSPI200전일대비               StartPos 243, Length 6
    char    kospidiff           [   6];    char    _kospidiff           ;    // [string,    6] KOSPI200등락율                 StartPos 250, Length 6
    char    listhprice          [   8];    char    _listhprice          ;    // [string,    8] 상장최고가                     StartPos 257, Length 8
    char    listlprice          [   8];    char    _listlprice          ;    // [string,    8] 상장최저가                     StartPos 266, Length 8
    char    delt                [   6];    char    _delt                ;    // [string,    6] 델타                           StartPos 275, Length 6
    char    gama                [   6];    char    _gama                ;    // [string,    6] 감마                           StartPos 282, Length 6
    char    ceta                [   6];    char    _ceta                ;    // [string,    6] 세타                           StartPos 289, Length 6
    char    vega                [   6];    char    _vega                ;    // [string,    6] 베가                           StartPos 296, Length 6
    char    rhox                [   6];    char    _rhox                ;    // [string,    6] 로우                           StartPos 303, Length 6
    char    gmprice             [   8];    char    _gmprice             ;    // [string,    8] 근월물현재가                   StartPos 310, Length 8
    char    gmsign              [   1];    char    _gmsign              ;    // [string,    1] 근월물전일대비구분             StartPos 319, Length 1
    char    gmchange            [   8];    char    _gmchange            ;    // [string,    8] 근월물전일대비                 StartPos 321, Length 8
    char    gmdiff              [   6];    char    _gmdiff              ;    // [string,    6] 근월물등락율                   StartPos 330, Length 6
    char    theorypriceg        [   8];    char    _theorypriceg        ;    // [string,    8] 이론가                         StartPos 337, Length 8
    char    histimpv            [   6];    char    _histimpv            ;    // [string,    6] 역사적변동성                   StartPos 346, Length 6
    char    impv                [   6];    char    _impv                ;    // [string,    6] 내재변동성                     StartPos 353, Length 6
    char    sbasis              [   8];    char    _sbasis              ;    // [string,    8] 시장BASIS                      StartPos 360, Length 8
    char    ibasis              [   8];    char    _ibasis              ;    // [string,    8] 이론BASIS                      StartPos 369, Length 8
    char    gmfutcode           [   8];    char    _gmfutcode           ;    // [string,    8] 근월물종목코드                 StartPos 378, Length 8
    char    actprice            [   8];    char    _actprice            ;    // [string,    8] 행사가                         StartPos 387, Length 8
    char    shcode              [   6];    char    _shcode              ;    // [string,    6] 기초자산단축코드               StartPos 396, Length 6
    char    basehname           [  20];    char    _basehname           ;    // [string,   20] 기초자산한글명                 StartPos 403, Length 20
    char    baseprice           [   8];    char    _baseprice           ;    // [string,    8] 기초자산현재가                 StartPos 424, Length 8
    char    basesign            [   1];    char    _basesign            ;    // [string,    1] 기초자산현재가대비구분         StartPos 433, Length 1
    char    basechange          [   8];    char    _basechange          ;    // [string,    8] 기초자산현재가전일대비         StartPos 435, Length 8
    char    basediff            [   6];    char    _basediff            ;    // [string,    6] 기초자산등락률                 StartPos 444, Length 6
    char    basevol             [  12];    char    _basevol             ;    // [string,   12] 기초자산거래량                 StartPos 451, Length 12
    char    baseprevol          [  12];    char    _baseprevol          ;    // [string,   12] 기초자산전일거래량             StartPos 464, Length 12
    char    basebidprc          [   9];    char    _basebidprc          ;    // [string,    9] 기초자산매수호가               StartPos 477, Length 9
    char    baseaskprc          [   9];    char    _baseaskprc          ;    // [string,    9] 기초자산매도호가               StartPos 487, Length 9
    char    basefornetbid       [  12];    char    _basefornetbid       ;    // [string,   12] 기초자산외국계회원사순매수     StartPos 497, Length 12
    char    prodgrp             [  20];    char    _prodgrp             ;    // [string,   20] 상품군                         StartPos 510, Length 20
    char    mulcnt              [  12];    char    _mulcnt              ;    // [string,   12] 승수                           StartPos 531, Length 12
    char    realupdnyn          [   1];    char    _realupdnyn          ;    // [string,    1] 실시간상/하한가적용여부        StartPos 544, Length 1
    char    realuplmtprice      [  10];    char    _realuplmtprice      ;    // [string,   10] 실시간상한가                   StartPos 546, Length 10
    char    realdnlmtprice      [  10];    char    _realdnlmtprice      ;    // [string,   10] 실시간하한가                   StartPos 557, Length 10
    char    uplmtpricestep      [   2];    char    _uplmtpricestep      ;    // [string,    2] 상한가적용단계                 StartPos 568, Length 2
    char    uplmtprice1         [   8];    char    _uplmtprice1         ;    // [string,    8] 상한가1                        StartPos 571, Length 8
    char    uplmtprice2         [   8];    char    _uplmtprice2         ;    // [string,    8] 상한가2                        StartPos 580, Length 8
    char    uplmtprice3         [   8];    char    _uplmtprice3         ;    // [string,    8] 상한가3                        StartPos 589, Length 8
    char    dnlmtpricestep      [   2];    char    _dnlmtpricestep      ;    // [string,    2] 하한가적용단계                 StartPos 598, Length 2
    char    dnlmtprice1         [   8];    char    _dnlmtprice1         ;    // [string,    8] 하한가1                        StartPos 601, Length 8
    char    dnlmtprice2         [   8];    char    _dnlmtprice2         ;    // [string,    8] 하한가2                        StartPos 610, Length 8
    char    dnlmtprice3         [   8];    char    _dnlmtprice3         ;    // [string,    8] 하한가3                        StartPos 619, Length 8
} s3009OutBlock, *LPs3009OutBlock;
#define NAME_s3009OutBlock     "s3009OutBlock"


#pragma pack( pop )
///////////////////////////////////////////////////////////////////////////////////////////////////
