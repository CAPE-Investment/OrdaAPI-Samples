#pragma once

///////////////////////////////////////////////////////////////////////////////////////////////////
// 선물옵션 일자별 잔고 및 평가현황 ( headtype=B ) 
#pragma pack( push, 1 )

#define NAME_b8804     "b8804"

// 입력
typedef struct _b8804InBlock
{
    char    reccnt              [   5];    // [string,    5] 레코드갯수                     StartPos 0, Length 5
    char    accno               [  20];    // [string,   20] 계좌번호                       StartPos 5, Length 20
    char    passwd              [   8];    // [string,    8] 입력비밀번호                   StartPos 25, Length 8
    char    orddt               [   8];    // [string,    8] 주문일                         StartPos 33, Length 8
    char    jangestgb           [   1];    // [string,    1] 잔고평가구분                   StartPos 41, Length 1
    char    futpriceestgb       [   1];    // [string,    1] 선물가격평가구분               StartPos 42, Length 1
    char    chsuqrygb           [   1];    // [string,    1] 청산수량조회구분               StartPos 43, Length 1
} b8804InBlock, *LPb8804InBlock;
#define NAME_b8804InBlock     "b8804InBlock"

// 출력
typedef struct _b8804OutBlock
{
    char    reccnt              [   5];    // [string,    5] 레코드갯수                     StartPos 0, Length 5
    char    accno               [  20];    // [string,   20] 계좌번호                       StartPos 5, Length 20
    char    passwd              [   8];    // [string,    8] 입력비밀번호                   StartPos 25, Length 8
    char    orddt               [   8];    // [string,    8] 주문일                         StartPos 33, Length 8
    char    jangestgb           [   1];    // [string,    1] 잔고평가구분                   StartPos 41, Length 1
    char    futpriceestgb       [   1];    // [string,    1] 선물가격평가구분               StartPos 42, Length 1
    char    chsuqrygb           [   1];    // [string,    1] 청산수량조회구분               StartPos 43, Length 1
} b8804OutBlock, *LPb8804OutBlock;
#define NAME_b8804OutBlock     "b8804OutBlock"

typedef struct _b8804OutBlock1
{
    char    reccnt              [   5];    // [string,    5] 레코드갯수                     StartPos 0, Length 5
    char    accnm               [  40];    // [string,   40] 계좌명                         StartPos 5, Length 40
    char    estyetotamt         [  15];    // [string,   15] 평가예탁금총액                 StartPos 45, Length 15
    char    cashyetotamt        [  15];    // [string,   15] 현금평가예탁금액               StartPos 60, Length 15
    char    ytaktot             [  16];    // [string,   16] 예탁금총액                     StartPos 75, Length 16
    char    ytcash              [  16];    // [string,   16] 예탁현금                       StartPos 91, Length 16
    char    ytakdy              [  16];    // [string,   16] 예탁대용                       StartPos 107, Length 16
    char    fordyamt            [  16];    // [string,   16] 외화대용금액                   StartPos 123, Length 16
    char    intot               [  15];    // [string,   15] 인출가능총금액                 StartPos 139, Length 15
    char    incash              [  16];    // [string,   16] 인출가능현금액                 StartPos 154, Length 16
    char    indy                [  16];    // [string,   16] 인출가능대용금액               StartPos 170, Length 16
    char    jugatot             [  15];    // [string,   15] 주문가능총금액                 StartPos 186, Length 15
    char    cashjuga            [  16];    // [string,   16] 현금주문가능금액               StartPos 201, Length 16
    char    wtakjgtot           [  16];    // [string,   16] 위탁증거금총액                 StartPos 217, Length 16
    char    cashwtakjg          [  16];    // [string,   16] 현금위탁증거금액               StartPos 233, Length 16
    char    yujijgtot           [  15];    // [string,   15] 유지증거금총액                 StartPos 249, Length 15
    char    cashyujijg          [  16];    // [string,   16] 현금유지증거금액               StartPos 264, Length 16
    char    adddeposittot       [  15];    // [string,   15] 추가증거금총액                 StartPos 280, Length 15
    char    cashadddeposit      [  16];    // [string,   16] 현금추가증거금액               StartPos 295, Length 16
    char    fee                 [  16];    // [string,   16] 수수료                         StartPos 311, Length 16
    char    misamt              [  16];    // [string,   16] 미수금액                       StartPos 327, Length 16
    char    yfee                [  16];    // [string,   16] 미수연체료                     StartPos 343, Length 16
    char    futsonamt           [  16];    // [string,   16] 선물평가손익금액               StartPos 359, Length 16
    char    optsonamt           [  16];    // [string,   16] 옵션평가손익금액               StartPos 375, Length 16
    char    optpyungamt         [  16];    // [string,   16] 옵션평가금액                   StartPos 391, Length 16
    char    optmesonamt         [  16];    // [string,   16] 옵션매매손익금액               StartPos 407, Length 16
    char    futsetldifam        [  16];    // [string,   16] 선물정산차금                   StartPos 423, Length 16
    char    totsonikamt         [  16];    // [string,   16] 총손익금액                     StartPos 439, Length 16
    char    sunsonikamt         [  16];    // [string,   16] 순손익금액                     StartPos 455, Length 16
} b8804OutBlock1, *LPb8804OutBlock1;
#define NAME_b8804OutBlock1     "b8804OutBlock1"

typedef struct _b8804OutBlock2
{
    char    expcode             [  32];    // [string,   32] 선물옵션종목번호               StartPos 0, Length 32
    char    expname             [  40];    // [string,   40] 종목명                         StartPos 32, Length 40
    char    mmgb                [   1];    // [string,    1] 매매구분                       StartPos 72, Length 1
    char    mmgbnm              [  10];    // [string,   10] 매매구분명                     StartPos 73, Length 10
    char    misetqty            [  16];    // [string,   16] 미결제수량                     StartPos 83, Length 16
    char    avgamt              [  19];    // [string,   19] 평균가                         StartPos 99, Length 19
    char    price               [  13];    // [string,   13] 현재가                         StartPos 118, Length 13
    char    daebi               [  13];    // [string,   13] 대비가                         StartPos 131, Length 13
    char    sonik               [  16];    // [string,   16] 평가손익                       StartPos 144, Length 16
    char    sonikyul            [  12];    // [string,   12] 손익률                         StartPos 160, Length 12
    char    pyungamt            [  16];    // [string,   16] 평가금액                       StartPos 172, Length 16
    char    estrate             [   7];    // [string,    7] 평가비율                       StartPos 188, Length 7
    char    mdordnamtqty        [  16];    // [string,   16] 청산가능수량                   StartPos 195, Length 16
} b8804OutBlock2, *LPb8804OutBlock2;
#define NAME_b8804OutBlock2     "b8804OutBlock2"


#pragma pack( pop )
///////////////////////////////////////////////////////////////////////////////////////////////////
