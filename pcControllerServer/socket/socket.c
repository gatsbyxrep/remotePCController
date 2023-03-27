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
}
