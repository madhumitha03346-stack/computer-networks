#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080

typedef struct Packet {
    int seq_num;
    int ack_num;
    char content[50];
    int test_case; // 1: Timeout (Drop), 2: Normal, 3: Mismatch
} Packet;

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    bind(server_fd, (struct sockaddr *)&address, sizeof(address));
    listen(server_fd, 3);

    printf("=========================================\n");
    printf("        RECEIVER SIDE (SERVER)           \n");
    printf("=========================================\n");

    // Loop continuously to accept multiple client connections until terminated
    while (1) {
        printf("Waiting for Sender connection on port %d...\n", PORT);
        new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen);
        printf("Sender connected!\n\n");

        int expected_seq = 0;
        Packet pkt;

        while (read(new_socket, &pkt, sizeof(Packet)) > 0) {
            // CRITICAL FIX: Reset expected sequence if the sender restarts a new test case phase
            if (pkt.seq_num == 0 && expected_seq != 0) {
                expected_seq = 0;
            }

            printf("-----------------------------------------\n");

            // TEST CASE 1: Simulate Frame Loss (Drop packet)
            if (pkt.test_case == 1 && pkt.seq_num == 1) {
                printf("[RECEIVER] Packet Loss Simulated! Dropping Frame %d.\n", pkt.seq_num);
                printf("[RECEIVER] (No ACK sent to Sender)\n");
                pkt.test_case = 0; // Reset condition for retry
                continue;
            }

            // TEST CASE 3: Simulate Corrupted Sequence / Mismatch
            if (pkt.test_case == 3 && pkt.seq_num == 1) {
                printf("[RECEIVER] Mismatch! Expected Frame %d, but received corrupted Frame 99.\n", expected_seq);
                Packet ack;
                ack.ack_num = expected_seq; // Request expected sequence
                printf("[RECEIVER] Sending Duplicate ACK %d to request retransmission.\n", ack.ack_num);
                send(new_socket, &ack, sizeof(Packet), 0);
                pkt.test_case = 0; // Reset condition for retry
                continue;
            }

            // Normal Reception Check
            if (pkt.seq_num == expected_seq) {
                printf("[RECEIVER] Received Frame %d: \"%s\"\n", pkt.seq_num, pkt.content);
                expected_seq++;
                Packet ack;
                ack.ack_num = expected_seq;
                printf("[RECEIVER] Frame accepted. Sending ACK %d back to Sender.\n", ack.ack_num);
                send(new_socket, &ack, sizeof(Packet), 0);
            } else {
                // Safety fallback: if frame is out of order, let the sender know what we are waiting for
                printf("[RECEIVER] Out of order frame! Expected %d, Got %d\n", expected_seq, pkt.seq_num);
                Packet ack;
                ack.ack_num = expected_seq;
                send(new_socket, &ack, sizeof(Packet), 0);
            }
        }

        printf("\nTransmission session complete. Closing current client connection.\n\n");
        close(new_socket);

        // Use Ctrl+C in your terminal when you want to exit the entire server environment.
    }

    close(server_fd);
    return 0;
}
