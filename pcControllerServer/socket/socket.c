#include "socket.h"


Socket Socket_init(int port, int type) {
    WORD wVersionReqested = MAKEWORD(1, 1);
    WSADATA wsaData;

    WSAStartup(wVersionReqested, &wsaData);

    int sock = socket(AF_INET, type, 0);

    if(sock < 0) {
        printf("%s\n", "Socket() failed");
        exit(-1);
    }
    struct sockaddr_in serverAddr = {
        .sin_family = AF_INET,
        .sin_port = htons(port),
        .sin_addr.s_addr = htonl(INADDR_ANY)
    };

    if(bind(sock, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        printf("Bind() failed");
        exit(-1);
    }

    listen(sock, 1);
    return sock;
}

void Socket_close(Socket socket) {
    if(closesocket(socket) < 0) {
        printf("closesocket() failed");
        exit(-1);
    }
    WSACleanup();
}

void TCPServer_handleConnections(TCPServer* server, void (*handler)(int, const char *)) {
    printf("%s\n", "Start listening");
    for(;;) {
        struct sockaddr_in clientAddr;
        int addrLen = sizeof(clientAddr);    
        int connectionSocket = accept(server->socket, (struct sockaddr*)&clientAddr, &addrLen);
        if(connectionSocket < 0) {
            printf("accept() failed");
            closesocket(connectionSocket);
            return;
        }
        struct in_addr clientIP;
        memcpy_s(&clientIP, 4, &clientAddr.sin_addr, 4);
        printf("Accept completed (IP address of client = %s  port = %d) \n", inet_ntoa(clientIP),
               ntohs(clientAddr.sin_port));

        char inputBuff[1024];
        if(recv(connectionSocket, inputBuff, sizeof(inputBuff), 0) < 0) {
            printf("recv() failed");
            closesocket(connectionSocket);
            return;
        }
        handler(connectionSocket, inputBuff);
        closesocket(connectionSocket);
    }
}
