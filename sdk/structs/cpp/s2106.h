#pragma once

///////////////////////////////////////////////////////////////////////////////////////////////////
// 주식선물투자자 시간별 동향 ( attr,block,headtype=A ) 
#pragma pack( push, 1 )

#define NAME_s2106     "s2106"

// 입력
typedef struct _s2106InBlock
{
    char    eitem               [  10];    char    _eitem               ;    // [string,   10] 기초자산                       StartPos 0, Length 10
    char    flag                [   1];    char    _flag                ;    // [string,    1] 금액수량구분(1:금액2:수량)     StartPos 11, Length 1
    char    unit                [   1];    char    _unit                ;    // [string,    1] 단위(1:백만원2:억원)           StartPos 13, Length 1
    char    jnilflag            [   1];    char    _jnilflag            ;    // [string,    1] 전일구분(1:당일2:전일)         StartPos 15, Length 1
    char    cont                [  20];    char    _cont                ;    // [string,   20] 연속처리키                     StartPos 17, Length 20
} s2106InBlock, *LPs2106InBlock;
#define NAME_s2106InBlock     "s2106InBlock"

// 출력
typedef struct _s2106OutBlock
{
    char    val00md             [  12];    char    _val00md             ;    // [string,   12] 사모매도                       StartPos 0, Length 12
    char    val00ms             [  12];    char    _val00ms             ;    // [string,   12] 사모매수                       StartPos 13, Length 12
    char    val00ss             [  12];    char    _val00ss             ;    // [string,   12] 사모순매수                     StartPos 26, Length 12
    char    val01md             [  12];    char    _val01md             ;    // [string,   12] 금투매도                       StartPos 39, Length 12
    char    val01ms             [  12];    char    _val01ms             ;    // [string,   12] 금투매수                       StartPos 52, Length 12
    char    val01ss             [  12];    char    _val01ss             ;    // [string,   12] 금투순매수                     StartPos 65, Length 12
    char    val02md             [  12];    char    _val02md             ;    // [string,   12] 보험매도                       StartPos 78, Length 12
    char    val02ms             [  12];    char    _val02ms             ;    // [string,   12] 보험매수                       StartPos 91, Length 12
    char    val02ss             [  12];    char    _val02ss             ;    // [string,   12] 보험순매수                     StartPos 104, Length 12
    char    val03md             [  12];    char    _val03md             ;    // [string,   12] 투신매도                       StartPos 117, Length 12
    char    val03ms             [  12];    char    _val03ms             ;    // [string,   12] 투신매수                       StartPos 130, Length 12
    char    val03ss             [  12];    char    _val03ss             ;    // [string,   12] 투신순매수                     StartPos 143, Length 12
    char    val04md             [  12];    char    _val04md             ;    // [string,   12] 은행매도                       StartPos 156, Length 12
    char    val04ms             [  12];    char    _val04ms             ;    // [string,   12] 은행매수                       StartPos 169, Length 12
    char    val04ss             [  12];    char    _val04ss             ;    // [string,   12] 은행순매수                     StartPos 182, Length 12
    char    val05md             [  12];    char    _val05md             ;    // [string,   12] 기타금융매도                   StartPos 195, Length 12
    char    val05ms             [  12];    char    _val05ms             ;    // [string,   12] 기타금융매수                   StartPos 208, Length 12
    char    val05ss             [  12];    char    _val05ss             ;    // [string,   12] 기타금융순매수                 StartPos 221, Length 12
    char    val06md             [  12];    char    _val06md             ;    // [string,   12] 연기금매도                     StartPos 234, Length 12
    char    val06ms             [  12];    char    _val06ms             ;    // [string,   12] 연기금매수                     StartPos 247, Length 12
    char    val06ss             [  12];    char    _val06ss             ;    // [string,   12] 연기금순매수                   StartPos 260, Length 12
    char    val07md             [  12];    char    _val07md             ;    // [string,   12] 기타매도                       StartPos 273, Length 12
    char    val07ms             [  12];    char    _val07ms             ;    // [string,   12] 기타매수                       StartPos 286, Length 12
    char    val07ss             [  12];    char    _val07ss             ;    // [string,   12] 기타순매수                     StartPos 299, Length 12
    char    val08md             [  12];    char    _val08md             ;    // [string,   12] 개인매도                       StartPos 312, Length 12
    char    val08ms             [  12];    char    _val08ms             ;    // [string,   12] 개인매수                       StartPos 325, Length 12
    char    val08ss             [  12];    char    _val08ss             ;    // [string,   12] 개인순매수                     StartPos 338, Length 12
    char    val09md             [  12];    char    _val09md             ;    // [string,   12] 외국인매도                     StartPos 351, Length 12
    char    val09ms             [  12];    char    _val09ms             ;    // [string,   12] 외국인매수                     StartPos 364, Length 12
    char    val09ss             [  12];    char    _val09ss             ;    // [string,   12] 외국인순매수                   StartPos 377, Length 12
    char    val10md             [  12];    char    _val10md             ;    // [string,   12] 기타외국인매도                 StartPos 390, Length 12
    char    val10ms             [  12];    char    _val10ms             ;    // [string,   12] 기타외국인매수                 StartPos 403, Length 12
    char    val10ss             [  12];    char    _val10ss             ;    // [string,   12] 기타외국인순매수               StartPos 416, Length 12
    char    val11md             [  12];    char    _val11md             ;    // [string,   12] 국가매도                       StartPos 429, Length 12
    char    val11ms             [  12];    char    _val11ms             ;    // [string,   12] 국가매수                       StartPos 442, Length 12
    char    val11ss             [  12];    char    _val11ss             ;    // [string,   12] 국가순매수                     StartPos 455, Length 12
    char    valOrmd             [  12];    char    _valOrmd             ;    // [string,   12] 기관매도                       StartPos 468, Length 12
    char    valOrms             [  12];    char    _valOrms             ;    // [string,   12] 기관매수                       StartPos 481, Length 12
    char    valOrss             [  12];    char    _valOrss             ;    // [string,   12] 기관순매수                     StartPos 494, Length 12
    char    val19md             [  12];    char    _val19md             ;    // [string,   12] 전체매도                       StartPos 507, Length 12
    char    val19ms             [  12];    char    _val19ms             ;    // [string,   12] 전체매수                       StartPos 520, Length 12
    char    val19ss             [  12];    char    _val19ss             ;    // [string,   12] 전체순매수                     StartPos 533, Length 12
    char    cont                [  20];    char    _cont                ;    // [string,   20] 연속처리키                     StartPos 546, Length 20
} s2106OutBlock, *LPs2106OutBlock;
#define NAME_s2106OutBlock     "s2106OutBlock"

typedef struct _s2106OutBlock1
{
    char    date                [   8];    char    _date                ;    // [string,    8] 일자                           StartPos 0, Length 8
    char    time                [   6];    char    _time                ;    // [string,    6] 시간                           StartPos 9, Length 6
    char    val00               [  12];    char    _val00               ;    // [string,   12] 사모                           StartPos 16, Length 12
    char    val01               [  12];    char    _val01               ;    // [string,   12] 금투                           StartPos 29, Length 12
    char    val02               [  12];    char    _val02               ;    // [string,   12] 보험                           StartPos 42, Length 12
    char    val03               [  12];    char    _val03               ;    // [string,   12] 투신                           StartPos 55, Length 12
    char    val04               [  12];    char    _val04               ;    // [string,   12] 은행                           StartPos 68, Length 12
    char    val05               [  12];    char    _val05               ;    // [string,   12] 기타금융                       StartPos 81, Length 12
    char    val06               [  12];    char    _val06               ;    // [string,   12] 연기금                         StartPos 94, Length 12
    char    val07               [  12];    char    _val07               ;    // [string,   12] 기타                           StartPos 107, Length 12
    char    val08               [  12];    char    _val08               ;    // [string,   12] 개인                           StartPos 120, Length 12
    char    val09               [  12];    char    _val09               ;    // [string,   12] 외국인                         StartPos 133, Length 12
    char    val10               [  12];    char    _val10               ;    // [string,   12] 기타외국인                     StartPos 146, Length 12
    char    val11               [  12];    char    _val11               ;    // [string,   12] 국가                           StartPos 159, Length 12
    char    valOr               [  12];    char    _valOr               ;    // [string,   12] 기관                           StartPos 172, Length 12
    char    val19               [  12];    char    _val19               ;    // [string,   12] 전체                           StartPos 185, Length 12
} s2106OutBlock1, *LPs2106OutBlock1;
#define NAME_s2106OutBlock1     "s2106OutBlock1"


#pragma pack( pop )
///////////////////////////////////////////////////////////////////////////////////////////////////
