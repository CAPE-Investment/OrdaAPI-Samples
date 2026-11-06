#pragma once

///////////////////////////////////////////////////////////////////////////////////////////////////
// 계좌비밀번호 검증 ( headtype=B ) 
#pragma pack( push, 1 )

#define NAME_b7423     "b7423"

// 입력
typedef struct _b7423InBlock
{
    char    reccnt              [   5];    // [string,    5] 레코드갯수                     StartPos 0, Length 5
    char    AcntNo              [  20];    // [string,   20] 계좌번호                       StartPos 5, Length 20
    char    AcntClssTp          [   1];    // [string,    1] 계좌계층구분                   StartPos 25, Length 1
    char    InptPwd             [   8];    // [string,    8] 입력비밀번호                   StartPos 26, Length 8
} b7423InBlock, *LPb7423InBlock;
#define NAME_b7423InBlock     "b7423InBlock"

// 출력
typedef struct _b7423OutBlock
{
    char    reccnt              [   5];    // [string,    5] 레코드갯수                     StartPos 0, Length 5
    char    AcntNo              [  20];    // [string,   20] 계좌번호                       StartPos 5, Length 20
    char    AcntClssTp          [   1];    // [string,    1] 계좌계층구분                   StartPos 25, Length 1
    char    InptPwd             [   8];    // [string,    8] 입력비밀번호                   StartPos 26, Length 8
} b7423OutBlock, *LPb7423OutBlock;
#define NAME_b7423OutBlock     "b7423OutBlock"

typedef struct _b7423OutBlock1
{
    char    reccnt              [   5];    // [string,    5] 레코드갯수                     StartPos 0, Length 5
    char    AcntNm              [  40];    // [string,   40] 계좌명                         StartPos 5, Length 40
    char    AvalYn              [   1];    // [string,    1] 유효여부                       StartPos 45, Length 1
    char    CintNo              [  20];    // [string,   20] 고객번호                       StartPos 46, Length 20
    char    AppCnt              [   6];    // [string,    6] 적용횟수                       StartPos 66, Length 6
    char    PwdContErrCnt       [  10];    // [string,   10] 비밀번호연속오류횟수           StartPos 72, Length 10
} b7423OutBlock1, *LPb7423OutBlock1;
#define NAME_b7423OutBlock1     "b7423OutBlock1"


#pragma pack( pop )
///////////////////////////////////////////////////////////////////////////////////////////////////

