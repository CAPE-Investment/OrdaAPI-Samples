#pragma once

///////////////////////////////////////////////////////////////////////////////////////////////////
// 현물계좌별미결제잔고조회 ( headtype=B )
#pragma pack( push, 1 )

#define NAME_b7001     "b7001"

// 입력
typedef struct _b7001InBlock
{
    char    In                  [   5];    // [string,    5] In                             StartPos 0, Length 5
    char    acntno              [  20];    // [string,   20] 계좌번호                       StartPos 5, Length 20
    char    pwd                 [   8];    // [string,    8] 비밀번호                       StartPos 25, Length 8
    char    balcretp            [   1];    // [string,    1] 잔고생성구분                   StartPos 33, Length 1
    char    cmsnamtapptp        [   1];    // [string,    1] 수수료적용구분                 StartPos 34, Length 1
    char    d2balbaseqrytp      [   1];    // [string,    1] d2잔고기준조회구분             StartPos 35, Length 1
} b7001InBlock, *LPb7001InBlock;
#define NAME_b7001InBlock     "b7001InBlock"

// 출력
typedef struct _b7001OutBlock
{
    char    In                  [   5];    // [string,    5] In                             StartPos 0, Length 5
    char    acntno              [  20];    // [string,   20] 계좌번호                       StartPos 5, Length 20
    char    pwd                 [   8];    // [string,    8] 비밀번호                       StartPos 25, Length 8
    char    balcretp            [   1];    // [string,    1] 잔고생성구분                   StartPos 33, Length 1
    char    cmsnamtapptp        [   1];    // [string,    1] 수수료적용구분                 StartPos 34, Length 1
    char    d2balbaseqrytp      [   1];    // [string,    1] d2잔고기준조회구분             StartPos 35, Length 1
} b7001OutBlock, *LPb7001OutBlock;
#define NAME_b7001OutBlock     "b7001OutBlock"

typedef struct _b7001OutBlock1
{
    char    Out                 [   5];    // [string,    5] Out                            StartPos 0, Length 5
    char    brnnm               [  40];    // [string,   40] 지점명                         StartPos 5, Length 40
    char    acntnm              [  40];    // [string,   40] 계좌명                         StartPos 45, Length 40
    char    mnyordableamt       [  16];    // [string,   16] 현금주문가능금액               StartPos 85, Length 16
    char    mnyoutableamt       [  16];    // [string,   16] 출금가능금액                   StartPos 101, Length 16
    char    seordableamt        [  16];    // [string,   16] 거래소금액                     StartPos 117, Length 16
    char    kdqordableamt       [  16];    // [string,   16] 코스닥금액                     StartPos 133, Length 16
    char    htsordableamt       [  16];    // [string,   16] hts주문가능금액                StartPos 149, Length 16
    char    mgnrat100pctordableamt[  16];    // [string,   16] 증거금률100퍼센트주문가능금액  StartPos 165, Length 16
    char    evalamt             [  16];    // [string,   16] 평가금액                       StartPos 181, Length 16
    char    pchsamt             [  16];    // [string,   16] 매입금액                       StartPos 197, Length 16
    char    rcvblamt            [  16];    // [string,   16] 미수금액                       StartPos 213, Length 16
    char    mnyrclamt           [  16];    // [string,   16] 현금미수금액                   StartPos 229, Length 16
    char    pnlrat              [  12];    // [string,   12] 손익률                         StartPos 245, Length 12
    char    invstorgamt         [  20];    // [string,   20] 투자원금                       StartPos 257, Length 20
    char    invstplamt          [  16];    // [string,   16] 투자손익금액                   StartPos 277, Length 16
    char    pldgmnyamt          [  16];    // [string,   16] 담보현금금액                   StartPos 293, Length 16
    char    pldgsubstamt        [  16];    // [string,   16] 담보대용금액                   StartPos 309, Length 16
    char    crdtpldgordamt      [  16];    // [string,   16] 신용담보주문금액               StartPos 325, Length 16
    char    dps                 [  16];    // [string,   16] 예수금                         StartPos 341, Length 16
    char    d1dps               [  16];    // [string,   16] d1예수금                       StartPos 357, Length 16
    char    d2dps               [  16];    // [string,   16] d2예수금                       StartPos 373, Length 16
    char    orddt               [   8];    // [string,    8] 주문일                         StartPos 389, Length 8
    char    mnymgn              [  16];    // [string,   16] 현금증거금액                   StartPos 397, Length 16
    char    substmgn            [  16];    // [string,   16] 대용증거금액                   StartPos 413, Length 16
    char    substamt            [  16];    // [string,   16] 대용금액                       StartPos 429, Length 16
    char    prdaybuyexecamt     [  16];    // [string,   16] 전일매수체결금액               StartPos 445, Length 16
    char    prdaysellexecamt    [  16];    // [string,   16] 전일매도체결금액               StartPos 461, Length 16
    char    crdaybuyexecamt     [  16];    // [string,   16] 금일매수체결금액               StartPos 477, Length 16
    char    crdaysellexecamt    [  16];    // [string,   16] 금일매도체결금액               StartPos 493, Length 16
    char    evalpnlsum          [  15];    // [string,   15] 평가손익합계                   StartPos 509, Length 15
    char    dpsasttotamt        [  16];    // [string,   16] 예탁자산총액                   StartPos 524, Length 16
    char    evrprc              [  19];    // [string,   19] 제비용                         StartPos 540, Length 19
    char    ruseamt             [  16];    // [string,   16] 재사용금액                     StartPos 559, Length 16
    char    etclndamt           [  16];    // [string,   16] 기타대여금액                   StartPos 575, Length 16
    char    prcadjstamt         [  16];    // [string,   16] 가정산금액                     StartPos 591, Length 16
    char    d1cmsnamt           [  16];    // [string,   16] d1수수료                       StartPos 607, Length 16
    char    d2cmsnamt           [  16];    // [string,   16] d2수수료                       StartPos 623, Length 16
    char    d1evrtax            [  16];    // [string,   16] d1제세금                       StartPos 639, Length 16
    char    d2evrtax            [  16];    // [string,   16] d2제세금                       StartPos 655, Length 16
    char    d1settprergamt      [  16];    // [string,   16] d1결제예정금액                 StartPos 671, Length 16
    char    d2settprergamt      [  16];    // [string,   16] d2결제예정금액                 StartPos 687, Length 16
    char    prdayksemnymgn      [  16];    // [string,   16] 전일kse현금증거금              StartPos 703, Length 16
    char    prdayksesubstmgn    [  16];    // [string,   16] 전일kse대용증거금              StartPos 719, Length 16
    char    prdayksecrdtmnymgn  [  16];    // [string,   16] 전일kse신용현금증거금          StartPos 735, Length 16
    char    prdayksecrdtsubstmgn[  16];    // [string,   16] 전일kse신용대용증거금          StartPos 751, Length 16
    char    crdayksemnymgn      [  16];    // [string,   16] 금일kse현금증거금              StartPos 767, Length 16
    char    crdayksesubstmgn    [  16];    // [string,   16] 금일kse대용증거금              StartPos 783, Length 16
    char    crdayksecrdtmnymgn  [  16];    // [string,   16] 금일kse신용현금증거금          StartPos 799, Length 16
    char    crdayksecrdtsubstmgn[  16];    // [string,   16] 금일kse신용대용증거금          StartPos 815, Length 16
    char    prdaykdqmnymgn      [  16];    // [string,   16] 전일코스닥현금증거금           StartPos 831, Length 16
    char    prdaykdqsubstmgn    [  16];    // [string,   16] 전일코스닥대용증거금           StartPos 847, Length 16
    char    prdaykdqcrdtmnymgn  [  16];    // [string,   16] 전일코스닥신용현금증거금       StartPos 863, Length 16
    char    prdaykdqcrdtsubstmgn[  16];    // [string,   16] 전일코스닥신용대용증거금       StartPos 879, Length 16
    char    crdaykdqmnymgn      [  16];    // [string,   16] 금일코스닥현금증거금           StartPos 895, Length 16
    char    crdaykdqsubstmgn    [  16];    // [string,   16] 금일코스닥대용증거금           StartPos 911, Length 16
    char    crdaykdqcrdtmnymgn  [  16];    // [string,   16] 금일코스닥신용현금증거금       StartPos 927, Length 16
    char    crdaykdqcrdtsubstmgn[  16];    // [string,   16] 금일코스닥신용대용증거금       StartPos 943, Length 16
    char    prdayfrbrdmnymgn    [  16];    // [string,   16] 전일프리보드현금증거금         StartPos 959, Length 16
    char    prdayfrbrdsubstmgn  [  16];    // [string,   16] 전일프리보드대용증거금         StartPos 975, Length 16
    char    crdayfrbrdmnymgn    [  16];    // [string,   16] 금일프리보드현금증거금         StartPos 991, Length 16
    char    crdayfrbrdsubstmgn  [  16];    // [string,   16] 금일프리보드대용증거금         StartPos 1007, Length 16
    char    prdaycrbmkmnymgn    [  16];    // [string,   16] 전일장외현금증거금             StartPos 1023, Length 16
    char    prdaycrbmksubstmgn  [  16];    // [string,   16] 전일장외대용증거금             StartPos 1039, Length 16
    char    crdaycrbmkmnymgn    [  16];    // [string,   16] 금일장외현금증거금             StartPos 1055, Length 16
    char    crdaycrbmksubstmgn  [  16];    // [string,   16] 금일장외대용증거금             StartPos 1071, Length 16
    char    dpspdgqty           [  16];    // [string,   16] 예탁담보수량                   StartPos 1087, Length 16
    char    d1substamt          [  16];    // [string,   16] d1대용금                       StartPos 1103, Length 16
    char    d2substamt          [  16];    // [string,   16] d2대용금                       StartPos 1119, Length 16
    char    d1mnyoutableamt     [  16];    // [string,   16] d1출금가능금액                 StartPos 1135, Length 16
    char    d2mnyoutableamt     [  16];    // [string,   16] d2출금가능금액                 StartPos 1151, Length 16
    char    addpldgmny          [  16];    // [string,   16] 추가담보현금                   StartPos 1167, Length 16
    char    addcrdtpldgsubst    [  16];    // [string,   16] 추가신용담보대용               StartPos 1183, Length 16
} b7001OutBlock1, *LPb7001OutBlock1;
#define NAME_b7001OutBlock1     "b7001OutBlock1"

typedef struct _b7001OutBlock2
{
    char    isuno               [  12];    // [string,   12] 종목번호                       StartPos 0, Length 12
    char    isunm               [  40];    // [string,   40] 종목명                         StartPos 12, Length 40
    char    secbalptncode       [   2];    // [string,    2] 유가증권잔고유형코드           StartPos 52, Length 2
    char    secbalptnnm         [  40];    // [string,   40] 유가증권잔고유형명             StartPos 54, Length 40
    char    balqty              [  16];    // [string,   16] 잔고수량                       StartPos 94, Length 16
    char    bnsbasebalqty       [  16];    // [string,   16] 매매기준잔고수량               StartPos 110, Length 16
    char    crdaybuyexecqty     [  16];    // [string,   16] 금일매수체결수량               StartPos 126, Length 16
    char    crdaysellexecqty    [  16];    // [string,   16] 금일매도체결수량               StartPos 142, Length 16
    char    selprc              [  11];    // [string,   11] 매도가                         StartPos 158, Length 11
    char    buyprc              [  13];    // [string,   13] 매수가                         StartPos 169, Length 13
    char    sellpnlamt          [  16];    // [string,   16] 매도손익금액                   StartPos 182, Length 16
    char    pnlrat              [  12];    // [string,   12] 손익률                         StartPos 198, Length 12
    char    nowprc              [  13];    // [string,   13] 현재가                         StartPos 210, Length 13
    char    crdtamt             [  16];    // [string,   16] 신용금액                       StartPos 223, Length 16
    char    duedt               [   8];    // [string,    8] 만기일                         StartPos 239, Length 8
    char    prdaysellexecprc    [  13];    // [string,   13] 전일매도체결가                 StartPos 247, Length 13
    char    prdaysellqty        [  16];    // [string,   16] 전일매도수량                   StartPos 260, Length 16
    char    prdaybuyexecprc     [  13];    // [string,   13] 전일매수체결가                 StartPos 276, Length 13
    char    prdaybuyqty         [  16];    // [string,   16] 전일매수수량                   StartPos 289, Length 16
    char    loandt              [   8];    // [string,    8] 대출일                         StartPos 305, Length 8
    char    avruprc2            [  15];    // [string,   15] 평균단가2                      StartPos 313, Length 15
    char    sellableqty         [  16];    // [string,   16] 매도가능수량                   StartPos 328, Length 16
    char    sellordqty          [  16];    // [string,   16] 매도주문수량                   StartPos 344, Length 16
    char    crdaybuyexecamt     [  16];    // [string,   16] 금일매수체결금액               StartPos 360, Length 16
    char    crdaysellexecamt    [  16];    // [string,   16] 금일매도체결금액               StartPos 376, Length 16
    char    prdaybuyexecamt     [  16];    // [string,   16] 전일매수체결금액               StartPos 392, Length 16
    char    prdaysellexecamt    [  16];    // [string,   16] 전일매도체결금액               StartPos 408, Length 16
    char    evalamt             [  16];    // [string,   16] 평가금액                       StartPos 424, Length 16
    char    evalpnl             [  16];    // [string,   16] 평가손익                       StartPos 440, Length 16
    char    mnyordableamt       [  16];    // [string,   16] 현금주문가능금액               StartPos 456, Length 16
    char    ordableamt          [  16];    // [string,   16] 주문가능금액                   StartPos 472, Length 16
    char    sellunercqty        [  16];    // [string,   16] 매도미체결수량                 StartPos 488, Length 16
    char    sellunsttqty        [  16];    // [string,   16] 매도미결제수량                 StartPos 504, Length 16
    char    buyunsttqty         [  16];    // [string,   16] 매수미결제수량                 StartPos 520, Length 16
    char    unsttqty            [  16];    // [string,   16] 미결제수량                     StartPos 536, Length 16
    char    prdaycprc           [  13];    // [string,   13] 전일종가                       StartPos 552, Length 13
    char    pchsamt             [  16];    // [string,   16] 매입금액                       StartPos 565, Length 16
    char    regmktcode          [   2];    // [string,    2] 등록시장코드                   StartPos 581, Length 2
    char    loandtlclsscode     [   2];    // [string,    2] 대출상세분류코드               StartPos 583, Length 2
    char    dpspdgloanqty       [  16];    // [string,   16] 예탁담보대출수량               StartPos 585, Length 16
    char    ropsetupval         [  18];    // [string,   18] 질권설정수량                   StartPos 601, Length 18
} b7001OutBlock2, *LPb7001OutBlock2;
#define NAME_b7001OutBlock2     "b7001OutBlock2"

typedef struct _b7001OutBlock3
{
    char    substdsgnptncode    [   2];    // [string,    2] 대용지정유형코드               StartPos 0, Length 2
    char    setupptnnm          [  40];    // [string,   40] 설정유형명                     StartPos 2, Length 40
    char    isuno               [  12];    // [string,   12] 종목번호                       StartPos 42, Length 12
    char    isunm               [  40];    // [string,   40] 종목명                         StartPos 54, Length 40
    char    futsacntno          [  20];    // [string,   20] 선물계좌번호                   StartPos 94, Length 20
    char    pdptncode           [   2];    // [string,    2] 상품유형코드                   StartPos 114, Length 2
    char    pdptnnm             [  40];    // [string,   40] 상품유형명                     StartPos 116, Length 40
    char    fnosubstdsgnqty     [  16];    // [string,   16] 선물옵션대용지정수량           StartPos 156, Length 16
    char    fnosubstsellordqty  [  16];    // [string,   16] 선물옵션대용매도주문수량       StartPos 172, Length 16
    char    fnosubstsellexecqty [  16];    // [string,   16] 선물옵션대용매도체결수량       StartPos 188, Length 16
    char    fnosubstsellexecamt [  16];    // [string,   16] 선물옵션대용매도체결금액       StartPos 204, Length 16
    char    crdaykpiftsubtsellexecamt[  16];    // [string,   16] 금일코스피선물대용매도체결금액 StartPos 220, Length 16
    char    crdaykpiftsubtsellexecqty[  16];    // [string,   16] 금일코스피선물대용매도체결수량 StartPos 236, Length 16
    char    regmktcode          [   2];    // [string,    2] 등록시장코드                   StartPos 252, Length 2
} b7001OutBlock3, *LPb7001OutBlock3;
#define NAME_b7001OutBlock3     "b7001OutBlock3"


#pragma pack( pop )
///////////////////////////////////////////////////////////////////////////////////////////////////