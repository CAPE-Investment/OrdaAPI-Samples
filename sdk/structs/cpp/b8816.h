#pragma once

///////////////////////////////////////////////////////////////////////////////////////////////////
// 미결제잔고(원장) ( headtype=B ) 
#pragma pack( push, 1 )

#define NAME_b8816     "b8816"

// 입력
typedef struct _b8816InBlock
{
    char    reccnt              [   5];    // [string,    5] 레코드갯수                     StartPos 0, Length 5
    char    accno               [  20];    // [string,   20] 계좌번호                       StartPos 5, Length 20
    char    passwd              [   8];    // [string,    8] 비밀번호                       StartPos 25, Length 8
    char    mkcode              [   2];    // [string,    2] 등록시장코드                   StartPos 33, Length 2
    char    msdate              [   8];    // [string,    8] 매수일자                       StartPos 35, Length 8
} b8816InBlock, *LPb8816InBlock;
#define NAME_b8816InBlock     "b8816InBlock"

// 출력
typedef struct _b8816OutBlock
{
    char    reccnt              [   5];    // [string,    5] 레코드갯수                     StartPos 0, Length 5
    char    accno               [  20];    // [string,   20] 계좌번호                       StartPos 5, Length 20
    char    passwd              [   8];    // [string,    8] 비밀번호                       StartPos 25, Length 8
    char    mkcode              [   2];    // [string,    2] 등록시장코드                   StartPos 33, Length 2
    char    msdate              [   8];    // [string,    8] 매수일자                       StartPos 35, Length 8
} b8816OutBlock, *LPb8816OutBlock;
#define NAME_b8816OutBlock     "b8816OutBlock"

typedef struct _b8816OutBlock1
{
    char    reccnt              [   5];    // [string,    5] 레코드갯수                     StartPos 0, Length 5
    char    accname             [  40];    // [string,   40] 계좌명                         StartPos 5, Length 40
    char    futyaksu            [  16];    // [string,   16] 선물약정수량                   StartPos 45, Length 16
    char    optyaksu            [  16];    // [string,   16] 옵션약정수량                   StartPos 61, Length 16
    char    yaksu               [  16];    // [string,   16] 약정수량                       StartPos 77, Length 16
    char    futyakamt           [  16];    // [string,   16] 선물약정금액                   StartPos 93, Length 16
    char    futyakmsamt         [  16];    // [string,   16] 선물매수약정금액               StartPos 109, Length 16
    char    futyakmdamt         [  16];    // [string,   16] 선물매도약정금액               StartPos 125, Length 16
    char    callyakamt          [  16];    // [string,   16] 콜옵션약정금액                 StartPos 141, Length 16
    char    callmsamt           [  16];    // [string,   16] 콜매수금액                     StartPos 157, Length 16
    char    callmdamt           [  16];    // [string,   16] 콜매도금액                     StartPos 173, Length 16
    char    putyakamt           [  16];    // [string,   16] 풋옵션약정금액                 StartPos 189, Length 16
    char    putmsamt            [  16];    // [string,   16] 풋매수금액                     StartPos 205, Length 16
    char    putmdamt            [  16];    // [string,   16] 풋매도금액                     StartPos 221, Length 16
    char    totyakamt           [  16];    // [string,   16] 전체약정금액                   StartPos 237, Length 16
    char    msyaknamt           [  16];    // [string,   16] 매수약정누계금액               StartPos 253, Length 16
    char    mdyaknamt           [  16];    // [string,   16] 매도약정누계금액               StartPos 269, Length 16
    char    futsontot           [  16];    // [string,   16] 선물손익합계                   StartPos 285, Length 16
    char    optsontot           [  16];    // [string,   16] 옵션손익합계                   StartPos 301, Length 16
    char    totsontot           [  16];    // [string,   16] 전체손익합계                   StartPos 317, Length 16
} b8816OutBlock1, *LPb8816OutBlock1;
#define NAME_b8816OutBlock1     "b8816OutBlock1"

typedef struct _b8816OutBlock2
{
    char    futoptgb            [   1];    // [string,    1] 선물옵션품목구분               StartPos 0, Length 1
    char    futmdsu             [  16];    // [string,   16] 선물매도수량                   StartPos 1, Length 16
    char    futmdson            [  16];    // [string,   16] 선물매도손익                   StartPos 17, Length 16
    char    futsmssu            [  16];    // [string,   16] 선물매수수량                   StartPos 33, Length 16
    char    futmsson            [  16];    // [string,   16] 선물매수손익                   StartPos 49, Length 16
    char    callmdsu            [  16];    // [string,   16] 콜매도수량                     StartPos 65, Length 16
    char    callmdson           [  16];    // [string,   16] 콜매도손익                     StartPos 81, Length 16
    char    callmssu            [  16];    // [string,   16] 콜매수수량                     StartPos 97, Length 16
    char    callmsson           [  16];    // [string,   16] 콜매수손익                     StartPos 113, Length 16
    char    putmdsu             [  16];    // [string,   16] 풋매도수량                     StartPos 129, Length 16
    char    putmdson            [  16];    // [string,   16] 풋매도손익                     StartPos 145, Length 16
    char    putmssu             [  16];    // [string,   16] 풋매수수량                     StartPos 161, Length 16
    char    putmsson            [  16];    // [string,   16] 풋매수손익                     StartPos 177, Length 16
} b8816OutBlock2, *LPb8816OutBlock2;
#define NAME_b8816OutBlock2     "b8816OutBlock2"

typedef struct _b8816OutBlock3
{
    char    shcode              [  12];    // [string,   12] 종목번호                       StartPos 0, Length 12
    char    hname               [  40];    // [string,   40] 종목명                         StartPos 12, Length 40
    char    mmgubun             [   1];    // [string,    1] 매매구분                       StartPos 52, Length 1
    char    mmgubun2            [  10];    // [string,   10] 매매구분                       StartPos 53, Length 10
    char    jansu               [  16];    // [string,   16] 잔고수량                       StartPos 63, Length 16
    char    avgamt              [  19];    // [string,   19] 평균가                         StartPos 79, Length 19
    char    firstamt            [  16];    // [string,   16] 당초금액                       StartPos 98, Length 16
    char    tdaychsu            [  16];    // [string,   16] 당일청산수량                   StartPos 114, Length 16
    char    price               [  13];    // [string,   13] 현재가                         StartPos 130, Length 13
    char    pyungamt            [  16];    // [string,   16] 평가금액                       StartPos 143, Length 16
    char    sonamt              [  16];    // [string,   16] 평가손익금액                   StartPos 159, Length 16
    char    buyrate             [  12];    // [string,   12] 평가수익률                     StartPos 175, Length 12
} b8816OutBlock3, *LPb8816OutBlock3;
#define NAME_b8816OutBlock3     "b8816OutBlock3"


#pragma pack( pop )
///////////////////////////////////////////////////////////////////////////////////////////////////
