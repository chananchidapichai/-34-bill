#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FILE_NAME "bill.csv"
#define MAX_BILLS 1000

typedef struct {
    char ReceiptID[10];
    char CustomerName[50];
    int Amount;
    char Date[20];
} Bill;

int readAllBills(Bill bills[], int max) {
    FILE *fp = fopen(FILE_NAME, "r");
    if (!fp) return 0;

    int count = 0;
    while (count < max && fscanf(fp, "%[^,],%[^,],%d,%s\n", bills[count].ReceiptID, bills[count].CustomerName, &bills[count].Amount, bills[count].Date) == 4) {
        count++;
    }
    fclose(fp);
    return count;
}

void writeAllBills(Bill bills[], int count) {
    FILE *fp = fopen(FILE_NAME, "w");
    if (!fp) { printf("Cannot write file.\n"); return; }

    for (int i = 0; i < count; i++)
        fprintf(fp, "%s,%s,%d,%s\n", bills[i].ReceiptID, bills[i].CustomerName, bills[i].Amount, bills[i].Date);

    fclose(fp); 
}


int receiptExists(const char *id)
{
    Bill bills[MAX_BILLS];
    int count = readAllBills(bills, MAX_BILLS);
    for (int i = 0; i < count; i++)
        if (strcmp(bills[i].ReceiptID, id) == 0) return 1;
    return 0;
}

// Add new bill
void addBill()
{
    FILE *fp = fopen(FILE_NAME, "a");
    if (!fp) { printf("Cannot open file.\n"); return; }

    Bill b;
    int duplicate;
    do {
        duplicate = 0;
        printf("Enter Receipt ID: ");
        scanf("%9s", b.ReceiptID);
        getchar();
        if (receiptExists(b.ReceiptID)) {
            printf("Error: Receipt ID already exists.\n");
            duplicate = 1;
        }
    } while(duplicate);

    printf("Enter Customer Name: ");
    fgets(b.CustomerName, sizeof(b.CustomerName), stdin);
    b.CustomerName[strcspn(b.CustomerName, "\n")] = 0;

    printf("Enter Amount: ");
    scanf("%d", &b.Amount);
    getchar();

    printf("Enter Date (yyyy-mm-dd): ");
    scanf("%19s", b.Date);
    getchar();

    fprintf(fp, "%s, %s, %d, %s\n", b.ReceiptID, b.CustomerName, b.Amount, b.Date);
    fclose(fp);
    printf("Bill added successfully!\n");
}

// Show all bills
void showBills()
{
    Bill bills[MAX_BILLS];
    int count = readAllBills(bills, MAX_BILLS);
    if (count == 0) { printf("No bills to show.\n"); return; }

    printf("\nReceiptID | CustomerName | Amount | Date\n");
    printf("----------------------------------------\n");
    for (int i = 0; i < count; i++)
        printf("%s | %s | %d | %s\n", bills[i].ReceiptID, bills[i].CustomerName, bills[i].Amount, bills[i].Date);
}

// Search bill
void searchBill()
{
    char keyword[50];
    printf("Enter Receipt ID or Customer Name to search: ");
    fgets(keyword, sizeof(keyword), stdin);
    keyword[strcspn(keyword, "\n")] = 0;

    Bill bills[MAX_BILLS];
    int count = readAllBills(bills, MAX_BILLS);
    int found = 0;

    for (int i = 0; i < count; i++) {
        if (strstr(bills[i].ReceiptID, keyword) || strstr(bills[i].CustomerName, keyword)) {
            printf("%s | %s | %d | %s\n", bills[i].ReceiptID, bills[i].CustomerName, bills[i].Amount, bills[i].Date);
            found = 1;
        }
    }
    if (!found) printf("No matching bill found.\n");
}

// Update bill
void updateBill()
{
    char id[10];
    printf("Enter Receipt ID to update: ");
    scanf("%9s", id);
    getchar();

    Bill bills[MAX_BILLS];
    int count = readAllBills(bills, MAX_BILLS);
    int found = 0;

    for (int i = 0; i < count; i++) {
        if (strcmp(bills[i].ReceiptID, id) == 0) {
            printf("Enter new Customer Name: ");
            fgets(bills[i].CustomerName, sizeof(bills[i].CustomerName), stdin);
            bills[i].CustomerName[strcspn(bills[i].CustomerName, "\n")] = 0;

            printf("Enter new Amount: ");
            scanf("%d", &bills[i].Amount);
            getchar();

            printf("Enter new Date (yyyy-mm-dd): ");
            scanf("%19s", bills[i].Date);
            getchar();

            found = 1;
            break;
        }
    }

    if (found) {
        writeAllBills(bills, count);
        printf("Bill updated successfully.\n");
    } else {
        printf("Receipt ID not found.\n");
    }
}

// Delete bill
void deleteBill()
{
    char id[10];
    printf("Enter Receipt ID to delete: ");
    scanf("%9s", id);
    getchar();

    Bill bills[MAX_BILLS];
    int count = readAllBills(bills, MAX_BILLS);
    int found = 0;

    for (int i = 0; i < count; i++) {
        if (strcmp(bills[i].ReceiptID, id) == 0) {
            for (int j = i; j < count - 1; j++)
                bills[j] = bills[j + 1];
            count--;
            found = 1;
            break;
        }
    }

    if (found) {
        writeAllBills(bills, count);
        printf("Bill deleted successfully.\n");
    } else {
        printf("Receipt ID not found.\n");
    }
}

// Display menu
void displayMenu()
{
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

int main(void)
{
    displayMenu();
    return 0;
}


