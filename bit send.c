#include <stdio.h>
#include <string.h>

#define MAX 1000

char message[100];
char binary[MAX] = "";
char stuffed[MAX];
char frame[MAX];

void charToBinary(char ch)
{
    int i;

    for(i = 7; i >= 0; i--)
    {
        if((ch >> i) & 1)
            strcat(binary, "1");
        else
            strcat(binary, "0");
    }
}

int main()
{
    FILE *fp;
    int i, j = 0, count = 0;

    /* Read message from file */

    fp = fopen("message.txt", "r");

    if(fp == NULL)
    {
        printf("Cannot open message.txt");
        return 0;
    }

    fgets(message, 100, fp);
    fclose(fp);

    printf("Original Message : %s\n", message);

    /* Convert Message to Binary */

    for(i = 0; message[i] != '\0' && message[i] != '\n'; i++)
    {
        charToBinary(message[i]);
    }

    printf("Binary Data :\n%s\n", binary);

    /* Bit Stuffing */

    for(i = 0; binary[i] != '\0'; i++)
    {
        stuffed[j++] = binary[i];

        if(binary[i] == '1')
            count++;
        else
            count = 0;

        if(count == 5)
        {
            stuffed[j++] = '0';
            count = 0;
        }
    }

    stuffed[j] = '\0';

    printf("Stuffed Data :\n%s\n", stuffed);
    /* Create HDLC Frame */

    strcpy(frame, "01111110");
    strcat(frame, stuffed);
    strcat(frame, "01111110");

    printf("\nHDLC Frame :\n%s\n", frame);

    /* Store HDLC Frame in frame.txt */

    fp = fopen("frame.txt", "w");

    if(fp == NULL)
    {
        printf("Cannot create frame.txt");
        return 0;
    }

    fputs(frame, fp);
    fclose(fp);

    /* Store Sender Output in output.txt */

    fp = fopen("output.txt", "w");

    fprintf(fp, "********** HDLC SENDER **********\n\n");

    fprintf(fp, "Original Message :\n%s\n", message);

    fprintf(fp, "Binary Data :\n%s\n\n", binary);

    fprintf(fp, "Stuffed Data :\n%s\n\n", stuffed);

    fprintf(fp, "HDLC Frame :\n");
    fprintf(fp, "----------------------------------------\n");
    fprintf(fp, "| Flag | Stuffed Data | Flag |\n");
    fprintf(fp, "----------------------------------------\n");
    fprintf(fp, "|01111110|%s|01111110|\n", stuffed);
    fprintf(fp, "----------------------------------------\n");

    fclose(fp);

    printf("\nHDLC Frame saved in frame.txt");
    printf("\nSender output saved in output.txt");

    return 0;
}
