#include<stdio.h>
#include<string.h>

#define PACKET_SIZE 16
#define FRAGMENT_SIZE 10

int port;
char portbin[17];

char sourcePortBin[17];
char destPortBin[17];

char transportSegment[5000];

char mes[100];
char binary[1000];

char srcurl[50], desurl[50];
char srcip[50], desip[50];
char srcmac[50], desmac[50];
char srcipbin[100], desipbin[100];
char srcmacbin[200], desmacbin[200];

int n;
int i, j, k;

char bin[9];
char url[50];
char ip[50];
char mac[50];
char ipbin[100];
char macbin[200];
int x[4];

#define SOURCE_PORT 4567
#define DEST_PORT 8921

char packet[500][100];
char fragment[500][100];

int packetcount = 0;
int fragmentcount = 0;

struct url {
    char url[50];
    char ip[50];
} urltab[] = {
    {"google.com","172.217.160.78"},
    {"mepcoeng.ac.in","172.16.16.1"},
    {"gmail.com","172.217.179.98"},
    {"youtube.com","172.15.178.10"},
    {"facebook.com","172.254.16.56"},
    {"instagram.com","172.145.166.176"}
};

struct mac {
    char ip[50];
    char mac[50];
} mactab[] = {
    {"172.217.160.78","AA:1B:34:6C:33:1A"},
    {"172.217.179.98","AA:1B:34:6C:33:02"},
    {"172.15.178.10","AA:1B:34:5C:32:34"},
    {"172.254.16.56","AA:1B:34:4C:33:04"},
    {"172.145.166.176","A2:1B:34:6C:34:06"},
    {"172.16.16.1","A2:34:1A:03:3C:12"}
};

int urlcount = 6;
int maccount = 6;

void dectobin() {
    int local_i;
    for(local_i = 7; local_i >= 0; local_i--) {
        bin[local_i] = (n % 2) + '0';
        n = n / 2;
    }
    bin[8] = '\0';
}

void mestobin() {
    int count = 0;
    int local_i, local_j;
    for(local_i = 0; mes[local_i] != '\0'; local_i++) {
        n = mes[local_i];
        dectobin();
        for(local_j = 0; local_j < 8; local_j++) {
            binary[count++] = bin[local_j];
        }
    }
    binary[count] = '\0';
}

void searchip() {
    int local_i;
    strcpy(ip, "0.0.0.0");
    for(local_i = 0; local_i < urlcount; local_i++) {
        if(strcmp(url, urltab[local_i].url) == 0) {
            strcpy(ip, urltab[local_i].ip);
            break;
        }
    }
}

void searchmac() {
    int local_i;
    strcpy(mac, "00:00:00:00:00:00");
    for(local_i = 0; local_i < maccount; local_i++) {
        if(strcmp(ip, mactab[local_i].ip) == 0) {
            strcpy(mac, mactab[local_i].mac);
            break;
        }
    }
}

void iptobin() {
    int pos = 0;
    int local_i, local_k;
    sscanf(ip, "%d.%d.%d.%d", &x[0], &x[1], &x[2], &x[3]);
    for(local_i = 0; local_i < 4; local_i++) {
        n = x[local_i];
        dectobin();
        for(local_k = 0; local_k < 8; local_k++) {
            ipbin[pos++] = bin[local_k];
        }
    }
    ipbin[pos] = '\0';
}

void mactobin() {
    int pos = 0;
    int local_i;
    for(local_i = 0; mac[local_i] != '\0'; local_i++) {
        if(mac[local_i] != ':') { 
            if(mac[local_i] >= '0' && mac[local_i] <= '9')
                n = mac[local_i] - '0';
            else
                n = mac[local_i] - 'A' + 10;
            
            dectobin();
            macbin[pos++] = bin[4];
            macbin[pos++] = bin[5];
            macbin[pos++] = bin[6];
            macbin[pos++] = bin[7];
        }
    }
    macbin[pos] = '\0';
}

void portToBinary() {
    int local_i;
    for(local_i = 15; local_i >= 0; local_i--) {
        portbin[local_i] = (port % 2) + '0';
        port = port / 2;
    }
    portbin[16] = '\0';
}

void transportLayer() {
    transportSegment[0] = '\0';
    
    port = SOURCE_PORT;
    portToBinary();
    strcpy(sourcePortBin, portbin);

    port = DEST_PORT;
    portToBinary();
    strcpy(destPortBin, portbin);

    strcpy(transportSegment, sourcePortBin);
    strcat(transportSegment, destPortBin);
    strcat(transportSegment, binary);
}

void createPackets() {
    int len = strlen(binary); 
    int pos = 0, c = 0, local_i;
    char temp[20];

    while(pos < len) {
        for(local_i = 0; local_i < PACKET_SIZE; local_i++) {
            if(pos < len)
                temp[local_i] = binary[pos++]; 
            else
                temp[local_i] = '0';
        }
        temp[PACKET_SIZE] = '\0';
        
        strcpy(packet[c], temp);
        strcat(packet[c], sourcePortBin);
        strcat(packet[c], destPortBin);
        c++;
    }
    packetcount = c;
}

void createFragments() {
    int len = strlen(binary); 
    int pos = 0, c = 0, local_i;
    char temp[20];

    while(pos < len) {
        for(local_i = 0; local_i < FRAGMENT_SIZE; local_i++) {
            if(pos < len)
                temp[local_i] = binary[pos++]; 
            else
                temp[local_i] = '0';
        }
        temp[FRAGMENT_SIZE] = '\0';
        
        strcpy(fragment[c], temp);
        strcat(fragment[c], sourcePortBin);
        strcat(fragment[c], destPortBin);
        c++;
    }
    fragmentcount = c;
}

int main() {
    int local_i;
    char errorCode[] = "00000000";

    printf("Enter message:");
    scanf("%s", mes);

    printf("Enter source url:");
    scanf("%s", srcurl);

    printf("Enter destination url:");
    scanf("%s", desurl);

    mestobin();
    transportLayer();
    createPackets();
    createFragments();

    strcpy(url, srcurl);
    searchip();
    strcpy(srcip, ip);

    strcpy(url, desurl);
    searchip();
    strcpy(desip, ip);

    strcpy(ip, srcip);
    searchmac();
    strcpy(srcmac, mac);

    strcpy(ip, desip);
    searchmac();
    strcpy(desmac, mac);

    strcpy(ip, srcip);
    iptobin();
    strcpy(srcipbin, ipbin);

    strcpy(ip, desip);
    iptobin();
    strcpy(desipbin, ipbin);

    strcpy(mac, srcmac);
    mactobin();
    strcpy(srcmacbin, macbin);

    strcpy(mac, desmac);
    mactobin();
    strcpy(desmacbin, macbin);

    printf("\n\n=========== APPLICATION LAYER ===========");
    printf("\nMessage : %s", mes);

    printf("\n\n=========== TRANSPORT LAYER ===========");
    printf("\nSource Port              : %d", SOURCE_PORT);
    printf("\nDestination Port         : %d", DEST_PORT);
    printf("\n\nSource Port(Binary)      : %s", sourcePortBin);
    printf("\nDestination Port(Binary) : %s", destPortBin);
    printf("\n\nMessage(Binary):\n%s", binary);
    printf("\nTransport Layer Output:\n%s\n", transportSegment);

    printf("\n\n=========== NETWORK LAYER ===========");
    for(local_i = 0; local_i < packetcount; local_i++) {
        printf("\nPacket %d", local_i + 1);
        printf("\nSource IP      : %s", srcipbin);
        printf("\nDestination IP : %s", desipbin);
        printf("\nPacket Data    : %s\n", packet[local_i]);
    }

    printf("\n\n=========== DATA LINK LAYER ===========");
    printf("\n----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------");
    printf("\nFrag\tSource MAC\t\t\t\tDestination MAC\t\t\t\tSource IP\t\t\tDestination IP\t\t\tSrc Port\tDes Port\tFragment Data\t\t\t\tError Code");
    printf("\n----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------");
    
    for(local_i = 0; local_i < fragmentcount; local_i++) {
        printf("\n%d\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s", 
               local_i + 1, srcmacbin, desmacbin, srcipbin, desipbin, sourcePortBin, destPortBin, fragment[local_i], errorCode);
    }
    printf("\n----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------");
	
    printf("\n\n=========== SUMMARY ===========");
    printf("\nTotal Bits      : %ld", strlen(binary));
    printf("\nTotal Packets   : %d", packetcount);
    printf("\nTotal Fragments : %d\n", fragmentcount);

    return 0;
}
