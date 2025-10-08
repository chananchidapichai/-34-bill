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

// Unit Test
void unitTest() {
    char formattedDate[20];
    printf("\n==== UNIT TEST ====\n");

    printf("[isValidReceiptID] 'A123' => %d (expected: 1)\n", isValidReceiptID("A123"));
    printf("[isValidReceiptID] 'a123' => %d (expected: 1)\n", isValidReceiptID("a123"));
    printf("[isValidReceiptID] '1234' => %d (expected: 0)\n", isValidReceiptID("1234"));
    printf("[isValidReceiptID] 'AB12' => %d (expected: 0)\n", isValidReceiptID("AB12"));
    printf("[isValidReceiptID] 'A12' => %d (expected: 0)\n", isValidReceiptID("A12"));
    printf("[isValidReceiptID] 'A1234' => %d (expected: 0)\n", isValidReceiptID("A1234"));

    printf("[validateAndFormatDate] '2023-01-01' => %d (expected: 1)\n", validateAndFormatDate("2023-01-01", formattedDate));
    printf("[validateAndFormatDate] '2099-12-31' => %d (expected: 0)\n", validateAndFormatDate("2099-12-31", formattedDate));
    printf("[validateAndFormatDate] 'abcd-ef-gh' => %d (expected: 0)\n", validateAndFormatDate("abcd-ef-gh", formattedDate));
    printf("[validateAndFormatDate] '2023-02-29' => %d (expected: 0)\n", validateAndFormatDate("2023-02-29", formattedDate));
    printf("[validateAndFormatDate] '2024-02-29' => %d (expected: 1)\n", validateAndFormatDate("2024-02-29", formattedDate));
    printf("[validateAndFormatDate] '1900-02-29' => %d (expected: 0)\n", validateAndFormatDate("1900-02-29", formattedDate));
    printf("[validateAndFormatDate] '2000-02-29' => %d (expected: 1)\n", validateAndFormatDate("2000-02-29", formattedDate));
    printf("[validateAndFormatDate] '2023-04-31' => %d (expected: 0)\n", validateAndFormatDate("2023-04-31", formattedDate));

    FILE *fp = fopen(FILE_NAME, "w");
    fprintf(fp, "B001,UnitTestUser,100,2023-01-01\n");
    fclose(fp);

    printf("[receiptExists] 'B001' => %d (expected: 1)\n", receiptExists("B001"));
    printf("[receiptExists] 'X999' => %d (expected: 0)\n", receiptExists("X999"));
}

int main() {
    printf("\n===== RUNNING TEST SUITE =====\n");
    unitTest();
    printf("\n===== TEST SUITE COMPLETE =====\n");
    return 0;
}
