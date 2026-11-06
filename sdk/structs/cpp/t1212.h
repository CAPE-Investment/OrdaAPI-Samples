#pragma once

///////////////////////////////////////////////////////////////////////////////////////////////////
// 시간별예상체결가 ( attr,block,headtype=A ) 
#pragma pack( push, 1 )

#define NAME_t1212     "t1212"

// 입력
typedef struct _t1212InBlock
{
    char    shcode              [   6];    char    _shcode              ;    // [string,    6] 단축코드                       StartPos 0, Length 6
    char    cts_time            [  10];    char    _cts_time            ;    // [string,   10] 시간CTS                        StartPos 7, Length 10
    char    cnt                 [   4];    char    _cnt                 ;    // [string,    4] 조회건수                       StartPos 18, Length 4
    char    exchange            [   1];    char    _exchange            ;    // [string,    1] 거래소구분                     StartPos 23, Length 1
} t1212InBlock, *LPt1212InBlock;
#define NAME_t1212InBlock     "t1212InBlock"

// 출력
typedef struct _t1212OutBlock
{
    char    cts_time            [  10];    char    _cts_time            ;    // [string,   10] 시간CTS                        StartPos 0, Length 10
} t1212OutBlock, *LPt1212OutBlock;
#define NAME_t1212OutBlock     "t1212OutBlock"

typedef struct _t1212OutBlock1
{
    char    chetime             [   8];    char    _chetime             ;    // [string,    8] 시간                           StartPos 0, Length 8
    char    price               [   8];    char    _price               ;    // [string,    8] 예상체결가                     StartPos 9, Length 8
    char    sign                [   1];    char    _sign                ;    // [string,    1] 전일대비구분                   StartPos 18, Length 1
    char    change              [   8];    char    _change              ;    // [string,    8] 전일대비                       StartPos 20, Length 8
    char    diff                [   6];    char    _diff                ;    // [string,    6] 등락율                         StartPos 29, Length 6
    char    cvolume             [  12];    char    _cvolume             ;    // [string,   12] 예상체결량                     StartPos 36, Length 12
    char    offerho1            [   8];    char    _offerho1            ;    // [string,    8] 매도호가                       StartPos 49, Length 8
    char    bidho1              [   8];    char    _bidho1              ;    // [string,    8] 매수호가                       StartPos 58, Length 8
    char    offerrem1           [  12];    char    _offerrem1           ;    // [string,   12] 매도잔량                       StartPos 67, Length 12
    char    bidrem1             [  12];    char    _bidrem1             ;    // [string,   12] 매수잔량                       StartPos 80, Length 12
} t1212OutBlock1, *LPt1212OutBlock1;
#define NAME_t1212OutBlock1     "t1212OutBlock1"


#pragma pack( pop )
///////////////////////////////////////////////////////////////////////////////////////////////////
