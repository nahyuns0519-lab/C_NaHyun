#include <stdio.h>
#define SIZE 6  // 배열의 크기를 상수로 선언 (수정이 편해요)

int main(void)
{
    // 1. 배열 선언과 동시에 초기화 (나누어 할 수 없음!)
    double score[] = { 89.3, 79.2, 84.83, 76.8, 92.52, 97.4 };
    double sum = 0;

    // 2. for 문을 이용하여 배열 순회하며 합계 구하기
    // i는 0부터 5까지 총 6번 반복됩니다.
    for (int i = 0; i < SIZE; i++) // 0부터 시작해, 개수보다 작을때, 다음칸으로 넘어가라
    {
        sum += score[i]; // 각 방의 값을 sum에 누적
        printf("score[%d] = %.2f\n", i, score[i]);
    }

    // 3. 결과 출력 (평균 계산 포함)
    printf("성적의 합은 %.2f이고 평균은 %.2f이다.\n", sum, sum / SIZE);

    return 0;
}