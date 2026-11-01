#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 8081
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

void searchIP(int sockfd)
{
    char ip[BUFFER_SIZE];
    char response[BUFFER_SIZE];

    printf("Enter IP address: ");
    fgets(ip, sizeof(ip), stdin);

    send(sockfd, ip, strlen(ip), 0);

    memset(response, 0, sizeof(response));

    recv(sockfd, response, sizeof(response) - 1, 0);

    printf("Server response: %s\n", response);
}

void runClient(char *ip)
{
    int sockfd;

    createClient(&sockfd);
    connectServer(sockfd, ip);

    searchIP(sockfd);

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