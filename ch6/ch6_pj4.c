#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>

int main(void)
{
    int type, point;

    printf("번호를 선택: 1(1종면허), 2(2종면허): ");
    scanf("%d", &type);
    printf("필기시험 점수 입력: ");
    scanf("%d", &point);

    if (type == 1) // 1종 면허를 선택했을 때 (Outer if)
    {
        if (point >= 70) // 1종 합격 기준 (Inner if)
            printf("1종면허 합격\n");
        else
            printf("1종면허 불합격\n");
    }
    else if (type == 2) // 2종 면허를 선택했을 때
    {
        if (point >= 60) // 2종 합격 기준 (Inner if)
            printf("2종면허 합격\n");
        else
            printf("2종면허 불합격\n");
    }

    return 0;
}