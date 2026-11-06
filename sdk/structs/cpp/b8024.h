#pragma once

///////////////////////////////////////////////////////////////////////////////////////////////////
// 종목별 신용한도 조회 ( headtype=B ) 
#pragma pack( push, 1 )

#define NAME_b8024     "b8024"

// 입력
typedef struct _b8024InBlock
{
    char    reccnt              [   5];    // [string,    5] 레코드갯수                     StartPos 0, Length 5
    char    lmtclasscd          [   2];    // [string,    2] 한도분류코드. 대주(A5), 융자(A4)  StartPos 5, Length 2
    char    expcode             [  12];    // [string,   12] 종목번호                       StartPos 7, Length 12
    char    AcntNo              [  20];    // [string,   20] 계좌번호                       StartPos 19, Length 20
    char    jolmtgb             [   1];    // [string,    1] 종목한도구분. 고정(1)          StartPos 39, Length 1
    char    lmtvaluegb          [   1];    // [string,    1] 한도값구분. 대주(2), 융자(1)   StartPos 40, Length 1
} b8024InBlock, *LPb8024InBlock;
#define NAME_b8024InBlock     "b8024InBlock"

// 출력
typedef struct _b8024OutBlock
{
    char    reccnt              [   5];    // [string,    5] 레코드갯수                     StartPos 0, Length 5
    char    lmtclasscd          [   2];    // [string,    2] 한도분류코드                   StartPos 5, Length 2
    char    expcode             [  12];    // [string,   12] 종목번호                       StartPos 7, Length 12
    char    AcntNo              [  20];    // [string,   20] 계좌번호                       StartPos 19, Length 20
    char    jolmtgb             [   1];    // [string,    1] 종목한도구분                   StartPos 39, Length 1
    char    lmtvaluegb          [   1];    // [string,    1] 한도값구분                     StartPos 40, Length 1
} b8024OutBlock, *LPb8024OutBlock;
#define NAME_b8024OutBlock     "b8024OutBlock"

typedef struct _b8024OutBlock1
{
    char    lmtclassnm          [  40];    // [string,   40] 한도분류명                     StartPos 0, Length 40
    char    hname               [  40];    // [string,   40] 종목명                         StartPos 40, Length 40
    char    lmtgbnm             [  40];    // [string,   40] 종목한도구분명                 StartPos 80, Length 40
    char    lmtvalgbnm          [  40];    // [string,   40] 한도값구분명                   StartPos 120, Length 40
    char    lmtval              [  18];    // [string,   18] 한도값                         StartPos 160, Length 18
    char    loanjanval          [  18];    // [string,   18] 대출잔고값                     StartPos 178, Length 18
    char    newordval           [  18];    // [string,   18] 신규주문값                     StartPos 196, Length 18
    char    newcheval           [  18];    // [string,   18] 신규체결값                     StartPos 214, Length 18
    char    sincheval           [  18];    // [string,   18] 상환체결값                     StartPos 232, Length 18
    char    loandamt            [  16];    // [string,   16] 대출위험금액                   StartPos 250, Length 16
    char    useablelmtamt       [  16];    // [string,   16] 사용가능한도금액               StartPos 266, Length 16
    char    lmtclasscode        [   2];    // [string,    2] 한도분류코드                   StartPos 282, Length 2
    char    expcode             [  12];    // [string,   12] 종목번호                       StartPos 284, Length 12
    char    jolmtclass          [   1];    // [string,    1] 종목한도구분                   StartPos 296, Length 1
    char    lmtvalgb            [   1];    // [string,    1] 한도값구분                     StartPos 297, Length 1
    char    jogrplmtclass       [   1];    // [string,    1] 종목그룹한도구분               StartPos 298, Length 1
    char    gbnm1               [  40];    // [string,   40] 구분명1                        StartPos 299, Length 40
    char    jogrp               [   2];    // [string,    2] 종목그룹                       StartPos 339, Length 2
    char    jogrpnm             [  40];    // [string,   40] 종목그룹명                     StartPos 341, Length 40
    char    zLoanInfoGrdCode    [   2];    // [string,    2] 대출정보등급코드               StartPos 381, Length 2
    char    lLstdQty            [  16];    // [string,   16] 상장수량                       StartPos 383, Length 16
} b8024OutBlock1, *LPb8024OutBlock1;
#define NAME_b8024OutBlock1     "b8024OutBlock1"


#pragma pack( pop )
///////////////////////////////////////////////////////////////////////////////////////////////////
