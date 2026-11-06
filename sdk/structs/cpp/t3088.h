#pragma once

///////////////////////////////////////////////////////////////////////////////////////////////////
// ELW마스터조회(t3088) ( attr,block,headtype=A ) 
#pragma pack( push, 1 )

#define NAME_t3088     "t3088"

// 입력
typedef struct _t3088InBlock
{
    char    expcode             [  12];    char    _expcode             ;    // [string,   12] 연속키                         StartPos 0, Length 12
} t3088InBlock, *LPt3088InBlock;
#define NAME_t3088InBlock     "t3088InBlock"

// 출력
typedef struct _t3088OutBlock
{
    char    expcode             [  12];    char    _expcode             ;    // [string,   12] 연속키                         StartPos 0, Length 12
} t3088OutBlock, *LPt3088OutBlock;
#define NAME_t3088OutBlock     "t3088OutBlock"

typedef struct _t3088OutBlock1
{
    char    shcode              [   6];    char    _shcode              ;    // [string,    6] 단축코드                       StartPos 0, Length 6
    char    expcode             [  12];    char    _expcode             ;    // [string,   12] 표준코드                       StartPos 7, Length 12
    char    hname               [  40];    char    _hname               ;    // [string,   40] 종목명                         StartPos 20, Length 40
    char    issuercd            [   3];    char    _issuercd            ;    // [string,    3] 발행사코드                     StartPos 61, Length 3
    char    issuernmk           [  40];    char    _issuernmk           ;    // [string,   40] 발행사명                       StartPos 65, Length 40
    char    lastymd             [   8];    char    _lastymd             ;    // [string,    8] 만기일                         StartPos 106, Length 8
    char    item1               [  12];    char    _item1               ;    // [string,   12] 기초자산코드1                  StartPos 115, Length 12
    char    item2               [  12];    char    _item2               ;    // [string,   12] 기초자산코드2                  StartPos 128, Length 12
    char    item3               [  12];    char    _item3               ;    // [string,   12] 기초자산코드3                  StartPos 141, Length 12
    char    item4               [  12];    char    _item4               ;    // [string,   12] 기초자산코드4                  StartPos 154, Length 12
    char    item5               [  12];    char    _item5               ;    // [string,   12] 기초자산코드5                  StartPos 167, Length 12
    char    elwopt              [   2];    char    _elwopt              ;    // [string,    2] 콜풋구분                       StartPos 180, Length 2
} t3088OutBlock1, *LPt3088OutBlock1;
#define NAME_t3088OutBlock1     "t3088OutBlock1"


#pragma pack( pop )
///////////////////////////////////////////////////////////////////////////////////////////////////
