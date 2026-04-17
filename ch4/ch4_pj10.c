#include <stdio.h>

int main(void)
{
    char abc, plus;
    printf("원하는 학점(A+)을 입력 >> ");
    abc = getchar(); // 한 문자를 집어냄
    
    scanf("%c", &plus); // 남은 문자를 연어어 집어냄
    printf("학점 %c%c\n", abc, plus);

    return 0;
}