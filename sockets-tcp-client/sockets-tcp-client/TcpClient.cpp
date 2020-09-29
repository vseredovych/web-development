#include <winsock2.h>
#include <iostream>
#include <ws2tcpip.h>
#include <sys/types.h>
#include <string>
#include "TcpClient.h"

#pragma warning(disable  : 4996)
#pragma comment(lib, "Ws2_32.lib")

#pragma once


TcpClient::TcpClient(char* address, char* port) {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(1, 1), &wsaData) != 0) {
        std::cout << stderr << ";WSAStartup failed.\n" << std::endl;
    }

    this->addr_info = get_addr(address, port);
}


TcpClient::~TcpClient() {
    freeaddrinfo(servers_info);
}


void TcpClient::start_session() {
    initialize_socket();
    handle();
    close_session();
}


void TcpClient::close_session() {
    closesocket(s_socket);
    WSACleanup();
    std::cout << "The session was finished." << std::endl;
}


addrinfo* TcpClient::get_addr(char* address, char* port)
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


void TcpClient::initialize_socket() {
    this->s_socket = socket(addr_info->ai_family, addr_info->ai_socktype, addr_info->ai_protocol);
    if (s_socket < 0) {
        throw std::exception("Failed to initialize socket.");
    }

    if (connect(s_socket, addr_info->ai_addr, addr_info->ai_addrlen) < 0) {
        throw std::exception("Failed connect to the server.");
    }
}


void TcpClient::handle() {
    std::cout << "Client: Starting a dialog." << std::endl;
    std::cout << "=============================================================" << std::endl;
    bool stop_chat = false;

    while (true) {
        char buff[MAX_MESSAGE_SIZE];
        memset(&buff, 0, sizeof buff);
        std::cout << "Client: > ";
        std::cin.getline(buff, MAX_MESSAGE_SIZE);

        if (send(s_socket, buff, MAX_MESSAGE_SIZE, 0) < 0) {
            std::cout << "Message failed to be sent." << std::endl;
        }

        memset(&buff, 0, sizeof buff);
        std::string msg;
        int bytes_so_far = 0;
        int num_of_bytes = 0;
        bool is_msg_received = false;

        while (bytes_so_far <= MAX_MESSAGE_SIZE && !is_msg_received) {
            if ((num_of_bytes = recv(s_socket, buff, MAX_MESSAGE_SIZE, 0)) < 0) {
                std::cout << "Failed to recieve message." << std::endl;
                break;
            }

            for (int i = 0; i < num_of_bytes; i++) {
                if (buff[i] == '\0') {
                    is_msg_received = true;
                    std::cout << "Client: " << msg << std::endl;
                    break;
                }
                if (buff[i] == 'q' && i + 1 <= MAX_MESSAGE_SIZE && buff[i + 1] == '\0') {
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
    }
    std::cout << "Client: Ending the dialog." << std::endl;
    std::cout << "=============================================================" << std::endl;
}
