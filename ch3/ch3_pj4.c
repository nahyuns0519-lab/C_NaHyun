#include <stdio.h>

int main(void)
{
    short sVar = 32000; // -32,767 ~ 32,767까지 저장가능
    int iVar = -2140000000; // int는 21억정도 저장가능

    printf("저장 값: %d %d\n", sVar, iVar);

    long long dist1 = 2720000000000; // long long은 64비트 정수(664경) 저장가능
    long long dist2 = 4500000000000;

    printf("지구와 천왕성 간의 거리(km): %lld\n", dist1); // 출력 시 형식지정자 %lld 필수
    printf("지구와 해왕성 간의 거리(km): %lld\n", dist2);

    return 0;
}