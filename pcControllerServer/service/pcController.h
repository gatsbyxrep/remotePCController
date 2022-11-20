#ifndef PCCONTROLLER_H
#define PCCONTROLLER_H
#include "commands/commands.h"
#include "../websocket/websocket.h"


void control(char* response, const char* request) {
    WEBSocket_writeEstablishConnectionHeaders(response, request);
    switch(request[0]) {
        case 0: {
            COMMANDS_mouseMove(request[1], request[2]);
            break;
        }
        case 1: {
            COMMANDS_sendKey(request[1]);
            break;
        }
    }
}


#endif // PCCONTROLLER_H
