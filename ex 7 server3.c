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
    char filename[100];
    char buffer[BUFFER_SIZE];
};

// Function declarations
int createServerSocket();
void bindServer(int server_socket);
void handleClient(struct Client *client);
void sendFile(struct Client *client);
void sendFileNotFound(struct Client *client);

int main()
{
    int server_socket;
    struct Client client;
    socklen_t client_len = sizeof(client.address);

    // Create server socket
    server_socket = createServerSocket();

    // Bind server
    bindServer(server_socket);

    // Listen
    listen(server_socket, 5);

    printf("====================================\n");
    printf("       ITERATIVE FILE SERVER\n");
    printf("====================================\n");
    printf("Server started on port %d\n", PORT);
    printf("Waiting for clients...\n");

    // Iterative server
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

        printf("\n------------------------------------\n");
        printf("Client connected.\n");
        printf("------------------------------------\n");

        // Handle client
        handleClient(&client);

        // Close client connection
        close(client.socket);

        printf("Client disconnected.\n");
        printf("Waiting for next client...\n");
    }

    close(server_socket);

    return 0;
}

// Create socket
int createServerSocket()
{
    int server_socket;

    server_socket = socket(
        AF_INET,
        SOCK_STREAM,
        0
    );

    if (server_socket < 0)
    {
        perror("Socket creation failed");
        exit(1);
    }

    return server_socket;
}

// Bind server
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

// Handle client
void handleClient(struct Client *client)
{
    int bytes_received;

    memset(
        client->filename,
        0,
        sizeof(client->filename)
    );

    // Receive filename
    bytes_received = recv(
        client->socket,
        client->filename,
        sizeof(client->filename) - 1,
        0
    );

    if (bytes_received <= 0)
    {
        printf("Client disconnected before sending filename.\n");
        return;
    }

    client->filename[bytes_received] = '\0';

    // Remove newline
    client->filename[
        strcspn(client->filename, "\r\n")
    ] = '\0';

    printf("Requested file: %s\n",
           client->filename);

    // Send file
    sendFile(client);
}

// Send file
void sendFile(struct Client *client)
{
    FILE *file;
    int bytes_read;

    // Open requested file
    file = fopen(client->filename, "rb");

    if (file == NULL)
    {
        printf("File not found: %s\n",
               client->filename);

        sendFileNotFound(client);

        return;
    }

    // Tell client that file exists
    send(
        client->socket,
        "FILE_FOUND",
        strlen("FILE_FOUND"),
        0
    );

    // Send file contents
    while ((bytes_read = fread(
                client->buffer,
                1,
                BUFFER_SIZE,
                file)) > 0)
    {
        send(
            client->socket,
            client->buffer,
            bytes_read,
            0
        );
    }

    // Close file
    fclose(file);

    printf("File transferred successfully.\n");
}

// Send file not found message
void sendFileNotFound(struct Client *client)
{
    send(
        client->socket,
        "FILE_NOT_FOUND",
        strlen("FILE_NOT_FOUND"),
        0
    );
}