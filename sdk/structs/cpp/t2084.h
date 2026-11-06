#pragma once

///////////////////////////////////////////////////////////////////////////////////////////////////
// 회원사매매수신 ( attr,block,headtype=A ) 
#pragma pack( push, 1 )

#define NAME_t2084     "t2084"

// 입력
typedef struct _t2084InBlock
{
    char    tradno              [   3];    char    _tradno              ;    // [string,    3] 회원사코드                     StartPos 0, Length 3
    char    shcode              [   6];    char    _shcode              ;    // [string,    6] 종목코드                       StartPos 4, Length 6
    char    grpcode             [   4];    char    _grpcode             ;    // [string,    4] 그룹코드                       StartPos 11, Length 4
    char    market              [   1];    char    _market              ;    // [string,    1] 시장구분                       StartPos 16, Length 1
    char    ssvolume            [  12];    char    _ssvolume            ;    // [string,   12] 순매수/순매도                  StartPos 18, Length 12
    char    cts_tradtime        [   8];    char    _cts_tradtime        ;    // [string,    8] CTSTIME                        StartPos 31, Length 8
    char    cts_idx             [   4];    char    _cts_idx             ;    // [string,    4] CTSIDX                         StartPos 40, Length 4
    char    cts_shcode          [   6];    char    _cts_shcode          ;    // [string,    6] CTS종목코드                    StartPos 45, Length 6
    char    cnt                 [   3];    char    _cnt                 ;    // [string,    3] 요청건수                       StartPos 52, Length 3
    char    tot_gb              [   1];    char    _tot_gb              ;    // [string,    1] 전체구분(1:전체(순매수/매도무시)2:순매수기준3:순매도기준) StartPos 56, Length 1
} t2084InBlock, *LPt2084InBlock;
#define NAME_t2084InBlock     "t2084InBlock"

// 출력
typedef struct _t2084OutBlock
{
    char    cts_tradtime        [   8];    char    _cts_tradtime        ;    // [string,    8] CTSTIME                        StartPos 0, Length 8
    char    cts_idx             [   4];    char    _cts_idx             ;    // [string,    4] CTSIDX                         StartPos 9, Length 4
} t2084OutBlock, *LPt2084OutBlock;
#define NAME_t2084OutBlock     "t2084OutBlock"

typedef struct _t2084OutBlock1
{
    char    tradtime            [   8];    char    _tradtime            ;    // [string,    8] 시간                           StartPos 0, Length 8
    char    tradname            [  20];    char    _tradname            ;    // [string,   20] 회원사이름                     StartPos 9, Length 20
    char    hname               [  20];    char    _hname               ;    // [string,   20] 종목명                         StartPos 30, Length 20
    char    price               [   8];    char    _price               ;    // [string,    8] 현재가                         StartPos 51, Length 8
    char    sign                [   1];    char    _sign                ;    // [string,    1] 대비구분                       StartPos 60, Length 1
    char    diff                [   6];    char    _diff                ;    // [string,    6] 등락율                         StartPos 62, Length 6
    char    ssvolume            [  12];    char    _ssvolume            ;    // [string,   12] 순매수                         StartPos 69, Length 12
    char    wssvolume           [  12];    char    _wssvolume           ;    // [string,   12] 창구순매수                     StartPos 82, Length 12
    char    fwvolume            [  12];    char    _fwvolume            ;    // [string,   12] 외국계전체                     StartPos 95, Length 12
    char    tradno              [   3];    char    _tradno              ;    // [string,    3] 회원사코드                     StartPos 108, Length 3
    char    shcode              [   6];    char    _shcode              ;    // [string,    6] 종목코드                       StartPos 112, Length 6
} t2084OutBlock1, *LPt2084OutBlock1;
#define NAME_t2084OutBlock1     "t2084OutBlock1"


#pragma pack( pop )
///////////////////////////////////////////////////////////////////////////////////////////////////
