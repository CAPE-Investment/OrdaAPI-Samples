#pragma once

///////////////////////////////////////////////////////////////////////////////////////////////////
// 영업일정보조회 ( headtype=C )
#pragma pack( push, 1 )

#define NAME_d7304     "d7304"

// 입력
typedef struct _d7304InBlock
{
    char    gubun               [   1];    // [string,    1] 구분(1:N일전 5:N영업일전)      StartPos 0, Length 1
    char    fromdt              [   8];    // [string,    8] 시작일자                       StartPos 1, Length 8
    char    todt                [   8];    // [string,    8] 종료일자                       StartPos 9, Length 8
    char    cnt                 [   4];    // [string,    4] 일수                           StartPos 17, Length 4
} d7304InBlock, *LPd7304InBlock;
#define NAME_d7304InBlock     "d7304InBlock"

// 출력
typedef struct _d7304OutBlock
{
    char    bizdt               [   8];    // [string,    8] 영업일                         StartPos 0, Length 8
    char    bfbizdt             [   8];    // [string,    8] 전영업일                       StartPos 8, Length 8
    char    afbizdt             [   8];    // [string,    8] 익영업일                       StartPos 16, Length 8
    char    afbizdt2            [   8];    // [string,    8] 2영업일                        StartPos 24, Length 8
    char    day_cnt             [   4];    // [string,    4] 일수                           StartPos 32, Length 4
} d7304OutBlock, *LPd7304OutBlock;
#define NAME_d7304OutBlock     "d7304OutBlock"


#pragma pack( pop )
///////////////////////////////////////////////////////////////////////////////////////////////////
