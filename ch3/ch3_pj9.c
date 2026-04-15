#include <stdio.h>

int main(void)
{
    printf("%d, %d\n", 010, 015); // 8진수를 10진수로 출력
    printf("%d, %d\n", 10, 15); // 10진수
    printf("%d, %d\n", 0x1a, 0x15); // 16진수를 10진수로 출력

    printf("%f", 2.71828); // 소수점 6자리로 출력
    printf("%f", 2.71828E+2); // 지수 10^2 하여 출력
    printf("%f", 2.71828e-2); // 지수 10^-2 하여 출력

    return 0;
}