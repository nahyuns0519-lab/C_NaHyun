#include <stdio.h>

int main(void)
{
    printf("    1234567890\n");
    int year = 1999;
    printf("C99 %8d\n", year); // 8칸확보
    printf("C99 %+8d\n", year); //8칸확보, 오른쪽 정렬
    printf("C99 %-8d\n", year); // 8칸확보, 왼쪽 정렬
    printf("C99 %-+8d\n\n", year); // 8칸확보, 왼쪽정렬, 부호+확보, 2줄바꿈

    float f = 3.141592F;
    printf("    1234567890\n");
    printf("PI= %10f\n", f); // 10칸확보
    printf("PI= %10.3f\n", f); // 10칸확보, 소수점3째자리
    printf("PI= %.3f\n", f); // 소수점 3째자리까지
    printf("PI= %-10.4f\n", f); // 10칸 확보, 왼쪽정렬, 소수점 4째자리까지

    return 0;
}