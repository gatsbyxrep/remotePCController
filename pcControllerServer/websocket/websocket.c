#include "websocket.h"



static const char* GUIDKey = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11"; //36

WEBSocketServer WEBSocket_init(int port, Handler* handler) {
    Socket socket = Socket_init(port, SOCK_STREAM);
    WEBSocketServer ws = {
        .handler = handler,
        .socket = socket
    };
    return ws;
}

void WEBSocket_readWebSocketKeyFromLine(char* dst, const char* request) {
       int i = 0, it = 0;
       for(i = 19; it < 24; i++, it++) {
          dst[it] = request[i];
        }
       strcat(dst, GUIDKey);
}
void WEBSocket_establishConnection(WEBSocketServer* wsServer, int connectionSocket) {
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
            snprintf(response,
                     130, "%s%s%s%s",
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

void WEBSocket_disconnect(WEBSocketServer* ws, int connectionSocket, const char* reason) {
    printf("Client has been disconected: %s \n", reason);
    Socket_close(connectionSocket);
    // TODO: Stop thread
}



void WEBSocket_send(WEBSocketServer* ws, int connectionSocket, int fin, const char* message) {
    unsigned int messageSize = strlen(message);
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
    strcat(response, message);
    if(send(connectionSocket, response, messageSize, 0) == -1) {
        WEBSocket_disconnect(ws, connectionSocket, "Cant answer");
    }

}

void WEBSocket_printConnectionData(char inputBuffer[WS_BUFF_FRAME_SIZE]) {
    printf("%s\n", "Frame recv");
    printf("FIN: 0x%02X\n", inputBuffer[0] & 0x01);
    printf("RSV1: 0x%02X\n", inputBuffer[0] & 0x02);
    printf("RSV2: 0x%02X\n", inputBuffer[0] & 0x04);
    printf("RSV3: 0x%02X\n", inputBuffer[0] & 0x08);
    printf("frameType: 0x%02X\n", inputBuffer[0] & 0x0F);
    printf("Mask: 0x%02x\n", inputBuffer[1] & 0x80 ? 1:0);
}


void WEBSocket_readPayload(unsigned char payload[WS_BUFF_FRAME_SIZE], char inputBuffer[WS_BUFF_FRAME_SIZE]) {
    char maskKey[4] = {0};
    for(unsigned int i = 0; i < 4; i++)
        maskKey[i] = inputBuffer[i + 2];
    for(unsigned int i = 6, pl = 0; pl < (inputBuffer[1] & 0x7F); i++, pl++)
        payload[pl] = inputBuffer[i] ^ maskKey[pl % 4];
}


void WEBSocket_performFrames(WEBSocketServer* ws, int connectionSocket, char inputBuffer[WS_BUFF_FRAME_SIZE]) {
    char frameType = inputBuffer[0] & 0x0F; // Opcode
    //unsigned char payloadLen = inputBuffer[1] & 0x7F;
    unsigned char payload[WS_BUFF_FRAME_SIZE] = {0};
    WEBSocket_readPayload(payload, inputBuffer);

    // Close 0x88, text 0x81
    switch(frameType) {
        case WS_CLOSING_FRAME: {
            WEBSocket_disconnect(ws, connectionSocket, "Closing frame");
            return;
        }
        case WS_TEXT_FRAME: {
            WEBSocket_send(ws, connectionSocket, 0x81, "");
            ws->handler->onMessage(connectionSocket, payload);
            break;
        }
        case WS_PONG_FRAME: {
            printf("Payload_len: %d\n", inputBuffer[1] & 0x7F);
            printf("Pong: \"%s\"\n", payload);
            break;
        }
    }

}

void WEBSocket_handleConnection(WEBSocketServer* ws, int connectionSocket) {
    char response[131] = {0};
    char inputBuffer[WS_BUFF_FRAME_SIZE] = {0};
    for(;;) {
        if(recv(connectionSocket, inputBuffer, WS_BUFF_FRAME_SIZE - 1, 0) > 0) {
            WEBSocket_printConnectionData(inputBuffer);
            WEBSocket_performFrames(ws, connectionSocket, inputBuffer);
        }
    }
}

void WEBSocket_acceptConnections(WEBSocketServer* ws) {
    for(;;) {
        struct sockaddr_in clientAddr;
        int addrLen = sizeof(clientAddr);
        int connectionSocket = accept(ws->socket, (struct sockaddr*)&clientAddr, &addrLen);
        if(connectionSocket < 0) {
            printf("accept() failed");
            closesocket(connectionSocket);
            return;
        }
        struct in_addr clientIP;
        memcpy_s(&clientIP, 4, &clientAddr.sin_addr, 4);
        printf("Accept completed (IP address of client = %s  port = %d) \n", inet_ntoa(clientIP),
               ntohs(clientAddr.sin_port));

        WEBSocket_establishConnection(ws, connectionSocket);

        WEBSocket_handleConnection(ws, connectionSocket); // TODO: Move to thread

       // Socket_close(connectionSocket);
    }
}

void WEBSocket_stop(WEBSocketServer *ws)
{
    Socket_close(ws->socket);
}
