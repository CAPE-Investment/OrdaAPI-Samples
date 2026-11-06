#pragma once

///////////////////////////////////////////////////////////////////////////////////////////////////
// 종목별 프로그램매매 추이 ( attr,block,headtype=A ) 
#pragma pack( push, 1 )

#define NAME_t2052     "t2052"

// 입력
typedef struct _t2052InBlock
{
    char    gubun1              [   1];    char    _gubun1              ;    // [string,    1] 수량금액구분(0:수량1:금액)     StartPos 0, Length 1
    char    gubun2              [   1];    char    _gubun2              ;    // [string,    1] 시간일별구분(0:시간1:일자)     StartPos 2, Length 1
    char    shcode              [   6];    char    _shcode              ;    // [string,    6] 종목코드                       StartPos 4, Length 6
    char    date                [   8];    char    _date                ;    // [string,    8] 일자                           StartPos 11, Length 8
    char    time                [   6];    char    _time                ;    // [string,    6] 시간                           StartPos 20, Length 6
    char    cts_idx             [   4];    char    _cts_idx             ;    // [string,    4] IDXCTS(9999:차트)              StartPos 27, Length 4
    char    exchange            [   1];    char    _exchange            ;    // [string,    1] 거래소구분                     StartPos 32, Length 1
} t2052InBlock, *LPt2052InBlock;
#define NAME_t2052InBlock     "t2052InBlock"

// 출력
typedef struct _t2052OutBlock
{
    char    cts_idx             [   4];    char    _cts_idx             ;    // [string,    4] IDXCTS                         StartPos 0, Length 4
} t2052OutBlock, *LPt2052OutBlock;
#define NAME_t2052OutBlock     "t2052OutBlock"

typedef struct _t2052OutBlock1
{
    char    date                [   8];    char    _date                ;    // [string,    8] 일자                           StartPos 0, Length 8
    char    time                [   6];    char    _time                ;    // [string,    6] 시간                           StartPos 9, Length 6
    char    price               [   8];    char    _price               ;    // [string,    8] 현재가                         StartPos 16, Length 8
    char    sign                [   1];    char    _sign                ;    // [string,    1] 대비구분                       StartPos 25, Length 1
    char    change              [   8];    char    _change              ;    // [string,    8] 대비                           StartPos 27, Length 8
    char    diff                [   6];    char    _diff                ;    // [string,    6] 등락율                         StartPos 36, Length 6
    char    volume              [  12];    char    _volume              ;    // [string,   12] 거래량                         StartPos 43, Length 12
    char    svalue              [  15];    char    _svalue              ;    // [string,   15] 순매수금액                     StartPos 56, Length 15
    char    offervalue          [  15];    char    _offervalue          ;    // [string,   15] 매도금액                       StartPos 72, Length 15
    char    stksvalue           [  15];    char    _stksvalue           ;    // [string,   15] 매수금액                       StartPos 88, Length 15
    char    svolume             [  12];    char    _svolume             ;    // [string,   12] 순매수수량                     StartPos 104, Length 12
    char    offervolume         [  12];    char    _offervolume         ;    // [string,   12] 매도수량                       StartPos 117, Length 12
    char    stksvolume          [  12];    char    _stksvolume          ;    // [string,   12] 매수수량                       StartPos 130, Length 12
    char    shcode              [   6];    char    _shcode              ;    // [string,    6] 종목코드                       StartPos 143, Length 6
    char    svalue_b            [  15];    char    _svalue_b            ;    // [string,   15] 순매수금액_직전대비            StartPos 150, Length 15
    char    svolume_b           [  12];    char    _svolume_b           ;    // [string,   12] 순매수수량_직전대비            StartPos 166, Length 12
} t2052OutBlock1, *LPt2052OutBlock1;
#define NAME_t2052OutBlock1     "t2052OutBlock1"


#pragma pack( pop )
///////////////////////////////////////////////////////////////////////////////////////////////////
