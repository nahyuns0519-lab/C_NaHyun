#define _CRT_SECURE_NO_WARNING
#include <stdio.h>

int main(void)
{
        int year, month, day;
        printf("당신의 생년월일은? ");
        scanf("%d - %d - %d", &year, &month, &day);
        printf("생년월일: %d %d %d\n", year, month, day);

        return 0;
}