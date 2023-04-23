#include <stdio.h>
#include <stdint.h>
#include "service/pcController.h"
#include "websocket/websocket.h"
#include "misc/logger/logger.h"
#include "signal.h"


WEBSocketServer ws;

static int testFunc(WEBSocketConnectionInfo* wsConnectionInfo, unsigned char payload[WS_BUFF_FRAME_SIZE], int size, char* response) {
    LOG_printf("Conn id: %d, Thead id: %d, Message: %s \n", wsConnectionInfo->id, getpid(), payload);
    response = NULL;
    return -1;
}

static void signalIntHandler(const int signal) {
    printf("SIGNAL INT");
    WEBSocket_stop(&ws);
}

static void startApp(int port, int maxClientSize) {
    Handler handler = {
        .onMessage = Controller_process,
        .onBinary = Controller_process
    };

    ws = WEBSocket_init(port, &handler, maxClientSize);
    WEBSocket_processConnections(&ws);
}

int main() {
    signal(SIGINT, signalIntHandler);
    startApp(228, 1);
}


