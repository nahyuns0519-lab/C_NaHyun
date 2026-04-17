#include <stdio.h>

int main(void)
{
    float mile = 0;
    printf("100 킬로미터(km)는 몇 마일?(mile) ");
    scanf("%f", &mile); //%f로 float형
    printf("80 킬로미터: %.2f 마일\n\n", mile * 80.);
    
    double liter = 0;
    printf("1 갤론(gallon)은 몇 리터(liter)? ");
    scanf("%lf", &liter); // %lf로 double형 필수
    printf("18 갤론: %.2f 리터\n", liter * 18);

    return 0;
}