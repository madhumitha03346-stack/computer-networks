#include <stdio.h>
#include <string.h>

#define MAX 1000

char frame[MAX];
char stuffed[MAX];
char destuffed[MAX];

int main()
{
    FILE *fp;
    int i, j = 0, count = 0;
    int len;

    /* Read HDLC Frame */

    fp = fopen("frame.txt", "r");

    if(fp == NULL)
    {
        printf("Cannot open frame.txt");
        return 0;
    }

    fgets(frame, MAX, fp);
    fclose(fp);

    printf("Received HDLC Frame :\n%s\n", frame);

    /* Remove HDLC Flags */

    len = strlen(frame);

    if(frame[len - 1] == '\n')
        len--;

    j = 0;

    for(i = 8; i < len - 8; i++)
    {
        stuffed[j++] = frame[i];
    }

    stuffed[j] = '\0';

    printf("\nStuffed Data :\n%s\n", stuffed);

    /* Bit Destuffing */

    j = 0;
    count = 0;

    for(i = 0; stuffed[i] != '\0'; i++)
    {
        destuffed[j++] = stuffed[i];

        if(stuffed[i] == '1')
            count++;
        else
            count = 0;

        if(count == 5)
        {
            if(stuffed[i + 1] == '0')
                i++;

            count = 0;
        }
    }

    destuffed[j] = '\0';

    printf("\nDestuffed Data :\n%s\n", destuffed);

    /* Append Receiver Output */

    fp = fopen("output.txt", "a");

    fprintf(fp, "\n\n********** HDLC RECEIVER **********\n\n");

    fprintf(fp, "Received HDLC Frame :\n");
    fprintf(fp, "----------------------------------------\n");
    fprintf(fp, "| Flag | Stuffed Data | Flag |\n");
    fprintf(fp, "----------------------------------------\n");
    fprintf(fp, "|01111110|%s|01111110|\n", stuffed);
    fprintf(fp, "----------------------------------------\n\n");

    fprintf(fp, "After Removing Flags :\n");
    fprintf(fp, "%s\n\n", stuffed);

    fprintf(fp, "Destuffed Data :\n");
    fprintf(fp, "%s\n\n", destuffed);

    fprintf(fp, "HDLC Frame After Destuffing :\n");
    fprintf(fp, "----------------------------------------\n");
    fprintf(fp, "| Flag | Original Data | Flag |\n");
    fprintf(fp, "----------------------------------------\n");
    fprintf(fp, "|01111110|%s|01111110|\n", destuffed);
    fprintf(fp, "----------------------------------------\n\n");

    fprintf(fp, "Data Received Successfully.\n");

    fclose(fp);

    printf("\nReceiver output saved in output.txt\n");

    return 0;
}