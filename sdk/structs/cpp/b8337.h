#pragma once

///////////////////////////////////////////////////////////////////////////////////////////////////
// 증거금율별주문가능금액 조회 ( headtype=B ) 
#pragma pack( push, 1 )

#define NAME_b8337     "b8337"

// 입력
typedef struct _b8337InBlock
{
    char    reccnt              [   5];    // [string,    5] 레코드갯수                     StartPos 0, Length 5
    char    memegb              [   1];    // [string,    1] 매매구분                       StartPos 5, Length 1
    char    accno               [  20];    // [string,   20] 계좌번호                       StartPos 6, Length 20
    char    passwd              [   8];    // [string,    8] 입력비밀번호                   StartPos 26, Length 8
    char    expcode             [  12];    // [string,   12] 종목번호                       StartPos 34, Length 12
    char    hopemesu            [  13];    // [string,   13] 주문가                         StartPos 46, Length 13
    char    mechegb             [   2];    // [string,    2] 통신매체코드                   StartPos 59, Length 2
} b8337InBlock, *LPb8337InBlock;
#define NAME_b8337InBlock     "b8337InBlock"

// 출력
typedef struct _b8337OutBlock
{
    char    reccnt              [   5];    // [string,    5] 레코드갯수                     StartPos 0, Length 5
    char    memegb              [   1];    // [string,    1] 매매구분                       StartPos 5, Length 1
    char    accno               [  20];    // [string,   20] 계좌번호                       StartPos 6, Length 20
    char    passwd              [   8];    // [string,    8] 입력비밀번호                   StartPos 26, Length 8
    char    expcode             [  12];    // [string,   12] 종목번호                       StartPos 34, Length 12
    char    hopemesu            [  13];    // [string,   13] 주문가                         StartPos 46, Length 13
    char    mechegb             [   2];    // [string,    2] 통신매체코드                   StartPos 59, Length 2
} b8337OutBlock, *LPb8337OutBlock;
#define NAME_b8337OutBlock     "b8337OutBlock"

typedef struct _b8337OutBlock1
{
    char    reccnt              [   5];    // [string,    5] 레코드갯수                     StartPos 0, Length 5
    char    accname             [  40];    // [string,   40] 계좌명                         StartPos 5, Length 40
    char    codename            [  40];    // [string,   40] 종목명                         StartPos 45, Length 40
    char    depoamt             [  16];    // [string,   16] 예수금                         StartPos 85, Length 16
    char    daeamt              [  16];    // [string,   16] 대용금액                       StartPos 101, Length 16
    char    cdamreuse           [  16];    // [string,   16] 신용담보재사용금액             StartPos 117, Length 16
    char    cashordamt          [  16];    // [string,   16] 현금주문가능금액               StartPos 133, Length 16
    char    orddae              [  16];    // [string,   16] 대용주문가능금액               StartPos 149, Length 16
    char    margincash          [  16];    // [string,   16] 현금증거금액                   StartPos 165, Length 16
    char    margindae           [  16];    // [string,   16] 대용증거금액                   StartPos 181, Length 16
    char    kospiamt            [  16];    // [string,   16] 거래소금액                     StartPos 197, Length 16
    char    kosdaqamt           [  16];    // [string,   16] 코스닥금액                     StartPos 213, Length 16
    char    d1cash              [  16];    // [string,   16] 추정예수금(D+1)                StartPos 229, Length 16
    char    d2cash              [  16];    // [string,   16] 추정예수금(D+2)                StartPos 245, Length 16
    char    chulcanamt          [  16];    // [string,   16] 출금가능금액                   StartPos 261, Length 16
    char    misuamt             [  16];    // [string,   16] 미수금액                       StartPos 277, Length 16
    char    susuryorate         [  11];    // [string,   11] 수수료율                       StartPos 293, Length 11
    char    addchamt            [  16];    // [string,   16] 추가징수금액                   StartPos 304, Length 16
    char    resuseamt           [  16];    // [string,   16] 재사용대상금액                 StartPos 320, Length 16
    char    cashresuseamt       [  16];    // [string,   16] 현금재사용대상금액             StartPos 336, Length 16
    char    usemarginrate       [   7];    // [string,    7] 이용사증거금률                 StartPos 352, Length 7
    char    daereuseamt         [  16];    // [string,   16] 대용재사용대상금액             StartPos 359, Length 16
    char    daeresuseamt        [   7];    // [string,    7] 종목증거금률                   StartPos 375, Length 7
    char    charate             [   7];    // [string,    7] 계좌증거금률                   StartPos 382, Length 7
    char    marginrate          [   7];    // [string,    7] 거래증거금률                   StartPos 389, Length 7
    char    susuryo             [  16];    // [string,   16] 수수료                         StartPos 396, Length 16
    char    ordamt20            [  16];    // [string,   16] 증거금률20퍼센트주문가능금액   StartPos 412, Length 16
    char    ordqty20            [  16];    // [string,   16] 증거금률20퍼센트주문가능수량   StartPos 428, Length 16
    char    ordamt30            [  16];    // [string,   16] 증거금률30퍼센트주문가능금액   StartPos 444, Length 16
    char    ordqty30            [  16];    // [string,   16] 증거금률30퍼센트주문가능수량   StartPos 460, Length 16
    char    ordamt40            [  16];    // [string,   16] 증거금률40퍼센트주문가능금액   StartPos 476, Length 16
    char    ordqty40            [  16];    // [string,   16] 증거금률40퍼센트주문가능수량   StartPos 492, Length 16
    char    ordamt100           [  16];    // [string,   16] 증거금률100퍼센트주문가능금액  StartPos 508, Length 16
    char    ordqty100           [  16];    // [string,   16] 증거금률100퍼센트주문가능수량  StartPos 524, Length 16
    char    ordcashamt100       [  16];    // [string,   16] 증거금률100퍼센트현금주문가능금액 StartPos 540, Length 16
    char    ordcashqty100       [  16];    // [string,   16] 증거금률100퍼센트현금주문가능수량 StartPos 556, Length 16
    char    reamt20             [  16];    // [string,   16] 증거금률20퍼센트재사용가능금액 StartPos 572, Length 16
    char    reamt30             [  16];    // [string,   16] 증거금률30퍼센트재사용가능금액 StartPos 588, Length 16
    char    reamt40             [  16];    // [string,   16] 증거금률40퍼센트재사용가능금액 StartPos 604, Length 16
    char    reamt100            [  16];    // [string,   16] 증거금률100퍼센트재사용가능금액 StartPos 620, Length 16
    char    memeqty             [  16];    // [string,   16] 주문가능수량                   StartPos 636, Length 16
    char    ordamt              [  16];    // [string,   16] 주문가능금액                   StartPos 652, Length 16
} b8337OutBlock1, *LPb8337OutBlock1;
#define NAME_b8337OutBlock1     "b8337OutBlock1"


#pragma pack( pop )
///////////////////////////////////////////////////////////////////////////////////////////////////
