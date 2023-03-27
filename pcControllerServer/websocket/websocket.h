#ifndef WEBSOCKET_H
#define WEBSOCKET_H

#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include "../socket/socket.h"
#include "../misc/datastructures/stack.h"
#include "../misc/datastructures/conqueue.h"

typedef enum  {
    FRAME_TYPE_TEXT = 0x1,
    FRAME_TYPE_BINARY = 0x2,
    FRAME_TYPE_PING = 0x09,
    FRAME_TYPE_PONG = 0x0A,
    FRAME_TYPE_CLOSING = 0x08
} WEBSocketFrameType;

typedef enum {
    SERVER_STATE_STOPPED,
    SERVER_STATE_HANDLING,
    SERVER_STATE_STOPPING,
} WEBSocketServerState;

typedef enum {
   CONNECTION_STATE_FINISHED,
   CONNECTION_STATE_ESTABLISHED
} WEBSocketConnectionState;

#define WS_BUFF_FRAME_SIZE 65552
#define SHA_DIGEST_LENGTH 20



struct Handler;
struct WEBSocketConnectionInfo;
typedef struct {
  WEBSocketServerState state;
  unsigned int clientsMax;
  Socket socket;
  struct Handler* handler;
  pthread_mutex_t mutex;
  struct WEBSocketConnectionInfo* connections;
  Stack* availableIds;
  ConQueue* acceptsQueue;
  unsigned int connectionsCount;
} WEBSocketServer;

typedef struct WEBSocketConnectionInfo {
  WEBSocketConnectionState state;
  int id;
  int socket;
  Stack* freePositions;
  WEBSocketServer* ws;
  pthread_t thread;
} WEBSocketConnectionInfo;

typedef int (*Reciever)(WEBSocketConnectionInfo* wsConnectionInfo, unsigned char payload[WS_BUFF_FRAME_SIZE], int inputSize, char* response);
typedef struct Handler {
    Reciever onMessage;
    Reciever onBinary;
} Handler;

static const char* GUIDKey;

WEBSocketServer WEBSocket_init(int port, Handler* handler, int clientsMax);
void WEBSocket_processConnections(WEBSocketServer* wsServer);
void WEBSocket_ping(WEBSocketServer* ws, int connectionSocket, const char* optionalText);
void WEBSocket_stop(WEBSocketServer* ws);

#endif // WEBSOCKET_H
