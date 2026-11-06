#pragma once

///////////////////////////////////////////////////////////////////////////////////////////////////
// 선물/옵션잔고평가 ( attr,headtype=D ) 
#pragma pack( push, 1 )

#define NAME_d8810     "d8810"

// 입력
typedef struct _d8810InBlock
{
    char    accno               [  11];    char    _accno               ;    // [string,   11] 계좌번호                       StartPos 0, Length 11
    char    passwd              [   8];    char    _passwd              ;    // [string,    8] 비밀번호                       StartPos 12, Length 8
    char    cts_expcode         [   8];    char    _cts_expcode         ;    // [string,    8] CTS_종목번호                   StartPos 21, Length 8
    char    cts_medocd          [   1];    char    _cts_medocd          ;    // [string,    1] CTS_매매구분                   StartPos 30, Length 1
    char    pamtgb              [   1];    char    _pamtgb              ;    // [string,    1] 잔고평가구분                   StartPos 32, Length 1
} d8810InBlock, *LPd8810InBlock;
#define NAME_d8810InBlock     "d8810InBlock"

// 출력
typedef struct _d8810OutBlock
{
    char    tdtsunik            [  18];    char    _tdtsunik            ;    // [string,   18] 매매손익합계                   StartPos 0, Length 18
    char    cts_expcode         [   8];    char    _cts_expcode         ;    // [string,    8] CTS_종목번호                   StartPos 19, Length 8
    char    cts_medocd          [   1];    char    _cts_medocd          ;    // [string,    1] CTS_매매구분                   StartPos 28, Length 1
    char    tappamt             [  18];    char    _tappamt             ;    // [string,   18] 평가금액                       StartPos 30, Length 18
    char    tsunik              [  18];    char    _tsunik              ;    // [string,   18] 평가손익                       StartPos 49, Length 18
} d8810OutBlock, *LPd8810OutBlock;
#define NAME_d8810OutBlock     "d8810OutBlock"

typedef struct _d8810OutBlock1
{
    char    expcode             [   8];    char    _expcode             ;    // [string,    8] 종목번호                       StartPos 0, Length 8
    char    medosu              [   4];    char    _medosu              ;    // [string,    4] 구분                           StartPos 9, Length 4
    char    jqty                [  10];    char    _jqty                ;    // [string,   10] 잔고수량                       StartPos 14, Length 10
    char    cqty                [  10];    char    _cqty                ;    // [string,   10] 청산가능수량                   StartPos 25, Length 10
    char    pamt                [  10];    char    _pamt                ;    // [string,   10] 평균단가                       StartPos 36, Length 10
    char    mamt                [  18];    char    _mamt                ;    // [string,   18] 총매입금액                     StartPos 47, Length 18
    char    medocd              [   1];    char    _medocd              ;    // [string,    1] 매매구분                       StartPos 66, Length 1
    char    dtsunik             [  18];    char    _dtsunik             ;    // [string,   18] 매매손익                       StartPos 68, Length 18
    char    sysprocseq          [  10];    char    _sysprocseq          ;    // [string,   10] 처리순번                       StartPos 87, Length 10
    char    price               [   9];    char    _price               ;    // [string,    9] 현재가                         StartPos 98, Length 9
    char    appamt              [  18];    char    _appamt              ;    // [string,   18] 평가금액                       StartPos 108, Length 18
    char    dtsunik1            [  18];    char    _dtsunik1            ;    // [string,   18] 평가손익                       StartPos 127, Length 18
    char    sunikrt             [  10];    char    _sunikrt             ;    // [string,   10] 수익율                         StartPos 146, Length 10
} d8810OutBlock1, *LPd8810OutBlock1;
#define NAME_d8810OutBlock1     "d8810OutBlock1"


#pragma pack( pop )
///////////////////////////////////////////////////////////////////////////////////////////////////
