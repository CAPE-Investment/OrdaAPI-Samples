#pragma once

///////////////////////////////////////////////////////////////////////////////////////////////////
// 계좌별 신용한도 조회 ( headtype=B ) 
#pragma pack( push, 1 )

#define NAME_b8022     "b8022"

// 입력
typedef struct _b8022InBlock
{
    char    reccnt              [   5];    // [string,    5] 레코드갯수                     StartPos 0, Length 5
    char    accno               [  20];    // [string,   20] 계좌번호                       StartPos 5, Length 20
    char    passwd              [   8];    // [string,    8] 입력비밀번호                   StartPos 25, Length 8
    char    loancd              [   2];    // [string,    2] 대출상세분류코드               StartPos 33, Length 2
    char    expcode             [  12];    // [string,   12] 종목번호                       StartPos 35, Length 12
    char    ordprice            [  13];    // [string,   13] 주문가                         StartPos 47, Length 13
} b8022InBlock, *LPb8022InBlock;
#define NAME_b8022InBlock     "b8022InBlock"

// 출력
typedef struct _b8022OutBlock
{
    char    reccnt              [   5];    // [string,    5] 레코드갯수                     StartPos 0, Length 5
    char    accno               [  20];    // [string,   20] 계좌번호                       StartPos 5, Length 20
    char    passwd              [   8];    // [string,    8] 입력비밀번호                   StartPos 25, Length 8
    char    loancd              [   2];    // [string,    2] 대출상세분류코드               StartPos 33, Length 2
    char    expcode             [  12];    // [string,   12] 종목번호                       StartPos 35, Length 12
    char    ordprice            [  13];    // [string,   13] 주문가                         StartPos 47, Length 13
} b8022OutBlock, *LPb8022OutBlock;
#define NAME_b8022OutBlock     "b8022OutBlock"

typedef struct _b8022OutBlock1
{
    char    reccnt              [   5];    // [string,    5] 레코드갯수                     StartPos 0, Length 5
    char    accname             [  40];    // [string,   40] 계좌명                         StartPos 5, Length 40
    char    ordprice            [  13];    // [string,   13] 주문가                         StartPos 45, Length 13
    char    lslmt               [  16];    // [string,   16] 대주한도                       StartPos 58, Length 16
    char    lsamtsum            [  16];    // [string,   16] 대주금액합계                   StartPos 74, Length 16
    char    lsnewamt            [  16];    // [string,   16] 대주신규금액                   StartPos 90, Length 16
    char    lssangamt           [  16];    // [string,   16] 대주상환금액                   StartPos 106, Length 16
    char    yutyungamt          [  16];    // [string,   16] 유통융자한도금액               StartPos 122, Length 16
    char    yutyungsum          [  16];    // [string,   16] 유통융자금액합계               StartPos 138, Length 16
    char    yutyungnewamt       [  16];    // [string,   16] 유통융자신규금액               StartPos 154, Length 16
    char    yutyungsangamt      [  16];    // [string,   16] 유통융자상환금액               StartPos 170, Length 16
    char    selfyungamt         [  16];    // [string,   16] 자기융자한도금액               StartPos 186, Length 16
    char    selfyungsum         [  16];    // [string,   16] 자기융자금액합계               StartPos 202, Length 16
    char    selfyungnewamt      [  16];    // [string,   16] 자기융자신규금액               StartPos 218, Length 16
    char    selfyungsangamt     [  16];    // [string,   16] 자기융자상환금액               StartPos 234, Length 16
    char    jiyutyungamt        [  16];    // [string,   16] 지점유통융자한도금액           StartPos 250, Length 16
    char    jiyutyungnewamt     [  16];    // [string,   16] 지점유통융자신규금액           StartPos 266, Length 16
    char    jiyutyungsangamt    [  16];    // [string,   16] 지점유통융자상환금액           StartPos 282, Length 16
    char    jiyutyunguseamt     [  16];    // [string,   16] 지점유통융자사용금액           StartPos 298, Length 16
    char    jiselfyungamt       [  16];    // [string,   16] 지점자기융자한도금액           StartPos 314, Length 16
    char    jiselfyungnewamt    [  16];    // [string,   16] 지점자기융자신규금액           StartPos 330, Length 16
    char    jiselftyungsangamt  [  16];    // [string,   16] 지점자기융자상환금액           StartPos 346, Length 16
    char    jiselftyunguseamt   [  16];    // [string,   16] 지점자기융자사용금액           StartPos 362, Length 16
    char    useyungmangyn       [   1];    // [string,    1] 이용사융자한도관리여부         StartPos 378, Length 1
    char    useshinitemresigb   [   1];    // [string,    1] 이용사신용종목제한구분         StartPos 379, Length 1
    char    dammntrate          [   7];    // [string,    7] 담보유지비율                   StartPos 380, Length 7
    char    usenm               [  50];    // [string,   50] 이용사명                       StartPos 387, Length 50
    char    damrate             [   7];    // [string,    7] 담보비율                       StartPos 437, Length 7
    char    yetakamtsum         [  17];    // [string,   17] 예탁자산합계                   StartPos 444, Length 17
    char    lmtchgposamt        [  16];    // [string,   16] 한도변경가능금액               StartPos 461, Length 16
    char    ordamt              [  16];    // [string,   16] 주문가능금액                   StartPos 477, Length 16
    char    ordqty              [  16];    // [string,   16] 주문가능수량                   StartPos 493, Length 16
    char    misdsblordqty       [  16];    // [string,   16] 미수불가주문가능수량           StartPos 509, Length 16
} b8022OutBlock1, *LPb8022OutBlock1;
#define NAME_b8022OutBlock1     "b8022OutBlock1"


#pragma pack( pop )
///////////////////////////////////////////////////////////////////////////////////////////////////
