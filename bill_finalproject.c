#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void)
{
    FILE *fp;
    char bill[] = "bill.csv";

    fp = fopen(bill, "a");
    if (fp == NULL)
    {
        printf("Unable to open file %s\n", bill);
        return 1;
    }

    char ReceiptID[10];
    char CustomerName[50];
    int Amount;
    char Date[20];
    int duplicate;

    do
    {
        duplicate = 0;
        printf("Please enter receipt ID number : ");
        if (scanf("%9s", ReceiptID) != 1)
        {
            printf("Invalid input.\n");
            return 1;
        }

        int ch;
        while ((ch = getchar()) != '\n' && ch != EOF);

        FILE *fr = fopen(bill, "r");
        if (fr)
        {
            char line[256];
            while (fgets(line, sizeof(line), fr))
            {
                char *token = strtok(line, ",");
                if (token && strcmp(token, ReceiptID) == 0)
                {
                    printf("Error: Receipt ID already exists. Please try again.\n");
                    duplicate = 1;
                    break;
                }
            }
            fclose(fr);
        }
    } while (duplicate);

    printf("Please enter name : ");
    fgets(CustomerName, sizeof(CustomerName), stdin);
    size_t len = strlen(CustomerName);
    if (len > 0 && CustomerName[len - 1] == '\n')
        CustomerName[len - 1] = '\0';

    printf("Please enter price amount : ");
    scanf("%d", &Amount);
    getchar();

    printf("Please enter date (yyyy-mm-dd) : ");
    scanf("%19s", &Date);
    getchar();

    fprintf(fp, "%s, %s, %d, %s\n", ReceiptID, CustomerName, Amount, Date);
    fclose(fp);
    printf("Data saved successfully to %s\n", bill);

    return 0;
}
