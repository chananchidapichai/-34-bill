#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

#define FILE_NAME "bill.csv"
#define MAX_BILLS 1000

typedef struct {
    char ReceiptID[10];
    char CustomerName[50];
    int Amount;
    char Date[20];
} Bill;

void toLowerStr(char *str) {
    for (int i = 0; str[i]; i++)
        str[i] = tolower((unsigned char)str[i]);
}

void ensureFileExists() {
    FILE *fp = fopen(FILE_NAME, "r");
    if (!fp) {
        fp = fopen(FILE_NAME, "w");
        if (!fp) {
            printf("Error: Cannot create file %s\n", FILE_NAME);
            exit(1);
        }
        printf("Created new file: %s\n", FILE_NAME);
        fclose(fp);
    } else {
        fclose(fp);
    }
}

int readAllBills(Bill bills[], int max) {
    ensureFileExists();
    FILE *fp = fopen(FILE_NAME, "r");
    if (!fp) return 0;

    int count = 0;
    while (count < max && fscanf(fp, "%[^,],%[^,],%d,%s\n",
           bills[count].ReceiptID, bills[count].CustomerName,
           &bills[count].Amount, bills[count].Date) == 4) {
        count++;
    }
    fclose(fp);
    return count;
}

void writeAllBills(Bill bills[], int count) {
    ensureFileExists();
    FILE *fp = fopen(FILE_NAME, "w");
    if (!fp) { printf("Cannot write file.\n"); return; }

    for (int i = 0; i < count; i++)
        fprintf(fp, "%s,%s,%d,%s\n",
                bills[i].ReceiptID, bills[i].CustomerName,
                bills[i].Amount, bills[i].Date);

    fclose(fp);
}

int receiptExists(const char *id) {
    Bill bills[MAX_BILLS];
    int count = readAllBills(bills, MAX_BILLS);
    for (int i = 0; i < count; i++)
        if (strcmp(bills[i].ReceiptID, id) == 0) return 1;
    return 0;
}

int isValidReceiptID(const char *id) {
    if (strlen(id) != 4) return 0;
    if (!isalpha(id[0])) return 0;
    for (int i = 1; i < 4; i++)
        if (!isdigit(id[i])) return 0;
    return 1;
}

int validateAndFormatDate(const char *inputStr, char *formattedDate) {
    int y, m, d;
    if (sscanf(inputStr, "%d-%d-%d", &y, &m, &d) != 3)
        return 0;

    if (y < 1900 || m < 1 || m > 12 || d < 1)
        return 0;

    int daysInMonth[] = {31,28,31,30,31,30,31,31,30,31,30,31};
    if ((y % 4 == 0 && y % 100 != 0) || (y % 400 == 0))
        daysInMonth[1] = 29;
    if (d > daysInMonth[m - 1])
        return 0;

    struct tm input = {0};
    input.tm_year = y - 1900;
    input.tm_mon = m - 1;
    input.tm_mday = d;
    time_t inputTime = mktime(&input);
    if (inputTime == -1)
        return 0;

    time_t now = time(NULL);
    struct tm *current = localtime(&now);
    struct tm today = {0};
    today.tm_year = current->tm_year;
    today.tm_mon = current->tm_mon;
    today.tm_mday = current->tm_mday;
    time_t todayTime = mktime(&today);

    if (difftime(inputTime, todayTime) > 0)
        return 0;

    sprintf(formattedDate, "%04d-%02d-%02d", y, m, d);
    return 1;
}

void addBill() {
    ensureFileExists();
    FILE *fp = fopen(FILE_NAME, "a");
    if (!fp) { printf("Cannot open file.\n"); return; }

    Bill b;
    int duplicate;
    do {
        duplicate = 0;
        printf("\nEnter Receipt ID (1 letter + 3 digits): ");
        scanf("%9s", b.ReceiptID);
        getchar();

        b.ReceiptID[0] = toupper((unsigned char)b.ReceiptID[0]);

        if (!isValidReceiptID(b.ReceiptID)) {
            printf("Error: Invalid format. Must be 1 letter followed by 3 digits.\n");
            duplicate = 1;
            continue;
        }

        if (receiptExists(b.ReceiptID)) {
            printf("Error: Receipt ID already exists.\n");
            duplicate = 1;
        }
    } while(duplicate);

    printf("Enter Customer Name : ");
    fgets(b.CustomerName, sizeof(b.CustomerName), stdin);
    b.CustomerName[strcspn(b.CustomerName, "\n")] = 0;

    printf("Enter Amount : ");
    scanf("%d", &b.Amount);
    getchar();

    char tempDate[20];
    do {
        printf("Enter Date (yyyy-mm-dd) : ");
        scanf("%19s", tempDate);
        getchar();
        if (!validateAndFormatDate(tempDate, b.Date))
            printf("Error: Invalid or future date. Please try again.\n");
    } while (!validateAndFormatDate(tempDate, b.Date));

    fprintf(fp, "%s,%s,%d,%s\n", b.ReceiptID, b.CustomerName, b.Amount, b.Date);
    fclose(fp);
    printf("Bill added successfully!\n");
}

void showBills() {
    Bill bills[MAX_BILLS];
    int count = readAllBills(bills, MAX_BILLS);
    if (count == 0) { 
        printf("No bills to show.\n"); 
        return; 
    }

    printf("\n+------------+-------------------------+---------+------------+\n");
    printf("| Receipt ID |  Customer Name          | Amount | Date        |\n");
    printf("+------------+-------------------------+---------+------------+\n");

    for (int i = 0; i < count; i++) {
        printf("|  %-10s|  %-23s|%6d  | %-10s  |\n",
               bills[i].ReceiptID, bills[i].CustomerName,
               bills[i].Amount, bills[i].Date);
    }

    printf("+------------+-------------------------+--------+-------------+\n");
}

void searchBill() {
    char keyword[50];
    printf("\nEnter Receipt ID or Customer Name to search : ");
    fgets(keyword, sizeof(keyword), stdin);
    keyword[strcspn(keyword, "\n")] = 0;

    toLowerStr(keyword);

    Bill bills[MAX_BILLS];
    int count = readAllBills(bills, MAX_BILLS);
    int found = 0;

    printf("\nSearch results:\n");
    printf("+------------+-------------------------+---------+------------+\n");
    printf("| Receipt ID | Customer Name           | Amount  | Date       |\n");
    printf("+------------+-------------------------+---------+------------+\n");

    for (int i = 0; i < count; i++) {
        char receiptLower[10], nameLower[50];
        strcpy(receiptLower, bills[i].ReceiptID);
        strcpy(nameLower, bills[i].CustomerName);
        toLowerStr(receiptLower);
        toLowerStr(nameLower);

        if (strstr(receiptLower, keyword) || strstr(nameLower, keyword)) {
            printf("| %-10s | %-23s | %7d | %-10s |\n",
                   bills[i].ReceiptID, bills[i].CustomerName,
                   bills[i].Amount, bills[i].Date);
            found = 1;
        }
    }

    printf("+------------+-------------------------+---------+------------+\n");

    if (!found)
        printf("No matching bill found.\n");
}

void updateBill() {
    char id[10];
    printf("\nEnter Receipt ID to update : ");
    scanf("%9s", id);
    getchar();

    id[0] = toupper((unsigned char)id[0]);
    if (!isValidReceiptID(id)) {
        printf("Error: Invalid Receipt ID format.\n");
        return;
    }

    Bill bills[MAX_BILLS];
    int count = readAllBills(bills, MAX_BILLS);
    int found = -1;

    for (int i = 0; i < count; i++) {
        if (strcmp(bills[i].ReceiptID, id) == 0) {
            found = i;
            break;
        }
    }

    if (found == -1) {
        printf("Receipt ID not found.\n");
        return;
    }

    char choice;
    do {
        printf("\nCurrent bill:\n");
        printf("%s | %s | %d | %s\n",
               bills[found].ReceiptID, bills[found].CustomerName,
               bills[found].Amount, bills[found].Date);

        printf("\nWhat do you want to update?\n");
        printf("a) Customer Name\n");
        printf("b) Amount\n");
        printf("c) Date\n");
        printf("x) Finish updating\n");
        printf("Enter choice: ");
        scanf(" %c", &choice);
        getchar();

        if (choice == 'a' || choice == 'A') {
            printf("\nEnter new Customer Name : ");
            fgets(bills[found].CustomerName, sizeof(bills[found].CustomerName), stdin);
            bills[found].CustomerName[strcspn(bills[found].CustomerName, "\n")] = 0;
            printf("Customer Name updated.\n");
        } else if (choice == 'b' || choice == 'B') {
            printf("\nEnter new Amount : ");
            scanf("%d", &bills[found].Amount);
            getchar();
            printf("Amount updated.\n");
        } else if (choice == 'c' || choice == 'C') {
            char tempDate[20];
            do {
                printf("\nEnter new Date (yyyy-mm-dd): ");
                scanf("%19s", tempDate);
                getchar();
                if (!validateAndFormatDate(tempDate, bills[found].Date))
                    printf("Error: Invalid or future date. Please try again.\n");
            } while (!validateAndFormatDate(tempDate, bills[found].Date));
            printf("Date updated.\n");
        } else if (choice == 'x' || choice == 'X') {
            break;
        } else {
            printf("Invalid choice.\n");
        }
    } while (1);

    char confirm;
    printf("\nAre you sure you want to save these changes? (y/n): ");
    scanf(" %c", &confirm);
    getchar();

    if (confirm == 'y' || confirm == 'Y') {
        writeAllBills(bills, count);
        printf("Bill updated successfully.\n");
    } else {
        printf("Update canceled.\n");
    }
}

void deleteBill() {
    char id[10];
    printf("\nEnter Receipt ID to delete : ");
    scanf("%9s", id);
    getchar();

    Bill bills[MAX_BILLS];
    int count = readAllBills(bills, MAX_BILLS);
    int found = 0;

    for (int i = 0; i < count; i++) {
        if (strcmp(bills[i].ReceiptID, id) == 0) {
            printf("\nBill found:\n");
            printf("%s | %s | %d | %s\n",
                   bills[i].ReceiptID, bills[i].CustomerName,
                   bills[i].Amount, bills[i].Date);

            char confirm;
            printf("\nAre you sure you want to delete this bill? (y/n): ");
            scanf(" %c", &confirm);
            getchar();

            if (confirm == 'y' || confirm == 'Y') {
                for (int j = i; j < count - 1; j++)
                    bills[j] = bills[j + 1];
                count--;
                writeAllBills(bills, count);
                printf("Bill deleted successfully.\n");
            } else {
                printf("Deletion canceled.\n");
            }

            found = 1;
            break;
        }
    }

    if (!found) {
        printf("Receipt ID not found.\n");
    }
}

// ===== UNIT TEST =====
void unitTest() {
    printf("\n==== UNIT TEST ====\n");

    printf("Test: isValidReceiptID(\"A123\") => %d (expected: 1)\n", isValidReceiptID("A123"));
    printf("Test: isValidReceiptID(\"1234\") => %d (expected: 0)\n", isValidReceiptID("1234"));
    printf("Test: isValidReceiptID(\"AB12\") => %d (expected: 0)\n", isValidReceiptID("AB12"));

    char formattedDate[20];
    printf("Test: validateAndFormatDate(\"2023-01-01\") => %d (expected: 1)\n", validateAndFormatDate("2023-01-01", formattedDate));
    printf("Test: validateAndFormatDate(\"2025-12-31\") => %d (expected: 0)\n", validateAndFormatDate("2025-12-31", formattedDate));
}

// ===== E2E TEST =====
void e2eTest() {
    printf("\n==== E2E TEST ====\n");

    printf("Adding bill A999...\n");
    FILE *fp = fopen(FILE_NAME, "a");
    fprintf(fp, "A999,TestUser,100,2023-01-01\n");
    fclose(fp);

    printf("Searching bill A999...\n");
    if (receiptExists("A999"))
        printf("E2E Test Passed: Bill A999 exists.\n");
    else
        printf("E2E Test Failed: Bill A999 not found.\n");

    printf("Deleting bill A999...\n");
    Bill bills[MAX_BILLS];
    int count = readAllBills(bills, MAX_BILLS);
    for (int i = 0; i < count; i++) {
        if (strcmp(bills[i].ReceiptID, "A999") == 0) {
            for (int j = i; j < count - 1; j++)
                bills[j] = bills[j + 1];
            count--;
            writeAllBills(bills, count);
            break;
        }
    }

    if (!receiptExists("A999"))
        printf("E2E Test Passed: Bill A999 deleted.\n");
    else
        printf("E2E Test Failed: Bill A999 still exists.\n");
}

void displayMenu() {
    printf("\n--------------------------------------------------------------");
        printf("\n                   BILL MANAGEMENT MENU\n");
        printf("--------------------------------------------------------------");
        printf("\nplease enter the number in front of menu that you want to use.\n");
        printf("______________________________________________________________\n\n");
        printf("  1.  SHOW ALL BILLS\n");
        printf("  2.  ADD NEW BILL\n");
        printf("  3.  SEARCH BILL\n");
        printf("  4.  UPDATE BILL\n");
        printf("  5.  DELETE BILL\n");
        printf("  6.  UNIT TEST\n");
        printf("  7.  END-TO-END TEST\n");
        printf("                                                0 -->  EXIT\n");
        printf("______________________________________________________________\n\n");
        printf("What do you want to do?\n");
        printf("Please enter your choice : ");
}

int main() {
    int choice;
    do {
        displayMenu();
        scanf("%d", &choice);
        getchar();

        switch (choice) {
            case 1: showBills(); break;
            case 2: addBill(); break;
            case 3: searchBill(); break;
            case 4: updateBill(); break;
            case 5: deleteBill(); break;
            case 6: unitTest(); break;
            case 7: e2eTest(); break;
            case 0: printf("Exiting...\n"); break;
            default: printf("Invalid choice.\n");
        }
    } while (choice != 0);

    return 0;
}
