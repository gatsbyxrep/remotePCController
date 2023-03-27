#ifndef TCPSERVER_H
#define	TCPSERVER_H

#include <stdio.h>
#include "../defines.h"
#ifdef _WIN_
    #include "windows.h"
#endif


typedef int Socket;

Socket Socket_init(int port, int type);
void Socket_close(Socket socket);

#endif
