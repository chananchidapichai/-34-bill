#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main()
{
    FILE *fp;
    char bill[] = "bill.csv";

    fp = fopen(bill, "a");
    if(fp == NULL)
    {
        printf("Unable to open file %s\n", bill);
        return 1;
    }

    char ReceiptID[10];
    char CustomerName[50];
    int Amount;
    char Date[20];

    printf("Please enter receipt ID number : ");
    scanf("%9s", &ReceiptID);
    getchar();

    printf("Please enter name : ");
    fgets(CustomerName, sizeof(CustomerName), stdin);
    size_t len = strlen(CustomerName);
    if(len > 0 && CustomerName[len - 1] == '\n')
    {
        CustomerName[len - 1] = '\0';
    }
    printf("Please enter price amount : ");
    scanf("%d", &Amount);
    getchar();

    printf("Please enter date (yyyy-mm-dd) : ");
    fgets(Date, sizeof(Date), stdin);
    len = strlen(Date);
    if(len > 0 && Date[len - 1] == '\n')
    {
        Date[len - 1] = '\0';
    }

    fprintf(fp, "%s, %s, %d, %s\n", ReceiptID, CustomerName, Amount, Date);
    printf("Data saved successfully to %s\n", bill);
    fclose(fp);

    return 0;
}
