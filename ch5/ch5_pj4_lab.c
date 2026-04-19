#include <stdio.h>

int main(void)
{
    int amount;

    printf("총 금액 입력 >> ");
    scanf("%d", &amount);
    printf("계산 금액: %d\n", amount);

    // 1. 오만원권 계산
    int cnt50000 = amount / 50000;      // 몫: 지폐 개수
    int changes = amount % 50000;       // 나머지: 남은 금액
    printf("오만원권 %d개 ", cnt50000);

    // 2. 만원권 계산 (첫 번째 빈칸)
    int cnt10000 = changes / 10000;     // 남은 돈에서 만원권 개수 추출
    changes %= 10000;                   // 만원권 제외한 나머지 갱신
    printf("만원권 %d개 ", cnt10000);

    // 3. 오천원권 계산 (두 번째 빈칸 포함)
    int cnt5000 = changes / 5000;       // 남은 돈에서 오천원권 개수 추출
    changes %= 5000;                    // 오천원권 제외한 나머지 갱신
    printf("오천원권 %d개 ", cnt5000);

    // 4. 최종 나머지 출력
    printf("나머지 %d원\n", changes);

    return 0;
}