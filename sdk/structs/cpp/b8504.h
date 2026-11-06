#pragma once

///////////////////////////////////////////////////////////////////////////////////////////////////
// 선물옵션 복수계좌 복수종목주문 ( headtype=B ) 
#pragma pack( push, 1 )

#define NAME_b8504     "b8504"

// 입력
typedef struct _b8504InBlock
{
    char    reccnt              [   5];    // [string,    5] 레코드갯수                     StartPos 0, Length 5
    char    tongsingb           [   2];    // [string,    2] 통신매체코드                   StartPos 5, Length 2
    char    reccnt1             [   5];    // [string,    5] 레코드갯수1                    StartPos 7, Length 5
} b8504InBlock, *LPb8504InBlock;
#define NAME_b8504InBlock     "b8504InBlock"

typedef struct _b8504InBlock1
{
    char    accno               [  20];    // [string,   20] 계좌번호                       StartPos 0, Length 20
    char    passwd              [  44];    // [string,   44] 입력비밀번호                   StartPos 20, Length 44
    char    expcode             [  32];    // [string,   32] 선물옵션종목번호               StartPos 64, Length 32
    char    mmgubun             [   1];    // [string,    1] 매매구분                       StartPos 96, Length 1
    char    hogagb              [   2];    // [string,    2] 선물옵션호가유형코드           StartPos 97, Length 2
    char    price               [  13];    // [string,   13] 주문가                         StartPos 99, Length 13
    char    qty                 [  16];    // [string,   16] 주문수량                       StartPos 112, Length 16
    char    opjisino            [  12];    // [string,   12] 운용지시번호                   StartPos 128, Length 12
} b8504InBlock1, *LPb8504InBlock1;
#define NAME_b8504InBlock1     "b8504InBlock1"

// 출력
typedef struct _b8504OutBlock
{
    char    reccnt              [   5];    // [string,    5] 레코드갯수                     StartPos 0, Length 5
    char    tongsingb           [   2];    // [string,    2] 통신매체코드                   StartPos 5, Length 2
} b8504OutBlock, *LPb8504OutBlock;
#define NAME_b8504OutBlock     "b8504OutBlock"

typedef struct _b8504OutBlock1
{
    char    accno               [  20];    // [string,   20] 계좌번호                       StartPos 0, Length 20
    char    passwd              [  44];    // [string,   44] 입력비밀번호                   StartPos 20, Length 44
    char    expcode             [  32];    // [string,   32] 선물옵션종목번호               StartPos 64, Length 32
    char    mmgubun             [   1];    // [string,    1] 매매구분                       StartPos 96, Length 1
    char    hogagb              [   2];    // [string,    2] 선물옵션호가유형코드           StartPos 97, Length 2
    char    price               [  13];    // [string,   13] 주문가                         StartPos 99, Length 13
    char    qty                 [  16];    // [string,   16] 주문수량                       StartPos 112, Length 16
    char    opjisino            [  12];    // [string,   12] 운용지시번호                   StartPos 128, Length 12
} b8504OutBlock1, *LPb8504OutBlock1;
#define NAME_b8504OutBlock1     "b8504OutBlock1"

typedef struct _b8504OutBlock2
{
    char    dummyordno          [   3];    // [string,    3] dummy주문번호                  StartPos 0, Length 3
    char    ordno               [   7];    // [string,    7] 주문번호                       StartPos 3, Length 7
    char    msgcode             [   4];    // [string,    4] 메시지코드                     StartPos 10, Length 4
    char    msg                 [ 100];    // [string,  100] 메시지내용                     StartPos 14, Length 100
    char    proctime            [   9];    // [string,    9] 처리시각                       StartPos 114, Length 9
} b8504OutBlock2, *LPb8504OutBlock2;
#define NAME_b8504OutBlock2     "b8504OutBlock2"


#pragma pack( pop )
///////////////////////////////////////////////////////////////////////////////////////////////////
