#pragma once

///////////////////////////////////////////////////////////////////////////////////////////////////
// 종목별 상위회원사 ( attr,block,headtype=A ) 
#pragma pack( push, 1 )

#define NAME_t2082     "t2082"

// 입력
typedef struct _t2082InBlock
{
    char    shcode              [   6];    char    _shcode              ;    // [string,    6] 종목코드                       StartPos 0, Length 6
    char    traddate1           [   8];    char    _traddate1           ;    // [string,    8] 조회날짜1                      StartPos 7, Length 8
    char    traddate2           [   8];    char    _traddate2           ;    // [string,    8] 조회날짜2                      StartPos 16, Length 8
    char    fwgubun1            [   1];    char    _fwgubun1            ;    // [string,    1] 외국계구분                     StartPos 25, Length 1
    char    cts_idx             [   4];    char    _cts_idx             ;    // [string,    4] CTSIDX                         StartPos 27, Length 4
    char    exchange            [   1];    char    _exchange            ;    // [string,    1] 거래소구분                     StartPos 32, Length 1
} t2082InBlock, *LPt2082InBlock;
#define NAME_t2082InBlock     "t2082InBlock"

// 출력
typedef struct _t2082OutBlock
{
    char    fwdvl               [  12];    char    _fwdvl               ;    // [string,   12] 외국계매도                     StartPos 0, Length 12
    char    fwsvl               [  12];    char    _fwsvl               ;    // [string,   12] 외국계매수                     StartPos 13, Length 12
    char    cts_idx             [   4];    char    _cts_idx             ;    // [string,    4] CTSIDX                         StartPos 26, Length 4
} t2082OutBlock, *LPt2082OutBlock;
#define NAME_t2082OutBlock     "t2082OutBlock"

typedef struct _t2082OutBlock1
{
    char    tradname            [  20];    char    _tradname            ;    // [string,   20] 회원사                         StartPos 0, Length 20
    char    tradmdvol           [  12];    char    _tradmdvol           ;    // [string,   12] 매도수량                       StartPos 21, Length 12
    char    tradmsvol           [  12];    char    _tradmsvol           ;    // [string,   12] 매수수량                       StartPos 34, Length 12
    char    tradmssvol          [  12];    char    _tradmssvol          ;    // [string,   12] 순매수                         StartPos 47, Length 12
    char    wintrd              [  12];    char    _wintrd              ;    // [string,   12] 창구거래                       StartPos 60, Length 12
    char    winrat              [   6];    char    _winrat              ;    // [string,    6] 비중                           StartPos 73, Length 6
    char    tradno              [   3];    char    _tradno              ;    // [string,    3] 회원사코드                     StartPos 80, Length 3
    char    wgubun              [   1];    char    _wgubun              ;    // [string,    1] 외국계여부                     StartPos 84, Length 1
    char    swinrat             [   6];    char    _swinrat             ;    // [string,    6] 순비중                         StartPos 86, Length 6
} t2082OutBlock1, *LPt2082OutBlock1;
#define NAME_t2082OutBlock1     "t2082OutBlock1"


#pragma pack( pop )
///////////////////////////////////////////////////////////////////////////////////////////////////
