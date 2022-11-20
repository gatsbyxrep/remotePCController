#ifndef TCPSERVER_H
#define	TCPSERVER_H

#include <stdio.h>
#include "windows.h"


typedef int Socket;

Socket Socket_init(int port, int type);
void Socket_close(Socket socket);

#endif
