#pragma once

///////////////////////////////////////////////////////////////////////////////////////////////////
// 외인기관 종목별동향 ( attr,block,headtype=A ) 
#pragma pack( push, 1 )

#define NAME_t2063     "t2063"

// 입력
typedef struct _t2063InBlock
{
    char    shcode              [   6];    char    _shcode              ;    // [string,    6] 종목코드                       StartPos 0, Length 6
    char    todt                [   8];    char    _todt                ;    // [string,    8] 시작일자                       StartPos 7, Length 8
    char    volvalgb            [   1];    char    _volvalgb            ;    // [string,    1] 금액수량구분                   StartPos 16, Length 1
    char    msmdgb              [   1];    char    _msmdgb              ;    // [string,    1] 매수매도구분                   StartPos 18, Length 1
    char    cumulgb             [   1];    char    _cumulgb             ;    // [string,    1] 누적구분                       StartPos 20, Length 1
    char    cts_date            [   8];    char    _cts_date            ;    // [string,    8] CTSDATE                        StartPos 22, Length 8
    char    cts_idx             [   4];    char    _cts_idx             ;    // [string,    4] CTSIDX                         StartPos 31, Length 4
    char    exchange            [   1];    char    _exchange            ;    // [string,    1] 거래소구분                     StartPos 36, Length 1
} t2063InBlock, *LPt2063InBlock;
#define NAME_t2063InBlock     "t2063InBlock"

// 출력
typedef struct _t2063OutBlock
{
    char    cts_idx             [   4];    char    _cts_idx             ;    // [string,    4] CTSIDX                         StartPos 0, Length 4
    char    cts_date            [   8];    char    _cts_date            ;    // [string,    8] CTSDATE                        StartPos 5, Length 8
} t2063OutBlock, *LPt2063OutBlock;
#define NAME_t2063OutBlock     "t2063OutBlock"

typedef struct _t2063OutBlock1
{
    char    date                [   8];    char    _date                ;    // [string,    8] 일자                           StartPos 0, Length 8
    char    close               [   8];    char    _close               ;    // [string,    8] 종가                           StartPos 9, Length 8
    char    sign                [   1];    char    _sign                ;    // [string,    1] 전일대비구분                   StartPos 18, Length 1
    char    change              [   8];    char    _change              ;    // [string,    8] 전일대비                       StartPos 20, Length 8
    char    diff                [   6];    char    _diff                ;    // [string,    6] 등락율                         StartPos 29, Length 6
    char    volume              [  12];    char    _volume              ;    // [string,   12] 거래량                         StartPos 36, Length 12
    char    amt0008             [  12];    char    _amt0008             ;    // [string,   12] 개인                           StartPos 49, Length 12
    char    amt0009             [  12];    char    _amt0009             ;    // [string,   12] 외국인                         StartPos 62, Length 12
    char    amt0018             [  12];    char    _amt0018             ;    // [string,   12] 기관                           StartPos 75, Length 12
    char    amt0001             [  12];    char    _amt0001             ;    // [string,   12] 증권                           StartPos 88, Length 12
    char    amt0002             [  12];    char    _amt0002             ;    // [string,   12] 보험                           StartPos 101, Length 12
    char    amt0003             [  12];    char    _amt0003             ;    // [string,   12] 투신                           StartPos 114, Length 12
    char    amt0004             [  12];    char    _amt0004             ;    // [string,   12] 은행                           StartPos 127, Length 12
    char    amt0005             [  12];    char    _amt0005             ;    // [string,   12] 종금                           StartPos 140, Length 12
    char    amt0006             [  12];    char    _amt0006             ;    // [string,   12] 기금                           StartPos 153, Length 12
    char    amt0007             [  12];    char    _amt0007             ;    // [string,   12] 기타법인                       StartPos 166, Length 12
    char    amt0010             [  12];    char    _amt0010             ;    // [string,   12] 내외국인                       StartPos 179, Length 12
    char    amt0000             [  12];    char    _amt0000             ;    // [string,   12] 사모펀드                       StartPos 192, Length 12
    char    amt0011             [  12];    char    _amt0011             ;    // [string,   12] 국가외                         StartPos 205, Length 12
    char    amt0088             [  12];    char    _amt0088             ;    // [string,   12] 외인계(등록+미등록)            StartPos 218, Length 12
    char    amt0099             [  12];    char    _amt0099             ;    // [string,   12] 기타계(기타+국가)              StartPos 231, Length 12
} t2063OutBlock1, *LPt2063OutBlock1;
#define NAME_t2063OutBlock1     "t2063OutBlock1"


#pragma pack( pop )
///////////////////////////////////////////////////////////////////////////////////////////////////
