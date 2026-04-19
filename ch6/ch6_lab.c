#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>

int main(void)
{
    // 열거형 선언: TRIANGLE은 1, RECTANGLE은 자동으로 2가 됨
    enum shape { TRIANGLE = 1, RECTANGLE };
    int input;
    double width, height;

    printf("삼각형[1], 사각형[2] 중의 번호 하나를 선택 >> ");
    scanf("%d", &input);
    printf("가로와 세로를 입력(실수 입력) >> ");
    scanf("%lf %lf", &width, &height);
    printf("가로: %.2lf 세로: %.2lf\n", width, height);

    switch (input)
    {
    case TRIANGLE: // 숫자 1 대신 의미 있는 이름을 사용
        printf("삼각형 면적: %.2lf\n", width * height / 2);
        break;
    case RECTANGLE: // 숫자 2 대신 의미 있는 이름을 사용
        printf("사각형 면적: %.2lf\n", width * height);
        break;
    default:
        printf("잘못된 입력\n");
    }

    return 0;
}