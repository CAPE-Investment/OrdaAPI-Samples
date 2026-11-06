#pragma once

///////////////////////////////////////////////////////////////////////////////////////////////////
// FNG 재무제표 ( attr,block,headtype=A ) 
#pragma pack( push, 1 )

#define NAME_s2506     "s2506"

// 입력
typedef struct _s2506InBlock
{
    char    gicode              [   7];    char    _gicode              ;    // [string,    7] 종목코드                       StartPos 0, Length 7
    char    gs_ym               [   6];    char    _gs_ym               ;    // [string,    6] 회계년월                       StartPos 8, Length 6
    char    gubun               [   1];    char    _gubun               ;    // [string,    1] gubun                          StartPos 15, Length 1
} s2506InBlock, *LPs2506InBlock;
#define NAME_s2506InBlock     "s2506InBlock"

// 출력
typedef struct _s2506OutBlock
{
    char    gsym1               [   6];    char    _gsym1               ;    // [string,    6] 결산년월1                      StartPos 0, Length 6
    char    gsym2               [   6];    char    _gsym2               ;    // [string,    6] 결산년월2                      StartPos 7, Length 6
    char    gsym3               [   6];    char    _gsym3               ;    // [string,    6] 결산년월3                      StartPos 14, Length 6
    char    gsym4               [   6];    char    _gsym4               ;    // [string,    6] 결산년월4                      StartPos 21, Length 6
    char    gsym5               [   6];    char    _gsym5               ;    // [string,    6] 결산년월5                      StartPos 28, Length 6
} s2506OutBlock, *LPs2506OutBlock;
#define NAME_s2506OutBlock     "s2506OutBlock"

typedef struct _s2506OutBlock1
{
    char    gubunnm             [  80];    char    _gubunnm             ;    // [string,   80] 구분                           StartPos 0, Length 80
    char    data1               [  13];    char    _data1               ;    // [string,   13] data1                          StartPos 81, Length 13
    char    data2               [  13];    char    _data2               ;    // [string,   13] data2                          StartPos 95, Length 13
    char    data3               [  13];    char    _data3               ;    // [string,   13] data3                          StartPos 109, Length 13
    char    data4               [  13];    char    _data4               ;    // [string,   13] data4                          StartPos 123, Length 13
    char    data5               [  13];    char    _data5               ;    // [string,   13] data5                          StartPos 137, Length 13
} s2506OutBlock1, *LPs2506OutBlock1;
#define NAME_s2506OutBlock1     "s2506OutBlock1"


#pragma pack( pop )
///////////////////////////////////////////////////////////////////////////////////////////////////
