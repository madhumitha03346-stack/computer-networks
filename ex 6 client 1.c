#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int createSocket();
void setServerAddress(struct sockaddr_in *server_addr);
void connectToServer(int sock, struct sockaddr_in *server_addr);
void chatWithServer(int sock);
void startClient();

int main()
{
    startClient();
    return 0;
}

// Create TCP socket
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

// Set server address
void setServerAddress(struct sockaddr_in *server_addr)
{
    server_addr->sin_family = AF_INET;
    server_addr->sin_port = htons(PORT);
    server_addr->sin_addr.s_addr =
        inet_addr("127.0.0.1");
}

// Connect client to server
void connectToServer(int sock,
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

// Chat with server
void chatWithServer(int sock)
{
    char buffer[BUFFER_SIZE];

    while (1)
    {
        // Client message
        printf("Client: ");

        fgets(buffer, BUFFER_SIZE, stdin);

        buffer[strcspn(buffer, "\n")] = '\0';

        send(sock,
             buffer,
             strlen(buffer),
             0);

        if (strcmp(buffer, "exit") == 0)
        {
            break;
        }

        // Receive server message
        memset(buffer, 0, BUFFER_SIZE);

        int bytes = recv(sock,
                         buffer,
                         BUFFER_SIZE - 1,
                         0);

        if (bytes <= 0)
        {
            printf("Server disconnected.\n");
            break;
        }

        buffer[bytes] = '\0';

        printf("Server: %s\n", buffer);

        if (strcmp(buffer, "exit") == 0)
        {
            break;
        }
    }
}

// Start client
void startClient()
{
    int sock;
    struct sockaddr_in server_addr;

    sock = createSocket();

    setServerAddress(&server_addr);

    connectToServer(sock, &server_addr);

    printf("Connected to server!\n");

    chatWithServer(sock);

    close(sock);
}