//ต้องรันคู่กับโปรแกรม
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#define MAX_BILLS 100
#define FILE_NAME "bills.csv"

typedef struct {
    char ReceiptID[20];
    char CustomerName[50];
    double Amount;
    char Date[20];
} Bill;

bool isValidReceiptID(const char *id);
bool receiptExists(const char *id);
bool validateAndFormatDate(const char *inputDate, char *outputDate);
int readAllBills(Bill bills[], int maxBills);
void writeAllBills(Bill bills[], int count);

void runUnitTests() {
    printf("\n===== UNIT TEST START =====\n");

    char oldFile[] = FILE_NAME;
    rename(FILE_NAME, "temp.csv");
    FILE *fp = fopen(FILE_NAME, "w");
    fclose(fp);

    char dateFormatted[20];

    printf("\n[TEST] ReceiptID Validation:\n");
    printf(!isValidReceiptID("A12") ? "PASS\n" : "FAIL\n");
    printf(isValidReceiptID("A123") ? "PASS\n" : "FAIL\n");

    printf("\n[TEST] Duplicate ReceiptID:\n");
    fp = fopen(FILE_NAME, "a");
    fprintf(fp, "A123,John Doe,100,2020-01-01\n");
    fclose(fp);
    printf(receiptExists("A123") ? "PASS\n" : "FAIL\n");
    printf(!receiptExists("B999") ? "PASS\n" : "FAIL\n");

    printf("\n[TEST] Date Validation:\n");
    printf(!validateAndFormatDate("3000-01-01", dateFormatted) ? "PASS\n" : "FAIL\n");
    printf(validateAndFormatDate("2020-02-29", dateFormatted) ? "PASS\n" : "FAIL\n");
    printf(!validateAndFormatDate("2021-02-29", dateFormatted) ? "PASS\n" : "FAIL\n");

    printf("\n[TEST] Add Bill:\n");
    Bill newBill = {"B001", "Alice", 200.5, "2020-12-12"};
    fp = fopen(FILE_NAME, "a");
    fprintf(fp, "%s,%s,%.2f,%s\n", newBill.ReceiptID, newBill.CustomerName, newBill.Amount, newBill.Date);
    fclose(fp);
    printf(receiptExists("B001") ? "PASS\n" : "FAIL\n");

    printf(receiptExists("A123") ? "PASS\n" : "FAIL\n");
    printf(!validateAndFormatDate("2025-13-01", dateFormatted) ? "PASS\n" : "FAIL\n");

    printf("\n[TEST] Update Bill:\n");
    Bill bills[MAX_BILLS];
    int count = readAllBills(bills, MAX_BILLS);
    int updated = 0;
    for (int i = 0; i < count; i++) {
        if (strcmp(bills[i].ReceiptID, "B001") == 0) {
            strcpy(bills[i].CustomerName, "Alice Updated");
            bills[i].Amount = 500.0;
            if (validateAndFormatDate("2020-10-10", bills[i].Date)) {
                updated = 1;
            }
            break;
        }
    }
    writeAllBills(bills, count);
    printf(updated ? "PASS\n" : "FAIL\n");

    updated = 0;
    for (int i = 0; i < count; i++) {
        if (strcmp(bills[i].ReceiptID, "Z999") == 0) {
            updated = 1;
            break;
        }
    }
    printf(!updated ? "PASS\n" : "FAIL\n");

    updated = 0;
    for (int i = 0; i < count; i++) {
        if (strcmp(bills[i].ReceiptID, "B001") == 0) {
            if (!validateAndFormatDate("2025-15-15", bills[i].Date)) {
                updated = 1;
            }
            break;
        }
    }
    printf(updated ? "PASS\n" : "FAIL\n");

    printf("\n===== UNIT TEST END =====\n");

    remove(FILE_NAME);
    rename("temp.csv", oldFile);
}