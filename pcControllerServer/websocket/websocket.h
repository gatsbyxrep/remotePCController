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
    void (*onMessage)(Socket socket, unsigned char payload[WS_BUFF_FRAME_SIZE]);
} Handler;

typedef struct WEBSocketServer {
    Socket socket;
    Handler* handler;
} WEBSocketServer;




static const char* GUIDKey;

WEBSocketServer WEBSocket_init(int port, Handler* handler);
void WEBSocket_acceptConnections(WEBSocketServer* wsServer);
void WEBSocket_readWebSocketKeyFromLine(char* dst, const char* line);
void WEBSocket_establishConnection(WEBSocketServer* wsServer, int connectionSocket); // Do handshake
void WEBSocket_handleConnection(WEBSocketServer* wsServer, int connectionSocket);
void WEBSocket_performFrames(WEBSocketServer* ws, int connectionSocket, char inputBuffer[WS_BUFF_FRAME_SIZE]);
void WEBSocket_ping(WEBSocketServer* ws, int connectionSocket, const char* optionalText);
void WEBSocket_readPayload(unsigned char payload[WS_BUFF_FRAME_SIZE], char inputBuffer[WS_BUFF_FRAME_SIZE]);
void WEBSocket_disconnect(WEBSocketServer* ws, int connectionSocket, const char* reason);
void WEBSocket_send(WEBSocketServer* ws, int connectionSocket, int fin, const char* message);
void WEBSocket_stop(WEBSocketServer* ws);

#endif // WEBSOCKET_H
