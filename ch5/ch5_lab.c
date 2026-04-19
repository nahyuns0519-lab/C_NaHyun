#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>

int main(void)
{
    double celsius, fahrenheit;
    printf("변환할 섭씨 온도를 입력 >> ");
    scanf("%lf", &celsius);

    // 정답 포인트: 9.0 / 5.0을 사용해야 함
    fahrenheit = (9.0 / 5.0) * celsius + 32.0;

    printf("섭씨 %.2f: 화씨 %.2f\n", celsius, fahrenheit);

    return 0;
}

// 섭씨를 화씨로 변환하기