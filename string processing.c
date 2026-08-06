#include <stdio.h>
#include <string.h>

int main()
{
    int T;
    char S[1000];

    scanf("%d", &T);

    while (T--)
    {
        int sum = 0;

        scanf("%s", S);

        for (int i = 0; S[i] != '\0'; i++)
        {
            if (S[i] >= '0' && S[i] <= '9')
            {
                sum += S[i] - '0';
            }
        }

        printf("%d\n", sum);
    }

    return 0;
}