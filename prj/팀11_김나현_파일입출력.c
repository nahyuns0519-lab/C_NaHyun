/* ============================================================
   담당: 파일 입출력 (File I/O) 모듈 — 개별 단독 실행 버전
   기능: 차량 입·출차 기록, 운영 이벤트, 현황을 파일로 저장·조회
   출력 파일: parking_log.txt / parking_event.txt / parking_status.txt
   ============================================================ */
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define LOG_FILE    "parking_log.txt"
#define EVENT_FILE  "parking_event.txt"
#define STATUS_FILE "parking_status.txt"

/* 이 파트 단독 실행을 위해 필요한 구조체 정의 */
typedef struct {
    char   plate[16];   /* 차량번호       */
    int    zone, slot;  /* 구역/자리      */
    time_t in;          /* 입차 시각      */
} Vehicle;

typedef struct {
    int count;          /* 현재 차량 수   */
    int capacity;       /* 최대 수용 대수 */
    int barrier;        /* 차단기 0/1     */
    int full;           /* 만차 0/1       */
} ParkingLot;

/* 시간 변환: time_t → "2026-06-04", "14:30" */
void fmtTime(time_t t, char* date, char* hm) {
    struct tm *tmv = localtime(&t);   /* 표준 함수 */
    strftime(date, 12, "%Y-%m-%d", tmv);
    strftime(hm, 8, "%H:%M", tmv);
}

/* [1] 출차 차량 1건을 로그 파일에 추가 (모드 "a") */
void writeParkingLog(Vehicle car, time_t out, int fee, const char* status) {
    FILE* f;
    int isNew = 0;
    char inDate[12], inHM[8], outDate[12], outHM[8];
    fmtTime(car.in, inDate, inHM);
    fmtTime(out, outDate, outHM);
    int parkMin = (int)((out - car.in) / 60);

    if ((f = fopen(LOG_FILE, "r")) == NULL) isNew = 1;
    else fclose(f);

    if ((f = fopen(LOG_FILE, "a")) == NULL) {   /* 표준 fopen + NULL 체크 */
        printf("로그 파일을 열 수 없습니다.\n");
        return;
    }
    if (isNew)
        fprintf(f, "%-12s%-12s%-8s%-8s%-10s%-8s%-8s\n",
            "날짜", "차량번호", "입차", "출차", "주차(분)", "요금", "상태");
    fprintf(f, "%-12s%-12s%-8s%-8s%-10d%-8d%-8s\n",
        inDate, car.plate, inHM, outHM, parkMin, fee, status);
    fclose(f);
}

/* [2] 운영 이벤트 기록 (모드 "a") */
void logEvent(const char* event) {
    FILE* f;
    char date[12], hm[8];
    fmtTime(time(NULL), date, hm);
    if ((f = fopen(EVENT_FILE, "a")) == NULL) {
        printf("이벤트 파일을 열 수 없습니다.\n");
        return;
    }
    fprintf(f, "[%s %s] %s\n", date, hm, event);
    fclose(f);
}

/* [3] 현재 현황 저장 (모드 "w": 덮어쓰기) */
void saveStatus(ParkingLot lot) {
    FILE* f;
    char date[12], hm[8];
    fmtTime(time(NULL), date, hm);
    if ((f = fopen(STATUS_FILE, "w")) == NULL) {
        printf("현황 파일을 열 수 없습니다.\n");
        return;
    }
    fprintf(f, "갱신 시각 : %s %s\n", date, hm);
    fprintf(f, "현재 차량 : %d대\n", lot.count);
    fprintf(f, "최대 수용 : %d대\n", lot.capacity);
    fprintf(f, "남은 자리 : %d대\n", lot.capacity - lot.count);
    fprintf(f, "차단기    : %s\n", lot.barrier ? "개방" : "폐쇄");
    fprintf(f, "만차 여부 : %s\n", lot.full ? "만차" : "여유");
    fclose(f);
}

/* [4] 저장된 로그 전체를 화면에 출력 (모드 "r") */
void printParkingLog(void) {
    FILE* f;
    char line[128];
    if ((f = fopen(LOG_FILE, "r")) == NULL) {
        printf("아직 주차 기록이 없습니다.\n");
        return;
    }
    printf("\n===== 주차장 운행 로그 =====\n");
    while (fgets(line, sizeof(line), f) != NULL)
        printf("%s", line);
    fclose(f);
}

/* ===== 개별 파트 동작 확인용 main ===== */
int main(void) {
    /* 1. 가상의 입차 차량 한 대 생성 */
    Vehicle car;
    strcpy(car.plate, "12가3456");
    car.zone = 0; car.slot = 0;
    car.in = time(NULL) - 3600;   /* 1시간 전 입차로 가정 */

    /* 2. 현황 저장 */
    ParkingLot lot = { 1, 15, 1, 0 };  /* 차량1, 수용15, 차단기개방, 여유 */
    saveStatus(lot);
    printf(">> parking_status.txt 저장 완료\n");

    /* 3. 이벤트 기록 */
    logEvent("차단기 OPEN 동작");
    logEvent("차량 입차: 12가3456");
    printf(">> parking_event.txt 기록 완료\n");

    /* 4. 출차 기록 (주차 60분, 요금 4000원 가정) */
    writeParkingLog(car, time(NULL), 4000, "출차완료");
    printf(">> parking_log.txt 기록 완료\n");

    /* 5. 저장된 로그 화면 출력 */
    printParkingLog();

    return 0;
}