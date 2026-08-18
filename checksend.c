#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX 1000

char flag[]="01111110";
char escape[]="01111101";

char message[100];
char binary[MAX]="";
char stuffed[MAX];
char frame[MAX];

void charToBinary(char ch, char bin[])
{
    int i;
    for(i=7; i>=0; i--)
    {
        if((ch>>i)&1)
            bin[7-i]='1';
        else
            bin[7-i]='0';
    }
    bin[8]='\0';
}

void messageToBinary(char msg[], char binary[])
{
    int i;
    char temp[9];
    binary[0]='\0';
    for(i=0; msg[i]!='\0' && msg[i]!='\n' && msg[i]!='\r'; i++)
    {
        charToBinary(msg[i], temp);
        strcat(binary, temp);
    }
}

void byteStuffing(char input[], char output[])
{
    int i=0, j=0;
    char byte[9];
    while(input[i] != '\0')
    {
        int k;
        for(k=0; k<8; k++)
            byte[k]=input[i+k];
        byte[8]='\0';
        if(strcmp(byte, flag)==0 || strcmp(byte, escape)==0)
        {
            strcpy(&output[j], escape);
            j+=8;
        }
        strcpy(&output[j], byte);
        j+=8;
        i+=8;
    }
    output[j]='\0';
}

unsigned short calculateChecksum(char stuffedData[])
{
    unsigned long sum = 0;
    int len = strlen(stuffedData);
    int i, bit;

    for (i = 0; i < len; i += 16)
    {
        unsigned short word = 0;
        for (bit = 0; bit < 16; bit++)
        {
            word <<= 1;
            if (i + bit < len && stuffedData[i + bit] == '1') {
                word |= 1;
            }
        }
        sum += word;
    }

    while (sum >> 16) {
        sum = (sum & 0xFFFF) + (sum >> 16);
    }

    return (unsigned short)(~sum);
}

void checksumToBinaryString(unsigned short checksum, char outputStr[])
{
    int i;
    for (i = 15; i >= 0; i--) {
        outputStr[15 - i] = ((checksum >> i) & 1) ? '1' : '0';
    }
    outputStr[16] = '\0';
}

int main()
{
    FILE *fp;
    char checksumBinary[17];
    unsigned short checksumVal;
    int msgLen, stuffedLen, remainder, paddingNeeded, p;

    printf("Enter the message to send: ");
    fgets(message, 100, stdin);

    msgLen = strlen(message);
    if(message[msgLen-1] == '\n' || message[msgLen-1] == '\r') {
        message[msgLen-1] = '\0';
    }

    fp = fopen("message.txt", "w");
    if(fp == NULL) { printf("Cannot write to message.txt\n"); return 0; }
    fputs(message, fp);
    fclose(fp);

    messageToBinary(message, binary);
    byteStuffing(binary, stuffed);

    stuffedLen = strlen(stuffed);
    remainder = stuffedLen % 16;
    if (remainder != 0) {
        paddingNeeded = 16 - remainder;
        for (p = 0; p < paddingNeeded; p++) {
            stuffed[stuffedLen + p] = '0';
    }
        stuffed[stuffedLen + paddingNeeded] = '\0';
    }

    printf("\nOriginal Message Saved: %s\n", message);
    printf("Binary Data :\n%s\n", binary);
    printf("Stuffed Data (16-bit Padded) :\n%s\n", stuffed);

    checksumVal = calculateChecksum(stuffed);
    checksumToBinaryString(checksumVal, checksumBinary);
    printf("Calculated Checksum (Binary): %s\n", checksumBinary);

    strcpy(frame, flag);
    strcat(frame, stuffed);
    strcat(frame, checksumBinary);
    strcat(frame, flag);

    printf("\nPPP Frame Layout:\n| %s | %s | %s | %s |\n", flag, stuffed, checksumBinary, flag);

    fp = fopen("frame.txt", "w");
    if(fp == NULL) { printf("Cannot create frame.txt\n"); return 0; }
    fputs(frame, fp);
    fclose(fp);

    fp = fopen("output.txt", "w");
    if(fp == NULL) { printf("Cannot create output.txt\n"); return 0; }
    fprintf(fp, "********** PPP SENDER **********\n\n");
    fprintf(fp, "Original Message :\n%s\n", message);
    fprintf(fp, "Binary Data :\n%s\n\n", binary);
    fprintf(fp, "Stuffed Data (Padded) :\n%s\n\n", stuffed);
    fprintf(fp, "Calculated Checksum (Binary): %s\n\n", checksumBinary);
    fprintf(fp, "PPP Frame Layout:\n| %s | %s | %s | %s |\n", flag, stuffed, checksumBinary, flag);
    fclose(fp);

    printf("\nPPP Frame saved successfully in frame.txt\n");
    printf("Sender output log saved in output.txt\n");
    return 0;
}
