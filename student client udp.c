#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 8080
#define MAX 1024

int main()
{
    int sockfd;
    char regno[MAX];
    char buffer[MAX];

    struct sockaddr_in serverAddr;
    socklen_t addrLen = sizeof(serverAddr);

    // Create UDP socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    if (sockfd < 0)
    {
        perror("Socket creation failed");
        exit(1);
    }

    // Server address
    memset(&serverAddr, 0, sizeof(serverAddr));

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);

    // Localhost
    if (inet_pton(AF_INET, "127.0.0.1",
                  &serverAddr.sin_addr) <= 0)
    {
        perror("Invalid server address");
        close(sockfd);
        exit(1);
    }

    printf("===== Student Information Lookup =====\n");

    while (1)
    {
        printf("\nEnter Registration Number");
        printf(" (or 'exit' to quit): ");

        fgets(regno, MAX, stdin);

        // Remove newline
        regno[strcspn(regno, "\r\n")] = '\0';

        // Handle empty input
        if (strlen(regno) == 0)
        {
            printf("ERROR: Registration number cannot be empty.\n");
            continue;
        }

        // Exit
        if (strcmp(regno, "exit") == 0)
        {
            sendto(sockfd, regno, strlen(regno), 0,
                   (struct sockaddr *)&serverAddr,
                   addrLen);

            break;
        }

        // Send registration number
        sendto(sockfd, regno, strlen(regno), 0,
               (struct sockaddr *)&serverAddr,
               addrLen);

        // Receive student details
        memset(buffer, 0, MAX);

        int n = recvfrom(sockfd, buffer, MAX - 1, 0,
                         (struct sockaddr *)&serverAddr,
                         &addrLen);

        if (n < 0)
        {
            perror("Receive failed");
            continue;
        }

        buffer[n] = '\0';

        printf("\n----- Server Response -----\n");
        printf("%s\n", buffer);
        printf("---------------------------\n");
    }

    close(sockfd);

    printf("\nClient terminated.\n");

    return 0;
}