#include <stdio.h>

#define MAX 100
#define BITS 8

int data[MAX][BITS];
int recvRowParity[MAX];
int calcRowParity[MAX];
int recvColParity[BITS];
int calcColParity[BITS];
char message[MAX];

int main()
{
    FILE *fp, *fp2;
    int rows;
    int i, j;

    fp = fopen("output.txt", "r");

    if(fp == NULL)
    {
        printf("Cannot open output.txt\n");
        return 0;
    }

    fscanf(fp,"%d",&rows);

    for(i=0;i<rows;i++)
        fscanf(fp," %c",&message[i]);

    for(i=0;i<rows;i++)
    {
        for(j=0;j<BITS;j++)
            fscanf(fp,"%d",&data[i][j]);

        fscanf(fp,"%d",&recvRowParity[i]);
    }

    for(j=0;j<BITS;j++)
        fscanf(fp,"%d",&recvColParity[j]);

    fclose(fp);

    fp2 = fopen("output2.txt","w");

    printf("\n========================================\n");
    printf("         2-D PARITY RECEIVER\n");
    printf("========================================\n");

    fprintf(fp2,"\n========================================\n");
    fprintf(fp2,"         2-D PARITY RECEIVER\n");
    fprintf(fp2,"========================================\n");

    printf("\nReceived Message : ");

    fprintf(fp2,"\nReceived Message : ");

    for(i=0;i<rows;i++)
    {
        printf("%c",message[i]);
        fprintf(fp2,"%c",message[i]);
    }

    printf("\n");
    fprintf(fp2,"\n");

    printf("\nReceived Matrix\n");
    printf("-----------------------------------------------------------\n");
    printf("Char\t");

    fprintf(fp2,"\nReceived Matrix\n");
    fprintf(fp2,"-----------------------------------------------------------\n");
    fprintf(fp2,"Char\t");

    for(j=1;j<=BITS;j++)
    {
        printf("B%d ",j);
        fprintf(fp2,"B%d ",j);
    }

    printf("| RP\n");
    fprintf(fp2,"| RP\n");

    printf("-----------------------------------------------------------\n");
    fprintf(fp2,"-----------------------------------------------------------\n");

    for(i=0;i<rows;i++)
    {
        printf("%c\t",message[i]);
        fprintf(fp2,"%c\t",message[i]);

        for(j=0;j<BITS;j++)
        {
            printf("%d  ",data[i][j]);
            fprintf(fp2,"%d  ",data[i][j]);
        }

        printf("| %d\n",recvRowParity[i]);
        fprintf(fp2,"| %d\n",recvRowParity[i]);
    }

    printf("-----------------------------------------------------------\n");

    fprintf(fp2,"-----------------------------------------------------------\n");

    printf("CP\t");
    fprintf(fp2,"CP\t");

    for(j=0;j<BITS;j++)
    {
        printf("%d  ",recvColParity[j]);
        fprintf(fp2,"%d  ",recvColParity[j]);
    }

    printf("\n");
    fprintf(fp2,"\n");

    /* ---------- MODIFY BITS ---------- */

    int n,row,col;

    printf("\nHow many bits do you want to modify (0-4)? : ");
    scanf("%d",&n);

    if(n<0)
        n=0;

    if(n>4)
        n=4;

    for(i=0;i<n;i++)
    {
        printf("\nModification %d\n",i+1);

        printf("Enter Row Number (1-%d): ",rows);
        scanf("%d",&row);

        printf("Enter Column Number (1-8): ");
        scanf("%d",&col);

        row--;
        col--;

        if(row>=0 && row<rows && col>=0 && col<8)
        {
            if(data[row][col]==0)
                data[row][col]=1;
            else
                data[row][col]=0;

            printf("Bit Modified Successfully.\n");
        }
        else
        {
            printf("Invalid Position.\n");
            i--;
        }
    }
        /* Recalculate parity after modification */

for(i=0;i<rows;i++)
{
    int count=0;

    for(j=0;j<BITS;j++)
    {
        if(data[i][j]==1)
            count++;
    }

    calcRowParity[i]=count%2;
}


for(j=0;j<BITS;j++)
{
    int count=0;

    for(i=0;i<rows;i++)
    {
        if(data[i][j]==1)
            count++;
    }

    calcColParity[j]=count%2;
}

    printf("\nModified Matrix\n");
    printf("-----------------------------------------------------------\n");
    printf("Char\t");

    fprintf(fp2,"\nModified Matrix\n");
    fprintf(fp2,"-----------------------------------------------------------\n");
    fprintf(fp2,"Char\t");

    for(j=1;j<=BITS;j++)
    {
        printf("B%d ",j);
        fprintf(fp2,"B%d ",j);
    }

    printf("| RP\n");
    fprintf(fp2,"| RP\n");

    printf("-----------------------------------------------------------\n");
    fprintf(fp2,"-----------------------------------------------------------\n");

    for(i=0;i<rows;i++)
    {
        printf("%c\t",message[i]);
        fprintf(fp2,"%c\t",message[i]);

        for(j=0;j<BITS;j++)
        {
            printf("%d  ",data[i][j]);
            fprintf(fp2,"%d  ",data[i][j]);
        }

        printf("| %d\n",calcRowParity[i]);
        fprintf(fp2,"| %d\n",calcRowParity[i]);
    }

    printf("-----------------------------------------------------------\n");
    fprintf(fp2,"-----------------------------------------------------------\n");

    printf("CP\t");

    fprintf(fp2,"CP\t");

    for(j=0;j<BITS;j++)
{
    printf("%d  ",calcColParity[j]);
    fprintf(fp2,"%d  ",calcColParity[j]);
}

    printf("\n");
    fprintf(fp2,"\n");

        int error = 0;
    int count;

    /* Calculate Row Parity */

    for(i=0;i<rows;i++)
    {
        count = 0;

        for(j=0;j<BITS;j++)
        {
            if(data[i][j] == 1)
                count++;
        }

        calcRowParity[i] = count % 2;
    }

    /* Calculate Column Parity */

    for(j=0;j<BITS;j++)
    {
        count = 0;

        for(i=0;i<rows;i++)
        {
            if(data[i][j] == 1)
                count++;
        }

        calcColParity[j] = count % 2;
    }

    printf("\n");
    printf("========================================\n");
    printf("ROW PARITY VERIFICATION\n");
    printf("========================================\n");

    fprintf(fp2,"\n");
    fprintf(fp2,"========================================\n");
    fprintf(fp2,"ROW PARITY VERIFICATION\n");
    fprintf(fp2,"========================================\n");

    for(i=0;i<rows;i++)
    {
        printf("Row %d : Received = %d  Calculated = %d --> ",
               i+1,
               recvRowParity[i],
               calcRowParity[i]);

        fprintf(fp2,
                "Row %d : Received = %d  Calculated = %d --> ",
                i+1,
                recvRowParity[i],
                calcRowParity[i]);

        if(recvRowParity[i] == calcRowParity[i])
        {
            printf("CORRECT\n");
            fprintf(fp2,"CORRECT\n");
        }
        else
        {
            printf("ERROR\n");
            fprintf(fp2,"ERROR\n");
            error = 1;
        }
    }

    printf("\n");
    printf("========================================\n");
    printf("COLUMN PARITY VERIFICATION\n");
    printf("========================================\n");

    fprintf(fp2,"\n");
    fprintf(fp2,"========================================\n");
    fprintf(fp2,"COLUMN PARITY VERIFICATION\n");
    fprintf(fp2,"========================================\n");

    for(j=0;j<BITS;j++)
    {
        printf("Column %d : Received = %d  Calculated = %d --> ",
               j+1,
               recvColParity[j],
               calcColParity[j]);

        fprintf(fp2,
                "Column %d : Received = %d  Calculated = %d --> ",
                j+1,
                recvColParity[j],
                calcColParity[j]);

        if(recvColParity[j] == calcColParity[j])
        {
            printf("CORRECT\n");
            fprintf(fp2,"CORRECT\n");
        }
        else
        {
            printf("ERROR\n");
            fprintf(fp2,"ERROR\n");
            error = 1;
        }
    }

    printf("\n");
    printf("========================================\n");

    fprintf(fp2,"\n");
    fprintf(fp2,"========================================\n");

    if(error == 0)
    {
        printf("MESSAGE RECEIVED WITHOUT ERROR\n");
        fprintf(fp2,"MESSAGE RECEIVED WITHOUT ERROR\n");
    }
    else
    {
        printf("MESSAGE RECEIVED WITH ERROR\n");
        fprintf(fp2,"MESSAGE RECEIVED WITH ERROR\n");
    }

    printf("========================================\n");

    fprintf(fp2,"========================================\n");

    fclose(fp2);

    printf("\nResult successfully written into output2.txt\n");

    return 0;
}