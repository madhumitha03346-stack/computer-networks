#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/time.h>

#define PORT 8080

// Linked List Node
typedef struct Frame {
    int frame_id;
    char content[50];
    struct Frame* next;
} Frame;

typedef struct Packet {
    int seq_num;
    int ack_num;
    char content[50];
    int test_case;
} Packet;

void appendFrame(Frame** head, int id, const char* content) {
    Frame* newFrame = (Frame*)malloc(sizeof(Frame));
    newFrame->frame_id = id;
    strncpy(newFrame->content, content, sizeof(newFrame->content) - 1);
    newFrame->content[sizeof(newFrame->content) - 1] = '\0'; // Added safety terminator
    newFrame->next = NULL;

    if (*head == NULL) {
        *head = newFrame;
        return;
    }
    Frame* temp = *head;
    while (temp->next != NULL) temp = temp->next;
    temp->next = newFrame;
}

void runTestCase(int sock, Frame* head, int test_case_id) {
    Frame* current = head;
    int expected_ack = 1;

    // Set 2-second timeout on socket reads
    struct timeval timeout;
    timeout.tv_sec = 2;
    timeout.tv_usec = 0;
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

    while (current != NULL) {
        printf("\n[SENDER] Sending Frame %d: \"%s\"\n", current->frame_id, current->content);

        Packet pkt;
        memset(&pkt, 0, sizeof(Packet)); // Clear garbage data
        pkt.seq_num = current->frame_id;
        strncpy(pkt.content, current->content, sizeof(pkt.content) - 1);
        pkt.test_case = test_case_id;

        send(sock, &pkt, sizeof(Packet), 0);
        printf("[SENDER] Frame %d transmitted. Timer started (2s threshold)...\n", current->frame_id);

        Packet ack_pkt;
        int bytes_received = recv(sock, &ack_pkt, sizeof(Packet), 0);
        if (bytes_received < 0) {
            // TEST CASE 1: TIMEOUT
            printf("[SENDER] Timer Expired! No response from receiver.\n");
            printf("[SENDER] Action: Retransmitting Frame %d...\n", current->frame_id);
            test_case_id = 0; // Clear condition for retransmission
        }
        else if (ack_pkt.ack_num < expected_ack) {
            // TEST CASE 3: MISMATCH / DUPLICATE ACK
            printf("[SENDER] Received Duplicate ACK %d (Receiver reported mismatch).\n", ack_pkt.ack_num);
            printf("[SENDER] Action: Retransmitting Frame %d...\n", current->frame_id);
            test_case_id = 0; // Clear condition for retransmission
        }
        else if (ack_pkt.ack_num == expected_ack) {
            // SUCCESSFUL ACK
            printf("[SENDER] Positive ACK %d received successfully.\n", ack_pkt.ack_num);
            expected_ack++;
            current = current->next;
        }
    }
}

int main() {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in serv_addr;

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // CHANGED: Configured to target your server machine's IP address instead of localhost
    if (inet_pton(AF_INET, "172.16.16.200", &serv_addr.sin_addr) <= 0) {
        printf("Invalid network address configuration.\n");
        return -1;
    }

    printf("=========================================\n");
    printf("         SENDER SIDE (CLIENT)            \n");
    printf("=========================================\n");
    printf("Status: Connecting to Server at 172.16.16.200:%d...\n", PORT);

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("Connection Failed. Ensure receiver is running on 172.16.16.200 and port %d is open.\n", PORT);
        return -1;
    }
    printf("Status: Connected to Remote Receiver.\n\n");

    int total_frames;
    char buffer[50];

    printf("Enter number of frames to send: ");
    if (scanf("%d", &total_frames) != 1) return 0;

    Frame* frameList = NULL;
    int i;
    for (i = 0; i < total_frames; i++) {
        printf("Enter content for Frame %d: ", i);
        scanf("%49s", buffer);
        appendFrame(&frameList, i, buffer);
    }

    printf("\n>>> TEST CASE 1: TIMEOUT & RETRANSMISSION <<<");
    runTestCase(sock, frameList, 1);

    printf("\n>>> TEST CASE 2: SUCCESSFUL COMPLETION <<<");
    runTestCase(sock, frameList, 2);

    if (total_frames > 1) {
        printf("\n>>> TEST CASE 3: SEQUENCE MISMATCH & RESEND <<<");
        runTestCase(sock, frameList, 3);
    }

    // Clean up allocated linked list nodes before closing
    Frame* current = frameList;
    while (current != NULL) {
        Frame* next = current->next;
        free(current);
        current = next;
    }

    close(sock);
    return 0;
}