#pragma once

#include <WinSock2.h>

#define SERVER_PORT "5555"
#define HOST_ADDRESS "127.0.0.1"
#define MAX_MESSAGE_SIZE 256

class TcpClient {
public:
    TcpClient(char* address, char* port);
    ~TcpClient();
    void start_session();
    void close_session();

private:
    int s_socket;
    addrinfo* servers_info;
    addrinfo* addr_info;

    addrinfo* get_addr(char* address, char* port);
    void initialize_socket();
    void handle();
};
