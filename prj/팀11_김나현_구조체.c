#include <stdio.h>

#define CAPACITY 15     /* 최대 수용 가능 차량 수 */

/*----- 전체 주차장 상태를 하나로 묶는 구조체 -----*/
typedef struct {
    int count;      /* 현재 차량 수 */
    int capacity;   /* 최대 수용 가능 차량 수 */
    int barrier;    /* 차단기 상태  1=개방, 0=폐쇄 */
    int full;       /* 만차 상태    1=만차, 0=여유 */
} ParkingLot;

int main(void) {
    /* 구조체 변수 선언 + 초기화 (중괄호로 순서대로) */
    /* 이미 14대 주차된 상태, 최대 15대 */
    ParkingLot lot = { 14, CAPACITY, 0, 0 };

    /* --- 차 한 대 입차 → 멤버 접근(.)으로 상태 갱신 --- */
    lot.count   = lot.count + 1;                /* 현재 차량 수 +1 */
    lot.barrier = 1;                            /* 차단기 개방 */
    lot.full    = (lot.count >= lot.capacity);  /* 만차 여부 판단 */

    /* --- 구조체에 저장된 상태 출력 --- */
    printf("========= 주차장 상태 =========\n");
    printf(" 현재 차량 수   : %d대\n", lot.count);
    printf(" 최대 수용 대수 : %d대\n", lot.capacity);
    printf(" 남은 자리      : %d대\n", lot.capacity - lot.count);
    printf(" 차단기 상태    : %s\n", lot.barrier ? "개방" : "폐쇄");
    printf(" 만차 여부      : %s\n", lot.full ? "만차" : "여유");
    printf("===============================\n");

    return 0;
}