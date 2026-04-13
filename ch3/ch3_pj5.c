#include <stdio.h>

int main(void)
{
    float x = 3.14F; // float형 상수에는 반드시 F나 f를 붙이기, 아님 경고발생
    double y = -3.141592; // double은 float의 저장공간 2배
    long double z = 29.74; // double과 long double은 저장공간 모두 64비트

    printf("부등 소수 값: %f %f %f\n", x, y, z); // %f는 기본 소수점 6자리로 출력

    return 0;
}