#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 8080
#define MAX 1024

struct Student {
    char regno[20];
    char name[50];
    char department[30];
    int semester;
    float cgpa;
};

struct Student students[] = {
    {"23CSE001", "Arun", "CSE", 6, 8.45},
    {"23CSE002", "Priya", "CSE", 6, 9.10},
    {"23ECE001", "Rahul", "ECE", 5, 8.20},
    {"23IT001", "Divya", "IT", 6, 8.75},
    {"23ME001", "Karthik", "MECH", 4, 7.95}
};

int main()
{
    int sockfd;
    char buffer[MAX];
    char response[MAX];

    struct sockaddr_in serverAddr, clientAddr;
    socklen_t addrLen = sizeof(clientAddr);

    int n, found;

    int totalStudents = sizeof(students) / sizeof(students[0]);

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
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(PORT);

    // Bind socket
    if (bind(sockfd, (struct sockaddr *)&serverAddr,
             sizeof(serverAddr)) < 0)
    {
        perror("Bind failed");
        close(sockfd);
        exit(1);
    }

    printf("UDP Student Server started...\n");
    printf("Waiting for student registration numbers...\n\n");

    while (1)
    {
        memset(buffer, 0, MAX);

        // Receive registration number
        n = recvfrom(sockfd, buffer, MAX - 1, 0,
                     (struct sockaddr *)&clientAddr, &addrLen);

        if (n < 0)
        {
            perror("Receive failed");
            continue;
        }

        buffer[n] = '\0';

        // Remove newline if present
        buffer[strcspn(buffer, "\r\n")] = '\0';

        printf("Received Registration Number: %s\n", buffer);

        // Exit command
        if (strcmp(buffer, "exit") == 0)
        {
            printf("Client requested termination.\n");
            break;
        }

        found = 0;

        // Search database
        for (int i = 0; i < totalStudents; i++)
        {
            if (strcmp(buffer, students[i].regno) == 0)
            {
                found = 1;

                snprintf(response, MAX,
                         "Student Found!\n"
                         "Registration No : %s\n"
                         "Name            : %s\n"
                         "Department      : %s\n"
                         "Semester        : %d\n"
                         "CGPA            : %.2f",
                         students[i].regno,
                         students[i].name,
                         students[i].department,
                         students[i].semester,
                         students[i].cgpa);

                break;
            }
        }

        // If student does not exist
        if (!found)
        {
            snprintf(response, MAX,
                     "ERROR: Student with Registration Number '%s' "
                     "does not exist.",
                     buffer);
        }

        // Send response
        sendto(sockfd, response, strlen(response), 0,
               (struct sockaddr *)&clientAddr, addrLen);

        printf("Response sent to client.\n\n");
    }

    close(sockfd);

    return 0;
}