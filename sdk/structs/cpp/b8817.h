#pragma once

///////////////////////////////////////////////////////////////////////////////////////////////////
// 체결/미체결(원장) ( headtype=B ) 
#pragma pack( push, 1 )

#define NAME_b8817     "b8817"

// 입력
typedef struct _b8817InBlock
{
    char    reccnt              [   5];    // [string,    5] 레코드갯수                     StartPos 0, Length 5
    char    accno               [  20];    // [string,   20] 계좌번호                       StartPos 5, Length 20
    char    passwd              [   8];    // [string,    8] 입력비밀번호                   StartPos 25, Length 8
    char    startdate           [   8];    // [string,    8] 조회시작일                     StartPos 33, Length 8
    char    finishdate          [   8];    // [string,    8] 조회종료일                     StartPos 41, Length 8
    char    fokindcd            [   2];    // [string,    2] 선물옵션분류코드               StartPos 49, Length 2
    char    spcode              [   2];    // [string,    2] 상품군코드                     StartPos 51, Length 2
    char    chegb               [   1];    // [string,    1] 체결구분                       StartPos 53, Length 1
    char    sortgb              [   1];    // [string,    1] 정렬순서구분                   StartPos 54, Length 1
    char    mechecd             [   2];    // [string,    2] 통신매체코드                   StartPos 55, Length 2
} b8817InBlock, *LPb8817InBlock;
#define NAME_b8817InBlock     "b8817InBlock"

// 출력
typedef struct _b8817OutBlock
{
    char    reccnt              [   5];    // [string,    5] 레코드갯수                     StartPos 0, Length 5
    char    accno               [  20];    // [string,   20] 계좌번호                       StartPos 5, Length 20
    char    passwd              [   8];    // [string,    8] 입력비밀번호                   StartPos 25, Length 8
    char    startdate           [   8];    // [string,    8] 조회시작일                     StartPos 33, Length 8
    char    finishdate          [   8];    // [string,    8] 조회종료일                     StartPos 41, Length 8
    char    fokindcd            [   2];    // [string,    2] 선물옵션분류코드               StartPos 49, Length 2
    char    spcode              [   2];    // [string,    2] 상품군코드                     StartPos 51, Length 2
    char    chegb               [   1];    // [string,    1] 체결구분                       StartPos 53, Length 1
    char    sortgb              [   1];    // [string,    1] 정렬순서구분                   StartPos 54, Length 1
    char    mechecd             [   2];    // [string,    2] 통신매체코드                   StartPos 55, Length 2
} b8817OutBlock, *LPb8817OutBlock;
#define NAME_b8817OutBlock     "b8817OutBlock"

typedef struct _b8817OutBlock1
{
    char    reccnt              [   5];    // [string,    5] 레코드갯수                     StartPos 0, Length 5
    char    accname             [  40];    // [string,   40] 계좌명                         StartPos 5, Length 40
    char    futordqty           [  16];    // [string,   16] 선물주문수량                   StartPos 45, Length 16
    char    futcheqty           [  16];    // [string,   16] 선물체결수량                   StartPos 61, Length 16
    char    optordqty           [  16];    // [string,   16] 옵션주문수량                   StartPos 77, Length 16
    char    optcheqty           [  16];    // [string,   16] 옵션체결수량                   StartPos 93, Length 16
} b8817OutBlock1, *LPb8817OutBlock1;
#define NAME_b8817OutBlock1     "b8817OutBlock1"

typedef struct _b8817OutBlock2
{
    char    orddate             [   8];    // [string,    8] 주문일                         StartPos 0, Length 8
    char    dummyordno          [   3];    // [string,    3] dummy주문번호                  StartPos 8, Length 3
    char    ordno               [   7];    // [string,    7] 주문번호                       StartPos 11, Length 7
    char    dummyorgordno       [   3];    // [string,    3] dummy원주문번호                StartPos 18, Length 3
    char    orgordno            [   7];    // [string,    7] 원주문번호                     StartPos 21, Length 7
    char    ordtime             [   9];    // [string,    9] 주문시각                       StartPos 28, Length 9
    char    futitemno           [  32];    // [string,   32] 선물옵션종목번호               StartPos 37, Length 32
    char    itemname            [  40];    // [string,   40] 종목명                         StartPos 69, Length 40
    char    memegb              [  10];    // [string,   10] 매매구분                       StartPos 109, Length 10
    char    mcgbname            [  10];    // [string,   10] 정정취소구분명                 StartPos 119, Length 10
    char    fohogaycd           [   2];    // [string,    2] 선물옵션호가유형코드           StartPos 129, Length 2
    char    fohogayname         [  40];    // [string,   40] 선물옵션호가유형명             StartPos 131, Length 40
    char    ordprice            [  13];    // [string,   13] 주문가                         StartPos 171, Length 13
    char    ordqty              [  16];    // [string,   16] 주문수량                       StartPos 184, Length 16
    char    ordgbname           [  10];    // [string,   10] 주문구분명                     StartPos 200, Length 10
    char    chegbname           [  10];    // [string,   10] 체결구분명                     StartPos 210, Length 10
    char    cheprc              [  13];    // [string,   13] 체결가                         StartPos 220, Length 13
    char    cheqty              [  16];    // [string,   16] 체결수량                       StartPos 233, Length 16
    char    yaktime             [   9];    // [string,    9] 약정시각                       StartPos 249, Length 9
    char    yakno               [  10];    // [string,   10] 약정번호                       StartPos 258, Length 10
    char    cheno               [  10];    // [string,   10] 체결번호                       StartPos 268, Length 10
    char    memesonamt          [  16];    // [string,   16] 매매손익금액                   StartPos 278, Length 16
    char    micheqty            [  16];    // [string,   16] 미체결수량                     StartPos 294, Length 16
    char    userid              [  16];    // [string,   16] 사용자ID                       StartPos 310, Length 16
    char    meche               [   2];    // [string,    2] 통신매체코드                   StartPos 326, Length 2
    char    mechename           [  40];    // [string,   40] 통신매체코드명                 StartPos 328, Length 40
} b8817OutBlock2, *LPb8817OutBlock2;
#define NAME_b8817OutBlock2     "b8817OutBlock2"


#pragma pack( pop )
///////////////////////////////////////////////////////////////////////////////////////////////////
