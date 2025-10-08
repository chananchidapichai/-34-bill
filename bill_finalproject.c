#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define FILE_NAME "bill.csv"
#define MAX_BILLS 1000

typedef struct {
    char ReceiptID[10];
    char CustomerName[50];
    int Amount;
    char Date[20];
} Bill;

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
        printf("\nEnter Receipt ID : ");
        scanf("%9s", b.ReceiptID);
        getchar();
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

    Bill bills[MAX_BILLS];
    int count = readAllBills(bills, MAX_BILLS);
    int found = 0;

    for (int i = 0; i < count; i++) {
        if (strstr(bills[i].ReceiptID, keyword) || strstr(bills[i].CustomerName, keyword)) {
            printf("\n%s | %s | %d | %s\n",
                   bills[i].ReceiptID, bills[i].CustomerName,
                   bills[i].Amount, bills[i].Date);
            found = 1;
        }
    }
    if (!found) printf("No matching bill found.\n");
}

void updateBill() {
    char id[10];
    printf("Enter Receipt ID to update: ");
    scanf("%9s", id);
    getchar();

    Bill bills[MAX_BILLS];
    int count = readAllBills(bills, MAX_BILLS);
    int found = 0;

    for (int i = 0; i < count; i++) {
        if (strcmp(bills[i].ReceiptID, id) == 0) {
            printf("\nCurrent bill:\n");
            printf("%s | %s | %d | %s\n",
                   bills[i].ReceiptID, bills[i].CustomerName,
                   bills[i].Amount, bills[i].Date);

            printf("\nEnter new Customer Name: ");
            fgets(bills[i].CustomerName, sizeof(bills[i].CustomerName), stdin);
            bills[i].CustomerName[strcspn(bills[i].CustomerName, "\n")] = 0;

            printf("Enter new Amount: ");
            scanf("%d", &bills[i].Amount);
            getchar();

            char tempDate[20];
            do {
                printf("Enter new Date (yyyy-mm-dd): ");
                scanf("%19s", tempDate);
                getchar();
                if (!validateAndFormatDate(tempDate, bills[i].Date))
                    printf("Error: Invalid or future date. Please try again.\n");
            } while (!validateAndFormatDate(tempDate, bills[i].Date));

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

            found = 1;
            break;
        }
    }

    if (!found) {
        printf("Receipt ID not found.\n");
    }
}

void deleteBill() {
    char id[10];
    printf("Enter Receipt ID to delete: ");
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
            case 0: printf("Exiting program.\n"); break;
            default: printf("Invalid choice. Try again.\n");
        }
    } while(choice != 0);
}

int main(void) {
    ensureFileExists();
    displayMenu();
    return 0;
}
