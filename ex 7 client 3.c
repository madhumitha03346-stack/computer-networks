#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 8082
#define BUFFER_SIZE 1024

void createClient(int *sockfd)
{
    *sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (*sockfd < 0)
    {
        perror("Socket creation failed");
        exit(1);
    }
}

void connectServer(int sockfd, char *ip)
{
    struct sockaddr_in server;

    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    inet_pton(AF_INET, ip, &server.sin_addr);

    if (connect(sockfd,
                (struct sockaddr *)&server,
                sizeof(server)) < 0)
    {
        perror("Connection failed");
        exit(1);
    }
}

void requestFile(int sockfd)
{
    char filename[BUFFER_SIZE];
    char buffer[BUFFER_SIZE];
    int n;

    printf("Enter file name: ");
    fgets(filename, sizeof(filename), stdin);

    send(sockfd, filename, strlen(filename), 0);

    while ((n = recv(sockfd, buffer, sizeof(buffer) - 1, 0)) > 0)
    {
        buffer[n] = '\0';
        printf("%s", buffer);
    }

    printf("\n");
}

void runClient(char *ip)
{
    int sockfd;

    createClient(&sockfd);
    connectServer(sockfd, ip);

    requestFile(sockfd);

    close(sockfd);
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Usage: %s <server_ip>\n", argv[0]);
        return 1;
    }

    runClient(argv[1]);

    return 0;
}