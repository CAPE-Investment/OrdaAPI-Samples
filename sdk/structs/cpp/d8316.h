#pragma once

///////////////////////////////////////////////////////////////////////////////////////////////////
// 주식잔고2 ( attr,headtype=D ) 
#pragma pack( push, 1 )

#define NAME_d8316     "d8316"

// 입력
typedef struct _d8316InBlock
{
    char    accno               [  11];    char    _accno               ;    // [string,   11] 계좌번호                       StartPos 0, Length 11
    char    passwd              [   8];    char    _passwd              ;    // [string,    8] 비밀번호                       StartPos 12, Length 8
    char    prcgb               [   1];    char    _prcgb               ;    // [string,    1] 단가구분                       StartPos 21, Length 1
    char    chegb               [   1];    char    _chegb               ;    // [string,    1] 체결구분                       StartPos 23, Length 1
    char    dangb               [   1];    char    _dangb               ;    // [string,    1] 단일가구분                     StartPos 25, Length 1
    char    charge              [   1];    char    _charge              ;    // [string,    1] 제비용포함여부                 StartPos 27, Length 1
    char    tongsingb           [   1];    char    _tongsingb           ;    // [string,    1] 통신구분                       StartPos 29, Length 1
    char    cts_expcode         [  22];    char    _cts_expcode         ;    // [string,   22] CTS_종목번호                   StartPos 31, Length 22
    char    exchange            [   1];    char    _exchange            ;    // [string,    1] 거래소구분                     StartPos 54, Length 1
} d8316InBlock, *LPd8316InBlock;
#define NAME_d8316InBlock     "d8316InBlock"

// 출력
typedef struct _d8316OutBlock
{
    char    dtsunik             [  13];    char    _dtsunik             ;    // [string,   13] 실현손익                       StartPos 0, Length 13
    char    mamt                [  13];    char    _mamt                ;    // [string,   13] 매입금액                       StartPos 14, Length 13
    char    sunamt1             [  13];    char    _sunamt1             ;    // [string,   13] 추정D2예수금                   StartPos 28, Length 13
    char    cts_expcode         [  22];    char    _cts_expcode         ;    // [string,   22] CTS_종목번호                   StartPos 42, Length 22
} d8316OutBlock, *LPd8316OutBlock;
#define NAME_d8316OutBlock     "d8316OutBlock"

typedef struct _d8316OutBlock1
{
    char    expcode             [  12];    char    _expcode             ;    // [string,   12] 종목번호                       StartPos 0, Length 12
    char    jangb               [   1];    char    _jangb               ;    // [string,    1] 잔고구분                       StartPos 13, Length 1
    char    janqty              [   9];    char    _janqty              ;    // [string,    9] 잔고수량                       StartPos 15, Length 9
    char    mdposqt             [   9];    char    _mdposqt             ;    // [string,    9] 매도가능수량                   StartPos 25, Length 9
    char    pamt                [   9];    char    _pamt                ;    // [string,    9] 평균단가                       StartPos 35, Length 9
    char    mamt                [  13];    char    _mamt                ;    // [string,   13] 매입금액                       StartPos 45, Length 13
    char    sinamt              [  13];    char    _sinamt              ;    // [string,   13] 대출금액                       StartPos 59, Length 13
    char    lastdt              [   8];    char    _lastdt              ;    // [string,    8] 만기일자                       StartPos 73, Length 8
    char    sysprocseq          [  10];    char    _sysprocseq          ;    // [string,   10] 처리순번                       StartPos 82, Length 10
    char    loandt              [   8];    char    _loandt              ;    // [string,    8] 대출일자                       StartPos 93, Length 8
    char    hname               [  40];    char    _hname               ;    // [string,   40] 종목명                         StartPos 102, Length 40
    char    marketgb            [   1];    char    _marketgb            ;    // [string,    1] 시장구분                       StartPos 143, Length 1
    char    janrt               [  10];    char    _janrt               ;    // [string,   10] 보유비중                       StartPos 145, Length 10
    char    price               [   8];    char    _price               ;    // [string,    8] 현재가                         StartPos 156, Length 8
    char    sininter            [  10];    char    _sininter            ;    // [string,   10] 신용이자                       StartPos 165, Length 10
    char    sinrate             [   3];    char    _sinrate             ;    // [string,    3] 신용융자비율                   StartPos 176, Length 3
    char    sfee                [  10];    char    _sfee                ;    // [string,   10] 매수수수료                     StartPos 180, Length 10
    char    diff                [   6];    char    _diff                ;    // [string,    6] 등락율                         StartPos 191, Length 6
} d8316OutBlock1, *LPd8316OutBlock1;
#define NAME_d8316OutBlock1     "d8316OutBlock1"


#pragma pack( pop )
///////////////////////////////////////////////////////////////////////////////////////////////////
