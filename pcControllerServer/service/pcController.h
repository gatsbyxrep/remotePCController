#ifndef PCCONTROLLER_H
#define PCCONTROLLER_H

#include "codogram/codogrambase.h"
#include "codogram/mousemovecodogram.h"
#include "../websocket/websocket.h"
#include "codogram/codogramids.h"


static void Controller_mouseMove(MousePoint mousePoint) {
    printf("Move mouse: %f %f \n", mousePoint.x, mousePoint.y);
}
static void Controller_sendKey(char key) {
    printf("Char key: %c \n", key);
}
static void Controller_leftMouseClick() {
    printf("left mouse click \n");
}

int Controller_process(WEBSocketConnectionInfo* wsConnectionInfo, unsigned char payload[WS_BUFF_FRAME_SIZE], int size, char* response) {
    CodogramBase cdgBase = Codogram_construct(payload);
    switch(Codogram_getId(&cdgBase)) {
        case CODOGRAM_ID_MOUSE_MOVE: {
            Controller_mouseMove(MouseMoveCodogram_getMousePoint(cdgBase));
            break;
        }
        case CODOGRAM_ID_SEND_KEY: {
            Controller_sendKey('a');
            break;
        }
        case CODOGRAM_ID_LEFT_MOUSE_CLICK: {
            Controller_leftMouseClick();
            break;
        }
    }
    return -1;
}

#endif // PCCONTROLLER_H
