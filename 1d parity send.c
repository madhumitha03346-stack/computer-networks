#include <stdio.h>
#include <string.h>

char str[100];
unsigned char data[100];
int n;
FILE *fp;

void getinput()
{
    printf("Enter string: ");
    fgets(str, 100, stdin);
    n = strlen(str);
    if(str[n-1]=='\n')
    {
        str[n-1]='\0';
        n--;
    }
}

void makeparity()
{
    int i, j, count, bit;
    unsigned char ch;
    for(i=0;i<n;i++)
    {
        ch = str[i] & 0x7F;
        count = 0;
        for(j=0;j<7;j++)
        {
            bit = (ch >> j) & 1;
            if(bit==1)
                count++;
        }
        if(count%2==0)
            ch = ch & 0x7F;
        else
            ch = ch | 0x80;
        data[i] = ch;
    }
}

void display()
{
    int i, j, bit;
    printf("\nSent data (8 bits, MSB = parity):\n");
    for(i=0;i<n;i++)
    {
        printf("%c : ", str[i]);
        for(j=7;j>=0;j--)
        {
            bit = (data[i]>>j) & 1;
            printf("%d",bit);
        }
        printf("\n");
    }
}

void writefile()
{
    fp = fopen("channel.dat","wb");
    fwrite(&n, sizeof(int), 1, fp);
    fwrite(data, sizeof(unsigned char), n, fp);
    fclose(fp);
}

int main()
{
    getinput();
    makeparity();
    display();
    writefile();
    printf("\nData sent to receiver.\n");
    return 0;
}

