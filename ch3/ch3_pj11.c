#include <stdio.h>

int main(void)
{
    enum DAY { SUN, MON, TUE, WED, THU, FRI, SAT}; // 0부터 순차적 증가
    printf("%d %d\n", SUN, THU);

    enum SHAPE { POINT, LINE, TRI = 3, RECT, OCTA = 8, CIRCLE }; // 특정 값 지정 가능
    printf("LINE: %d, RECT: %d\n", LINE, RECT);

    return 0;
}