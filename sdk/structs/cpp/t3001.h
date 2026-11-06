#pragma once

///////////////////////////////////////////////////////////////////////////////////////////////////
// 선물/옵션 현재가 ( attr,block,headtype=A ) 
#pragma pack( push, 1 )

#define NAME_t3001     "t3001"

// 입력
typedef struct _t3001InBlock
{
    char    focode              [   8];    char    _focode              ;    // [string,    8] 단축코드                       StartPos 0, Length 8
} t3001InBlock, *LPt3001InBlock;
#define NAME_t3001InBlock     "t3001InBlock"

// 출력
typedef struct _t3001OutBlock
{
    char    hname               [  20];    char    _hname               ;    // [string,   20] 한글명                         StartPos 0, Length 20
    char    price               [   6];    char    _price               ;    // [string,    6] 현재가                         StartPos 21, Length 6
    char    sign                [   1];    char    _sign                ;    // [string,    1] 전일대비구분                   StartPos 28, Length 1
    char    change              [   6];    char    _change              ;    // [string,    6] 전일대비                       StartPos 30, Length 6
    char    jnilclose           [   6];    char    _jnilclose           ;    // [string,    6] 전일종가                       StartPos 37, Length 6
    char    diff                [   6];    char    _diff                ;    // [string,    6] 등락율                         StartPos 44, Length 6
    char    volume              [  12];    char    _volume              ;    // [string,   12] 거래량                         StartPos 51, Length 12
    char    value               [  12];    char    _value               ;    // [string,   12] 거래대금                       StartPos 64, Length 12
    char    mgjv                [   8];    char    _mgjv                ;    // [string,    8] 미결제량                       StartPos 77, Length 8
    char    mgjvdiff            [   8];    char    _mgjvdiff            ;    // [string,    8] 미결제증감                     StartPos 86, Length 8
    char    open                [   6];    char    _open                ;    // [string,    6] 시가                           StartPos 95, Length 6
    char    high                [   6];    char    _high                ;    // [string,    6] 고가                           StartPos 102, Length 6
    char    low                 [   6];    char    _low                 ;    // [string,    6] 저가                           StartPos 109, Length 6
    char    high52w             [   6];    char    _high52w             ;    // [string,    6] 52최고가                       StartPos 116, Length 6
    char    low52w              [   6];    char    _low52w              ;    // [string,    6] 52최저가                       StartPos 123, Length 6
    char    basis               [   6];    char    _basis               ;    // [string,    6] 베이시스                       StartPos 130, Length 6
    char    recprice            [   6];    char    _recprice            ;    // [string,    6] 기준가                         StartPos 137, Length 6
    char    theoryprice         [   6];    char    _theoryprice         ;    // [string,    6] 이론가                         StartPos 144, Length 6
    char    glyl                [   6];    char    _glyl                ;    // [string,    6] 괴리율                         StartPos 151, Length 6
    char    lastmonth           [   8];    char    _lastmonth           ;    // [string,    8] 만기일                         StartPos 158, Length 8
    char    jandatecnt          [   8];    char    _jandatecnt          ;    // [string,    8] 잔여일                         StartPos 167, Length 8
    char    pricejisu           [  10];    char    _pricejisu           ;    // [string,   10] 종합지수                       StartPos 176, Length 10
    char    jisusign            [   1];    char    _jisusign            ;    // [string,    1] 종합지수전일대비구분           StartPos 187, Length 1
    char    jisuchange          [   6];    char    _jisuchange          ;    // [string,    6] 종합지수전일대비               StartPos 189, Length 6
    char    jisudiff            [   6];    char    _jisudiff            ;    // [string,    6] 종합지수등락율                 StartPos 196, Length 6
    char    kospijisu           [  10];    char    _kospijisu           ;    // [string,   10] KOSPI200지수                   StartPos 203, Length 10
    char    kospisign           [   1];    char    _kospisign           ;    // [string,    1] KOSPI200전일대비구분           StartPos 214, Length 1
    char    kospichange         [   6];    char    _kospichange         ;    // [string,    6] KOSPI200전일대비               StartPos 216, Length 6
    char    kospidiff           [   6];    char    _kospidiff           ;    // [string,    6] KOSPI200등락율                 StartPos 223, Length 6
    char    listhprice          [   6];    char    _listhprice          ;    // [string,    6] 상장최고가                     StartPos 230, Length 6
    char    listlprice          [   6];    char    _listlprice          ;    // [string,    6] 상장최저가                     StartPos 237, Length 6
    char    delt                [   6];    char    _delt                ;    // [string,    6] 델타                           StartPos 244, Length 6
    char    gama                [   6];    char    _gama                ;    // [string,    6] 감마                           StartPos 251, Length 6
    char    ceta                [   6];    char    _ceta                ;    // [string,    6] 세타                           StartPos 258, Length 6
    char    vega                [   6];    char    _vega                ;    // [string,    6] 베가                           StartPos 265, Length 6
    char    rhox                [   6];    char    _rhox                ;    // [string,    6] 로우                           StartPos 272, Length 6
    char    gmprice             [   6];    char    _gmprice             ;    // [string,    6] 근월물현재가                   StartPos 279, Length 6
    char    gmsign              [   1];    char    _gmsign              ;    // [string,    1] 근월물전일대비구분             StartPos 286, Length 1
    char    gmchange            [   6];    char    _gmchange            ;    // [string,    6] 근월물전일대비                 StartPos 288, Length 6
    char    gmdiff              [   6];    char    _gmdiff              ;    // [string,    6] 근월물등락율                   StartPos 295, Length 6
    char    theorypriceg        [   6];    char    _theorypriceg        ;    // [string,    6] 이론가                         StartPos 302, Length 6
    char    histimpv            [   6];    char    _histimpv            ;    // [string,    6] 역사적변동성                   StartPos 309, Length 6
    char    impv                [   6];    char    _impv                ;    // [string,    6] 내재변동성                     StartPos 316, Length 6
    char    sbasis              [   6];    char    _sbasis              ;    // [string,    6] 시장BASIS                      StartPos 323, Length 6
    char    ibasis              [   6];    char    _ibasis              ;    // [string,    6] 이론BASIS                      StartPos 330, Length 6
    char    gmfutcode           [   8];    char    _gmfutcode           ;    // [string,    8] 근월물종목코드                 StartPos 337, Length 8
    char    actprice            [   6];    char    _actprice            ;    // [string,    6] 행사가                         StartPos 346, Length 6
    char    realupdnyn          [   1];    char    _realupdnyn          ;    // [string,    1] 실시간상/하한가적용여부        StartPos 353, Length 1
    char    realuplmtprice      [   8];    char    _realuplmtprice      ;    // [string,    8] 실시간상한가                   StartPos 355, Length 8
    char    realdnlmtprice      [   8];    char    _realdnlmtprice      ;    // [string,    8] 실시간하한가                   StartPos 364, Length 8
    char    uplmtpricestep      [   2];    char    _uplmtpricestep      ;    // [string,    2] 상한가적용단계                 StartPos 373, Length 2
    char    uplmtprice1         [   6];    char    _uplmtprice1         ;    // [string,    6] 상한가1                        StartPos 376, Length 6
    char    uplmtprice2         [   6];    char    _uplmtprice2         ;    // [string,    6] 상한가2                        StartPos 383, Length 6
    char    uplmtprice3         [   6];    char    _uplmtprice3         ;    // [string,    6] 상한가3                        StartPos 390, Length 6
    char    dnlmtpricestep      [   2];    char    _dnlmtpricestep      ;    // [string,    2] 하한가적용단계                 StartPos 397, Length 2
    char    dnlmtprice1         [   6];    char    _dnlmtprice1         ;    // [string,    6] 하한가1                        StartPos 400, Length 6
    char    dnlmtprice2         [   6];    char    _dnlmtprice2         ;    // [string,    6] 하한가2                        StartPos 407, Length 6
    char    dnlmtprice3         [   6];    char    _dnlmtprice3         ;    // [string,    6] 하한가3                        StartPos 414, Length 6
} t3001OutBlock, *LPt3001OutBlock;
#define NAME_t3001OutBlock     "t3001OutBlock"


#pragma pack( pop )
///////////////////////////////////////////////////////////////////////////////////////////////////
