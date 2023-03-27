#include <stdio.h>
#include <stdint.h>
#include "websocket/websocket.h"
#include "signal.h"


WEBSocketServer ws; // TODO: think about it

static int testFunc(WEBSocketConnectionInfo* wsConnectionInfo, unsigned char payload[WS_BUFF_FRAME_SIZE], int size, char* response) {
    printf("Conn id: %d, Thead id: %d, Message: %s \n", wsConnectionInfo->id, getpid(), payload);
    response = NULL;
    return -1;
}
static void signalIntHandler(const int signal) {
    printf("SIGNAL INT");
    WEBSocket_stop(&ws);
}
int main() {
    signal(SIGINT, signalIntHandler);
    Handler handler = {
        .onMessage = testFunc
    };
    ws = WEBSocket_init(228, &handler, 250);
    WEBSocket_processConnections(&ws);
}


