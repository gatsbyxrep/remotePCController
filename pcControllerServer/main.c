#include <stdio.h>
#include <stdint.h>
#include "websocket/websocket.h"

void testFunc(Socket socket, unsigned char payload[WS_BUFF_FRAME_SIZE]) {
    printf("%s \n", payload);
}

int main() {
    Handler handler = {
        .onMessage = testFunc
    };

    WEBSocketServer ws = WEBSocket_init(228, &handler);
    WEBSocket_acceptConnections(&ws);
    WEBSocket_stop(&ws);
    return 0;
}
