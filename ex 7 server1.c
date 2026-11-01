#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

// Structure for client information
struct Client
{
    int socket;
    struct sockaddr_in address;
    char message[BUFFER_SIZE];
};

// Function declarations
int createServerSocket();
void bindServer(int server_socket);
void chatWithClient(struct Client *client);

int main()
{
    int server_socket;
    struct Client client;
    socklen_t client_len = sizeof(client.address);

    // Create socket
    server_socket = createServerSocket();

    // Bind socket
    bindServer(server_socket);

    // Listen for clients
    listen(server_socket, 5);

    printf("====================================\n");
    printf("       ITERATIVE CHAT SERVER\n");
    printf("====================================\n");
    printf("Server started on port %d\n", PORT);
    printf("Waiting for clients...\n");

    while (1)
    {
        // Accept one client
        client.socket = accept(
            server_socket,
            (struct sockaddr *)&client.address,
            &client_len
        );

        if (client.socket < 0)
        {
            perror("Accept failed");
            continue;
        }

        printf("\nClient connected.\n");

        // Chat with this client
        chatWithClient(&client);

        // Close client connection
        close(client.socket);

        printf("Client disconnected.\n");
        printf("Waiting for next client...\n");
    }

    close(server_socket);

    return 0;
}

// Create server socket
int createServerSocket()
{
    int server_socket;

    server_socket = socket(AF_INET, SOCK_STREAM, 0);

    if (server_socket < 0)
    {
        perror("Socket creation failed");
        exit(1);
    }

    return server_socket;
}

// Bind server socket
void bindServer(int server_socket)
{
    struct sockaddr_in server_address;

    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(PORT);

    if (bind(
        server_socket,
        (struct sockaddr *)&server_address,
        sizeof(server_address)
    ) < 0)
    {
        perror("Bind failed");
        close(server_socket);
        exit(1);
    }
}

// Chat function
void chatWithClient(struct Client *client)
{
    while (1)
    {
        memset(client->message, 0, BUFFER_SIZE);

        // Receive message from client
        int bytes_received = recv(
            client->socket,
            client->message,
            BUFFER_SIZE - 1,
            0
        );

        if (bytes_received <= 0)
        {
            break;
        }

        client->message[bytes_received] = '\0';

        printf("Client: %s", client->message);

        // Exit condition
        if (strncmp(client->message, "exit", 4) == 0)
        {
            printf("Client ended the chat.\n");
            break;
        }

        // Get server message
        printf("Server: ");
        fgets(client->message, BUFFER_SIZE, stdin);

        send(
            client->socket,
            client->message,
            strlen(client->message),
            0
        );

        // Exit condition
        if (strncmp(client->message, "exit", 4) == 0)
        {
            printf("Server ended the chat.\n");
            break;
        }
    }
}