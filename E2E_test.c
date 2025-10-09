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
#define RED     "\033[31m"

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
void e2eTest() {
    printf("\n%s========== END-TO-END TEST ==========%s\n", BOLD, RESET);

    printf("\n%s[STEP 0]%s Clearing data file: %s\n", CYAN, RESET, FILE_NAME);
    FILE *fp = fopen(FILE_NAME, "w");
    if (fp == NULL) {
        printf("%s[ERROR]%s Could not open file '%s' for writing. Aborting test.\n", RED, RESET, FILE_NAME);
        return;
    }
    fclose(fp);
    printf("  %s[OK]%s Cleared the file successfully.\n", GREEN, RESET);

    printf("\n%s[STEP 1]%s Adding new bill 'A001' (A001,TestUser,500,2023-01-01)\n", CYAN, RESET);
    fp = fopen(FILE_NAME, "a");
    if (fp == NULL) {
        printf("%s[FAIL]%s Could not open '%s' for appending.\n", RED, RESET, FILE_NAME);
        return;
    }
    fprintf(fp, "A001,TestUser,500,2023-01-01\n");
    fclose(fp);
    printf("  %s[PASS]%s Added record: A001, TestUser, 500, 2023-01-01\n", GREEN, RESET);

    printf("\n%s[STEP 2]%s Verifying that A001 exists in the file\n", CYAN, RESET);
    if (receiptExists("A001")) {
        printf("  %s[PASS]%s Bill A001 exists in file (expected).\n", GREEN, RESET);
    } else {
        printf("  %s[FAIL]%s Bill A001 NOT found (expected present). Check file write or receiptExists implementation.\n", RED, RESET);
    }

    printf("\n%s[STEP 3]%s Showing current bills (output from showBills):\n", CYAN, RESET);
    showBills();
    printf("  %s[INFO]%s Above output should include the A001 record.\n", YELLOW, RESET);

    printf("\n%s[STEP 4]%s Updating A001 -> Customer: 'UpdatedUser', Amount: 999, Date: '2023-02-02'\n", CYAN, RESET);
    Bill bills[MAX_BILLS];
    int count = readAllBills(bills, MAX_BILLS);
    int updated = 0;
    for (int i = 0; i < count; i++) {
        if (strcmp(bills[i].ReceiptID, "A001") == 0) {
            strcpy(bills[i].CustomerName, "UpdatedUser");
            bills[i].Amount = 999;
            strcpy(bills[i].Date, "2023-02-02");
            updated = 1;
            break;
        }
    }
    if (updated) {
        writeAllBills(bills, count);
        printf("  %s[PASS]%s Updated A001 in memory and wrote changes to file.\n", GREEN, RESET);
    } else {
        printf("  %s[FAIL]%s A001 not found in readAllBills result. No update performed.\n", RED, RESET);
    }

    printf("\n%s[STEP 5]%s Verifying updated values for A001\n", CYAN, RESET);
    count = readAllBills(bills, MAX_BILLS);
    int found = 0;
    for (int i = 0; i < count; i++) {
        if (strcmp(bills[i].ReceiptID, "A001") == 0) {
            found = 1;
            if (strcmp(bills[i].CustomerName, "UpdatedUser") == 0 &&
                bills[i].Amount == 999 &&
                strcmp(bills[i].Date, "2023-02-02") == 0) {
                printf("  %s[PASS]%s A001 updated correctly (Customer: %s, Amount: %d, Date: %s)\n",
                       GREEN, RESET, bills[i].CustomerName, bills[i].Amount, bills[i].Date);
            } else {
                printf("  %s[FAIL]%s A001 found but values differ. Actual -> Customer: '%s', Amount: %d, Date: '%s'\n",
                       RED, RESET, bills[i].CustomerName, bills[i].Amount, bills[i].Date);
            }
            break;
        }
    }
    if (!found) {
        printf("  %s[FAIL]%s A001 not found during update verification. Check writeAllBills/readAllBills.\n", RED, RESET);
    }

    printf("\n%s[STEP 6]%s Deleting A001\n", CYAN, RESET);
    count = readAllBills(bills, MAX_BILLS);
    int deleted = 0;
    for (int i = 0; i < count; i++) {
        if (strcmp(bills[i].ReceiptID, "A001") == 0) {
            for (int j = i; j < count - 1; j++)
                bills[j] = bills[j + 1];
            count--;
            writeAllBills(bills, count);
            deleted = 1;
            break;
        }
    }
    if (deleted) {
        printf("  %s[PASS]%s Deleted A001 and wrote changes to file.\n", GREEN, RESET);
    } else {
        printf("  %s[FAIL]%s Could not find A001 to delete. No changes made.\n", RED, RESET);
    }

    printf("\n%s[STEP 7]%s Verifying that A001 is no longer present\n", CYAN, RESET);
    if (!receiptExists("A001")) {
        printf("  %s[PASS]%s Bill A001 deleted correctly (not found in file).\n", GREEN, RESET);
    } else {
        printf("  %s[FAIL]%s Bill A001 still exists (expected deleted). Check delete logic or writeAllBills.\n", RED, RESET);
    }

    printf("\n%s[FINAL]%s Remaining bills (output from showBills):\n", MAGENTA, RESET);
    showBills();

    printf("\n%s========== END-TO-END TEST COMPLETED ==========%s\n", BOLD, RESET);
}
