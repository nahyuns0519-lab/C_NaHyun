#define _CRT_SECURE_NO_WARNING
#include <stdio.h>

int main(void)
{
    int hex;
    printf("십진수 정수(1A 등)을 입력하세요 >> ");
    scanf("%x", &hex); // 16진수로 무조건 인식
    printf("%o %d %x\n\n", hex, hex, hex);

    printf("십진수 정수(0리딩 표시방식으로 0x1a 등)을 입력하세요 >> ");
    scanf("%i", &hex);
    printf("%o %d %#x\n\n", hex, hex, hex);

    return 0;
}