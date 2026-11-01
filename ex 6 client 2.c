#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int createSocket();
void setServerAddress(struct sockaddr_in *server_addr);
void connectServer(int sock,
                   struct sockaddr_in *server_addr);
void requestFile(int sock);
void startClient();

int main()
{
    startClient();
    return 0;
}

/* Create TCP socket */
int createSocket()
{
    int sock;

    sock = socket(AF_INET, SOCK_STREAM, 0);

    if (sock < 0)
    {
        perror("Socket creation failed");
        exit(1);
    }

    return sock;
}

/* Set server address */
void setServerAddress(struct sockaddr_in *server_addr)
{
    server_addr->sin_family = AF_INET;
    server_addr->sin_port = htons(PORT);
    server_addr->sin_addr.s_addr =
        inet_addr("127.0.0.1");
}

/* Connect to server */
void connectServer(int sock,
                   struct sockaddr_in *server_addr)
{
    if (connect(sock,
                (struct sockaddr *)server_addr,
                sizeof(*server_addr)) < 0)
    {
        perror("Connection failed");
        close(sock);
        exit(1);
    }
}

/* Request file */
void requestFile(int sock)
{
    char filename[BUFFER_SIZE];
    char buffer[BUFFER_SIZE];
    int bytes;

    printf("Enter filename: ");
    scanf("%s", filename);

    /* Send filename */
    send(sock,
         filename,
         strlen(filename),
         0);

    /* Receive status */
    memset(buffer, 0, BUFFER_SIZE);

    bytes = recv(sock,
                 buffer,
                 BUFFER_SIZE - 1,
                 0);

    if (bytes <= 0)
    {
        printf("Server disconnected.\n");
        return;
    }

    buffer[bytes] = '\0';

    printf("Server: %s\n", buffer);

    /* If file exists */
    if (strcmp(buffer, "File present") == 0)
    {
        printf("\nFile contents:\n");

        while (1)
        {
            bytes = recv(sock,
                         buffer,
                         BUFFER_SIZE - 1,
                         0);

            if (bytes <= 0)
                break;

            buffer[bytes] = '\0';

            printf("%s", buffer);
        }

        printf("\n");
    }
}

/* Start client */
void startClient()
{
    int sock;
    struct sockaddr_in server_addr;

    sock = createSocket();

    setServerAddress(&server_addr);

    connectServer(sock, &server_addr);

    printf("Connected to server.\n");

    requestFile(sock);

    close(sock);
}