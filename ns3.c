#include <stdio.h>
#include <stdlib.h>

#define MAX_NODES 10
#define MAX_LINKS 9

#define PACKET_SIZE 1024

#define INITIAL_CWND 1
#define MAX_CWND 50

typedef struct
{
    int id;
} Node;

typedef struct
{
    int id;
    int node1;
    int node2;
    int data_rate;
    int delay;
} Link;

typedef struct
{
    int packets_sent;
    int packets_received;
    int packets_lost;
    long total_bytes;
    double total_delay;
} Statistics;

int main()
{
    Node nodes[MAX_NODES];
    Link links[MAX_LINKS];

    Statistics stats;

    FILE *trace;
    FILE *cwnd_file;

    int total_packets;
    int buffer_size;

    int i;
    int packet;
    int cwnd;

    int buffer_count;
    int packets_in_window;

    int delivered;
    int dropped;

    double throughput;
    double delivery_ratio;
    double average_delay;

    stats.packets_sent = 0;
    stats.packets_received = 0;
    stats.packets_lost = 0;
    stats.total_bytes = 0;
    stats.total_delay = 0.0;

    printf("====================================\n");
    printf(" TCP CONGESTION CONTROL SIMULATION\n");
    printf("====================================\n");

    printf("\nEnter number of packets: ");
    scanf("%d", &total_packets);

    printf("Enter buffer size (number of packets): ");
    scanf("%d", &buffer_size);

    if (total_packets <= 0 || buffer_size <= 0)
    {
        printf("\nInvalid input.\n");
        return 1;
    }

    trace = fopen("network_trace.txt", "w");

    if (trace == NULL)
    {
        printf("Error opening network_trace.txt\n");
        return 1;
    }

    cwnd_file = fopen("cwnd.dat", "w");

    if (cwnd_file == NULL)
    {
        printf("Error opening cwnd.dat\n");
        fclose(trace);
        return 1;
    }

    printf("\n===== NODE CREATION =====\n");
    fprintf(trace, "===== NODE CREATION =====\n");

    for (i = 0; i < MAX_NODES; i++)
    {
        nodes[i].id = i;

        printf("Node %d created\n", nodes[i].id);

        fprintf(trace,
                "Node %d created\n",
                nodes[i].id);
    }


    printf("\n===== LINK CREATION =====\n");
    fprintf(trace, "\n===== LINK CREATION =====\n");

    for (i = 0; i < MAX_LINKS; i++)
    {
        links[i].id = i + 1;
        links[i].node1 = i;
        links[i].node2 = i + 1;

        links[i].data_rate = 10;
        links[i].delay = 2;

        if (i == 4)
        {
            links[i].data_rate = 2;
            links[i].delay = 10;
        }

        printf("Link %d: Node %d <-> Node %d | "
               "%d Mbps | %d ms\n",
               links[i].id,
               links[i].node1,
               links[i].node2,
               links[i].data_rate,
               links[i].delay);

        fprintf(trace,
                "Link %d: Node %d <-> Node %d | "
                "%d Mbps | %d ms\n",
                links[i].id,
                links[i].node1,
                links[i].node2,
                links[i].data_rate,
                links[i].delay);
    }

    printf("\n===== TOPOLOGY =====\n");

    printf("Node 0 ---- Node 1 ---- Node 2 ---- Node 3 ---- Node 4\n");
    printf("                                                    |\n");
    printf("                                              BOTTLENECK\n");
    printf("                                                    |\n");
    printf("Node 9 ---- Node 8 ---- Node 7 ---- Node 6 ---- Node 5\n");

    fprintf(trace, "\n===== TOPOLOGY =====\n");
    fprintf(trace,
            "Node 0 ---- Node 1 ---- Node 2 ---- Node 3 ---- Node 4\n");
    fprintf(trace,
            "                                                    |\n");
    fprintf(trace,
            "                                              BOTTLENECK\n");
    fprintf(trace,
            "                                                    |\n");
    fprintf(trace,
            "Node 9 ---- Node 8 ---- Node 7 ---- Node 6 ---- Node 5\n");


    printf("\n===== TCP SIMULATION =====\n");

    fprintf(trace,
            "\n===== TCP SIMULATION =====\n");

    printf("Total packets : %d\n", total_packets);
    printf("Buffer size   : %d packets\n", buffer_size);

    fprintf(trace,
            "Total packets : %d\n",
            total_packets);

    fprintf(trace,
            "Buffer size   : %d packets\n",
            buffer_size);

    cwnd = INITIAL_CWND;
    buffer_count = 0;
    packets_in_window = 0;

    for (packet = 1;
         packet <= total_packets;
         packet++)
    {

        stats.packets_sent++;

        buffer_count++;
        packets_in_window++;

        printf("\nPacket %d sent", packet);

        fprintf(trace,
                "Packet %d sent",
                packet);

        printf(" | Buffer = %d/%d",
               buffer_count,
               buffer_size);

        fprintf(trace,
                " | Buffer = %d/%d",
                buffer_count,
                buffer_size);

        if (buffer_count >= buffer_size)
        {
            printf("\n*** BUFFER FULL ***");

            fprintf(trace,
                    "\n*** BUFFER FULL ***");

            dropped = buffer_count;

            stats.packets_lost += dropped;

            printf("\nClearing traffic from buffer...");
            printf("\n%d packets dropped",
                   dropped);

            fprintf(trace,
                    "\nClearing traffic from buffer...");

            fprintf(trace,
                    "\n%d packets dropped",
                    dropped);

            buffer_count = 0;

            cwnd = cwnd / 2;

            if (cwnd < 1)
                cwnd = 1;

            printf("\nCongestion detected");
            printf("\nCongestion window reduced to %d\n",
                   cwnd);

            fprintf(trace,
                    "\nCongestion detected");

            fprintf(trace,
                    "\nCongestion window reduced to %d\n",
                    cwnd);
        }
        else
        {
            delivered = 1;

            stats.packets_received += delivered;

            stats.total_bytes +=
                (long)PACKET_SIZE;

            stats.total_delay += 26.0;

            if (packets_in_window >= cwnd)
            {
                if (cwnd < MAX_CWND)
                    cwnd++;

                packets_in_window = 0;
            }

            printf(" | Delivered");

            printf(" | cwnd = %d\n",
                   cwnd);

            fprintf(trace,
                    " | Delivered");

            fprintf(trace,
                    " | cwnd = %d\n",
                    cwnd);
        }

        fprintf(cwnd_file,
                "%d %d\n",
                packet,
                cwnd);
    }
    if (buffer_count > 0)
    {
        printf("\n===== BUFFER FLUSH =====\n");

        printf("%d packets remaining in buffer\n",
               buffer_count);

        printf("Flushing remaining traffic...\n");

        fprintf(trace,
                "\n===== BUFFER FLUSH =====\n");

        fprintf(trace,
                "%d packets remaining in buffer\n",
                buffer_count);

        stats.packets_received += buffer_count;

        stats.total_bytes +=
            (long)buffer_count * PACKET_SIZE;

        stats.total_delay +=
            buffer_count * 26.0;

        buffer_count = 0;
    }

    {
        double simulation_time;

        simulation_time =
            total_packets * 0.01;

        if (simulation_time <= 0)
            simulation_time = 1.0;

        throughput =
            (stats.total_bytes * 8.0) /
            simulation_time /
            1000000.0;
    }

    if (stats.packets_sent > 0)
    {
        delivery_ratio =
            ((double)stats.packets_received /
             stats.packets_sent) * 100.0;
    }
    else
    {
        delivery_ratio = 0.0;
    }
    if (stats.packets_received > 0)
    {
        average_delay =
            stats.total_delay /
            stats.packets_received;
    }
    else
    {
        average_delay = 0.0;
    }

    printf("\n====================================\n");
    printf("       PERFORMANCE RESULTS\n");
    printf("====================================\n");

    printf("Number of nodes       : %d\n",
           MAX_NODES);

    printf("Number of links       : %d\n",
           MAX_LINKS);

    printf("Packets requested     : %d\n",
           total_packets);

    printf("Buffer size           : %d packets\n",
           buffer_size);

    printf("Packets sent          : %d\n",
           stats.packets_sent);

    printf("Packets received      : %d\n",
           stats.packets_received);

    printf("Packets lost          : %d\n",
           stats.packets_lost);

    printf("Throughput            : %.3f Mbps\n",
           throughput);

    printf("Packet Delivery Ratio : %.2f %%\n",
           delivery_ratio);

    printf("Average Delay         : %.2f ms\n",
           average_delay);

    printf("====================================\n");

    fprintf(trace,
            "\n====================================\n");

    fprintf(trace,
            "       PERFORMANCE RESULTS\n");

    fprintf(trace,
            "====================================\n");

    fprintf(trace,
            "Number of nodes       : %d\n",
            MAX_NODES);

    fprintf(trace,
            "Number of links       : %d\n",
            MAX_LINKS);

    fprintf(trace,
            "Packets requested     : %d\n",
            total_packets);

    fprintf(trace,
            "Buffer size           : %d packets\n",
            buffer_size);

    fprintf(trace,
            "Packets sent          : %d\n",
            stats.packets_sent);

    fprintf(trace,
            "Packets received      : %d\n",
            stats.packets_received);

    fprintf(trace,
            "Packets lost          : %d\n",
            stats.packets_lost);

    fprintf(trace,
            "Throughput            : %.3f Mbps\n",
            throughput);

    fprintf(trace,
            "Packet Delivery Ratio : %.2f %%\n",
            delivery_ratio);

    fprintf(trace,
            "Average Delay         : %.2f ms\n",
            average_delay);

    fprintf(trace,
            "====================================\n");

    fclose(trace);
    fclose(cwnd_file);

    printf("\nFiles generated:\n");
    printf("network_trace.txt\n");
    printf("cwnd.dat\n");

    return 0;
}
======================================
import matplotlib.pyplot as plt

time = []
cwnd = []

with open("cwnd.dat", "r") as file:
    for line in file:
        values = line.split()

        if len(values) >= 2:
            time.append(float(values[0]))
            cwnd.append(float(values[1]))

plt.plot(time, cwnd)

plt.xlabel("Time (seconds)")
plt.ylabel("Congestion Window (packets)")
plt.title("TCP Congestion Window vs Time")

plt.grid(True)

plt.savefig("cwnd.png")

plt.show()