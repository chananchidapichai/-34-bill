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

void ensureFileExists() {
    FILE *fp = fopen(FILE_NAME, "a");
    if (fp) fclose(fp);
}

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

void showBills() {
    Bill bills[MAX_BILLS];
    int count = readAllBills(bills, MAX_BILLS);
    printf("\n--- ALL BILLS ---\n");
    for (int i = 0; i < count; i++) {
        printf("%s | %s | %.2f | %s\n", bills[i].ReceiptID, bills[i].CustomerName, bills[i].Amount, bills[i].Date);
    }
}

void addBill() {
    Bill bill;
    printf("Enter Receipt ID: ");
    scanf("%s", bill.ReceiptID);
    getchar();
    if (!isValidReceiptID(bill.ReceiptID) || receiptExists(bill.ReceiptID)) {
        printf("Invalid or duplicate Receipt ID.\n");
        return;
    }
    printf("Enter Customer Name: ");
    fgets(bill.CustomerName, sizeof(bill.CustomerName), stdin);
    bill.CustomerName[strcspn(bill.CustomerName, "\n")] = 0;
    printf("Enter Amount: ");
    scanf("%lf", &bill.Amount);
    getchar();
    char date[20];
    printf("Enter Date (YYYY-MM-DD): ");
    scanf("%s", date);
    if (!validateAndFormatDate(date, bill.Date)) {
        printf("Invalid date.\n");
        return;
    }
    FILE *fp = fopen(FILE_NAME, "a");
    fprintf(fp, "%s,%s,%.2f,%s\n", bill.ReceiptID, bill.CustomerName, bill.Amount, bill.Date);
    fclose(fp);
    printf("Bill added successfully.\n");
}

void searchBill() {
    char id[10];
    printf("Enter Receipt ID to search: ");
    scanf("%s", id);
    Bill bills[MAX_BILLS];
    int count = readAllBills(bills, MAX_BILLS);
    for (int i = 0; i < count; i++) {
        if (strcmp(bills[i].ReceiptID, id) == 0) {
            printf("%s | %s | %.2f | %s\n", bills[i].ReceiptID, bills[i].CustomerName, bills[i].Amount, bills[i].Date);
            return;
        }
    }
    printf("Bill not found.\n");
}

void updateBill() {
    char id[10];
    printf("Enter Receipt ID to update: ");
    scanf("%s", id);
    Bill bills[MAX_BILLS];
    int count = readAllBills(bills, MAX_BILLS);
    for (int i = 0; i < count; i++) {
        if (strcmp(bills[i].ReceiptID, id) == 0) {
            printf("Enter new Customer Name: ");
            getchar();
            fgets(bills[i].CustomerName, sizeof(bills[i].CustomerName), stdin);
            bills[i].CustomerName[strcspn(bills[i].CustomerName, "\n")] = 0;
            printf("Enter new Amount: ");
            scanf("%lf", &bills[i].Amount);
            getchar();
            char date[20];
            printf("Enter new Date (YYYY-MM-DD): ");
            scanf("%s", date);
            if (!validateAndFormatDate(date, bills[i].Date)) {
                printf("Invalid date.\n");
                return;
            }
            writeAllBills(bills, count);
            printf("Bill updated.\n");
            return;
        }
    }
    printf("Bill not found.\n");
}

void deleteBill() {
    char id[10];
    printf("Enter Receipt ID to delete: ");
    scanf("%s", id);
    Bill bills[MAX_BILLS];
    int count = readAllBills(bills, MAX_BILLS);
    int found = 0;
    for (int i = 0; i < count; i++) {
        if (strcmp(bills[i].ReceiptID, id) == 0) {
            found = 1;
            for (int j = i; j < count - 1; j++) bills[j] = bills[j+1];
            count--;
            break;
        }
    }
    if (found) {
        writeAllBills(bills, count);
        printf("Bill deleted.\n");
    } else {
        printf("Bill not found.\n");
    }
}

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
    // Add valid bill
    Bill newBill = {"B001", "Alice", 200.5, "2020-12-12"};
    fp = fopen(FILE_NAME, "a");
    fprintf(fp, "%s,%s,%.2f,%s\n", newBill.ReceiptID, newBill.CustomerName, newBill.Amount, newBill.Date);
    fclose(fp);
    printf(receiptExists("B001") ? "PASS\n" : "FAIL\n");

    // Add bill with duplicate ID
    printf(receiptExists("A123") ? "PASS\n" : "FAIL\n");

    // Add bill with invalid date
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

    // Try updating non-existent bill
    updated = 0;
    for (int i = 0; i < count; i++) {
        if (strcmp(bills[i].ReceiptID, "Z999") == 0) {
            updated = 1;
            break;
        }
    }
    printf(!updated ? "PASS\n" : "FAIL\n");

    // Update bill with invalid date
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

void displayMenu() {
    ensureFileExists();
    int choice;
    do {
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
        printf("                                                0 -->  EXIT\n");
        printf("______________________________________________________________\n\n");
        printf("What do you want to do?\n");
        printf("Please enter your choice : ");
        scanf("%d", &choice);
        getchar();

        switch(choice) {
            case 1: showBills(); break;
            case 2: addBill(); break;
            case 3: searchBill(); break;
            case 4: updateBill(); break;
            case 5: deleteBill(); break;
            case 6: runUnitTests(); break;
            case 0: printf("Exiting program.\n"); break;
            default: printf("Invalid choice. Try again.\n");
        }
    } while(choice != 0);
}

int main() {
    displayMenu();
    return 0;
}
