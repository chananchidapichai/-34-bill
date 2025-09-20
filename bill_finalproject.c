#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FILE_NAME "bill.csv"

typedef struct {
    char ReceiptID[10];
    char CustomerName[50];
    int Amount;
    char Date[20];
} Bill;

int receiptExists(const char *id)
{
    FILE *fp = fopen(FILE_NAME, "r");
    if (!fp) return 0;

    Bill b;
    while (fscanf(fp, "%[^,],%[^,],%d,%s\n", b.ReceiptID, b.CustomerName, &b.Amount, b.Date) == 4)
    {
        if (strcmp(b.ReceiptID, id) == 0) {
            fclose(fp);
            return 1;
        }
    }
    fclose(fp);
    return 0;
}

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

    fprintf(fp, "%s,%s,%d,%s\n", b.ReceiptID, b.CustomerName, b.Amount, b.Date);
    fclose(fp);
    printf("Bill added successfully!\n");
}

void showBills()
{
    FILE *fp = fopen(FILE_NAME, "r");
    if (!fp) { printf("No bills to show.\n"); return; }

    Bill b;
    printf("\nReceiptID | CustomerName | Amount | Date\n");
    printf("----------------------------------------\n");

    while (fscanf(fp, "%[^,],%[^,],%d,%s\n", b.ReceiptID, b.CustomerName, &b.Amount, b.Date) == 4)
        printf("%s | %s | %d | %s\n", b.ReceiptID, b.CustomerName, b.Amount, b.Date);

    fclose(fp);
}

void searchBill()
{
    char keyword[50];
    printf("Enter Receipt ID or Customer Name to search: ");
    fgets(keyword, sizeof(keyword), stdin);
    keyword[strcspn(keyword, "\n")] = 0;

    FILE *fp = fopen(FILE_NAME, "r");
    if (!fp) { printf("No bills found.\n"); return; }

    Bill b;
    int found = 0;
    while (fscanf(fp, "%[^,],%[^,],%d,%s\n", b.ReceiptID, b.CustomerName, &b.Amount, b.Date) == 4)
    {
        if (strstr(b.ReceiptID, keyword) || strstr(b.CustomerName, keyword))
        {
            printf("%s | %s | %d | %s\n", b.ReceiptID, b.CustomerName, b.Amount, b.Date);
            found = 1;
        }
    }
    if (!found) printf("No matching bill found.\n");
    fclose(fp);
}

void updateBill()
{
    char id[10];
    printf("Enter Receipt ID to update: ");
    scanf("%9s", id);
    getchar();

    FILE *fp = fopen(FILE_NAME, "r");
    if (!fp) { printf("No bills found.\n"); return; }

    FILE *temp = fopen("temp.csv", "w");
    if (!temp) { printf("Cannot create temp file.\n"); fclose(fp); return; }

    Bill b;
    int found = 0;
    while (fscanf(fp, "%[^,],%[^,],%d,%s\n", b.ReceiptID, b.CustomerName, &b.Amount, b.Date) == 4)
    {
        if (strcmp(b.ReceiptID, id) == 0)
        {
            printf("Enter new Customer Name: ");
            fgets(b.CustomerName, sizeof(b.CustomerName), stdin);
            b.CustomerName[strcspn(b.CustomerName, "\n")] = 0;

            printf("Enter new Amount: ");
            scanf("%d", &b.Amount);
            getchar();

            printf("Enter new Date (yyyy-mm-dd): ");
            scanf("%19s", b.Date);
            getchar();

            found = 1;
        }
        fprintf(temp, "%s,%s,%d,%s\n", b.ReceiptID, b.CustomerName, b.Amount, b.Date);
    }

    fclose(fp);
    fclose(temp);

    if (found)
    {
        remove(FILE_NAME);
        rename("temp.csv", FILE_NAME);
        printf("Bill updated successfully.\n");
    }
    else
    {
        remove("temp.csv");
        printf("Receipt ID not found.\n");
    }
}

void deleteBill()
{
    char id[10];
    printf("Enter Receipt ID to delete: ");
    scanf("%9s", id);
    getchar();

    FILE *fp = fopen(FILE_NAME, "r");
    if (!fp) { printf("No bills found.\n"); return; }

    FILE *temp = fopen("temp.csv", "w");
    if (!temp) { printf("Cannot create temp file.\n"); fclose(fp); return; }

    Bill b;
    int found = 0;
    while (fscanf(fp, "%[^,],%[^,],%d,%s\n", b.ReceiptID, b.CustomerName, &b.Amount, b.Date) == 4)
    {
        if (strcmp(b.ReceiptID, id) != 0)
            fprintf(temp, "%s,%s,%d,%s\n", b.ReceiptID, b.CustomerName, b.Amount, b.Date);
        else
            found = 1;
    }

    fclose(fp);
    fclose(temp);

    if (found)
    {
        remove(FILE_NAME);
        rename("temp.csv", FILE_NAME);
        printf("Bill deleted successfully.\n");
    }
    else
    {
        remove("temp.csv");
        printf("Receipt ID not found.\n");
    }
}

void displayMenu()
{
    int choice;
    do {
        printf("\n===== BILL MANAGEMENT MENU =====\n");
        printf("1. Show all bills\n");
        printf("2. Add new bill\n");
        printf("3. Search bill\n");
        printf("4. Update bill\n");
        printf("5. Delete bill\n");
        printf("0. Exit\n");
        printf("Enter your choice: ");
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

