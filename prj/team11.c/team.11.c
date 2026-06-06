/*===========================================================
 *  자동 주차장 출입 및 주차공간 제어 시스템
 *  - 비트 마스킹 / 배열(1D,2D) / 구조체 / 함수 모듈화
 *  - 포인터 / 파일 입출력(File I/O) 모두 포함
 *===========================================================*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* ===== [1] 비트 마스킹: 센서/차단기/상태를 각 1비트로 관리 ===== */
#define ST_ENTRY    (1u << 0)   /* bit0: 입구 센서 감지   */
#define ST_EXIT     (1u << 1)   /* bit1: 출구 센서 감지   */
#define ST_BARRIER  (1u << 2)   /* bit2: 차단기 개방(1)   */
#define ST_FULL     (1u << 3)   /* bit3: 만차 여부        */
#define ST_FIRE     (1u << 4)   /* bit4: 화재 감지        */

/* ===== 상수 정의 ===== */
#define ZONES     3                  /* A,B,C 구역        */
#define SLOTS     5                  /* 구역당 자리 수    */
#define CAPACITY  (ZONES * SLOTS)    /* 총 수용 대수(15)  */
#define UNIT_MIN  10                 /* 과금 단위(분)     */
#define UNIT_FEE  500                /* 단위당 요금       */
#define BASE_FEE  1000               /* 기본요금          */
#define LOG_FILE  "parking_log.txt"

/* 등록 차량 목록 (1D 배열) */
const char *REGISTERED[] = {"12가3456", "34나5678", "56다7890", "78라1234"};
const int   REG_COUNT = sizeof(REGISTERED) / sizeof(REGISTERED[0]);

/* ===== [2] 구조체: 차량 정보 & 주차장 전체 상태 ===== */
typedef struct {
    char   plate[16];   /* 차량번호           */
    int    zone, slot;  /* 배정된 구역/자리   */
    time_t in;          /* 입차 시각          */
    int    used;        /* 사용 중 여부       */
} Vehicle;

typedef struct {
    int           count;              /* 현재 주차 대수             */
    int           capacity;           /* 최대 수용 대수             */
    unsigned char status;             /* 비트 상태 플래그           */
    int           slots[ZONES][SLOTS];/* 2D 배열: 0=빈자리,1=점유   */
    Vehicle       cars[CAPACITY];     /* 주차 차량 정보             */
} ParkingLot;

/*-----------------------------------------------------------
 *  초기화 함수 (포인터로 구조체 직접 수정)
 *----------------------------------------------------------*/
void initLot(ParkingLot *lot) {
    lot->count = 0;
    lot->capacity = CAPACITY;
    lot->status = 0;                  /* 모든 상태 비트 OFF */
    for (int z = 0; z < ZONES; z++)
        for (int s = 0; s < SLOTS; s++)
            lot->slots[z][s] = 0;
    for (int i = 0; i < CAPACITY; i++)
        lot->cars[i].used = 0;
}

/* ===== [6] 파일 입출력: 로그 파일 헤더 생성 ===== */
void initLogFile(void) {
    FILE *fp = fopen(LOG_FILE, "r");
    if (fp) { fclose(fp); return; }   /* 이미 존재하면 유지 */
    fp = fopen(LOG_FILE, "w");
    if (fp) {
        fprintf(fp, "%-12s %-10s %-10s %-10s %8s %9s  %s\n",
                "날짜", "차량번호", "입차시간", "출차시간",
                "주차(분)", "정산요금", "상태");
        fprintf(fp, "----------------------------------------"
                    "----------------------------------------\n");
        fclose(fp);
    }
}

/* 등록 차량 여부 확인 */
int isRegistered(const char *plate) {
    for (int i = 0; i < REG_COUNT; i++)
        if (strcmp(plate, REGISTERED[i]) == 0) return 1;
    return 0;
}

/* ===== [3] 함수 모듈화: 차단기 제어 (비트 ON/OFF) ===== */
void setBarrier(ParkingLot *lot, int open) {
    if (open) lot->status |= ST_BARRIER;    /* 해당 비트 ON  */
    else      lot->status &= ~ST_BARRIER;   /* 해당 비트 OFF */
    printf(">> 차단기 %s\n", open ? "개방" : "폐쇄");
}

/* 최적 구역 탐색: 빈자리가 가장 많은 구역을 우선 안내 */
int findBestSlot(ParkingLot *lot, int *zone, int *slot) {
    int bestZone = -1, maxFree = -1;
    for (int z = 0; z < ZONES; z++) {       /* 구역별 빈자리 수 집계 */
        int freeN = 0;
        for (int s = 0; s < SLOTS; s++)
            if (lot->slots[z][s] == 0) freeN++;
        if (freeN > maxFree) { maxFree = freeN; bestZone = z; }
    }
    if (maxFree <= 0) return 0;             /* 빈자리 없음 */
    for (int s = 0; s < SLOTS; s++)
        if (lot->slots[bestZone][s] == 0) {
            *zone = bestZone; *slot = s; return 1;
        }
    return 0;
}

/* 요금 계산: 기본요금 + 단위시간 초과분(올림) */
int calcFee(int minutes) {
    int fee = BASE_FEE;
    if (minutes > UNIT_MIN) {
        /* (초과분)을 UNIT_MIN 단위로 올림 처리 */
        int extra = (minutes - UNIT_MIN + UNIT_MIN - 1) / UNIT_MIN;
        fee += extra * UNIT_FEE;
    }
    return fee;
}

/* ===== [6] 파일 입출력: 출차 기록 한 줄 추가(append) ===== */
void saveLog(const char *plate, time_t in, time_t out, int minutes, int fee) {
    FILE *fp = fopen(LOG_FILE, "a");        /* 추가 모드 */
    if (!fp) { printf("로그 파일 열기 실패\n"); return; }

    char dbuf[32], ibuf[16], obuf[16];
    struct tm *t;
    t = localtime(&in);
    strftime(dbuf, sizeof(dbuf), "%Y-%m-%d", t);
    strftime(ibuf, sizeof(ibuf), "%H:%M:%S", t);
    t = localtime(&out);
    strftime(obuf, sizeof(obuf), "%H:%M:%S", t);

    fprintf(fp, "%-12s %-10s %-10s %-10s %6d분  %8d원  %s\n",
            dbuf, plate, ibuf, obuf, minutes, fee, "출차완료");
    fclose(fp);
}

/* ===== [4] 입차 처리: 포인터로 상태 직접 수정 ===== */
void enterCar(ParkingLot *lot, const char *plate) {
    lot->status |= ST_ENTRY;                /* 입구 센서 ON */

    if (lot->count >= lot->capacity) {      /* 만차 검사 */
        lot->status |= ST_FULL;
        printf(">> 만차입니다. 입차 불가\n");
        setBarrier(lot, 0);
        lot->status &= ~ST_ENTRY;
        return;
    }
    if (!isRegistered(plate)) {             /* 보안: 미등록 차량 경고 */
        printf(">> [경고] 미등록 차량(%s) 진입 시도! 차단기 폐쇄\n", plate);
        setBarrier(lot, 0);
        lot->status &= ~ST_ENTRY;
        return;
    }

    int z, s;
    if (!findBestSlot(lot, &z, &s)) {       /* 최적 구역 배정 */
        printf(">> 배정 가능한 자리가 없습니다\n");
        lot->status &= ~ST_ENTRY;
        return;
    }

    Vehicle *v = &lot->cars[lot->count];    /* 차량 정보 등록 */
    strncpy(v->plate, plate, sizeof(v->plate) - 1);
    v->plate[sizeof(v->plate) - 1] = '\0';
    v->zone = z; v->slot = s;
    v->in   = time(NULL);
    v->used = 1;

    lot->slots[z][s] = 1;                   /* 자리 점유 표시 */
    lot->count++;

    setBarrier(lot, 1);                     /* 등록 차량 -> 개방 */
    printf(">> 입차 완료: %s -> %c구역 %d번\n", plate, 'A' + z, s + 1);

    if (lot->count >= lot->capacity) lot->status |= ST_FULL;
    lot->status &= ~ST_ENTRY;
}

/* ===== [4] 출차 처리: 차량 검색 -> 요금 정산 -> 로그 저장 ===== */
void exitCar(ParkingLot *lot, const char *plate) {
    lot->status |= ST_EXIT;                 /* 출구 센서 ON */

    int idx = -1;
    for (int i = 0; i < lot->count; i++)    /* 차량 검색 */
        if (lot->cars[i].used && strcmp(lot->cars[i].plate, plate) == 0) {
            idx = i; break;
        }
    if (idx < 0) {
        printf(">> 해당 차량(%s)을 찾을 수 없습니다\n", plate);
        lot->status &= ~ST_EXIT;
        return;
    }

    Vehicle *v = &lot->cars[idx];
    time_t out = time(NULL);
    int minutes = (int)(difftime(out, v->in) / 60.0);
    if (minutes < 1) minutes = 1;           /* 최소 1분 과금 */
    int fee = calcFee(minutes);

    /* ----- 정산 영수증 출력 ----- */
    printf("\n========== 정산 영수증 ==========\n");
    printf(" 차량번호 : %s\n", v->plate);
    printf(" 주차구역 : %c구역 %d번\n", 'A' + v->zone, v->slot + 1);
    printf(" 주차시간 : %d분\n", minutes);
    printf(" 정산요금 : %d원\n", fee);
    printf("=================================\n\n");

    saveLog(v->plate, v->in, out, minutes, fee);   /* 파일 기록 */

    /* 상태 갱신: 자리 해제 + 배열 압축(마지막 차량을 빈칸으로 이동) */
    lot->slots[v->zone][v->slot] = 0;
    lot->cars[idx] = lot->cars[lot->count - 1];
    lot->cars[lot->count - 1].used = 0;
    lot->count--;
    lot->status &= ~ST_FULL;                /* 만차 해제 */

    setBarrier(lot, 1);
    lot->status &= ~ST_EXIT;
}

/* ===== 비상 모드: 화재 감지 시 차단기 강제 개방 ===== */
void emergencyMode(ParkingLot *lot) {
    lot->status |= ST_FIRE;                 /* 화재 비트 ON */
    printf("\n!!!!! 화재 감지 - 비상 모드 !!!!!\n");
    setBarrier(lot, 1);                     /* 강제 개방 */
    printf(">> 전 차량 대피 안내, 차단기 강제 개방\n\n");
}

/* ===== 현황 출력 ===== */
void printStatus(ParkingLot *lot) {
    printf("\n========= 주차장 현황 =========\n");
    printf(" 주차 가능 대수 : %d / %d\n", lot->capacity - lot->count, lot->capacity);
    printf(" 차단기 상태    : %s\n", (lot->status & ST_BARRIER) ? "개방" : "폐쇄");
    printf(" 만차 여부      : %s\n", (lot->status & ST_FULL)    ? "만차" : "여유");
    printf(" 화재 경보      : %s\n", (lot->status & ST_FIRE)    ? "발생" : "정상");
    printf(" 구역별 점유    : (O=빈자리, X=점유)\n");
    for (int z = 0; z < ZONES; z++) {       /* 2D 배열 순회 출력 */
        printf("   %c구역: ", 'A' + z);
        for (int s = 0; s < SLOTS; s++)
            printf("[%c]", lot->slots[z][s] ? 'X' : 'O');
        printf("\n");
    }
    printf("===============================\n");
}

/*===========================================================
 *  main: 메뉴 기반 콘솔 인터페이스
 *===========================================================*/
int main(void) {
    ParkingLot lot;
    initLot(&lot);          /* 포인터 전달로 초기화 */
    initLogFile();

    int  menu;
    char plate[16];

    printf("== 자동 주차장 출입/공간 제어 시스템 ==\n");
    printf("등록 차량: ");
    for (int i = 0; i < REG_COUNT; i++) printf("%s ", REGISTERED[i]);
    printf("\n");

    while (1) {
        printf("\n[1]입차 [2]출차 [3]현황 [4]화재비상 [0]종료\n선택: ");
        if (scanf("%d", &menu) != 1) break;

        if (menu == 0) { printf("시스템 종료\n"); break; }
        else if (menu == 1) {
            printf("차량번호 입력: ");
            scanf("%15s", plate);
            enterCar(&lot, plate);
        } else if (menu == 2) {
            printf("차량번호 입력: ");
            scanf("%15s", plate);
            exitCar(&lot, plate);
        } else if (menu == 3) {
            printStatus(&lot);
        } else if (menu == 4) {
            emergencyMode(&lot);
        } else {
            printf("잘못된 선택\n");
        }
    }
    return 0;
}