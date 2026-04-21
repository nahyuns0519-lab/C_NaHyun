#include <stdio.h>

// 매크로 상수를 이용해 배열의 크기를 정의합니다.
#define SIZE 5

int main(void) {
    // 1. 배열 선언 (int형 상자 5개를 만듦)
    int score[SIZE]; // 실제로는 int score[5]; 와 같습니다.

    // 2. 배열 원소에 값 저장 (인덱스 사용)
    score[0] = 78; 
    score[1] = 97;
    score[2] = 85;
    
    // score[3]은 의도적으로 비워둡니다 -> 쓰레기 값이 출력될 자리입니다.
    
    score[4] = 91;

    // [주의] score[5] = 50; 처럼 존재하지 않는 인덱스에 접근하면
    // '인덱스 범위 초과(Out of Bounds)' 오류가 발생하므로 주의해야 합니다.

    // 3. 배열 원소 출력 (for 문 활용)
    printf("배열 원소 출력: ");
    for (int i = 0; i < SIZE; i++) {
        printf("%d ", score[i]);
    }
    printf("\n");

    return 0;
}