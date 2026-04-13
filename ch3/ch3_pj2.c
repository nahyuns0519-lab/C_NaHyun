#include <stdio.h>

int main(void)
{
    int year = 2022; // 선언과 동시 변수 초기화

    int credits; // 선언: 공간확보
    credits = 15; // 대입: 값 저장 , 선언된 변수에 초기화

    printf("%d년도\n", year);  // %d는 정수 데이터로 출력해라
    printf("이수학점: %d학점\n", credits);

    return 0;
}