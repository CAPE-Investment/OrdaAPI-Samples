#pragma once

///////////////////////////////////////////////////////////////////////////////////////////////////
// 선물/옵션 현재가 호가 조회 ( attr,block,headtype=A ) 
#pragma pack( push, 1 )

#define NAME_s3001     "s3001"

// 입력
typedef struct _s3001InBlock
{
    char    shcode              [   8];    char    _shcode              ;    // [string,    8] 단축코드                       StartPos 0, Length 8
} s3001InBlock, *LPs3001InBlock;
#define NAME_s3001InBlock     "s3001InBlock"

// 출력
typedef struct _s3001OutBlock
{
    char    hname               [  20];    char    _hname               ;    // [string,   20] 종목명                         StartPos 0, Length 20
    char    price               [  10];    char    _price               ;    // [string,   10] 현재가                         StartPos 21, Length 10
    char    sign                [   1];    char    _sign                ;    // [string,    1] 전일대비구분                   StartPos 32, Length 1
    char    change              [  10];    char    _change              ;    // [string,   10] 전일대비                       StartPos 34, Length 10
    char    diff                [   6];    char    _diff                ;    // [string,    6] 등락율                         StartPos 45, Length 6
    char    volume              [  12];    char    _volume              ;    // [string,   12] 거래량                         StartPos 52, Length 12
    char    stimeqrt            [   6];    char    _stimeqrt            ;    // [string,    6] 거래량전일동시간비율           StartPos 65, Length 6
    char    recprice            [  10];    char    _recprice            ;    // [string,   10] 전일종가                       StartPos 72, Length 10
    char    offerho1            [  10];    char    _offerho1            ;    // [string,   10] 매도호가1                      StartPos 83, Length 10
    char    bidho1              [  10];    char    _bidho1              ;    // [string,   10] 매수호가1                      StartPos 94, Length 10
    char    offerrem1           [   8];    char    _offerrem1           ;    // [string,    8] 매도호가수량1                  StartPos 105, Length 8
    char    bidrem1             [   8];    char    _bidrem1             ;    // [string,    8] 매수호가수량1                  StartPos 114, Length 8
    char    dcnt1               [   8];    char    _dcnt1               ;    // [string,    8] 매도호가건수1                  StartPos 123, Length 8
    char    scnt1               [   8];    char    _scnt1               ;    // [string,    8] 매수호가건수1                  StartPos 132, Length 8
    char    offerho2            [  10];    char    _offerho2            ;    // [string,   10] 매도호가2                      StartPos 141, Length 10
    char    bidho2              [  10];    char    _bidho2              ;    // [string,   10] 매수호가2                      StartPos 152, Length 10
    char    offerrem2           [   8];    char    _offerrem2           ;    // [string,    8] 매도호가수량2                  StartPos 163, Length 8
    char    bidrem2             [   8];    char    _bidrem2             ;    // [string,    8] 매수호가수량2                  StartPos 172, Length 8
    char    dcnt2               [   8];    char    _dcnt2               ;    // [string,    8] 매도호가건수2                  StartPos 181, Length 8
    char    scnt2               [   8];    char    _scnt2               ;    // [string,    8] 매수호가건수2                  StartPos 190, Length 8
    char    offerho3            [  10];    char    _offerho3            ;    // [string,   10] 매도호가3                      StartPos 199, Length 10
    char    bidho3              [  10];    char    _bidho3              ;    // [string,   10] 매수호가3                      StartPos 210, Length 10
    char    offerrem3           [   8];    char    _offerrem3           ;    // [string,    8] 매도호가수량3                  StartPos 221, Length 8
    char    bidrem3             [   8];    char    _bidrem3             ;    // [string,    8] 매수호가수량3                  StartPos 230, Length 8
    char    dcnt3               [   8];    char    _dcnt3               ;    // [string,    8] 매도호가건수3                  StartPos 239, Length 8
    char    scnt3               [   8];    char    _scnt3               ;    // [string,    8] 매수호가건수3                  StartPos 248, Length 8
    char    offerho4            [  10];    char    _offerho4            ;    // [string,   10] 매도호가4                      StartPos 257, Length 10
    char    bidho4              [  10];    char    _bidho4              ;    // [string,   10] 매수호가4                      StartPos 268, Length 10
    char    offerrem4           [   8];    char    _offerrem4           ;    // [string,    8] 매도호가수량4                  StartPos 279, Length 8
    char    bidrem4             [   8];    char    _bidrem4             ;    // [string,    8] 매수호가수량4                  StartPos 288, Length 8
    char    dcnt4               [   8];    char    _dcnt4               ;    // [string,    8] 매도호가건수4                  StartPos 297, Length 8
    char    scnt4               [   8];    char    _scnt4               ;    // [string,    8] 매수호가건수4                  StartPos 306, Length 8
    char    offerho5            [  10];    char    _offerho5            ;    // [string,   10] 매도호가5                      StartPos 315, Length 10
    char    bidho5              [  10];    char    _bidho5              ;    // [string,   10] 매수호가5                      StartPos 326, Length 10
    char    offerrem5           [   8];    char    _offerrem5           ;    // [string,    8] 매도호가수량5                  StartPos 337, Length 8
    char    bidrem5             [   8];    char    _bidrem5             ;    // [string,    8] 매수호가수량5                  StartPos 346, Length 8
    char    dcnt5               [   8];    char    _dcnt5               ;    // [string,    8] 매도호가건수5                  StartPos 355, Length 8
    char    scnt5               [   8];    char    _scnt5               ;    // [string,    8] 매수호가건수5                  StartPos 364, Length 8
    char    dvol                [   8];    char    _dvol                ;    // [string,    8] 매도호가총수량                 StartPos 373, Length 8
    char    svol                [   8];    char    _svol                ;    // [string,    8] 매수호가총수량                 StartPos 382, Length 8
    char    toffernum           [   8];    char    _toffernum           ;    // [string,    8] 총매도호가건수                 StartPos 391, Length 8
    char    tbidnum             [   8];    char    _tbidnum             ;    // [string,    8] 총매수호가건수                 StartPos 400, Length 8
    char    time                [   6];    char    _time                ;    // [string,    6] 수신시간                       StartPos 409, Length 6
    char    shcode              [   8];    char    _shcode              ;    // [string,    8] 단축코드                       StartPos 416, Length 8
} s3001OutBlock, *LPs3001OutBlock;
#define NAME_s3001OutBlock     "s3001OutBlock"


#pragma pack( pop )
///////////////////////////////////////////////////////////////////////////////////////////////////
