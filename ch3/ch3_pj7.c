#include <stdio.h>

int main(void)
{
    printf("자료형: 크기(바이트)\n");
    printf("char : %d\n", sizeof(char)); // sizeof()는 메모리 크기를 바이트 단위로 반환
    printf("int: %d %d\n", sizeof(int), sizeof(200)); // int와 숫자 200 모두 4byte
    printf("long long: %d %d\n", sizeof(long long), sizeof(900LL)); // 아주 큰 정수 8byte
    printf("float: %d %d\n", sizeof(float), sizeof(3.14F)); // 둘다 4byte
    printf("long double: %d %d\n", sizeof(long double), sizeof(3.24L)); // 8byte
    
    short s = 32767; // short는 2byte(=16bit) 이니 가장 큰 양수 2^15 - 1 = 32767 인 값
    printf("%d\n", s);
    s = s + 1;  // 이미 다 찼는데 1을 더 더해서 오버플로 발생
    printf("%d\n", s); // Max + 1 = Min 범위 초과시 오버플로 발생

    return 0;
}