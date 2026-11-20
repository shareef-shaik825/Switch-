#include <stdio.h>
#include <string.h>

#define MAX_MACS 100
#define MAX_PORTS 8

typedef struct {
    char mac[18];
    int port;
} MACTableEntry;

typedef struct {
    char src_mac[18];
    char dst_mac[18];
    int in_port;
} Frame;

MACTableEntry macTable[MAX_MACS];
int macCount = 0;

/* Learn source MAC */
void learnMAC(char *mac, int port)
{
    for (int i = 0; i < macCount; i++) {
        if (strcmp(macTable[i].mac, mac) == 0) {
            macTable[i].port = port;
            return;
        }
    }

    if (macCount < MAX_MACS) {
        strcpy(macTable[macCount].mac, mac);
        macTable[macCount].port = port;
        macCount++;
    }
}

/* Lookup destination MAC */
int lookupMAC(char *mac)
{
    for (int i = 0; i < macCount; i++) {
        if (strcmp(macTable[i].mac, mac) == 0) {
            return macTable[i].port;
        }
    }

    return -1;
}

/* Forward frame */
void forwardFrame(Frame *frame)
{
    printf("\nReceived Frame:\n");
    printf("SRC=%s DST=%s IN_PORT=%d\n",
           frame->src_mac, frame->dst_mac, frame->in_port);

    /* Learn source MAC address */
    learnMAC(frame->src_mac, frame->in_port);

    /* Look up destination MAC address */
    int outPort = lookupMAC(frame->dst_mac);

    if (outPort != -1) {
        if (outPort == frame->in_port) {
            printf("Destination is on the incoming port; filtering frame.\n");
        } else {
            printf("Known destination.\n");
            printf("Forwarding frame to port %d\n", outPort);
        }
    } else {
        printf("Unknown destination.\n");
        printf("Flooding on all ports except %d\n", frame->in_port);

        for (int p = 1; p <= MAX_PORTS; p++) {
            if (p != frame->in_port) {
                printf(" -> Port %d\n", p);
            }
        }
    }
}

/* Display MAC address table */
void printMACTable(void)
{
    printf("\n=== MAC TABLE ===\n");

    for (int i = 0; i < macCount; i++) {
        printf("%s --> Port %d\n",
               macTable[i].mac, macTable[i].port);
    }
}

/* Simplified STP root bridge election */
void runSpanningTree(int bridgeIDs[], int count)
{
    if (count <= 0) {
        printf("No switches available for STP election.\n");
        return;
    }

    int rootBridge = bridgeIDs[0];

    for (int i = 1; i < count; i++) {
        if (bridgeIDs[i] < rootBridge) {
            rootBridge = bridgeIDs[i];
        }
    }

    printf("\n=== SPANNING TREE ===\n");
    printf("Root Bridge = %d\n", rootBridge);

    for (int i = 0; i < count; i++) {
        if (bridgeIDs[i] == rootBridge) {
            printf("Switch %d : ROOT\n", bridgeIDs[i]);
        } else {
            printf("Switch %d : NON-ROOT\n", bridgeIDs[i]);
        }
    }
}

int main(void)
{
    int bridgeIDs[] = {10, 20, 5, 30};

    runSpanningTree(bridgeIDs, 4);

    Frame f1 = {
        "AA:AA:AA:AA:AA:01",
        "BB:BB:BB:BB:BB:02",
        1
    };

    Frame f2 = {
        "BB:BB:BB:BB:BB:02",
        "AA:AA:AA:AA:AA:01",
        3
    };

    Frame f3 = {
        "CC:CC:CC:CC:CC:03",
        "AA:AA:AA:AA:AA:01",
        2
    };

    forwardFrame(&f1);
    forwardFrame(&f2);
    forwardFrame(&f3);

    printMACTable();

    return 0;
}
