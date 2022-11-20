#ifndef WEBSOCKET_H
#define WEBSOCKET_H

#include <stdio.h>
#include <string.h>
#include "../misc/SHA1/sha1.h"
#include "../socket/socket.h"

#define WS_BUFF_FRAME_SIZE 65552
#define WS_TEXT_FRAME 0x01
#define WS_PING_FRAME 0x09
#define WS_PONG_FRAME 0x0A
#define WS_CLOSING_FRAME 0x08
#define SHA_DIGEST_LENGTH 20


typedef struct Handler {
    void (*onMessage)(Socket socket);
} Handler;

typedef struct WEBSocketServer {
    Socket socket;
    Handler* handler;
} WEBSocketServer;




static const char* GUIDKey;

WEBSocketServer WEBSocket_init(int port, Handler* handler);
short int WEBSocket_nextFrame(const char* request);
void WEBSocket_acceptConnections(WEBSocketServer* wsServer);
void WEBSocket_readWebSocketKeyFromLine(char* dst, const char* line);
void WEBSocket_establishConnection(WEBSocketServer* wsServer, int connectionSocket);
void WEBSocket_handleConnection(WEBSocketServer* wsServer, int connectionSocket);
void WEBSocket_onPongFrame(unsigned char payload[WS_BUFF_FRAME_SIZE], const char* inputBuffer,  unsigned char payloadLen);
#endif // WEBSOCKET_H
