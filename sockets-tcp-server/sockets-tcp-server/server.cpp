#include <winsock2.h>
#include <iostream>
#include <ws2tcpip.h>
#include <sys/types.h>
#include <corecrt_io.h>

#include "TcpServer.h"

int main()
{
    TcpServer* server = new TcpServer((char*)HOST_ADDRESS, (char*)SERVER_PORT);
    server->start_server();
  
    return 0;
}