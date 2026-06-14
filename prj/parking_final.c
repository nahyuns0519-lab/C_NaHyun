/*=============================================================================
 * [통합] 자동 주차장 출입 및 주차공간 제어 시스템 (수정본)
 *=============================================================================*/
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define EVENT_FILE  "C:\\parking\\parking_event.txt"

/* ===== [1] 비트 마스킹 플래그 정의 ===== */
#define ST_ENTRY    (1u << 0)   /* bit0: 입구 센서 감지   */
#define ST_EXIT     (1u << 1)   /* bit1: 출구 센서 감지   */
#define ST_BARRIER  (1u << 2)   /* bit2: 차단기 개방(1)   */
#define ST_FULL     (1u << 3)   /* bit3: 만차 여부        */
#define ST_FIRE     (1u << 4)   /* bit4: 화재 감지        */

/* ===== 상수 정의 ===== */
#define ZONES       3           /* A, B, C 구역           */
#define SLOTS       5           /* 구역당 자리 수         */
#define CAPACITY    (ZONES * SLOTS) /* 총 수용 대수 (15)  */
#define UNIT_MIN    10          /* 과금 단위(분)          */
#define UNIT_FEE    500         /* 단위당 요금            */
#define BASE_FEE    1000        /* 기본 요금              */

#define LOG_FILE    "parking_log.txt"      /* 출차/정산 기록   */
#define EVENT_FILE  "parking_event.txt"    /* 차단기/화재 로그 */
#define STATUS_FILE "parking_status.txt"   /* 현재 실시간 현황 */

/* 등록 차량 데이터베이스 (1D 배열) */
const char *REGISTERED[] = {"12가3456", "34나5678", "56다7890", "78라1234"};
const int REG_COUNT = sizeof(REGISTERED) / sizeof(REGISTERED[0]);

/* ===== [2] 구조체 정의 ===== */
typedef struct {
    char   plate[16];   /* 차량번호           */
    int    zone, slot;  /* 배정된 구역/자리   */
    time_t in;          /* 입차 시각          */
    int    used;        /* 사용 중 여부       */
} Vehicle;

typedef struct {
    int           count;                                /* 현재 주차 대수                  */
    int           capacity;                             /* 최대 수용 대수                  */
    unsigned char status;                               /* 비트 상태 플래그                */
    int           slots[ZONES][SLOTS];  /* 2D 배열: 0=빈자리, 1=점유       */
    Vehicle       cars[CAPACITY];       /* 주차 차량 정보 구조체 배열     */
} ParkingLot;

/* ===== [3] 시간 변환 헬퍼 함수 ===== */
void fmtTime(time_t t, char* date, char* hm) {
    struct tm *tmv = localtime(&t);
    if (tmv) {
        strftime(date, 12, "%Y-%m-%d", tmv);
        strftime(hm, 8, "%H:%M", tmv);
    }
}

/* ===== [4] 파일 입출력 (File I/O) 모듈 ===== */

void logEvent(const char* event) {
    FILE* f = fopen(EVENT_FILE, "a");
    if (!f) return;
    
    char date[12], hm[8];
    fmtTime(time(NULL), date, hm);
    fprintf(f, "[%s %s] %s\n", date, hm, event);
    fclose(f);
}

void saveStatusFile(ParkingLot lot) {
    FILE* f = fopen(STATUS_FILE, "w");
    if (!f) return;

    char date[12], hm[8];
    fmtTime(time(NULL), date, hm);

    fprintf(f, "========== 실시간 주차장 현황 ==========\n");
    fprintf(f, "갱신 시각 : %s %s\n", date, hm);
    fprintf(f, "현재 차량 : %d대\n", lot.count);
    fprintf(f, "최대 수용 : %d대\n", lot.capacity);
    fprintf(f, "남은 자리 : %d대\n", lot.capacity - lot.count);
    fprintf(f, "차단기    : %s\n", (lot.status & ST_BARRIER) ? "개방" : "폐쇄");
    fprintf(f, "만차 여부 : %s\n", (lot.status & ST_FULL) ? "만차" : "여유");
    fprintf(f, "화재 경보 : %s\n", (lot.status & ST_FIRE) ? "발생" : "정상");
    fclose(f);
}

void writeParkingLog(Vehicle car, time_t out, int fee, const char* statusStr) {
    FILE* f;
    int isNew = 0;
    char inDate[12], inHM[8], outDate[12], outHM[8];

    fmtTime(car.in, inDate, inHM);
    fmtTime(out, outDate, outHM);
    int parkMin = (int)((out - car.in) / 60);
    if (parkMin < 1) parkMin = 1;

    if ((f = fopen(LOG_FILE, "r")) == NULL) isNew = 1;
    else fclose(f);

    f = fopen(LOG_FILE, "a");
    if (!f) {
        printf("로그 파일을 열 수 없습니다.\n");
        return;
    }

    if (isNew) {
        fprintf(f, "%-12s%-12s%-8s%-8s%-10s%-8s%-8s\n",
                "날짜", "차량번호", "입차", "출차", "주차(분)", "요금", "상태");
    }

    fprintf(f, "%-12s%-12s%-8s%-8s%-10d%-8d%-8s\n",
            inDate, car.plate, inHM, outHM, parkMin, fee, statusStr);
    fclose(f);
}

/* * [수정] 미출차 차량(현재 주차 중) 정보를 함께 모니터링할 수 있도록 
 * ParkingLot 구조체 포인터를 매개변수로 받도록 변경했습니다.
 */
void printParkingLog(ParkingLot *lot) {
    FILE* f = fopen(LOG_FILE, "r");
    char line[128];

    printf("\n===== [파일조회] 출차 완료 기록 로그 =====\n");
    if (!f) {
        printf(">> 아직 누적된 기출차 기록(파일)이 없습니다.\n");
    } else {
        while (fgets(line, sizeof(line), f) != NULL) {
            printf("%s", line);
        }
        fclose(f);
    }
    printf("=======================================\n");

    /* 추가 요구사항 반영: 현재 주차 중인 차량 목록(미출차) 별도 출력 */
    printf("\n===== [실시간조회] 현재 주차 중인 차량 (미출차) =====\n");
    printf("%-12s%-8s%-10s\n", "차량번호", "입차시간", "주차위치");
    int currentCars = 0;
    for (int i = 0; i < CAPACITY; i++) {
        if (lot->cars[i].used) {
            char inDate[12], inHM[8];
            fmtTime(lot->cars[i].in, inDate, inHM);
            printf("%-12s%-8s%c구역 %d번\n", 
                   lot->cars[i].plate, inHM, 'A' + lot->cars[i].zone, lot->cars[i].slot + 1);
            currentCars++;
        }
    }
    if (currentCars == 0) {
        printf(">> 현재 주차장에 주차된 차량이 없습니다.\n");
    }
    printf("====================================================\n");
}

/* ===== [5] 주차장 제어 핵심 함수 모듈화 ===== */

void initLot(ParkingLot *lot) {
    lot->count = 0;
    lot->capacity = CAPACITY;
    lot->status = 0;

    for (int z = 0; z < ZONES; z++)
        for (int s = 0; s < SLOTS; s++)
            lot->slots[z][s] = 0;

    for (int i = 0; i < CAPACITY; i++)
        lot->cars[i].used = 0;
        
    saveStatusFile(*lot);
}

int isRegistered(const char *plate) {
    for (int i = 0; i < REG_COUNT; i++) {
        if (strcmp(plate, REGISTERED[i]) == 0) return 1;
    }
    return 0;
}

int calcFee(int minutes) {
    int fee = BASE_FEE;
    if (minutes > UNIT_MIN) {
        int extra = (minutes - UNIT_MIN + UNIT_MIN - 1) / UNIT_MIN;
        fee += extra * UNIT_FEE;
    }
    return fee;
}

void setBarrier(ParkingLot *lot, int open) {
    if (open) {
        lot->status |= ST_BARRIER;
        logEvent("차단기 OPEN 동작");
        printf(">> 차단기 OPEN\n");
    } else {
        lot->status &= ~ST_BARRIER;
        logEvent("차단기 CLOSE 동작");
        printf(">> 차단기 CLOSE\n");
    }
}

int findBestSlot(ParkingLot *lot, int *zone, int *slot) {
    int bestZone = -1, maxFree = -1;

    for (int z = 0; z < ZONES; z++) {
        int freeN = 0;
        for (int s = 0; s < SLOTS; s++) {
            if (lot->slots[z][s] == 0) freeN++;
        }
        if (freeN > maxFree) {
            maxFree = freeN;
            bestZone = z;
        }
    }
    if (maxFree <= 0) return 0;

    for (int s = 0; s < SLOTS; s++) {
        if (lot->slots[bestZone][s] == 0) {
            *zone = bestZone;
            *slot = s;
            return 1;
        }
    }
    return 0;
}

void print_parking_status(unsigned char status) {
    printf("\n==== [주차장 내부 비트 현황판] ====\n");
    printf(" 입구 센서 : %s\n", (status & ST_ENTRY)   ? "★ ON (차량 진입)" : "☆ OFF");
    printf(" 출구 센서 : %s\n", (status & ST_EXIT)    ? "★ ON (차량 진출)" : "☆ OFF");
    printf(" 차단기 상태: %s\n", (status & ST_BARRIER) ? "▲ 열림 (OPEN)"      : "▼ 닫힘 (CLOSE)");
    printf(" 만차 여부 : %s\n", (status & ST_FULL)    ? "🔴 만차 (FULL)"     : "🟢 여유 (VACANT)");
    printf(" 화재 경보 : %s\n", (status & ST_FIRE)    ? "🚨 화재 발생 (FIRE)" : "✅ 정상 (SAFE)");
    printf("===================================\n");
}

/* [수정] 입차 후 차단기 상태가 열린 채로 유지되지 않도록 프로세스 완료 후 폐쇄 구현 */
void enterCar(ParkingLot *lot, const char *plate) {
    lot->status |= ST_ENTRY;
    logEvent("입구 센서 차량 감지");

    if (lot->count >= lot->capacity) {
        lot->status |= ST_FULL;
        logEvent("[경고] 만차로 인한 진입 거부");
        printf(">> 만차입니다. 입차 불가\n");
        setBarrier(lot, 0);
        lot->status &= ~ST_ENTRY;
        saveStatusFile(*lot);
        return;
    }
    
    if (!isRegistered(plate)) {
        logEvent("[경고] 미등록 차량 진입 시도 차단");
        printf(">> [경고] 미등록 차량(%s) 진입 시도! 차단기 폐쇄\n", plate);
        setBarrier(lot, 0);
        lot->status &= ~ST_ENTRY;
        saveStatusFile(*lot);
        return;
    }

    int z, s;
    if (!findBestSlot(lot, &z, &s)) {
        printf(">> 배정 가능한 자리가 없습니다\n");
        lot->status &= ~ST_ENTRY;
        saveStatusFile(*lot);
        return;
    }

    int targetIdx = -1;
    for (int i = 0; i < CAPACITY; i++) {
        if (!lot->cars[i].used) {
            targetIdx = i;
            break;
        }
    }

    if (targetIdx != -1) {
        Vehicle *v = &lot->cars[targetIdx];
        strncpy(v->plate, plate, sizeof(v->plate) - 1);
        v->plate[sizeof(v->plate) - 1] = '\0';
        v->zone = z;
        v->slot = s;
        v->in   = time(NULL);
        v->used = 1;

        lot->slots[z][s] = 1;
        lot->count++;

        /* 1. 차량을 보고 차단기를 엽니다. */
        setBarrier(lot, 1);
        char evMsg[64];
        sprintf(evMsg, "차량 입차 완료: %s (%c구역 %d번)", plate, 'A' + z, s + 1);
        logEvent(evMsg);
        printf(">> 입차 완료: %s -> %c구역 %d번\n", plate, 'A' + z, s + 1);

        /* 2. [해결] 차량 진입이 끝났으므로 차단기를 다시 정상 폐쇄 구조로 돌려놓습니다. */
        setBarrier(lot, 0); 
    }

    if (lot->count >= lot->capacity) lot->status |= ST_FULL;
    lot->status &= ~ST_ENTRY;
    saveStatusFile(*lot);
}

void exitCar(ParkingLot *lot, const char *plate) {
    lot->status |= ST_EXIT;
    logEvent("출구 센서 차량 감지");

    int idx = -1;
    for (int i = 0; i < CAPACITY; i++) {
        if (lot->cars[i].used && strcmp(lot->cars[i].plate, plate) == 0) {
            idx = i;
            break;
        }
    }
    
    if (idx < 0) {
        printf(">> 해당 차량(%s)을 찾을 수 없습니다\n", plate);
        lot->status &= ~ST_EXIT;
        saveStatusFile(*lot);
        return;
    }

    Vehicle *v  = &lot->cars[idx];
    time_t  out = time(NULL);
    
    int minutes = (int)(difftime(out, v->in) / 60.0);
    if (minutes < 1) minutes = 1; 
    int fee = calcFee(minutes);

    printf("\n========== 정산 영수증 ==========\n");
    printf(" 차량번호 : %s\n",          v->plate);
    printf(" 주차구역 : %c구역 %d번\n", 'A' + v->zone, v->slot + 1);
    printf(" 주차시간 : %d분\n",        minutes);
    printf(" 정산요금 : %d원\n",        fee);
    printf("=================================\n\n");

    writeParkingLog(*v, out, fee, "출차완료");
    
    char evMsg[64];
    sprintf(evMsg, "차량 출차/정산 완료: %s", plate);
    logEvent(evMsg);

    lot->slots[v->zone][v->slot] = 0;
    v->used = 0;
    lot->count--;

    lot->status &= ~ST_FULL;
    
    /* 출차 통과 후 차단기 원위치(닫힘) 처리 */
    setBarrier(lot, 1);
    setBarrier(lot, 0); 
    
    lot->status &= ~ST_EXIT;
    saveStatusFile(*lot);
}

void emergencyMode(ParkingLot *lot) {
    lot->status |= ST_FIRE;
    logEvent("[비상] 화재 경보 발령 !!");
    printf("\n!!!!! 🚨 긴급 상황! 화재 발생 -> 차단기 강제 개방 및 만차 해제 !!!!!\n");
    setBarrier(lot, 1);   
    lot->status &= ~ST_FULL; 
    saveStatusFile(*lot);
}

void recoverEmergency(ParkingLot *lot) {
    lot->status &= ~ST_FIRE;
    logEvent("[복구] 화재 상황 해제 완료");
    printf("\n[상황 종료] 복구 완료 -> 화재 경보 해제 및 차단기 원위치\n");
    setBarrier(lot, 0);
    if (lot->count >= lot->capacity) lot->status |= ST_FULL;
    saveStatusFile(*lot);
}

void printStatus(ParkingLot *lot) {
    printf("\n========= [모니터] 주차장 실시간 현황 =========\n");
    printf(" 주차 가능 대수 : %d / %d\n", lot->capacity - lot->count, lot->capacity);
    printf(" 차단기 상태    : %s\n", (lot->status & ST_BARRIER) ? "▲ 개방" : "▼ 폐쇄");
    printf(" 만차 여부      : %s\n", (lot->status & ST_FULL)    ? "🔴 만차" : "🟢 여유");
    printf(" 화재 경보      : %s\n", (lot->status & ST_FIRE)    ? "🚨 발생!!" : "✅ 정상");
    printf(" 구역별 점유    : (O=빈자리, X=점유)\n");
    for (int z = 0; z < ZONES; z++) {
        printf("   %c구역: ", 'A' + z);
        for (int s = 0; s < SLOTS; s++)
            printf("[%c]", lot->slots[z][s] ? 'X' : 'O');
        printf("\n");
    }
    printf("===============================================\n");
}

/* ===== [6] 메인 제어 루프 ===== */
int main(void) {
    ParkingLot lot;
    initLot(&lot); 

    int  menu;
    char plate[16];

    printf("== 자동 주차장 출입/공간 복합 제어 시스템 ==\n");
    printf("등록 차량 목록: ");
    for (int i = 0; i < REG_COUNT; i++) {
        printf("[%s] ", REGISTERED[i]);
    }
    printf("\n");

    while (1) {
        printf("\n[1]입차 [2]출차 [3]콘솔현황 [4]화재비상 [5]비상해제 [6]로그파일조회 [0]종료\n선택: ");
        if (scanf("%d", &menu) != 1) break;

        if (menu == 0) { 
            printf("시스템을 안전하게 종료합니다.\n"); 
            break; 
        }
        else if (menu == 1) {
            printf("진입 차량번호 입력: ");
            scanf("%15s", plate);
            enterCar(&lot, plate);
        }
        else if (menu == 2) {
            printf("진출 차량번호 입력: ");
            scanf("%15s", plate);
            exitCar(&lot, plate);
        }
        else if (menu == 3) { 
            printStatus(&lot); 
            print_parking_status(lot.status); 
        }
        else if (menu == 4) { 
            emergencyMode(&lot); 
        }
        else if (menu == 5) {
            recoverEmergency(&lot);
        }
        else if (menu == 6) {
            /* [수정] 메모리 상태를 읽어 미출차 차량도 함께 조회하도록 주소값 전달 */
            printParkingLog(&lot); 
        }
        else { 
            printf("잘못된 선택입니다. 메뉴 번호를 확인하세요.\n"); 
        }
    }
    return 0;
}