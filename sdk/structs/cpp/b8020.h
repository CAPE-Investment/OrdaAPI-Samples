#pragma once

///////////////////////////////////////////////////////////////////////////////////////////////////
// 계좌별특정종목가능수량 ( headtype=B ) 
#pragma pack( push, 1 )

#define NAME_b8020     "b8020"

// 입력
typedef struct _b8020InBlock
{
    char    reccnt              [   5];    // [string,    5] 레코드갯수                     StartPos 0, Length 5
    char    memegb              [   1];    // [string,    1] 매매구분                       StartPos 5, Length 1
    char    accno               [  20];    // [string,   20] 계좌번호                       StartPos 6, Length 20
    char    passwd              [   8];    // [string,    8] 비밀번호                       StartPos 26, Length 8
    char    expcode             [  12];    // [string,   12] 종목번호                       StartPos 34, Length 12
    char    hopemesu            [  13];    // [string,   13] 주문가                         StartPos 46, Length 13
    char    tongsingb           [   2];    // [string,    2] 통신매체코드                   StartPos 59, Length 2
    char    loangb              [   2];    // [string,    2] 대출유형코드                   StartPos 61, Length 2
} b8020InBlock, *LPb8020InBlock;
#define NAME_b8020InBlock     "b8020InBlock"

// 출력
typedef struct _b8020OutBlock
{
    char    reccnt              [   5];    // [string,    5] 레코드갯수                     StartPos 0, Length 5
    char    memegb              [   1];    // [string,    1] 매매구분                       StartPos 5, Length 1
    char    accno               [  20];    // [string,   20] 계좌번호                       StartPos 6, Length 20
    char    passwd              [   8];    // [string,    8] 비밀번호                       StartPos 26, Length 8
    char    expcode             [  12];    // [string,   12] 종목번호                       StartPos 34, Length 12
    char    hopemesu            [  13];    // [string,   13] 주문가                         StartPos 46, Length 13
    char    tongsingb           [   2];    // [string,    2] 통신매체코드                   StartPos 59, Length 2
    char    loangb              [   2];    // [string,    2] 대출유형코드                   StartPos 61, Length 2
} b8020OutBlock, *LPb8020OutBlock;
#define NAME_b8020OutBlock     "b8020OutBlock"

typedef struct _b8020OutBlock1
{
    char    reccnt              [   5];    // [string,    5] 레코드갯수                     StartPos 0, Length 5
    char    name                [  40];    // [string,   40] 계좌명                         StartPos 5, Length 40
    char    hname               [  40];    // [string,   40] 종목명                         StartPos 45, Length 40
    char    memegb              [  10];    // [string,   10] 매매구분                       StartPos 85, Length 10
    char    depoamt             [  16];    // [string,   16] 예수금                         StartPos 95, Length 16
    char    daeamt              [  16];    // [string,   16] 대용금액                       StartPos 111, Length 16
    char    resercash           [  16];    // [string,   16] 현금증거금액                   StartPos 127, Length 16
    char    reserdae            [  16];    // [string,   16] 대용증거금액                   StartPos 143, Length 16
    char    misuamt             [  16];    // [string,   16] 미수합계금액                   StartPos 159, Length 16
    char    reuseordamt         [  16];    // [string,   16] 재사용주문가능금액             StartPos 175, Length 16
    char    cashreuseamt        [  16];    // [string,   16] 현금재사용대상금액             StartPos 191, Length 16
    char    daereuseamt         [  16];    // [string,   16] 대용재사용대상금액             StartPos 207, Length 16
    char    bfreuseordamt       [  16];    // [string,   16] 전일재사용주문가능금액         StartPos 223, Length 16
    char    tdreuseordamt       [  16];    // [string,   16] 금일재사용주문가능금액         StartPos 239, Length 16
    char    cashordamt          [  16];    // [string,   16] 현금주문가능금액               StartPos 255, Length 16
    char    daeordamt           [  16];    // [string,   16] 대용주문가능금액               StartPos 271, Length 16
    char    sindbreuseamt       [  16];    // [string,   16] 신용담보재사용금액             StartPos 287, Length 16
    char    kspamt              [  16];    // [string,   16] 거래소금액                     StartPos 303, Length 16
    char    ksdamt              [  16];    // [string,   16] 코스닥금액                     StartPos 319, Length 16
    char    jumunamt            [  16];    // [string,   16] 가능금액                       StartPos 335, Length 16
    char    ordamt100           [  16];    // [string,   16] 증거금률100퍼센트주문가능금액  StartPos 351, Length 16
    char    reserordqty100      [  16];    // [string,   16] 증거금률100퍼센트주문가능수량  StartPos 367, Length 16
    char    loandamrate         [   9];    // [string,    9] 대출담보율                     StartPos 383, Length 9
    char    damrate             [   7];    // [string,    7] 담보유지비율                   StartPos 392, Length 7
    char    ordqty              [  16];    // [string,   16] 주문가능수량                   StartPos 399, Length 16
    char    michebuyamt         [  16];    // [string,   16] 미체결매수주문금액             StartPos 415, Length 16
    char    feerate             [  11];    // [string,   11] 수수료율                       StartPos 431, Length 11
    char    addfeeamt           [  16];    // [string,   16] 추가수수료징수금액             StartPos 442, Length 16
    char    jangoqty            [  16];    // [string,   16] 잔고수량                       StartPos 458, Length 16
    char    htsjumunamt         [  16];    // [string,   16] HTS주문가능금액                StartPos 474, Length 16
    char    d1depoamt           [  16];    // [string,   16] D+1예수금                      StartPos 490, Length 16
    char    d2depoamt           [  16];    // [string,   16] D2예수금                       StartPos 506, Length 16
    char    bfreuseordqty       [  16];    // [string,   16] 전일재사용주문가능수량         StartPos 522, Length 16
    char    tdreuseordqty       [  16];    // [string,   16] 금일재사용주문가능수량         StartPos 538, Length 16
    char    sinorderamt         [  16];    // [string,   16] 신용주문가능금액               StartPos 554, Length 16
} b8020OutBlock1, *LPb8020OutBlock1;
#define NAME_b8020OutBlock1     "b8020OutBlock1"


#pragma pack( pop )
///////////////////////////////////////////////////////////////////////////////////////////////////
