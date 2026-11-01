#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define MAX_ENTRIES 20
#define BUFFER_SIZE 1024

// Structure for ARP table entry
struct ARPEntry
{
    char ip[20];
    char mac[20];
};

// Structure for client
struct Client
{
    int socket;
    struct sockaddr_in address;
    char ip[20];
    char mac[20];
};

// Function declarations
int createServerSocket();
void bindServer(int server_socket);
void initializeARPTable(struct ARPEntry table[], int *count);
void displayARPTable(struct ARPEntry table[], int count);
int searchARPTable(struct ARPEntry table[], int count, char ip[]);
void addARPEntry(struct ARPEntry table[], int *count, char ip[], char mac[]);
void handleClient(struct Client *client,
                  struct ARPEntry table[],
                  int *count);

int main()
{
    int server_socket;
    struct Client client;

    struct ARPEntry arp_table[MAX_ENTRIES];
    int arp_count = 0;

    socklen_t client_len = sizeof(client.address);

    // Create initial ARP table
    initializeARPTable(arp_table, &arp_count);

    // Create socket
    server_socket = createServerSocket();

    // Bind socket
    bindServer(server_socket);

    // Listen
    listen(server_socket, 5);

    printf("====================================\n");
    printf("          ARP SERVER\n");
    printf("====================================\n");

    displayARPTable(arp_table, arp_count);

    printf("\nServer is waiting for clients...\n");

    // Iterative server
    while (1)
    {
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

        // Handle one client
        handleClient(
            &client,
            arp_table,
            &arp_count
        );

        // Close client
        close(client.socket);

        printf("\nClient disconnected.");
        printf("\nWaiting for next client...\n");
    }

    close(server_socket);

    return 0;
}

// Create server socket
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

// Initialize ARP table
void initializeARPTable(
    struct ARPEntry table[],
    int *count)
{
    strcpy(table[0].ip, "192.168.1.1");
    strcpy(table[0].mac, "AA:BB:CC:DD:EE:01");

    strcpy(table[1].ip, "192.168.1.2");
    strcpy(table[1].mac, "AA:BB:CC:DD:EE:02");

    strcpy(table[2].ip, "192.168.1.3");
    strcpy(table[2].mac, "AA:BB:CC:DD:EE:03");

    strcpy(table[3].ip, "192.168.1.4");
    strcpy(table[3].mac, "AA:BB:CC:DD:EE:04");

    *count = 4;
}

// Display ARP table
void displayARPTable(
    struct ARPEntry table[],
    int count)
{
    int i;

    printf("\n====================================\n");
    printf("             ARP TABLE\n");
    printf("====================================\n");

    printf("%-20s %-20s\n", "IP ADDRESS", "MAC ADDRESS");
    printf("------------------------------------\n");

    for (i = 0; i < count; i++)
    {
        printf(
            "%-20s %-20s\n",
            table[i].ip,
            table[i].mac
        );
    }

    printf("====================================\n");
}

// Search IP in ARP table
int searchARPTable(
    struct ARPEntry table[],
    int count,
    char ip[])
{
    int i;

    for (i = 0; i < count; i++)
    {
        if (strcmp(table[i].ip, ip) == 0)
        {
            return i;
        }
    }

    return -1;
}

// Add new ARP entry
void addARPEntry(
    struct ARPEntry table[],
    int *count,
    char ip[],
    char mac[])
{
    if (*count >= MAX_ENTRIES)
    {
        printf("ARP table is full.\n");
        return;
    }

    strcpy(table[*count].ip, ip);
    strcpy(table[*count].mac, mac);

    (*count)++;

    printf("\nNew ARP entry added successfully.\n");
}

// Handle client
void handleClient(
    struct Client *client,
    struct ARPEntry table[],
    int *count)
{
    int index;

    memset(client->ip, 0, sizeof(client->ip));

    // Receive IP from client
    int bytes = recv(
        client->socket,
        client->ip,
        sizeof(client->ip) - 1,
        0
    );

    if (bytes <= 0)
    {
        return;
    }

    client->ip[bytes] = '\0';

    // Remove newline if present
    client->ip[strcspn(client->ip, "\r\n")] = '\0';

    printf("IP requested by client: %s\n",
           client->ip);

    // Search ARP table
    index = searchARPTable(
        table,
        *count,
        client->ip
    );

    if (index != -1)
    {
        // IP found
        printf("\nIP address found in ARP table.\n");
        printf("MAC Address: %s\n",
               table[index].mac);

        send(
            client->socket,
            table[index].mac,
            strlen(table[index].mac),
            0
        );
    }
    else
    {
        // IP not found
        printf("\nIP address not found in ARP table.\n");

        printf("Enter MAC address for %s: ",
               client->ip);

        fgets(
            client->mac,
            sizeof(client->mac),
            stdin
        );

        client->mac[
            strcspn(client->mac, "\r\n")
        ] = '\0';

        // Add new entry
        addARPEntry(
            table,
            count,
            client->ip,
            client->mac
        );

        // Send MAC to client
        send(
            client->socket,
            client->mac,
            strlen(client->mac),
            0
        );

        // Display updated table
        displayARPTable(
            table,
            *count
        );
    }
}