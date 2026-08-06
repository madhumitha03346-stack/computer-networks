#include<stdio.h>
#include<string.h>


int main()
{

    char code[200];
    int result[200];

    int h[200];

    int n;
    int i,j;


    FILE *fp;



    printf("========================================\n");
    printf("         HAMMING CODE RECEIVER\n");
    printf("========================================\n");



    fp=fopen("o2.txt","r");


    if(fp==NULL)
    {
        printf("\no2.txt file not found\n");
        return 0;
    }



    fscanf(fp,"%s",code);

    fclose(fp);



    n=strlen(code);



    printf("\nStep 1 : Reading Received Hamming Code\n");

    printf("----------------------------------------\n");

    printf("Received Code : %s\n",code);




    /*
        Convert received code into position array

        Example:

        Code : 101011

        Position:

        6 5 4 3 2 1

    */


    j=1;


    for(i=n-1;i>=0;i--)
    {
        h[j]=code[i]-'0';
        j++;
    }




    printf("\nReceived Bit Positions\n");

    printf("----------------------------------------\n");


    printf("Position : ");

    for(i=n;i>=1;i--)
        printf("%3d",i);



    printf("\nBit      : ");

    for(i=n;i>=1;i--)
        printf("%3d",h[i]);


    printf("\n");
	/* -----------------------------------------
   Simulate Transmission Error
----------------------------------------- */

int choice;
int bit;

printf("\nDo you want to introduce an error? (1-Yes / 0-No) : ");
scanf("%d",&choice);

if(choice==1)
{
    printf("Enter bit position to flip (1-%d) : ",n);
    scanf("%d",&bit);

    if(bit>=1 && bit<=n)
    {
        printf("\nTransmitted Bit Before Error : %d\n",h[bit]);

        h[bit]=!h[bit];

        printf("Transmitted Bit After Error  : %d\n",h[bit]);

        printf("\nReceived Hamming Code with Error : ");

        for(i=n;i>=1;i--)
            printf("%d",h[i]);

        printf("\n");
    }
    else
    {
        printf("Invalid Position\n");
    }
}

    /*
        Error Detection
    */


    printf("\nStep 2 : Error Detection\n");

    printf("----------------------------------------\n");



    int syndrome=0;


    int r=0;


    while((1<<r)<=n)
    {
        r++;
    }



    printf("Number of parity bits = %d\n",r);




    for(i=0;i<r;i++)
    {

        int pos=(1<<i);

        int parity=0;



        printf("\nChecking S%d\n",pos);

printf("Positions : ");

for(j=1;j<=n;j++)
{
    if(j & pos)
        printf("%d ",j);
}

printf("\nValues    : ");

parity = 0;

for(j=1;j<=n;j++)
{
    if(j & pos)
    {
        printf("%d",h[j]);

        parity ^= h[j];

        /* Print XOR only if another value follows */
        int k;
        for(k=j+1;k<=n;k++)
        {
            if(k & pos)
            {
                printf(" XOR ");
                break;
            }
        }
    }
}

printf("\nParity = %d\n",parity);



        if(parity==1)
        {
            syndrome=syndrome+pos;
        }


    }





    if(syndrome==0)
    {

        printf("\nNo Error Detected\n");

    }

    else
    {

        printf("\nError Detected\n");

        printf("Error Position : %d\n",syndrome);



        printf("Error Position Binary : ");


        for(i=r-1;i>=0;i--)
        {
            printf("%d",(syndrome>>i)&1);
        }



        printf("\n");



        printf("\nStep 3 : Error Correction\n");

        printf("----------------------------------------\n");


        printf("Wrong Bit : %d\n",h[syndrome]);


        h[syndrome]=!h[syndrome];


        printf("Corrected Bit : %d\n",h[syndrome]);


        printf("Position %d corrected successfully\n",syndrome);

    }






    /*
        Display corrected code
    */


    printf("\nStep 4 : Corrected Hamming Code\n");

    printf("----------------------------------------\n");


    printf("Position : ");

    for(i=n;i>=1;i--)
        printf("%3d",i);



    printf("\nBit      : ");

    for(i=n;i>=1;i--)
        printf("%3d",h[i]);



    printf("\n");



    printf("\nCorrected Code : ");



    for(i=n;i>=1;i--)
        printf("%d",h[i]);



    printf("\n");








    /*
        Extract Original Data

        IMPORTANT:
        Read from position 1 to n
        because sender inserted data in this order

    */


    printf("\nStep 5 : Extract Original Data\n");

    printf("----------------------------------------\n");



    int index=0;



    printf("Data Bit Positions : ");


    for(i=1;i<=n;i++)
    {

        if((i & (i-1))!=0)
        {
            printf("%d ",i);
        }

    }




    printf("\n\nRecovered Binary Data : ");




    for(i=1;i<=n;i++)
    {

        // skip parity positions
        if((i & (i-1))!=0)
        {

            printf("%d",h[i]);

            result[index++]=h[i];

        }

    }



    printf("\n");





    /*
        Binary to Character
    */


    printf("\nRecovered Message : ");



    for(i=0;i<index;i+=8)
    {

        int value=0;


        for(j=0;j<8;j++)
        {

            value=(value<<1)+result[i+j];

        }


        printf("%c",value);

    }



    printf("\n");


    return 0;

}