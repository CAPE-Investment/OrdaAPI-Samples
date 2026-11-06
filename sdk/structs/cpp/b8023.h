#pragma once

///////////////////////////////////////////////////////////////////////////////////////////////////
// 신용주문가능금액 상세조회 ( headtype=B ) 
#pragma pack( push, 1 )

#define NAME_b8023     "b8023"

// 입력
typedef struct _b8023InBlock
{
    char    reccnt              [   5];    // [string,    5] 레코드갯수                     StartPos 0, Length 5
    char    accno               [  20];    // [string,   20] 계좌번호                       StartPos 5, Length 20
    char    passwd              [   8];    // [string,    8] 입력비밀번호                   StartPos 25, Length 8
    char    loancd              [   2];    // [string,    2] 대출유형코드                   StartPos 33, Length 2
    char    expcode             [  12];    // [string,   12] 종목번호                       StartPos 35, Length 12
    char    price               [  13];    // [string,   13] 주문가                         StartPos 47, Length 13
} b8023InBlock, *LPb8023InBlock;
#define NAME_b8023InBlock     "b8023InBlock"

// 출력
typedef struct _b8023OutBlock
{
    char    reccnt              [   5];    // [string,    5] 레코드갯수                     StartPos 0, Length 5
    char    accno               [  20];    // [string,   20] 계좌번호                       StartPos 5, Length 20
    char    passwd              [   8];    // [string,    8] 입력비밀번호                   StartPos 25, Length 8
    char    loancd              [   2];    // [string,    2] 대출유형코드                   StartPos 33, Length 2
    char    expcode             [  12];    // [string,   12] 종목번호                       StartPos 35, Length 12
    char    price               [  13];    // [string,   13] 주문가                         StartPos 47, Length 13
} b8023OutBlock, *LPb8023OutBlock;
#define NAME_b8023OutBlock     "b8023OutBlock"

typedef struct _b8023OutBlock1
{
    char    reccnt              [   5];    // [string,    5] 레코드갯수                     StartPos 0, Length 5
    char    lslmt               [  16];    // [string,   16] 대주한도                       StartPos 5, Length 16
    char    lsamt               [  16];    // [string,   16] 대주금액                       StartPos 21, Length 16
    char    lsnewamt            [  16];    // [string,   16] 대주신규금액                   StartPos 37, Length 16
    char    lssangamt           [  16];    // [string,   16] 대주상환금액                   StartPos 53, Length 16
    char    selfyungamt         [  16];    // [string,   16] 자기융자한도금액               StartPos 69, Length 16
    char    selfyungsum         [  16];    // [string,   16] 자기융자금액                   StartPos 85, Length 16
    char    selfyungnewamt      [  16];    // [string,   16] 자기융자신규금액               StartPos 101, Length 16
    char    selfyungsangamt     [  16];    // [string,   16] 자기융자상환금액               StartPos 117, Length 16
    char    yutyungamt          [  16];    // [string,   16] 유통융자한도금액               StartPos 133, Length 16
    char    yutyungsum          [  16];    // [string,   16] 유통융자금액                   StartPos 149, Length 16
    char    yutyungnewamt       [  16];    // [string,   16] 유통융자신규금액               StartPos 165, Length 16
    char    yutyungsangamt      [  16];    // [string,   16] 유통융자상환금액               StartPos 181, Length 16
    char    jiyutyungamt        [  16];    // [string,   16] 지점유통융자한도금액           StartPos 197, Length 16
    char    jiyutyungnewamt     [  16];    // [string,   16] 지점유통융자신규금액           StartPos 213, Length 16
    char    jiyutyungsangamt    [  16];    // [string,   16] 지점유통융자상환금액           StartPos 229, Length 16
    char    jiyutyunguseamt     [  16];    // [string,   16] 지점유통융자사용금액           StartPos 245, Length 16
    char    jiselfyungamt       [  16];    // [string,   16] 지점자기융자한도금액           StartPos 261, Length 16
    char    jiselfyungnewamt    [  16];    // [string,   16] 지점자기융자신규금액           StartPos 277, Length 16
    char    jiselftyungsangamt  [  16];    // [string,   16] 지점자기융자상환금액           StartPos 293, Length 16
    char    jiselftyunguseamt   [  16];    // [string,   16] 지점자기융자사용금액           StartPos 309, Length 16
    char    useyungmangyn       [   1];    // [string,    1] 이용사융자한도관리여부         StartPos 325, Length 1
    char    useshinitemresigb   [   1];    // [string,    1] 이용사신용종목제한구분         StartPos 326, Length 1
    char    sindamrate          [   9];    // [string,    9] 신용담보율                     StartPos 327, Length 9
    char    useconm             [  50];    // [string,   50] 이용사명                       StartPos 336, Length 50
    char    totdamrate          [   9];    // [string,    9] 총담보율                       StartPos 386, Length 9
    char    aplmrtgrate         [   7];    // [string,    7] 적용담보비율                   StartPos 395, Length 7
    char    totamt              [  17];    // [string,   17] 예탁자산합계                   StartPos 402, Length 17
    char    expdamrate          [   7];    // [string,    7] 종목담보비율                   StartPos 419, Length 7
    char    usedamrate          [   7];    // [string,    7] 이용사담보비율                 StartPos 426, Length 7
    char    lmtchgposamt        [  16];    // [string,   16] 한도변경가능금액               StartPos 433, Length 16
    char    lmtaplgb            [   1];    // [string,    1] 한도적용구분                   StartPos 449, Length 1
    char    depoamt             [  16];    // [string,   16] 예수금                         StartPos 450, Length 16
    char    d2depoamt           [  16];    // [string,   16] D2예수금                       StartPos 466, Length 16
    char    dyamt               [  16];    // [string,   16] 대용금액                       StartPos 482, Length 16
    char    d2dyamt             [  16];    // [string,   16] D2대용금                       StartPos 498, Length 16
    char    loancshamt          [  16];    // [string,   16] 담보현금금액                   StartPos 514, Length 16
    char    d2loancshamt        [  16];    // [string,   16] D2담보현금                     StartPos 530, Length 16
    char    loandyamt           [  16];    // [string,   16] 담보대용금액                   StartPos 546, Length 16
    char    d2loandyamt         [  16];    // [string,   16] D2담보대용                     StartPos 562, Length 16
    char    cashordamt          [  16];    // [string,   16] 현금주문가능금액               StartPos 578, Length 16
    char    d2cashordamt        [  16];    // [string,   16] D2주문가능현금                 StartPos 594, Length 16
    char    orddyamt            [  16];    // [string,   16] 대용주문가능금액               StartPos 610, Length 16
    char    d2orddyamt          [  16];    // [string,   16] D2주문가능대용                 StartPos 626, Length 16
    char    d2damrate           [   9];    // [string,    9] D2담보비율                     StartPos 642, Length 9
    char    gjdepoamt           [  16];    // [string,   16] D2가정산예수금                 StartPos 651, Length 16
    char    d2addloanamt        [  16];    // [string,   16] D2추가담보금                   StartPos 667, Length 16
    char    margincash          [  16];    // [string,   16] 증거금현금                     StartPos 683, Length 16
    char    margindy            [  16];    // [string,   16] 증거금대용                     StartPos 699, Length 16
    char    d2sintotamt         [  16];    // [string,   16] D2신용합계금액                 StartPos 715, Length 16
    char    d2needloanamt       [  16];    // [string,   16] D2소요담보금액                 StartPos 731, Length 16
    char    d2orgloanamt        [  16];    // [string,   16] D2원담보금액                   StartPos 747, Length 16
} b8023OutBlock1, *LPb8023OutBlock1;
#define NAME_b8023OutBlock1     "b8023OutBlock1"

typedef struct _b8023OutBlock2
{
    char    groupcd             [   2];    // [string,    2] 그룹코드                       StartPos 0, Length 2
    char    ordposamt           [  16];    // [string,   16] 주문가능금액                   StartPos 2, Length 16
    char    ordposqty           [  16];    // [string,   16] 주문가능수량                   StartPos 18, Length 16
    char    lmtaplgb            [   1];    // [string,    1] 한도적용구분                   StartPos 34, Length 1
} b8023OutBlock2, *LPb8023OutBlock2;
#define NAME_b8023OutBlock2     "b8023OutBlock2"


#pragma pack( pop )
///////////////////////////////////////////////////////////////////////////////////////////////////
