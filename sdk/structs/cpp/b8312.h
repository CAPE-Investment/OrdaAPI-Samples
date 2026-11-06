#pragma once

///////////////////////////////////////////////////////////////////////////////////////////////////
// 현물계좌 주문체결 내역 조회 ( headtype=B ) 
#pragma pack( push, 1 )

#define NAME_b8312     "b8312"

// 입력
typedef struct _b8312InBlock
{
    char    reccnt              [   5];    // [string,    5] 레코드갯수                     StartPos 0, Length 5
    char    accno               [  20];    // [string,   20] 계좌번호                       StartPos 5, Length 20
    char    passwd              [   8];    // [string,    8] 입력비밀번호                   StartPos 25, Length 8
    char    ordmktcode          [   2];    // [string,    2] 주문시장코드                   StartPos 33, Length 2
    char    memegb              [   1];    // [string,    1] 매매구분                       StartPos 35, Length 1
    char    expcode             [  12];    // [string,   12] 종목번호                       StartPos 36, Length 12
    char    cheok               [   1];    // [string,    1] 체결여부                       StartPos 48, Length 1
    char    ordday              [   8];    // [string,    8] 주문일                         StartPos 49, Length 8
    char    startno             [  10];    // [string,   10] 시작주문번호                   StartPos 57, Length 10
    char    convertgb           [   1];    // [string,    1] 역순구분                       StartPos 67, Length 1
    char    ordtypegb           [   2];    // [string,    2] 주문유형코드                   StartPos 68, Length 2
} b8312InBlock, *LPb8312InBlock;
#define NAME_b8312InBlock     "b8312InBlock"

// 출력
typedef struct _b8312OutBlock
{
    char    reccnt              [   5];    // [string,    5] 레코드갯수                     StartPos 0, Length 5
    char    accno               [  20];    // [string,   20] 계좌번호                       StartPos 5, Length 20
    char    passwd              [   8];    // [string,    8] 입력비밀번호                   StartPos 25, Length 8
    char    ordmktcode          [   2];    // [string,    2] 주문시장코드                   StartPos 33, Length 2
    char    memegb              [   1];    // [string,    1] 매매구분                       StartPos 35, Length 1
    char    expcode             [  12];    // [string,   12] 종목번호                       StartPos 36, Length 12
    char    cheok               [   1];    // [string,    1] 체결여부                       StartPos 48, Length 1
    char    ordday              [   8];    // [string,    8] 주문일                         StartPos 49, Length 8
    char    startno             [  10];    // [string,   10] 시작주문번호                   StartPos 57, Length 10
    char    convertgb           [   1];    // [string,    1] 역순구분                       StartPos 67, Length 1
    char    ordtypegb           [   2];    // [string,    2] 주문유형코드                   StartPos 68, Length 2
} b8312OutBlock, *LPb8312OutBlock;
#define NAME_b8312OutBlock     "b8312OutBlock"

typedef struct _b8312OutBlock1
{
    char    reccnt              [   5];    // [string,    5] 레코드갯수                     StartPos 0, Length 5
    char    sellstlamt          [  16];    // [string,   16] 매도체결금액                   StartPos 5, Length 16
    char    buystlamt           [  16];    // [string,   16] 매수체결금액                   StartPos 21, Length 16
    char    sellstlqty          [  16];    // [string,   16] 매도체결수량                   StartPos 37, Length 16
    char    buystlqty           [  16];    // [string,   16] 매수체결수량                   StartPos 53, Length 16
    char    sellordqty          [  16];    // [string,   16] 매도주문수량                   StartPos 69, Length 16
    char    buyordqty           [  16];    // [string,   16] 매수주문수량                   StartPos 85, Length 16
} b8312OutBlock1, *LPb8312OutBlock1;
#define NAME_b8312OutBlock1     "b8312OutBlock1"

typedef struct _b8312OutBlock2
{
    char    ordday              [   8];    // [string,    8] 주문일                         StartPos 0, Length 8
    char    jijumno             [   3];    // [string,    3] 관리지점번호                   StartPos 8, Length 3
    char    marketgb            [   2];    // [string,    2] 주문시장코드                   StartPos 11, Length 2
    char    ordno               [  10];    // [string,   10] 주문번호                       StartPos 13, Length 10
    char    ongordno            [  10];    // [string,   10] 원주문번호                     StartPos 23, Length 10
    char    expcode             [  12];    // [string,   12] 종목번호                       StartPos 33, Length 12
    char    hname               [  40];    // [string,   40] 종목명                         StartPos 45, Length 40
    char    memegb1             [   1];    // [string,    1] 매매구분                       StartPos 85, Length 1
    char    memegb2             [  10];    // [string,   10] 매매구분                       StartPos 86, Length 10
    char    ordtypegb           [   2];    // [string,    2] 주문유형코드                   StartPos 96, Length 2
    char    ordtype             [  40];    // [string,   40] 주문유형명                     StartPos 98, Length 40
    char    ordprocode          [   9];    // [string,    9] 주문처리유형코드               StartPos 138, Length 9
    char    ordproname          [  50];    // [string,   50] 주문처리유형명                 StartPos 147, Length 50
    char    cfmgb               [   1];    // [string,    1] 정정취소구분                   StartPos 197, Length 1
    char    cfmgbname           [  10];    // [string,   10] 정정취소구분명                 StartPos 198, Length 10
    char    cfmqty              [  16];    // [string,   16] 정정취소수량                   StartPos 208, Length 16
    char    cfmokqty            [  16];    // [string,   16] 정정취소가능수량               StartPos 224, Length 16
    char    ordqty              [  16];    // [string,   16] 주문수량                       StartPos 240, Length 16
    char    ordamt              [  13];    // [string,   13] 주문가                         StartPos 256, Length 13
    char    cheqty              [  16];    // [string,   16] 체결수량                       StartPos 269, Length 16
    char    cheamt              [  13];    // [string,   13] 체결가                         StartPos 285, Length 13
    char    chetime             [   9];    // [string,    9] 체결처리시각                   StartPos 298, Length 9
    char    lastchetime         [   9];    // [string,    9] 체결처리시각                   StartPos 307, Length 9
    char    hogacode            [   2];    // [string,    2] 호가유형코드                   StartPos 316, Length 2
    char    hoganame            [  40];    // [string,   40] 호가유형명                     StartPos 318, Length 40
    char    ordcondigb          [   1];    // [string,    1] 주문조건구분                   StartPos 358, Length 1
    char    totcheqty           [  16];    // [string,   16] 전체체결수량                   StartPos 359, Length 16
    char    tongsincode         [   2];    // [string,    2] 통신매체코드                   StartPos 375, Length 2
    char    tongsinname         [  40];    // [string,   40] 통신매체명                     StartPos 377, Length 40
    char    memberno            [   3];    // [string,    3] 회원번호                       StartPos 417, Length 3
    char    reserordgb          [   1];    // [string,    1] 예약주문여부                   StartPos 420, Length 1
    char    loandt              [   8];    // [string,    8] 대출일                         StartPos 421, Length 8
    char    ordtime             [   9];    // [string,    9] 주문시각                       StartPos 429, Length 9
    char    operno              [  12];    // [string,   12] 운용지시번호                   StartPos 438, Length 12
    char    CndiPrc             [  13];    // [string,   13] 조건가격                       StartPos 450, Length 13
    char    CndiAvalTp          [   1];    // [string,    1] 조건호가유효구분               StartPos 463, Length 1
    char    CndiAvalNm          [  40];    // [string,   40] 조건호가유효구분명             StartPos 464, Length 40
    char    ExecMktCode         [   2];    // [string,    2] 체결시장코드                   StartPos 504, Length 2
} b8312OutBlock2, *LPb8312OutBlock2;
#define NAME_b8312OutBlock2     "b8312OutBlock2"


#pragma pack( pop )
///////////////////////////////////////////////////////////////////////////////////////////////////
