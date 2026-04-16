#include <stdio.h>

int main(void)
{
    printf("%d * %4d = %#5o\n", 2, 2, 2 * 2);  
    printf("%d * %04d = %#5o\n", 2, 3, 2 * 3);
    printf("%d * %+04d = %-#5x\n", 2, 4, 2 * 4);
    printf("%d * %+-4d = %+#5x\n", 2, 5, 2 * 5);

    printf("%15.3f\n", 123456.789); // 전체 15자리 확보, 소수점 3자리까지 표시
    printf("%e\n", 123456.789); // 지수표현
    printf("%g\n", 12.34e-5); // %f나 %e 중 간결한 방식을 자동선택
    printf("%G\n", 12.34e-6); // 지수표현 사용시 g는 e, G는 E

    return 0;
}