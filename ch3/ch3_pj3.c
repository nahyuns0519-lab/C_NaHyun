#include <stdio.h>

int main(void)
{
    int data1 = 20, data2 = 13; // l-value: 변수 자체 (저장공간)
    int diff = data1 - data2; // r-value: 변수 저장값 (연산)
    int sum = data1 + data2;
    printf("data1: %d, data2: %d\n", data1, data2);
    printf("차: %d, 합: %d\n", diff, sum);

    return 0;
}