#pragma once

///////////////////////////////////////////////////////////////////////////////////////////////////
// 기간별주가 ( attr,block,headtype=A ) 
#pragma pack( push, 1 )

#define NAME_t3012     "t3012"

// 입력
typedef struct _t3012InBlock
{
    char    shcode              [   8];    char    _shcode              ;    // [string,    8] 단축코드                       StartPos 0, Length 8
    char    futcheck            [   1];    char    _futcheck            ;    // [string,    1] 선물최근월물                   StartPos 9, Length 1
    char    date                [   8];    char    _date                ;    // [string,    8] 날짜                           StartPos 11, Length 8
    char    cts_code            [   8];    char    _cts_code            ;    // [string,    8] CTS종목코드                    StartPos 20, Length 8
    char    lastdate            [   8];    char    _lastdate            ;    // [string,    8] 전종목만기일                   StartPos 29, Length 8
    char    cnt                 [   3];    char    _cnt                 ;    // [string,    3] 조회요청건수                   StartPos 38, Length 3
} t3012InBlock, *LPt3012InBlock;
#define NAME_t3012InBlock     "t3012InBlock"

// 출력
typedef struct _t3012OutBlock
{
    char    date                [   8];    char    _date                ;    // [string,    8] 날짜                           StartPos 0, Length 8
    char    cts_code            [   8];    char    _cts_code            ;    // [string,    8] CTS종목코드                    StartPos 9, Length 8
    char    lastdate            [   8];    char    _lastdate            ;    // [string,    8] 전종목만기일                   StartPos 18, Length 8
    char    nowfutyn            [   1];    char    _nowfutyn            ;    // [string,    1] 최근월선물여부                 StartPos 27, Length 1
} t3012OutBlock, *LPt3012OutBlock;
#define NAME_t3012OutBlock     "t3012OutBlock"

typedef struct _t3012OutBlock1
{
    char    date                [   8];    char    _date                ;    // [string,    8] 날짜                           StartPos 0, Length 8
    char    open                [   6];    char    _open                ;    // [string,    6] 시가                           StartPos 9, Length 6
    char    high                [   6];    char    _high                ;    // [string,    6] 고가                           StartPos 16, Length 6
    char    low                 [   6];    char    _low                 ;    // [string,    6] 저가                           StartPos 23, Length 6
    char    close               [   6];    char    _close               ;    // [string,    6] 종가                           StartPos 30, Length 6
    char    sign                [   1];    char    _sign                ;    // [string,    1] 전일대비구분                   StartPos 37, Length 1
    char    change              [   6];    char    _change              ;    // [string,    6] 전일대비                       StartPos 39, Length 6
    char    diff                [   6];    char    _diff                ;    // [string,    6] 등락율                         StartPos 46, Length 6
    char    volume              [  12];    char    _volume              ;    // [string,   12] 거래량                         StartPos 53, Length 12
    char    diff_vol            [  10];    char    _diff_vol            ;    // [string,   10] 거래증가율                     StartPos 66, Length 10
    char    openyak             [   8];    char    _openyak             ;    // [string,    8] 미결수량                       StartPos 77, Length 8
    char    openyakupdn         [   8];    char    _openyakupdn         ;    // [string,    8] 미결증감                       StartPos 86, Length 8
    char    value               [  12];    char    _value               ;    // [string,   12] 거래대금                       StartPos 95, Length 12
} t3012OutBlock1, *LPt3012OutBlock1;
#define NAME_t3012OutBlock1     "t3012OutBlock1"


#pragma pack( pop )
///////////////////////////////////////////////////////////////////////////////////////////////////
