#pragma once

///////////////////////////////////////////////////////////////////////////////////////////////////
// 예상체결가상/하한가조회 ( attr,block,headtype=A )
#pragma pack( push, 1 )

#define NAME_t1214     "t1214"

// 입력
typedef struct _t1214InBlock
{
    char    gubun               [   1];    char    _gubun               ;    // [string,    1] 거래소구분                     StartPos 0, Length 1
    char    sign                [   1];    char    _sign                ;    // [string,    1] 상하한구분                     StartPos 2, Length 1
    char    jgubun              [   1];    char    _jgubun              ;    // [string,    1] 장구분                         StartPos 4, Length 1
    char    jongchk             [  12];    char    _jongchk             ;    // [string,   12] 종목체크                       StartPos 6, Length 12
    char    sprice              [   8];    char    _sprice              ;    // [string,    8] 시작가격                       StartPos 19, Length 8
    char    eprice              [   8];    char    _eprice              ;    // [string,    8] 종료가격                       StartPos 28, Length 8
    char    volume              [  12];    char    _volume              ;    // [string,   12] 거래량                         StartPos 37, Length 12
    char    idx                 [   4];    char    _idx                 ;    // [string,    4] 종목IDX                        StartPos 50, Length 4
} t1214InBlock, *LPt1214InBlock;
#define NAME_t1214InBlock     "t1214InBlock"

typedef struct _t1214OutBlock
{
    char    idx                 [   4];    char    _idx                 ;    // [string,    4] CTS                            StartPos 0, Length 4
    char    cnt                 [   4];    char    _cnt                 ;    // [string,    4] 종목수                         StartPos 5, Length 4
} t1214OutBlock, *LPt1214OutBlock;
#define NAME_t1214OutBlock     "t1214OutBlock"

// 출력
typedef struct _t1214OutBlock1
{
    char    hname               [  40];    char    _hname               ;    // [string,   40] 한글명                         StartPos 0, Length 40
    char    price               [   8];    char    _price               ;    // [string,    8] 현재가                         StartPos 41, Length 8
    char    sign                [   1];    char    _sign                ;    // [string,    1] 전일대비구분                   StartPos 50, Length 1
    char    change              [   8];    char    _change              ;    // [string,    8] 전일대비                       StartPos 52, Length 8
    char    diff                [   6];    char    _diff                ;    // [string,    6] 등락율                         StartPos 61, Length 6
    char    volume              [  12];    char    _volume              ;    // [string,   12] 누적거래량                     StartPos 68, Length 12
    char    offerrem            [   8];    char    _offerrem            ;    // [string,    8] 매도잔량                       StartPos 81, Length 8
    char    offerho             [   8];    char    _offerho             ;    // [string,    8] 매도호가                       StartPos 90, Length 8
    char    bidho               [   8];    char    _bidho               ;    // [string,    8] 매수호가                       StartPos 99, Length 8
    char    bidrem              [   8];    char    _bidrem              ;    // [string,    8] 매수잔량                       StartPos 108, Length 8
    char    cnt                 [   4];    char    _cnt                 ;    // [string,    4] 연속일수                       StartPos 117, Length 4
    char    shcode              [   6];    char    _shcode              ;    // [string,    6] 종목코드                       StartPos 122, Length 6
} t1214OutBlock1, *LPt1214OutBlock1;
#define NAME_t1214OutBlock1     "t1214OutBlock1"


#pragma pack( pop )
///////////////////////////////////////////////////////////////////////////////////////////////////