#include<stdio.h>
#include<string.h>


int hamcalc(int h[], int pos, int n)
{
    int parity=0;
    int i;


    printf("\nChecking positions for P%d : ",pos);


    for(i=1;i<=n;i++)
    {
        if(i & pos)
        {
            printf("%d ",i);
            parity = parity ^ h[i];
        }
    }


    printf("\nParity calculation : ");


    for(i=1;i<=n;i++)
    {
        if(i & pos)
        {
            printf("%d",h[i]);

            if(i!=n)
                printf(" XOR ");
        }
    }


    printf("\nP%d = %d\n",pos,parity);


    return parity;
}



int hamcode(int h[], int data[], int m)
{

    int r=0;
    int n;
    int i,j=0;



    while((1<<r)<(m+r+1))
    {
        r++;
    }



    printf("\nStep 1 : Calculate redundancy bits\n");


    printf("2^r > m+r+1\n");


    printf("2^%d > %d+%d+1\n",r,m,r);


    printf("%d > %d\n",(1<<r),m+r+1);



    printf("\nNumber of redundancy bits = %d\n",r);



    n=m+r;



    printf("\nStep 2 : Position allocation\n\n");


    printf("Position : ");

    for(i=n;i>=1;i--)
        printf("%3d",i);



    printf("\nBit      : ");


    for(i=n;i>=1;i--)
    {

        if((i & (i-1))==0)
            printf("%3c",'P');

        else
            printf("%3c",'D');

    }


    printf("\n");




    // Insert data bits

    j=0;


    for(i=1;i<=n;i++)
    {

        if((i & (i-1))==0)
        {
            h[i]=0;
        }

        else
        {
            h[i]=data[j];
            j++;
        }

    }




    printf("\nStep 3 : Data placement\n\n");


    printf("Position : ");

    for(i=n;i>=1;i--)
        printf("%3d",i);



    printf("\nBit      : ");


    for(i=n;i>=1;i--)
        printf("%3d",h[i]);


    printf("\n");




    printf("\nStep 4 : Calculate parity bits\n");



    for(i=0;i<r;i++)
    {
        int pos=(1<<i);

        h[pos]=hamcalc(h,pos,n);
    }



    return n;

}




int main()
{

    char message[100];

    int data[500];
    int h[500];


    int m=0;
    int n;

    int i,j;


    FILE *fp1,*fp2;



    printf("========================================\n");
    printf("          HAMMING CODE SENDER\n");
    printf("========================================\n");



    printf("Enter message : ");
    scanf("%s",message);




    // Character to Binary Conversion


    for(i=0;message[i]!='\0';i++)
    {

        int ascii=(int)message[i];


        for(j=7;j>=0;j--)
        {
            data[m++]=(ascii>>j)&1;
        }

    }



    printf("\nBinary Conversion : ");


    for(i=0;i<m;i++)
    {
        printf("%d",data[i]);
    }



    printf("\n");




    // Store binary data


    fp1=fopen("o1.txt","w");


    for(i=0;i<m;i++)
    {
        fprintf(fp1,"%d",data[i]);
    }


    fclose(fp1);




    printf("\nOriginal Binary Data stored in o1.txt\n");



    n=hamcode(h,data,m);





    printf("\nStep 5 : Final Hamming Code\n\n");



    printf("Position : ");


    for(i=n;i>=1;i--)
        printf("%3d",i);



    printf("\nBit      : ");



    for(i=n;i>=1;i--)
        printf("%3d",h[i]);



    printf("\n");




    fp2=fopen("o2.txt","w");



    for(i=n;i>=1;i--)
    {
        fprintf(fp2,"%d",h[i]);
    }



    fclose(fp2);




    printf("\nHamming Code : ");


    for(i=n;i>=1;i--)
        printf("%d",h[i]);



    printf("\n\nHamming code stored in o2.txt\n");



    return 0;
}