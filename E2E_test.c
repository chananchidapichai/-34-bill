// วิธีcomplie
// gcc end_to_end_test.c -o end_to_end_test
// ./end_to_end_test
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define FILE_NAME "bills.csv"
#define MAX_BILLS 100

typedef struct {
    char ReceiptID[10];
    char CustomerName[50];
    double Amount;
    char Date[20];
} Bill;

int isValidReceiptID(const char *id) {
    return strlen(id) == 4;
}

int receiptExists(const char *id) {
    FILE *fp = fopen(FILE_NAME, "r");
    if (!fp) return 0;
    char line[200];
    while (fgets(line, sizeof(line), fp)) {
        char fileID[10];
        sscanf(line, "%[^,]", fileID);
        if (strcmp(fileID, id) == 0) {
            fclose(fp);
            return 1;
        }
    }
    fclose(fp);
    return 0;
}

int validateAndFormatDate(const char *date, char *formatted) {
    int y, m, d;
    if (sscanf(date, "%d-%d-%d", &y, &m, &d) != 3) return 0;
    if (y < 1900 || m < 1 || m > 12 || d < 1 || d > 31) return 0;
    struct tm now = *localtime(&(time_t){time(NULL)});
    if (y > now.tm_year + 1900 ||
        (y == now.tm_year + 1900 && m > now.tm_mon + 1) ||
        (y == now.tm_year + 1900 && m == now.tm_mon + 1 && d > now.tm_mday))
        return 0;
    sprintf(formatted, "%04d-%02d-%02d", y, m, d);
    return 1;
}

int readAllBills(Bill *bills, int max) {
    FILE *fp = fopen(FILE_NAME, "r");
    if (!fp) return 0;
    char line[200];
    int count = 0;
    while (fgets(line, sizeof(line), fp) && count < max) {
        sscanf(line, "%[^,],%[^,],%lf,%s", bills[count].ReceiptID, bills[count].CustomerName, &bills[count].Amount, bills[count].Date);
        count++;
    }
    fclose(fp);
    return count;
}

void writeAllBills(Bill *bills, int count) {
    FILE *fp = fopen(FILE_NAME, "w");
    for (int i = 0; i < count; i++) {
        fprintf(fp, "%s,%s,%.2f,%s\n", bills[i].ReceiptID, bills[i].CustomerName, bills[i].Amount, bills[i].Date);
    }
    fclose(fp);
}

void runEndToEndTest() {
    printf("\n===== END-TO-END TEST START =====\n");

    char oldFile[] = FILE_NAME;
    rename(FILE_NAME, "temp.csv"); // สำรองไฟล์ของจริง
    FILE *fp = fopen(FILE_NAME, "w"); fclose(fp);

    printf("\n[TEST] Add Bills\n");
    Bill test1 = {"T001", "Test User1", 123.45, "2022-05-01"};
    Bill test2 = {"T002", "Test User2", 678.90, "2023-06-15"};
    fp = fopen(FILE_NAME, "a");
    fprintf(fp, "%s,%s,%.2f,%s\n", test1.ReceiptID, test1.CustomerName, test1.Amount, test1.Date);
    fprintf(fp, "%s,%s,%.2f,%s\n", test2.ReceiptID, test2.CustomerName, test2.Amount, test2.Date);
    fclose(fp);

    Bill bills[MAX_BILLS];
    int count = readAllBills(bills, MAX_BILLS);
    printf(count == 2 ? "PASS: Added 2 bills\n" : "FAIL: Add bills\n");

    printf("\n[TEST] Search Bill\n");
    if (receiptExists("T001")) printf("PASS: Bill T001 exists\n");
    else printf("FAIL: Bill T001 missing\n");

    printf("\n[TEST] Update Bill\n");
    for (int i = 0; i < count; i++) {
        if (strcmp(bills[i].ReceiptID, "T002") == 0) {
            strcpy(bills[i].CustomerName, "Test User2 Updated");
            bills[i].Amount = 999.99;
            strcpy(bills[i].Date, "2024-01-01");
            break;
        }
    }
    writeAllBills(bills, count);

    readAllBills(bills, MAX_BILLS);
    if (strcmp(bills[1].CustomerName, "Test User2 Updated") == 0 &&
        bills[1].Amount == 999.99) {
        printf("PASS: Update bill successful\n");
    } else {
        printf("FAIL: Update bill failed\n");
    }

    printf("\n[TEST] Delete Bill\n");
    for (int i = 0; i < count; i++) {
        if (strcmp(bills[i].ReceiptID, "T001") == 0) {
            for (int j = i; j < count - 1; j++) bills[j] = bills[j+1];
            count--;
            break;
        }
    }
    writeAllBills(bills, count);
    count = readAllBills(bills, MAX_BILLS);
    if (count == 1 && !receiptExists("T001")) {
        printf("PASS: Delete bill successful\n");
    } else {
        printf("FAIL: Delete bill failed\n");
    }

    printf("\n===== END-TO-END TEST END =====\n");

    remove(FILE_NAME);
    rename("temp.csv", oldFile); // คืนไฟล์เดิม
}

int main() {
    runEndToEndTest();
    return 0;
}