#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define MAX 100

int createSocket()
{
    int s = socket(AF_INET,SOCK_STREAM,0);

    if(s < 0)
    {
        perror("Socket");
        exit(1);
    }

    return s;
}

void connectServer(int s, struct sockaddr_in *server)
{
    server->sin_family = AF_INET;
    server->sin_port = htons(PORT);
    server->sin_addr.s_addr = inet_addr("127.0.0.1");

    if(connect(s,(struct sockaddr *)server,
               sizeof(*server)) < 0)
    {
        perror("Connect");
        exit(1);
    }
}

void requestARP(int s)
{
    char ip[MAX], mac[MAX], response[MAX];

    printf("Enter IP address: ");
    scanf("%19s",ip);

    send(s,ip,strlen(ip)+1,0);

    recv(s,response,MAX-1,0);

    if(strcmp(response,"NOT_FOUND")==0)
    {
        printf("IP not found.\n");

        printf("Enter MAC address: ");
        scanf("%19s",mac);

        send(s,mac,strlen(mac)+1,0);

        recv(s,response,MAX-1,0);

        printf("New ARP entry added.\n");
        printf("IP  : %s\n",ip);
        printf("MAC : %s\n",mac);
    }
    else
    {
        printf("MAC address found.\n");
        printf("IP  : %s\n",ip);
        printf("MAC : %s\n",response);
    }
}

void startClient()
{
    int s;
    struct sockaddr_in server;

    s = createSocket();

    connectServer(s,&server);

    requestARP(s);

    close(s);
}

int main()
{
    startClient();
    return 0;
}