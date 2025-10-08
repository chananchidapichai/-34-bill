#include <stdio.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

#define FILE_NAME "bill_data.txt"
#define MAX_BILLS 100

typedef struct {
    char ReceiptID[10];
    char CustomerName[50];
    int Amount;
    char Date[20];
} Bill;

// ฟังก์ชันระบบ
int isValidReceiptID(const char *id) {
    if (strlen(id) != 4) return 0;
    if (!isalpha(id[0])) return 0;
    for (int i = 1; i < 4; i++) {
        if (!isdigit(id[i])) return 0;
    }
    return 1;
}

int validateAndFormatDate(const char *date, char *formattedDate) {
    struct tm tm;
    if (strptime(date, "%Y-%m-%d", &tm) == NULL) return 0;

    time_t t = time(NULL);
    struct tm now = *localtime(&t);

    if (tm.tm_year + 1900 > now.tm_year + 1900) return 0;

    strcpy(formattedDate, date);
    return 1;
}

int receiptExists(const char *id) {
    FILE *fp = fopen(FILE_NAME, "r");
    if (!fp) return 0;
    char line[256];
    while (fgets(line, sizeof(line), fp)) {
        char temp[10];
        sscanf(line, "%[^,]", temp);
        if (strcmp(temp, id) == 0) {
            fclose(fp);
            return 1;
        }
    }
    fclose(fp);
    return 0;
}

int readAllBills(Bill bills[], int max) {
    FILE *fp = fopen(FILE_NAME, "r");
    if (!fp) return 0;
    int count = 0;
    while (fscanf(fp, "%[^,],%[^,],%d,%s\n",
                  bills[count].ReceiptID,
                  bills[count].CustomerName,
                  &bills[count].Amount,
                  bills[count].Date) == 4) {
        count++;
        if (count >= max) break;
    }
    fclose(fp);
    return count;
}

void writeAllBills(Bill bills[], int count) {
    FILE *fp = fopen(FILE_NAME, "w");
    for (int i = 0; i < count; i++) {
        fprintf(fp, "%s,%s,%d,%s\n",
                bills[i].ReceiptID,
                bills[i].CustomerName,
                bills[i].Amount,
                bills[i].Date);
    }
    fclose(fp);
}

void showBills() {
    Bill bills[MAX_BILLS];
    int count = readAllBills(bills, MAX_BILLS);
    for (int i = 0; i < count; i++) {
        printf("%s,%s,%d,%s\n",
               bills[i].ReceiptID,
               bills[i].CustomerName,
               bills[i].Amount,
               bills[i].Date);
    }
}

void e2eTest() {
    printf("\n==== E2E TEST ====\n");

    FILE *fp = fopen(FILE_NAME, "w");
    fclose(fp);

    printf("Adding bill A100...\n");
    fp = fopen(FILE_NAME, "a");
    fprintf(fp, "A100,E2ETestUser,200,2023-01-01\n");
    fclose(fp);

    if (receiptExists("A100"))
        printf("PASS: Bill A100 added.\n");
    else
        printf("FAIL: Bill A100 not found.\n");

    printf("Showing all bills:\n");
    showBills();

    printf("Updating bill A100...\n");
    Bill bills[MAX_BILLS];
    int count = readAllBills(bills, MAX_BILLS);
    for (int i = 0; i < count; i++) {
        if (strcmp(bills[i].ReceiptID, "A100") == 0) {
            strcpy(bills[i].CustomerName, "UpdatedUser");
            bills[i].Amount = 999;
            strcpy(bills[i].Date, "2023-02-02");
            break;
        }
    }
    writeAllBills(bills, count);

    printf("Showing bills after update:\n");
    showBills();

    printf("Deleting bill A100...\n");
    count = readAllBills(bills, MAX_BILLS);
    for (int i = 0; i < count; i++) {
        if (strcmp(bills[i].ReceiptID, "A100") == 0) {
            for (int j = i; j < count - 1; j++)
                bills[j] = bills[j + 1];
            count--;
            writeAllBills(bills, count);
            break;
        }
    }

    if (!receiptExists("A100"))
        printf("PASS: Bill A100 deleted.\n");
    else
        printf("FAIL: Bill A100 still exists.\n");

    showBills();
}

int main() {
    printf("\n===== RUNNING TEST SUITE =====\n");
    e2eTest();
    printf("\n===== TEST SUITE COMPLETE =====\n");
    return 0;
}
