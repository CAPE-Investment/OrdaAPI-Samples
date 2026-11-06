#pragma once

///////////////////////////////////////////////////////////////////////////////////////////////////
// ETF현재가조회 ( attr,block,headtype=A ) 
#pragma pack( push, 1 )

#define NAME_t1351     "t1351"

// 입력
typedef struct _t1351InBlock
{
    char    shcode              [   6];    char    _shcode              ;    // [string,    6] 단축코드                       StartPos 0, Length 6
} t1351InBlock, *LPt1351InBlock;
#define NAME_t1351InBlock     "t1351InBlock"

// 출력
typedef struct _t1351OutBlock
{
    char    hname               [  20];    char    _hname               ;    // [string,   20] 한글명                         StartPos 0, Length 20
    char    price               [   8];    char    _price               ;    // [string,    8] 현재가                         StartPos 21, Length 8
    char    sign                [   1];    char    _sign                ;    // [string,    1] 전일대비구분                   StartPos 30, Length 1
    char    change              [   8];    char    _change              ;    // [string,    8] 전일대비                       StartPos 32, Length 8
    char    diff                [   6];    char    _diff                ;    // [string,    6] 등락율                         StartPos 41, Length 6
    char    volume              [  12];    char    _volume              ;    // [string,   12] 누적거래량                     StartPos 48, Length 12
    char    recprice            [   8];    char    _recprice            ;    // [string,    8] 기준가                         StartPos 61, Length 8
    char    avg                 [   8];    char    _avg                 ;    // [string,    8] 가중평균                       StartPos 70, Length 8
    char    uplmtprice          [   8];    char    _uplmtprice          ;    // [string,    8] 상한가                         StartPos 79, Length 8
    char    dnlmtprice          [   8];    char    _dnlmtprice          ;    // [string,    8] 하한가                         StartPos 88, Length 8
    char    jnilvolume          [  12];    char    _jnilvolume          ;    // [string,   12] 전일거래량                     StartPos 97, Length 12
    char    volumediff          [  12];    char    _volumediff          ;    // [string,   12] 거래량차                       StartPos 110, Length 12
    char    open                [   8];    char    _open                ;    // [string,    8] 시가                           StartPos 123, Length 8
    char    opentime            [   6];    char    _opentime            ;    // [string,    6] 시가시간                       StartPos 132, Length 6
    char    high                [   8];    char    _high                ;    // [string,    8] 고가                           StartPos 139, Length 8
    char    hightime            [   6];    char    _hightime            ;    // [string,    6] 고가시간                       StartPos 148, Length 6
    char    low                 [   8];    char    _low                 ;    // [string,    8] 저가                           StartPos 155, Length 8
    char    lowtime             [   6];    char    _lowtime             ;    // [string,    6] 저가시간                       StartPos 164, Length 6
    char    high52w             [   8];    char    _high52w             ;    // [string,    8] 52최고가                       StartPos 171, Length 8
    char    high52wdate         [   8];    char    _high52wdate         ;    // [string,    8] 52최고가일                     StartPos 180, Length 8
    char    low52w              [   8];    char    _low52w              ;    // [string,    8] 52최저가                       StartPos 189, Length 8
    char    low52wdate          [   8];    char    _low52wdate          ;    // [string,    8] 52최저가일                     StartPos 198, Length 8
    char    exhratio            [   6];    char    _exhratio            ;    // [string,    6] 소진율                         StartPos 207, Length 6
    char    flmtvol             [  12];    char    _flmtvol             ;    // [string,   12] 외국인보유수량                 StartPos 214, Length 12
    char    per                 [   6];    char    _per                 ;    // [string,    6] PER                            StartPos 227, Length 6
    char    listing             [  12];    char    _listing             ;    // [string,   12] 상장주식수(천)                 StartPos 234, Length 12
    char    jkrate              [   8];    char    _jkrate              ;    // [string,    8] 증거금율                       StartPos 247, Length 8
    char    vol                 [   6];    char    _vol                 ;    // [string,    6] 회전율                         StartPos 256, Length 6
    char    shcode              [   6];    char    _shcode              ;    // [string,    6] 단축코드                       StartPos 263, Length 6
    char    value               [  12];    char    _value               ;    // [string,   12] 누적거래대금                   StartPos 270, Length 12
    char    highyear            [   8];    char    _highyear            ;    // [string,    8] 연중최고가                     StartPos 283, Length 8
    char    highyeardate        [   8];    char    _highyeardate        ;    // [string,    8] 연중최고일자                   StartPos 292, Length 8
    char    lowyear             [   8];    char    _lowyear             ;    // [string,    8] 연중최저가                     StartPos 301, Length 8
    char    lowyeardate         [   8];    char    _lowyeardate         ;    // [string,    8] 연중최저일자                   StartPos 310, Length 8
    char    upname              [  20];    char    _upname              ;    // [string,   20] 업종명                         StartPos 319, Length 20
    char    upcode              [   3];    char    _upcode              ;    // [string,    3] 업종코드                       StartPos 340, Length 3
    char    upprice             [   7];    char    _upprice             ;    // [string,    7] 업종현재가                     StartPos 344, Length 7
    char    upsign              [   1];    char    _upsign              ;    // [string,    1] 업종전일비구분                 StartPos 352, Length 1
    char    upchange            [   6];    char    _upchange            ;    // [string,    6] 업종전일대비                   StartPos 354, Length 6
    char    updiff              [   6];    char    _updiff              ;    // [string,    6] 업종등락율                     StartPos 361, Length 6
    char    futname             [  20];    char    _futname             ;    // [string,   20] 선물최근월물명                 StartPos 368, Length 20
    char    futcode             [   8];    char    _futcode             ;    // [string,    8] 선물최근월물코드               StartPos 389, Length 8
    char    futprice            [   6];    char    _futprice            ;    // [string,    6] 선물현재가                     StartPos 398, Length 6
    char    futsign             [   1];    char    _futsign             ;    // [string,    1] 선물전일비구분                 StartPos 405, Length 1
    char    futchange           [   6];    char    _futchange           ;    // [string,    6] 선물전일대비                   StartPos 407, Length 6
    char    futdiff             [   6];    char    _futdiff             ;    // [string,    6] 선물등락율                     StartPos 414, Length 6
    char    nav                 [   8];    char    _nav                 ;    // [string,    8] NAV                            StartPos 421, Length 8
    char    navsign             [   1];    char    _navsign             ;    // [string,    1] NAV전일대비구분                StartPos 430, Length 1
    char    navchange           [   8];    char    _navchange           ;    // [string,    8] NAV전일대비                    StartPos 432, Length 8
    char    navdiff             [   6];    char    _navdiff             ;    // [string,    6] NAV등락율                      StartPos 441, Length 6
    char    cocrate             [   6];    char    _cocrate             ;    // [string,    6] 추적오차율                     StartPos 448, Length 6
    char    kasis               [   6];    char    _kasis               ;    // [string,    6] 괴리율                         StartPos 455, Length 6
    char    subprice            [  10];    char    _subprice            ;    // [string,   10] 대용가                         StartPos 462, Length 10
    char    offerno1            [   6];    char    _offerno1            ;    // [string,    6] 매도증권사코드1                StartPos 473, Length 6
    char    bidno1              [   6];    char    _bidno1              ;    // [string,    6] 매수증권사코드1                StartPos 480, Length 6
    char    dvol1               [   8];    char    _dvol1               ;    // [string,    8] 총매도수량1                    StartPos 487, Length 8
    char    svol1               [   8];    char    _svol1               ;    // [string,    8] 총매수수량1                    StartPos 496, Length 8
    char    dcha1               [   8];    char    _dcha1               ;    // [string,    8] 매도증감1                      StartPos 505, Length 8
    char    scha1               [   8];    char    _scha1               ;    // [string,    8] 매수증감1                      StartPos 514, Length 8
    char    ddiff1              [   6];    char    _ddiff1              ;    // [string,    6] 매도비율1                      StartPos 523, Length 6
    char    sdiff1              [   6];    char    _sdiff1              ;    // [string,    6] 매수비율1                      StartPos 530, Length 6
    char    offerno2            [   6];    char    _offerno2            ;    // [string,    6] 매도증권사코드2                StartPos 537, Length 6
    char    bidno2              [   6];    char    _bidno2              ;    // [string,    6] 매수증권사코드2                StartPos 544, Length 6
    char    dvol2               [   8];    char    _dvol2               ;    // [string,    8] 총매도수량2                    StartPos 551, Length 8
    char    svol2               [   8];    char    _svol2               ;    // [string,    8] 총매수수량2                    StartPos 560, Length 8
    char    dcha2               [   8];    char    _dcha2               ;    // [string,    8] 매도증감2                      StartPos 569, Length 8
    char    scha2               [   8];    char    _scha2               ;    // [string,    8] 매수증감2                      StartPos 578, Length 8
    char    ddiff2              [   6];    char    _ddiff2              ;    // [string,    6] 매도비율2                      StartPos 587, Length 6
    char    sdiff2              [   6];    char    _sdiff2              ;    // [string,    6] 매수비율2                      StartPos 594, Length 6
    char    offerno3            [   6];    char    _offerno3            ;    // [string,    6] 매도증권사코드3                StartPos 601, Length 6
    char    bidno3              [   6];    char    _bidno3              ;    // [string,    6] 매수증권사코드3                StartPos 608, Length 6
    char    dvol3               [   8];    char    _dvol3               ;    // [string,    8] 총매도수량3                    StartPos 615, Length 8
    char    svol3               [   8];    char    _svol3               ;    // [string,    8] 총매수수량3                    StartPos 624, Length 8
    char    dcha3               [   8];    char    _dcha3               ;    // [string,    8] 매도증감3                      StartPos 633, Length 8
    char    scha3               [   8];    char    _scha3               ;    // [string,    8] 매수증감3                      StartPos 642, Length 8
    char    ddiff3              [   6];    char    _ddiff3              ;    // [string,    6] 매도비율3                      StartPos 651, Length 6
    char    sdiff3              [   6];    char    _sdiff3              ;    // [string,    6] 매수비율3                      StartPos 658, Length 6
    char    offerno4            [   6];    char    _offerno4            ;    // [string,    6] 매도증권사코드4                StartPos 665, Length 6
    char    bidno4              [   6];    char    _bidno4              ;    // [string,    6] 매수증권사코드4                StartPos 672, Length 6
    char    dvol4               [   8];    char    _dvol4               ;    // [string,    8] 총매도수량4                    StartPos 679, Length 8
    char    svol4               [   8];    char    _svol4               ;    // [string,    8] 총매수수량4                    StartPos 688, Length 8
    char    dcha4               [   8];    char    _dcha4               ;    // [string,    8] 매도증감4                      StartPos 697, Length 8
    char    scha4               [   8];    char    _scha4               ;    // [string,    8] 매수증감4                      StartPos 706, Length 8
    char    ddiff4              [   6];    char    _ddiff4              ;    // [string,    6] 매도비율4                      StartPos 715, Length 6
    char    sdiff4              [   6];    char    _sdiff4              ;    // [string,    6] 매수비율4                      StartPos 722, Length 6
    char    offerno5            [   6];    char    _offerno5            ;    // [string,    6] 매도증권사코드5                StartPos 729, Length 6
    char    bidno5              [   6];    char    _bidno5              ;    // [string,    6] 매수증권사코드5                StartPos 736, Length 6
    char    dvol5               [   8];    char    _dvol5               ;    // [string,    8] 총매도수량5                    StartPos 743, Length 8
    char    svol5               [   8];    char    _svol5               ;    // [string,    8] 총매수수량5                    StartPos 752, Length 8
    char    dcha5               [   8];    char    _dcha5               ;    // [string,    8] 매도증감5                      StartPos 761, Length 8
    char    scha5               [   8];    char    _scha5               ;    // [string,    8] 매수증감5                      StartPos 770, Length 8
    char    ddiff5              [   6];    char    _ddiff5              ;    // [string,    6] 매도비율5                      StartPos 779, Length 6
    char    sdiff5              [   6];    char    _sdiff5              ;    // [string,    6] 매수비율5                      StartPos 786, Length 6
    char    fwdvl               [  12];    char    _fwdvl               ;    // [string,   12] 외국계매도합계수량             StartPos 793, Length 12
    char    ftradmdcha          [  12];    char    _ftradmdcha          ;    // [string,   12] 외국계매도직전대비             StartPos 806, Length 12
    char    ftradmddiff         [   6];    char    _ftradmddiff         ;    // [string,    6] 외국계매도비율                 StartPos 819, Length 6
    char    fwsvl               [  12];    char    _fwsvl               ;    // [string,   12] 외국계매수합계수량             StartPos 826, Length 12
    char    ftradmscha          [  12];    char    _ftradmscha          ;    // [string,   12] 외국계매수직전대비             StartPos 839, Length 12
    char    ftradmsdiff         [   6];    char    _ftradmsdiff         ;    // [string,    6] 외국계매수비율                 StartPos 852, Length 6
    char    tax                 [   1];    char    _tax                 ;    // [string,    1] 세금종류                       StartPos 859, Length 1
    char    etnbasnm            [  80];    char    _etnbasnm            ;    // [string,   80] ETN대상지수                    StartPos 861, Length 80
    char    etnnum              [   6];    char    _etnnum              ;    // [string,    6] ETN구성종목수                  StartPos 942, Length 6
    char    etnlpqty            [  12];    char    _etnlpqty            ;    // [string,   12] ETNLP보유수량                  StartPos 949, Length 12
    char    etnissuer           [  40];    char    _etnissuer           ;    // [string,   40] ETN발행사                      StartPos 962, Length 40
    char    etnjisuassetid      [  12];    char    _etnjisuassetid      ;    // [string,   12] ETN자산분류                    StartPos 1003, Length 12
    char    etnlastdt           [   8];    char    _etnlastdt           ;    // [string,    8] ETN최종거래일                  StartPos 1016, Length 8
    char    etnpayday           [   8];    char    _etnpayday           ;    // [string,    8] ETN지급일                      StartPos 1025, Length 8
    char    etnprebaseprice     [   9];    char    _etnprebaseprice     ;    // [string,    9] ETN과표기준가                  StartPos 1034, Length 9
    char    etnlast_iiv         [   9];    char    _etnlast_iiv         ;    // [string,    9] ETN지표가치                    StartPos 1044, Length 9
    char    etniiv_amt          [  15];    char    _etniiv_amt          ;    // [string,   15] ETN지표가치금액                StartPos 1054, Length 15
    char    midrfunqty          [  10];    char    _midrfunqty          ;    // [string,   10] 중도상환최소청구수량           StartPos 1070, Length 10
    char    midrfunfee          [  10];    char    _midrfunfee          ;    // [string,   10] 중도상환수수료율               StartPos 1081, Length 10
    char    investatt           [   1];    char    _investatt           ;    // [string,    1] 투자유의구분Y/N                StartPos 1092, Length 1
    char    etftrcrate          [   6];    char    _etftrcrate          ;    // [string,    6] 추적수익율배수                 StartPos 1094, Length 6
    char    ordpopup            [  12];    char    _ordpopup            ;    // [string,   12] 투자유의주문팝업               StartPos 1101, Length 12
    char    operhname           [  40];    char    _operhname           ;    // [string,   40] 운용사명                       StartPos 1114, Length 40
    char    main_close          [   8];    char    _main_close          ;    // [string,    8] KRX종가                        StartPos 1155, Length 8
} t1351OutBlock, *LPt1351OutBlock;
#define NAME_t1351OutBlock     "t1351OutBlock"


#pragma pack( pop )
///////////////////////////////////////////////////////////////////////////////////////////////////
