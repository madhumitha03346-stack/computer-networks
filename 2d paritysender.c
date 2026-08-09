#include <stdio.h>
#include <string.h>

#define MAX 100
#define BITS 8

int data[MAX][BITS];
int rowParity[MAX];
int colParity[BITS];

int main()
{
    char message[MAX];
    int rows, i, j;

    FILE *fp;

    printf("=====================================\n");
    printf("      2-D PARITY SENDER\n");
    printf("=====================================\n");

    printf("Enter the message : ");
    fgets(message, sizeof(message), stdin);

    message[strcspn(message, "\n")] = '\0';

    rows = strlen(message);

    printf("\nOriginal Message : %s\n", message);

    printf("\nASCII Values\n");
    printf("----------------------\n");

    for(i = 0; i < rows; i++)
    {
        printf("%c = %d\n", message[i], message[i]);
    }

    printf("\nBinary Equivalent\n");
    printf("----------------------\n");

    for(i = 0; i < rows; i++)
    {
        unsigned char ch = message[i];

        for(j = 7; j >= 0; j--)
        {
            data[i][7-j] = (ch >> j) & 1;
            printf("%d", data[i][7-j]);
        }

        printf("   (%c)\n", message[i]);
    }

    /* Row parity */

    for(i = 0; i < rows; i++)
    {
        int count = 0;

        for(j = 0; j < BITS; j++)
        {
            if(data[i][j] == 1)
                count++;
        }

        rowParity[i] = count % 2;
    }

    /* Column parity */

    for(j = 0; j < BITS; j++)
    {
        int count = 0;

        for(i = 0; i < rows; i++)
        {
            if(data[i][j] == 1)
                count++;
        }

        colParity[j] = count % 2;
    }

    printf("\n\n2-D PARITY MATRIX\n");
    printf("-----------------------------------------------------------\n");
    printf("Char\t");

    for(j = 1; j <= BITS; j++)
        printf("B%d ", j);

    printf("| Row Parity\n");
    printf("-----------------------------------------------------------\n");

    for(i = 0; i < rows; i++)
    {
        printf("%c\t", message[i]);

        for(j = 0; j < BITS; j++)
        {
            printf("%d  ", data[i][j]);
        }

        printf("|     %d\n", rowParity[i]);
    }

    printf("-----------------------------------------------------------\n");

    printf("CP\t");

    for(j = 0; j < BITS; j++)
        printf("%d  ", colParity[j]);

    printf("|\n");

    /* Write to output.txt */

    fp = fopen("output.txt", "w");

    if(fp == NULL)
    {
        printf("Cannot create output.txt\n");
        return 0;
    }

    fprintf(fp, "%d\n", rows);

    for(i = 0; i < rows; i++)
    {
        fprintf(fp, "%c\n", message[i]);
    }

    fprintf(fp, "\n");

    for(i = 0; i < rows; i++)
    {
        for(j = 0; j < BITS; j++)
            fprintf(fp, "%d ", data[i][j]);

        fprintf(fp, "%d\n", rowParity[i]);
    }

    for(j = 0; j < BITS; j++)
        fprintf(fp, "%d ", colParity[j]);

    fprintf(fp, "\n");

    fclose(fp);

    printf("\nData successfully written into output.txt\n");

    printf("\nRow Parity Bits\n");
    printf("----------------------\n");

    for(i = 0; i < rows; i++)
    {
        printf("Row %d : %d\n", i+1, rowParity[i]);
    }

    printf("\nColumn Parity Bits\n");
    printf("----------------------\n");

    for(j = 0; j < BITS; j++)
    {
        printf("Column %d : %d\n", j+1, colParity[j]);
    }

    printf("\nSender completed successfully.\n");

    return 0;
}