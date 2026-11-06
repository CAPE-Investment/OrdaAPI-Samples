#pragma once

///////////////////////////////////////////////////////////////////////////////////////////////////
// 외국계매매상위 ( attr,block,headtype=A ) 
#pragma pack( push, 1 )

#define NAME_t2091     "t2091"

// 입력
typedef struct _t2091InBlock
{
    char    gubun1              [   1];    char    _gubun1              ;    // [string,    1] 구분1                          StartPos 0, Length 1
    char    gubun2              [   1];    char    _gubun2              ;    // [string,    1] 구분2                          StartPos 2, Length 1
    char    gubun3              [   1];    char    _gubun3              ;    // [string,    1] 구분3                          StartPos 4, Length 1
} t2091InBlock, *LPt2091InBlock;
#define NAME_t2091InBlock     "t2091InBlock"

// 출력
typedef struct _t2091OutBlock1
{
    char    hname               [  20];    char    _hname               ;    // [string,   20] 한글명                         StartPos 0, Length 20
    char    price               [   8];    char    _price               ;    // [string,    8] 현재가                         StartPos 21, Length 8
    char    sign                [   1];    char    _sign                ;    // [string,    1] 전일대비구분                   StartPos 30, Length 1
    char    change              [   8];    char    _change              ;    // [string,    8] 전일대비                       StartPos 32, Length 8
    char    diff                [   6];    char    _diff                ;    // [string,    6] 등락율                         StartPos 41, Length 6
    char    volume              [  12];    char    _volume              ;    // [string,   12] 누적거래량                     StartPos 48, Length 12
    char    fwdvl               [  12];    char    _fwdvl               ;    // [string,   12] 매도합계                       StartPos 61, Length 12
    char    fwsvl               [  12];    char    _fwsvl               ;    // [string,   12] 매수합계                       StartPos 74, Length 12
    char    chad                [  12];    char    _chad                ;    // [string,   12] 순매도                         StartPos 87, Length 12
    char    shcode              [   6];    char    _shcode              ;    // [string,    6] 종목코드                       StartPos 100, Length 6
} t2091OutBlock1, *LPt2091OutBlock1;
#define NAME_t2091OutBlock1     "t2091OutBlock1"

typedef struct _t2091OutBlock2
{
    char    hname               [  20];    char    _hname               ;    // [string,   20] 한글명                         StartPos 0, Length 20
    char    price               [   8];    char    _price               ;    // [string,    8] 현재가                         StartPos 21, Length 8
    char    sign                [   1];    char    _sign                ;    // [string,    1] 전일대비구분                   StartPos 30, Length 1
    char    change              [   8];    char    _change              ;    // [string,    8] 전일대비                       StartPos 32, Length 8
    char    diff                [   6];    char    _diff                ;    // [string,    6] 등락율                         StartPos 41, Length 6
    char    volume              [  12];    char    _volume              ;    // [string,   12] 누적거래량                     StartPos 48, Length 12
    char    fwdvl               [  12];    char    _fwdvl               ;    // [string,   12] 매도합계                       StartPos 61, Length 12
    char    fwsvl               [  12];    char    _fwsvl               ;    // [string,   12] 매수합계                       StartPos 74, Length 12
    char    chas                [  12];    char    _chas                ;    // [string,   12] 순매도                         StartPos 87, Length 12
    char    shcode              [   6];    char    _shcode              ;    // [string,    6] 종목코드                       StartPos 100, Length 6
} t2091OutBlock2, *LPt2091OutBlock2;
#define NAME_t2091OutBlock2     "t2091OutBlock2"


#pragma pack( pop )
///////////////////////////////////////////////////////////////////////////////////////////////////
