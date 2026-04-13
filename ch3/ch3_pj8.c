#include <stdio.h>

int main(void)
{
    printf("Cava%c", '\a'); // \a 는경고음 소리 출력, '' 필수

    char sq = '\\';  // 작은따옴표 출력
    printf("%c\7\n", '\a');  // 알람문자 2번 출력
    printf("%c자바 언어\'\n", sq); 

    printf("\"C언어\" 정말 재미있다!\n");  // 이스케이프 필요
}