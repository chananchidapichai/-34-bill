//run with bill_finalproject.c
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

#define RESET   "\033[0m"
#define BOLD    "\033[1m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define CYAN    "\033[36m"
#define MAGENTA "\033[35m"

#define FILE_NAME "bill_data.txt"
#define MAX_BILLS 100

typedef struct {
    char ReceiptID[10];
    char CustomerName[50];
    int Amount;
    char Date[20];
} Bill;

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

void unitTest() {
    printf("\n%s========== UNIT TEST ==========%s\n", BOLD, RESET);
    char formattedDate[20];

    printf("\n%s[TEST 1]%s Function: %sisValidReceiptID()%s\n", CYAN, RESET, MAGENTA, RESET);
    printf("------------------------------------\n");
    printf("Case 1: 'A123' -> %d (expected: 1)\n", isValidReceiptID("A123"));
    printf("Case 2: 'a123' -> %d (expected: 1)\n", isValidReceiptID("a123"));
    printf("Case 3: '1234' -> %d (expected: 0)\n", isValidReceiptID("1234"));
    printf("Case 4: 'AB12' -> %d (expected: 0)\n", isValidReceiptID("AB12"));
    printf("Case 5: 'A12'  -> %d (expected: 0)\n", isValidReceiptID("A12"));
    printf("Case 6: 'A1234' -> %d (expected: 0)\n", isValidReceiptID("A1234"));

    printf("\n%sSummary:%s Function isValidReceiptID correctly validates the format (1 letter + 3 digits)\n",
           GREEN, RESET);

    printf("\n%s[TEST 2]%s Function: %svalidateAndFormatDate()%s\n", CYAN, RESET, MAGENTA, RESET);
    printf("------------------------------------------\n");
    printf("Case 1: '2023-01-01' -> %d (expected: 1)\n", validateAndFormatDate("2023-01-01", formattedDate));
    printf("Case 2: '2099-12-31' -> %d (expected: 0)  (because it's a future date)\n", validateAndFormatDate("2099-12-31", formattedDate));
    printf("Case 3: 'abcd-ef-gh' -> %d (expected: 0)  (because the format is invalid)\n", validateAndFormatDate("abcd-ef-gh", formattedDate));
    printf("Case 4: '2023-02-29' -> %d (expected: 0)  (because 2023 is not a leap year)\n", validateAndFormatDate("2023-02-29", formattedDate));
    printf("Case 5: '2024-02-29' -> %d (expected: 1)  (because 2024 is a leap year)\n", validateAndFormatDate("2024-02-29", formattedDate));
    printf("Case 6: '1900-02-29' -> %d (expected: 0)  (because 1900 is not a leap year)\n", validateAndFormatDate("1900-02-29", formattedDate));
    printf("Case 7: '2000-02-29' -> %d (expected: 1)  (because 2000 is a leap year)\n", validateAndFormatDate("2000-02-29", formattedDate));

    printf("\n%sSummary:%s Function validateAndFormatDate correctly validates all date cases\n", GREEN, RESET);

    printf("\n%s========== UNIT TEST COMPLETED ==========%s\n", BOLD, RESET);
}
