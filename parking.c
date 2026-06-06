/* ============================================================
   주차장 시스템 - 구조체 + 파일 입출력 통합본 (Mac OS 표준 규격)
   ============================================================ */

#include <stdio.h>
#include <stdlib.h>   // exit()
#include <string.h>   // strcpy()
#include <time.h>     // time_t, ctime, localtime, strftime

#define CAPACITY    15
#define LOG_FILE    "parking_log.txt"
#define EVENT_FILE  "parking_event.txt"
#define STATUS_FILE "parking_status.txt"

typedef struct {
    char   plate[16];
    int    zone, slot;
    time_t in;
    int    used;
} Vehicle;

typedef struct {
    int count;
    int capacity;
    int barrier;
    int full;
} ParkingLot;

void fmtTime(time_t t, char* date, char* hm) {
    struct tm* tmv = localtime(&t);
    if (tmv != NULL) {
        strftime(date, 12, "%Y-%m-%d", tmv);
        strftime(hm, 8, "%H:%M", tmv);
    }
}

void writeParkingLog(Vehicle car, time_t out, const char* status) {
    FILE* f;
    int isNew = 0;
    char inDate[12], inHM[8], outDate[12], outHM[8];

    fmtTime(car.in, inDate, inHM);
    fmtTime(out, outDate, outHM);
    int parkMin = (int)((out - car.in) / 60);

    if ((f = fopen(LOG_FILE, "r")) == NULL)
        isNew = 1;
    else
        fclose(f);

    if ((f = fopen(LOG_FILE, "a")) == NULL) {
        printf("로그 파일을 열 수 없습니다.\n");
        exit(1);
    }
    if (isNew)
        fprintf(f, "%-12s%-12s%-8s%-8s%-10s%-8s\n",
            "날짜", "차량번호", "입차", "출차", "주차(분)", "상태");

    fprintf(f, "%-12s%-12s%-8s%-8s%-10d%-8s\n",
        inDate, car.plate, inHM, outHM, parkMin, status);

    fclose(f);
}

void logEvent(const char* event) {
    FILE* f;
    char date[12], hm[8];
    fmtTime(time(NULL), date, hm);

    if ((f = fopen(EVENT_FILE, "a")) == NULL) {
        printf("이벤트 파일을 열 수 없습니다.\n");
        exit(1);
    }
    fprintf(f, "[%s %s] %s\n", date, hm, event);
    fclose(f);
}

void saveStatus(ParkingLot lot) {
    FILE* f;
    char date[12], hm[8];
    fmtTime(time(NULL), date, hm);

    if ((f = fopen(STATUS_FILE, "w")) == NULL) {
        printf("현황 파일을 열 수 없습니다.\n");
        exit(1);
    }
    fprintf(f, "갱신 시각 : %s %s\n", date, hm);
    fprintf(f, "현재 차량 : %d대\n", lot.count);
    fprintf(f, "최대 수용 : %d대\n", lot.capacity);
    fprintf(f, "남은 자리 : %d대\n", lot.capacity - lot.count);
    fprintf(f, "차단기    : %s\n", lot.barrier ? "개방" : "폐쇄");
    fprintf(f, "만차 여부 : %s\n", lot.full ? "만차" : "여유");
    fclose(f);
}

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

int main(void) {

    Vehicle car;
    strcpy(car.plate, "123가4567");
    car.zone = 'A';
    car.slot = 14;
    car.in   = time(NULL);

    printf("===== 입차 정보 =====\n");
    printf("차량번호: %s\n", car.plate);
    printf("배정구역: %c구역\n", car.zone);
    printf("배정자리: %d번\n", car.slot);
    printf("입차시각: %s", ctime(&car.in));
    printf("=====================\n");

    logEvent("차단기 개방 - 등록차량 123가4567 입차");

    ParkingLot lot = { 14, CAPACITY, 0, 0 };
    lot.count++;
    lot.barrier = 1;
    lot.full = (lot.count >= lot.capacity);

    printf("\n========= 주차장 상태 =========\n");
    printf(" 현재 차량 수   : %d대\n", lot.count);
    printf(" 남은 자리      : %d대\n", lot.capacity - lot.count);
    printf(" 차단기 상태    : %s\n", lot.barrier ? "개방" : "폐쇄");
    printf(" 만차 여부      : %s\n", lot.full ? "만차" : "여유");
    printf("===============================\n");

    if (lot.full) logEvent("만차 발생 - 잔여 0면");
    saveStatus(lot);

    time_t out = car.in + 150 * 60;
    writeParkingLog(car, out, "정상");

    printParkingLog();

    return 0;
}