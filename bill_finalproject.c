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

    int bill_no;
    char customer_name;
    double amount;
    char date[20];

    printf("Please enter bill number : ");
    scanf("%d", &bill_no);
    getchar();

    printf("Please enter name : ");
    fgets(customer_name, sizeof(customer_name), stdin);
    size_t len = strlen(customer_name);
    if(len > 0 && customer_name[len - 1] == '\n')
    {
        customer_name[len - 1] = '\0';
    }
    printf("Please enter price amount : ");
    scanf("%lf", &amount);
    getchar();

    printf("Please enter date (dd/mm/yyyy) : ");
    fgets(date, sizeof(date), stdin);
    len = strlen(date);
    if(len > 0 && date[len - 1] == '\n')
    {
        date[len - 1] = '\0';
    }

    fprintf(fp, "%d, %s, %.2f, %s\n", bill_no, customer_name, amount, date);
    printf("Data saved successfully to %s\n", bill);
    fclose(fp);

    return 0;
}