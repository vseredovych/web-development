#include <winsock2.h>
#include <iostream>
#include <ws2tcpip.h>
#include <sys/types.h>
#include <corecrt_io.h>

#pragma warning(disable  : 4996)
#pragma comment(lib, "Ws2_32.lib")

#include "TcpServer.h"

TcpServer::TcpServer(char* address, char* port) {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(1, 1), &wsaData) != 0) {
        std::cout << stderr << ";WSAStartup failed.\n" << std::endl;
    }

    this->addr_info = get_addr(address, port);
}


TcpServer::~TcpServer() {
    freeaddrinfo(servers_info);
}


void TcpServer::start_server() {
    initialize_socket();
    handle();
    close_server();
}


void TcpServer::close_server() {
    closesocket(s_socket);
    WSACleanup();
    std::cout << "Server: Stopping." << std::endl;
}


addrinfo* TcpServer::get_addr(char* address, char* port)
{
    struct addrinfo* servers_iter = nullptr;
    struct addrinfo* server = nullptr;

    struct addrinfo hints;
    memset(&hints, 0, sizeof hints);

    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    int status;

    if ((status = getaddrinfo(address, port, &hints, &servers_info)) < 0) {
        throw new std::exception("Failed to get address info. ");
    }

    for (servers_iter = servers_info; servers_iter != NULL; servers_iter = servers_iter->ai_next) {
        if (servers_iter->ai_family == AF_INET) {
            struct sockaddr_in* ipv4 = (struct sockaddr_in*)servers_iter->ai_addr;
            server = servers_iter;
        }
    }

    delete servers_iter;
    return server;
}


void TcpServer::initialize_socket() {
    char ipstr[INET_ADDRSTRLEN];
    struct sockaddr_in* ipv4;

    this->s_socket = socket(addr_info->ai_family, addr_info->ai_socktype, addr_info->ai_protocol);
    if (s_socket < 0) {
        throw std::exception("Failed to initialize socket.");
    }

    if (bind(s_socket, addr_info->ai_addr, addr_info->ai_addrlen) < 0) {
        throw std::exception("Failed to bind socket to port.");
    }
    else {
        ipv4 = (sockaddr_in*)addr_info->ai_addr;
        inet_ntop(addr_info->ai_family, &(ipv4->sin_addr), ipstr, sizeof ipstr);
        std::cout << "Server: Address - " << ipstr << ";" << std::endl;
    }

    if (listen(s_socket, BACKLOG) < 0) {
        throw std::exception("Failed to listen.");
    }
    std::cout << "Server: Listening for connections."  << std::endl;
}


void TcpServer::handle() {
    struct sockaddr_storage their_addr;
    char their_ip[INET6_ADDRSTRLEN];
    socklen_t their_addr_size = sizeof their_addr;

    int connection_socket;

    connection_socket = accept(s_socket, (struct sockaddr*)&their_addr, &their_addr_size);
    if (connection_socket < 0) {
        throw new std::exception("Failed to accept connection.");
    }

    inet_ntop(their_addr.ss_family, &((struct sockaddr_in*)&their_addr)->sin_addr, their_ip, sizeof their_ip);

    bool stop_chat = false;

    std::cout << "Server: Got a connection from: " << their_ip << std::endl;
    std::cout << "Server: Starting a dialog." << std::endl;
    std::cout << "=============================================================" << std::endl;
    while (true) {
        std::string msg;
        int bytes_so_far = 0;
        int num_of_bytes = 0;
        bool is_msg_received = false;
        char buff[MAX_MESSAGE_SIZE];
        memset(&buff, 0, sizeof buff);

        while (bytes_so_far <= MAX_MESSAGE_SIZE && !is_msg_received) {
            if ((num_of_bytes = recv(connection_socket, buff, MAX_MESSAGE_SIZE, 0)) < 0) {
                std::cout << "Failed to recieve message." << std::endl;
                break;
            }

            for (int i = 0; i < num_of_bytes; i++) {
                if (buff[i] == '\0') {
                    is_msg_received = true;
                    std::cout << "Client: " << msg << std::endl;
                    break;
                }
                if (buff[i] == 'q' && i + 1 <= MAX_MESSAGE_SIZE && buff[i+1] == '\0') {
                    is_msg_received = true;
                    stop_chat = true;
                    break;
                }
                msg += buff[i];
                bytes_so_far += 1;
            }
        }

        if (stop_chat == true) {
            break;
        }

        memset(&buff, 0, sizeof buff);
        std::cout << "Server: > ";
        std::cin.getline(buff, MAX_MESSAGE_SIZE);

        if (send(connection_socket, buff, MAX_MESSAGE_SIZE, 0) < 0) {
            std::cout << "Message failed to be sent." << std::endl;
        }
    }
    std::cout << "Server: Ending the dialog." << std::endl;
    std::cout << "=============================================================" << std::endl;
}