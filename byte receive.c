#include <stdio.h>
#include <string.h>

#define MAX 1000

char flag[]="01111110";
char escape[]="01111101";

char frame[MAX];
char stuffed[MAX];
char destuffed[MAX];
char message[100];

void byteDestuffing(char input[],char output[])
{
    int i=0,j=0;
    char byte[9];

    while(input[i]!='\0')
    {
        int k;

        for(k=0;k<8;k++)
            byte[k]=input[i+k];

        byte[8]='\0';

        if(strcmp(byte,escape)==0)
        {
            i=i+8;

            for(k=0;k<8;k++)
                byte[k]=input[i+k];

            byte[8]='\0';
        }

        strcpy(&output[j],byte);

        j=j+8;
        i=i+8;
    }

    output[j]='\0';
}

void binaryToMessage(char binary[],char msg[])
{
    int i=0,j=0;

    while(binary[i]!='\0')
    {
        int value=0;
        int k;

        for(k=0;k<8;k++)
        {
            value=value*2+(binary[i+k]-'0');
        }

        msg[j]=(char)value;

        j++;
        i=i+8;
    }

    msg[j]='\0';
}

int main()
{
    FILE *fp;
    int i,j=0,len;

    fp=fopen("frame.txt","r");

    if(fp==NULL)
    {
        printf("Cannot open frame.txt");
        return 0;
    }

    fgets(frame,MAX,fp);
    fclose(fp);

    printf("Received PPP Frame :\n%s\n",frame);

    len=strlen(frame);

    if(frame[len-1]=='\n')
        len--;

    for(i=8;i<len-8;i++)
    {
        stuffed[j]=frame[i];
        j++;
    }

    stuffed[j]='\0';

    printf("\nStuffed Data :\n%s\n",stuffed);

    byteDestuffing(stuffed,destuffed);

    printf("\nDestuffed Binary :\n%s\n",destuffed);

    binaryToMessage(destuffed,message);

    printf("\nOriginal Message : %s\n",message);
fp = fopen("output.txt", "a");

    if(fp == NULL)
    {
        printf("Cannot open output.txt");
        return 0;
    }

    fprintf(fp, "\n\n********** PPP RECEIVER **********\n\n");

    fprintf(fp, "Received PPP Frame :\n");
    fprintf(fp, "------------------------------------------\n");
    fprintf(fp, "| Flag | Stuffed Data | Flag |\n");
    fprintf(fp, "------------------------------------------\n");
    fprintf(fp, "|%s|%s|%s|\n", flag, stuffed, flag);
    fprintf(fp, "------------------------------------------\n\n");

    fprintf(fp, "After Removing Flags :\n%s\n\n", stuffed);

    fprintf(fp, "Destuffed Binary :\n%s\n\n", destuffed);

    fprintf(fp, "Original Message :\n%s\n\n", message);

    fprintf(fp, "PPP Frame After Destuffing :\n");
    fprintf(fp, "------------------------------------------\n");
    fprintf(fp, "| Flag | Original Data | Flag |\n");
    fprintf(fp, "------------------------------------------\n");
    fprintf(fp, "|%s|%s|%s|\n", flag, destuffed, flag);
    fprintf(fp, "------------------------------------------\n");

    fprintf(fp, "\nData Received Successfully.\n");

    fclose(fp);

    printf("\nReceiver output saved in output.txt\n");

    return 0;
}
