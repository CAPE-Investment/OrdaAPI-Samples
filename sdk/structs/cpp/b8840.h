#pragma once

///////////////////////////////////////////////////////////////////////////////////////////////////
// 주문가능수량/증거금 ( headtype=B ) 
#pragma pack( push, 1 )

#define NAME_b8840     "b8840"

// 입력
typedef struct _b8840InBlock
{
    char    reccnt              [   5];    // [string,    5] 레코드갯수                     StartPos 0, Length 5
    char    accno               [  20];    // [string,   20] 계좌번호                       StartPos 5, Length 20
    char    passwd              [   8];    // [string,    8] 입력비밀번호                   StartPos 25, Length 8
    char    gubun               [   1];    // [string,    1] 금액수량구분                   StartPos 33, Length 1
    char    fohogagb            [   2];    // [string,    2] 선물옵션호가유형코드           StartPos 34, Length 2
    char    ordamt              [  16];    // [string,   16] 주문금액                       StartPos 36, Length 16
} b8840InBlock, *LPb8840InBlock;
#define NAME_b8840InBlock     "b8840InBlock"

// 출력
typedef struct _b8840InBlock1
{
    char    reccnt2             [   5];    // [string,    5] 레코드갯수2                    StartPos 0, Length 5
} b8840InBlock1, *LPb8840InBlock1;
#define NAME_b8840InBlock1     "b8840InBlock1"

typedef struct _b8840InBlock2
{
    char    focodeno            [  32];    // [string,   32] 선물옵션종목번호               StartPos 0, Length 32
    char    memegb              [   1];    // [string,    1] 매매구분                       StartPos 32, Length 1
    char    price               [  13];    // [string,   13] 주문가                         StartPos 33, Length 13
    char    ordqty              [  16];    // [string,   16] 주문수량                       StartPos 46, Length 16
} b8840InBlock2, *LPb8840InBlock2;
#define NAME_b8840InBlock2     "b8840InBlock2"

typedef struct _b8840OutBlock
{
    char    reccnt              [   5];    // [string,    5] 레코드갯수                     StartPos 0, Length 5
    char    accno               [  20];    // [string,   20] 계좌번호                       StartPos 5, Length 20
    char    passwd              [   8];    // [string,    8] 입력비밀번호                   StartPos 25, Length 8
    char    gubun               [   1];    // [string,    1] 금액수량구분                   StartPos 33, Length 1
    char    fohogagb            [   2];    // [string,    2] 선물옵션호가유형코드           StartPos 34, Length 2
    char    ordamt              [  16];    // [string,   16] 주문금액                       StartPos 36, Length 16
} b8840OutBlock, *LPb8840OutBlock;
#define NAME_b8840OutBlock     "b8840OutBlock"

typedef struct _b8840OutBlock1
{
    char    focodeno            [  32];    // [string,   32] 선물옵션종목번호               StartPos 0, Length 32
    char    memegb              [   1];    // [string,    1] 매매구분                       StartPos 32, Length 1
    char    price               [  13];    // [string,   13] 주문가                         StartPos 33, Length 13
    char    ordqty              [  16];    // [string,   16] 주문수량                       StartPos 46, Length 16
} b8840OutBlock1, *LPb8840OutBlock1;
#define NAME_b8840OutBlock1     "b8840OutBlock1"

typedef struct _b8840OutBlock2
{
    char    reccnt              [   5];    // [string,    5] 레코드갯수                     StartPos 0, Length 5
    char    accname             [  40];    // [string,   40] 계좌명                         StartPos 5, Length 40
    char    date                [   8];    // [string,    8] 주문일                         StartPos 45, Length 8
    char    ordnamtqty          [  16];    // [string,   16] 주문가능금액                   StartPos 53, Length 16
    char    ordcashamt          [  16];    // [string,   16] 현금주문가능금액               StartPos 69, Length 16
    char    orddyamt            [  16];    // [string,   16] 대용주문가능금액               StartPos 85, Length 16
    char    syfpriceamt         [  16];    // [string,   16] 소요선물증거금액               StartPos 101, Length 16
    char    syfcashamt          [  16];    // [string,   16] 소요선물현금증거금액           StartPos 117, Length 16
    char    syfdyamt            [  16];    // [string,   16] 소요선물대용증거금액           StartPos 133, Length 16
    char    syoptpriceamt       [  16];    // [string,   16] 소요옵션증거금액               StartPos 149, Length 16
    char    syoptchashamt       [  16];    // [string,   16] 소요옵션현금증거금액           StartPos 165, Length 16
    char    syoptdyamt          [  16];    // [string,   16] 소요옵션대용증거금액           StartPos 181, Length 16
    char    syspdpriceamt       [  16];    // [string,   16] 소요스프레드증거금액           StartPos 197, Length 16
    char    syspdcashamt        [  16];    // [string,   16] 소요스프레드현금증거금액       StartPos 213, Length 16
    char    syspddyamt          [  16];    // [string,   16] 소요스프레드대용증거금액       StartPos 229, Length 16
    char    sypriceamt          [  16];    // [string,   16] 소요증거금액                   StartPos 245, Length 16
    char    sychashamt          [  16];    // [string,   16] 소요현금증거금액               StartPos 261, Length 16
    char    sydyamt             [  16];    // [string,   16] 소요대용증거금액               StartPos 277, Length 16
    char    namamt              [  16];    // [string,   16] 증거금잔액                     StartPos 293, Length 16
    char    cashnamamt          [  16];    // [string,   16] 현금증거금잔액                 StartPos 309, Length 16
    char    dynamamt            [  16];    // [string,   16] 대용증거금잔액                 StartPos 325, Length 16
} b8840OutBlock2, *LPb8840OutBlock2;
#define NAME_b8840OutBlock2     "b8840OutBlock2"


#pragma pack( pop )
///////////////////////////////////////////////////////////////////////////////////////////////////
