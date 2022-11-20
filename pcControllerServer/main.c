#include <stdio.h>
#include <stdint.h>
#include "websocket/websocket.h"

void testFunc(Socket socket) {

}

int main() {
    Handler handler = {
        .onMessage = testFunc
    };

    WEBSocketServer ws = WEBSocket_init(228, &handler);
    return 0;
}
