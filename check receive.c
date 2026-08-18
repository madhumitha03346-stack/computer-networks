#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX 1000

char flag[]="01111110";
char escape[]="01111101";

char frame[MAX];
char stuffed[MAX];
char receivedChecksum[17];
char destuffed[MAX];
char message[100];

void byteDestuffing(char input[], char output[])
{
    int i=0, j=0;
    char byte[9];

    while(input[i] != '\0')
    {
        int k;
        for(k=0; k<8; k++)
            byte[k]=input[i+k];
        byte[8]='\0';

        if(strcmp(byte, escape) == 0)
        {
            i = i + 8;
            for(k=0; k<8; k++)
                byte[k]=input[i+k];
            byte[8]='\0';
        }

        if (byte[0] == '\0') break;

        strcpy(&output[j], byte);
        j = j + 8;
        i = i + 8;
    }
    output[j]='\0';
}

void binaryToMessage(char binary[], char msg[])
{
    int i=0, j=0;
    while(binary[i] != '\0' && binary[i+7] != '\0')
    {
        int value=0;
        int k;
        for(k=0; k<8; k++)
        {
            value = value * 2 + (binary[i+k] - '0');
        }
        if (value == 0) break;
        msg[j] = (char)value;
        j++;
        i = i + 8;
    }
    msg[j]='\0';
}

unsigned short calculateActualChecksum(char stuffedData[], char checksumStr[])
{
    unsigned long sum = 0;
    char combined[MAX];
    int len, i, bit;

    strcpy(combined, stuffedData);
    strcat(combined, checksumStr);
    len = strlen(combined);

    for (i = 0; i < len; i += 16)
    {
        unsigned short word = 0;
        for (bit = 0; bit < 16; bit++)
        {
            word <<= 1;
            if (i + bit < len && combined[i + bit] == '1') {
                word |= 1;
            }
        }
        sum += word;
    }

    while (sum >> 16) {
        sum = (sum & 0xFFFF) + (sum >> 16);
    }
    return (unsigned short)(sum);
}
void intToBinary16(unsigned short value, char outputBits[])
{
    int i;
    for (i = 15; i >= 0; i--) {
        outputBits[i] = (value & 1) ? '1' : '0';
        value >>= 1;
    }
    outputBits[16] = '\0';
}

int main()
{
    FILE *fp;
    int i, j = 0, len, dataEndIndex;
    int simulateError, bitPos;
    int status;
    unsigned short actualChecksumValue;
    char checksumBinaryStr[17];

    fp = fopen("frame.txt", "r");
    if(fp == NULL) { printf("Cannot open frame.txt\n"); return 0; }
    fgets(frame, MAX, fp);
    fclose(fp);

    len = strlen(frame);
    while(len > 0 && (frame[len-1] == '\n' || frame[len-1] == '\r')) {
        frame[len-1] = '\0';
        len = strlen(frame);
    }

    printf("Received PPP Frame :\n%s\n", frame);

    dataEndIndex = len - 8 - 16;

    j = 0;
    for(i = 8; i < dataEndIndex; i++) {
        stuffed[j++] = frame[i];
    }
    stuffed[j] = '\0';

    j = 0;
    for(i = dataEndIndex; i < len - 8; i++) {
        receivedChecksum[j++] = frame[i];
    }
    receivedChecksum[j] = '\0';

    printf("\nExtracted Stuffed Data :\n%s\n", stuffed);
    printf("Extracted Checksum Binary :\n%s\n", receivedChecksum);

    printf("\nDo you want to simulate a bit error? (1 for Yes, 0 for No): ");
    scanf("%d", &simulateError);

    if (simulateError == 1)
    {
        printf("Enter bit index position to flip (0 to %d): ", (int)(strlen(stuffed) - 1));
        scanf("%d", &bitPos);

        if (bitPos >= 0 && bitPos < (int)strlen(stuffed))
        {
            stuffed[bitPos] = (stuffed[bitPos] == '0') ? '1' : '0';
            printf("\nData Stream after simulated error:\n%s\n", stuffed);
        }
    }

    actualChecksumValue = calculateActualChecksum(stuffed, receivedChecksum);
    intToBinary16(actualChecksumValue, checksumBinaryStr);
    
    if (status == 1)
    {
        printf("\n>>> RESULT: VALID <<<\n");
        printf("Calculated Checksum Status: %d\n", status);
        printf("Calculated Checksum: %s\n", checksumBinaryStr);
        byteDestuffing(stuffed, destuffed);
        binaryToMessage(destuffed, message);
        printf("Decoded Message: %s\n", message);
    }
    else
    {
        printf("\n>>> RESULT: INVALID <<<\n");
        printf("Calculated Checksum Status: %d\n", status);
        printf("Calculated Checksum: %s\n", checksumBinaryStr);
        printf("Integrity check failed.\n");
    }

    fp = fopen("output.txt", "a");
    if(fp != NULL)
    {
        fprintf(fp, "\n\n********** PPP RECEIVER **********\n\n");
        fprintf(fp, "Received PPP Frame :\n%s\n\n", frame);
        if (status == 1) {
            fprintf(fp, "Verification Result: VALID\n");
            fprintf(fp, "Calculated Checksum Status: %d\n", status);
            fprintf(fp, "Calculated Checksum: %s\n", checksumBinaryStr);
            fprintf(fp, "Original Message: %s\n", message);
        } else {
            fprintf(fp, "Verification Result: INVALID\n");
            fprintf(fp, "Calculated Checksum Status: %d\n", status);
            fprintf(fp, "Calculated Checksum: %s\n", checksumBinaryStr);
        }
        fclose(fp);
    }

    printf("\nReceiver results appended to output.txt\n");
    return 0;
}
