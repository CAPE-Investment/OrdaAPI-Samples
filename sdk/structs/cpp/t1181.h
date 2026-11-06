#pragma once

///////////////////////////////////////////////////////////////////////////////////////////////////
// 거래량급증 ( attr,block,headtype=A ) 
#pragma pack( push, 1 )

#define NAME_t1181     "t1181"

// 입력
typedef struct _t1181InBlock
{
    char    gubun               [   1];    char    _gubun               ;    // [string,    1] 구분                           StartPos 0, Length 1
    char    type1               [   1];    char    _type1               ;    // [string,    1] 정렬필드                       StartPos 2, Length 1
    char    type2               [   1];    char    _type2               ;    // [string,    1] 기준타입                       StartPos 4, Length 1
    char    before              [   3];    char    _before              ;    // [string,    3] 기준카운터                     StartPos 6, Length 3
    char    jc_num              [  12];    char    _jc_num              ;    // [string,   12] 대상제외                       StartPos 10, Length 12
    char    sprice              [   8];    char    _sprice              ;    // [string,    8] 시작가격                       StartPos 23, Length 8
    char    eprice              [   8];    char    _eprice              ;    // [string,    8] 종료가격                       StartPos 32, Length 8
    char    volume              [  12];    char    _volume              ;    // [string,   12] 거래량                         StartPos 41, Length 12
    char    idx                 [   4];    char    _idx                 ;    // [string,    4] IDX                            StartPos 54, Length 4
    char    exchange            [   1];    char    _exchange            ;    // [string,    1] 거래소구분                     StartPos 59, Length 1
} t1181InBlock, *LPt1181InBlock;
#define NAME_t1181InBlock     "t1181InBlock"

// 출력
typedef struct _t1181OutBlock
{
    char    idx                 [   4];    char    _idx                 ;    // [string,    4] IDX                            StartPos 0, Length 4
} t1181OutBlock, *LPt1181OutBlock;
#define NAME_t1181OutBlock     "t1181OutBlock"

typedef struct _t1181OutBlock1
{
    char    shcode              [   6];    char    _shcode              ;    // [string,    6] 종목코드                       StartPos 0, Length 6
    char    hname               [  40];    char    _hname               ;    // [string,   40] 종목명                         StartPos 7, Length 40
    char    price               [   8];    char    _price               ;    // [string,    8] 현재가                         StartPos 48, Length 8
    char    sign                [   1];    char    _sign                ;    // [string,    1] 전일대비구분                   StartPos 57, Length 1
    char    change              [   8];    char    _change              ;    // [string,    8] 전일대비                       StartPos 59, Length 8
    char    diff                [   6];    char    _diff                ;    // [string,    6] 등락율                         StartPos 68, Length 6
    char    stdvolume           [  12];    char    _stdvolume           ;    // [string,   12] 기준거래량                     StartPos 75, Length 12
    char    volume              [  12];    char    _volume              ;    // [string,   12] 거래량                         StartPos 88, Length 12
    char    volchange           [  12];    char    _volchange           ;    // [string,   12] 급증량                         StartPos 101, Length 12
    char    voldiff             [   8];    char    _voldiff             ;    // [string,    8] 급증율                         StartPos 114, Length 8
} t1181OutBlock1, *LPt1181OutBlock1;
#define NAME_t1181OutBlock1     "t1181OutBlock1"


#pragma pack( pop )
///////////////////////////////////////////////////////////////////////////////////////////////////
