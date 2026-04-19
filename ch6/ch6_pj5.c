#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>

int main(void)
{
    int age = 0;

    printf("당신의 나이는? ");
    scanf("%d", &age);

    if (age >= 20) // 20세 이상일 때 (성인 범주)
    {
        if (age >= 65) // 그중에서도 65세 이상일 때
            printf("당신은 어르신입니다.\n");
        else // 20세 이상 65세 미만일 때
            printf("당신은 성인입니다.\n");
    }
    else // 20세 미만일 때
    {
        printf("당신은 미성년자입니다.\n");
    }

    return 0;
}