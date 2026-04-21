#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>

int main(void)
{
    // 1. 초기화로 모든 원소에 0을 저장 (안전장치)
    int input[20] = { 0 }; 
    int i = 0;

    printf("배열에 저장할 정수를 여러 개 입력하시오. \n");
    printf("0을 입력하면 입력을 종료합니다.\n");

    // 2. 숫자를 입력받아 배열에 저장 (0이 들어올 때까지)
    do {
        // 배열의 i번째 칸 주소(&)에 값을 입력받음
        scanf("%d", &input[i]);
    } while (input[i++] != 0); // 입력받은 값이 0이 아니면 i를 1 늘리고 반복

    // 3. 배열에 저장된 내용 출력
    i = 0;
    while (input[i] != 0) {
        printf("%d ", input[i++]);
    }
    puts(""); // 줄바꿈

    return 0;
}