#include "websocket.h"
#include "../misc/SHA1/sha1.h"


static const char* GUIDKey = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11"; //36

static void WEBSocket_readWebSocketKeyFromLine(char* dst, const char* request) {
       int i = 0, it = 0;
       for(i = 19; it < 24; i++, it++) {
          dst[it] = request[i];
        }
       strcat_s(dst, 64, GUIDKey);
}
static void WEBSocket_establishConnection(WEBSocketServer* wsServer, int connectionSocket) {
    char* line = NULL;
    char buffer[1024] = {0};
    printf("Establish connection");
    if(recv(connectionSocket, buffer, 1024, 0) > 0) {
        if((line = strstr(buffer, "Sec-WebSocket-Key:")) != NULL) {
            char clientKey[64] = {0};
            WEBSocket_readWebSocketKeyFromLine(clientKey, line);
            printf("%s\n", clientKey);
            char hash[SHA_DIGEST_LENGTH];
            SHA1((unsigned char*)hash, clientKey, strlen(clientKey));
            ////////////////////////////Base64////////////////////////////////////
            unsigned char key_out[64] = {0};
            base64_encode((unsigned char*)hash, key_out, sizeof(hash));
            printf("\nKey_for_client:%s\n", key_out);

            char response[131] = {0};
            _snprintf_s(response,
                     130, _TRUNCATE, "%s%s%s%s",
                     "HTTP/1.1 101 Switching Protocols\r\n",
                     "Upgrade: websocket\r\n"
                     "Connection: Upgrade\r\n"
                     "Sec-WebSocket-Accept: ", key_out,
                     "\r\n\r\n\r\n");
            if(send(connectionSocket, response, strlen(response), 0) < 0) {
                printf("Can't send handshake headers");
                return;
            }
        }
    }
}
static void WEBSocket_destroy(WEBSocketServer* ws) {
    free(ws->connections);
    Stack_destroy(ws->availableIds);
    ConQueue_destroy(ws->acceptsQueue);
    Socket_close(ws->socket);
    WSACleanup();
    printf("%s\n", "WEBSOCKET SHUTDOWNED");
}
static void WEBSocket_disconnect(WEBSocketServer* ws, int connectionSocket, const char* reason) {
    printf("Close socket: %d due client has been disconected: %s \n", connectionSocket, reason);
    Socket_close(connectionSocket);
}
static void WEBSocket_send(WEBSocketServer* ws, int connectionSocket, int fin, const char* message, int messageSize) {
    char response[WS_BUFF_FRAME_SIZE] = {0};
    response[0] = fin;
    if(messageSize < 126) {
        response[1] = messageSize;
        messageSize += 2;
    }
    else {
        response[1] = 126;
        response[2] = (messageSize >> 8) & 0xFF;
        response[3] = messageSize & 0xFF;
        messageSize += 4;
    }
    strcat_s(response, messageSize, message);
    if(send(connectionSocket, response, messageSize, 0) == -1) {
        WEBSocket_disconnect(ws, connectionSocket, "Cant answer");
    }

}

static void WEBSocket_printConnectionData(char inputBuffer[WS_BUFF_FRAME_SIZE]) {
    printf("%s\n", "Frame recv");
    printf("FIN: 0x%02X\n", inputBuffer[0] & 0x01);
    printf("RSV1: 0x%02X\n", inputBuffer[0] & 0x02);
    printf("RSV2: 0x%02X\n", inputBuffer[0] & 0x04);
    printf("RSV3: 0x%02X\n", inputBuffer[0] & 0x08);
    printf("frameType: 0x%02X\n", inputBuffer[0] & 0x0F);
    printf("Mask: 0x%02x\n", inputBuffer[1] & 0x80 ? 1:0);
}


static void WEBSocket_readPayload(unsigned char payload[WS_BUFF_FRAME_SIZE], char inputBuffer[WS_BUFF_FRAME_SIZE]) {
    char maskKey[4] = {0};
    for(unsigned int i = 0; i < 4; i++)
        maskKey[i] = inputBuffer[i + 2];
    for(unsigned int i = 6, pl = 0; pl < (inputBuffer[1] & 0x7F); i++, pl++)
        payload[pl] = inputBuffer[i] ^ maskKey[pl % 4];
}


static short int WEBSocket_performFrames(WEBSocketConnectionInfo* wsConnectionInfo, char inputBuffer[WS_BUFF_FRAME_SIZE]) {
    WEBSocketFrameType frameType = inputBuffer[0] & 0x0F; // Opcode
    unsigned char payload[WS_BUFF_FRAME_SIZE] = {0};
    WEBSocket_readPayload(payload, inputBuffer);
    short int connectionInProgress = 1;
    // Close 0x88, text 0x81
    char* response = NULL;
    int responseSize = -1;
    switch(frameType) {
        case FRAME_TYPE_CLOSING: {
            WEBSocket_disconnect(wsConnectionInfo->ws, wsConnectionInfo->socket, "Client send disconect frame");
            connectionInProgress = 0;
            break;
        }
        case FRAME_TYPE_TEXT: {
            responseSize = wsConnectionInfo->ws->handler->onMessage(wsConnectionInfo, payload, WS_BUFF_FRAME_SIZE, response);
            break;
        }
        case FRAME_TYPE_PONG: {
            printf("Payload_len: %d\n", inputBuffer[1] & 0x7F);
            printf("Pong: \"%s\"\n", payload);
        }
        case FRAME_TYPE_PING: {
            break;
        }
        case FRAME_TYPE_BINARY: {
            responseSize = wsConnectionInfo->ws->handler->onBinary(wsConnectionInfo, payload, WS_BUFF_FRAME_SIZE, response);
            break;
        }
        default: {
            break;
        }
    }
    if(responseSize != -1) {
        WEBSocket_send(wsConnectionInfo->ws, wsConnectionInfo->socket, 0x81, response, responseSize);
    }
    if(wsConnectionInfo->ws->state == SERVER_STATE_STOPPING) {
        WEBSocket_send(wsConnectionInfo->ws, wsConnectionInfo->socket, 0x88, "Server is stopping", strlen("Server is stopping"));
        connectionInProgress = 0;
    }
    return connectionInProgress;
}

static void* WEBSocket_handleConnection(void* args) {
    WEBSocketConnectionInfo* wsConnectionInfo = (WEBSocketConnectionInfo*)args;
    unsigned long mode = 1; // Non-blocking mode
    ioctlsocket(wsConnectionInfo->socket, FIONBIO, &mode);
    for(;;) {
        char inputBuffer[WS_BUFF_FRAME_SIZE] = {0};
        pthread_mutex_lock(&wsConnectionInfo->ws->mutex);
        if(wsConnectionInfo->ws->state == SERVER_STATE_STOPPING) {
            wsConnectionInfo->ws->connectionsCount--;
            pthread_mutex_unlock(&wsConnectionInfo->ws->mutex);
            break;
        }
        pthread_mutex_unlock(&wsConnectionInfo->ws->mutex);
        if(recv(wsConnectionInfo->socket, inputBuffer, WS_BUFF_FRAME_SIZE - 1, 0) > 0) {
            WEBSocket_printConnectionData(inputBuffer);
            if(WEBSocket_performFrames(wsConnectionInfo, inputBuffer) == 0) {
               pthread_mutex_lock(&wsConnectionInfo->ws->mutex);
               Stack_push(wsConnectionInfo->freePositions, (void*)(intptr_t)wsConnectionInfo->id);
               wsConnectionInfo->ws->connectionsCount--;
               pthread_mutex_unlock(&wsConnectionInfo->ws->mutex);
               break;
            }
        }
    }
    return (void*)1;
}



WEBSocketServer WEBSocket_init(int port, Handler* handler, int clientsMax) {
    Socket socket = Socket_init(port, SOCK_STREAM);
    pthread_mutex_t mutex;
    pthread_mutex_init(&mutex, NULL);
    WEBSocketConnectionInfo* connections = malloc(clientsMax * sizeof(WEBSocketConnectionInfo));
    Stack* availableIds = Stack_create();
    Stack_push(availableIds, (void*)(intptr_t)clientsMax - 1); // TODO: fix this
    ConQueue* acceptsQueue = ConQueue_create();
    WEBSocketServer ws = {
        .state = SERVER_STATE_STOPPED,
        .handler = handler,
        .socket = socket,
        .clientsMax = clientsMax,
        .mutex = mutex,
        .connections = connections,
        .availableIds = availableIds,
        .acceptsQueue = acceptsQueue,
        .connectionsCount = 0
    };
    return ws;
}
static void WEBSocket_createConnectionThread(WEBSocketServer* ws, unsigned int id, Socket connectionSocket) {
    pthread_t connectionThread;
    WEBSocketConnectionInfo connectionInfo = {
        .id = id,
        .socket = connectionSocket,
        .ws = ws,
        .thread = connectionThread,
    };
    ws->connections[id] = connectionInfo;
    if(pthread_create(&ws->connections[id].thread, NULL,  WEBSocket_handleConnection, &ws->connections[id]) != 0) {
        printf("%s\n", "can't create connection thread");
        WEBSocket_disconnect(ws, connectionSocket, "cant create connection thread");
        return;
    }
    pthread_mutex_lock(&ws->mutex);
    ws->connectionsCount++;
    pthread_mutex_unlock(&ws->mutex);
    pthread_detach(ws->connections[id].thread);
}

static void* WEBSocket_acceptConnections(void* args) {
    WEBSocketServer* ws = (WEBSocketServer*)args;
    unsigned long mode = 1; // Non-blocking mode
    ioctlsocket(ws->socket, FIONBIO, &mode);
    for(;;) {
        pthread_mutex_lock(&ws->mutex);
        short int serverState = ws->state;
        pthread_mutex_unlock(&ws->mutex);
        if(serverState != SERVER_STATE_HANDLING) {
            printf("finish accepting");
            break;
        }
        struct in_addr clientIP;
        struct sockaddr_in clientAddr;
        int addrLen = sizeof(struct sockaddr_in);
        int connectionSocket = accept(ws->socket, (struct sockaddr*)&clientAddr, &addrLen);
        if(connectionSocket <= 0) {
            continue;
        }
        memcpy_s(&clientIP, 4, &clientAddr.sin_addr, 4);
        ConQueue_push(ws->acceptsQueue, (void*)(uintptr_t)(connectionSocket));
        // TODO: Probably we have to establishConnection here
        printf("Accept completed (IP address of client = %s  port = %d) \n", inet_ntoa(clientIP),
               ntohs(clientAddr.sin_port));
    }
    return (void*)1;
}

void WEBSocket_processConnections(WEBSocketServer* ws) {
    unsigned int clientsCount = 0, highestIndex = 0;
    ws->state = SERVER_STATE_HANDLING;
    pthread_t acceptThread;
    if(pthread_create(&acceptThread, NULL,  WEBSocket_acceptConnections, ws) != 0) {
        printf("%s\n", "can't create accept connections thread");
        return;
    }
    for(;;) {
        pthread_mutex_lock(&ws->mutex);
        short int state = ws->state;
        pthread_mutex_unlock(&ws->mutex);
        if(state == SERVER_STATE_STOPPING) {
            printf("Server is stopping, cons: %d, clients max: %d \n", ws->connectionsCount, ws->clientsMax);
            pthread_mutex_lock(&ws->mutex);
            int connectionsCount = ws->connectionsCount;
            pthread_mutex_unlock(&ws->mutex);
            if(connectionsCount == 0) { // If no active connections
                printf("Main thread is finished");
                break;
            }
        }
        if(ConQueue_isEmpty(ws->acceptsQueue))
            continue;
        int connectionSocket = (uintptr_t)ConQueue_pop(ws->acceptsQueue);
        printf("new con \n");
        int id = 0;
        if(highestIndex < ws->clientsMax - 1) {
            id = highestIndex++;
        } else {
            pthread_mutex_lock(&ws->mutex);
            short int isEmpty = !Stack_isEmpty(ws->availableIds);
            pthread_mutex_unlock(&ws->mutex);
            if(!isEmpty) {
                continue;
            }
            pthread_mutex_lock(&ws->mutex);
            Node* node = Stack_pop(ws->availableIds);
            pthread_mutex_unlock(&ws->mutex);
            id = *((int *)node->value);
        }
        WEBSocket_establishConnection(ws, connectionSocket);
        WEBSocket_createConnectionThread(ws, id, connectionSocket);
    }
    pthread_mutex_lock(&ws->mutex);
    ws->state = SERVER_STATE_STOPPED;
    pthread_mutex_unlock(&ws->mutex);
    pthread_join(acceptThread, NULL);
    WEBSocket_destroy(ws);
}

void WEBSocket_stop(WEBSocketServer* ws) {
    pthread_mutex_lock(&ws->mutex);
    ws->state = SERVER_STATE_STOPPING;
    pthread_mutex_unlock(&ws->mutex);
}
