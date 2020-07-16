#include "tcp.h"
#include <netinet/in.h>
#include <string>
#include <netdb.h>
#include <cstring>
#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "stdexcept"

namespace NCustom {

TTCPClient::TTCPClient(const std::string& host, size_t port) {
    std::cout << "Try to get new connection...\n";
    Socket = socket(AF_INET , SOCK_STREAM , 0);
    if (Socket == -1)  {
        throw std::runtime_error("Fail to create a socket\n");
    }
    if(inet_addr(host.c_str()) == INADDR_NONE) {
        struct hostent *he;
        struct in_addr **addr_list;
        if ( (he = gethostbyname( host.c_str() ) ) == NULL) {
            throw std::runtime_error("Failed to resolve hostname\n");
        }
        addr_list = (struct in_addr **) he->h_addr_list;
        for(int i = 0; addr_list[i] != NULL; i++) {
            Server.sin_addr = *addr_list[i];
            break;
        }
    } else {
        Server.sin_addr.s_addr = inet_addr( host.c_str() );
    }
    Server.sin_family = AF_INET;
    Server.sin_port = htons(port);
    if (connect(Socket , (struct sockaddr *)&Server , sizeof(Server)) < 0) {
        throw std::runtime_error("Fail to create a connection with remote Host\n");
    }
    std::cout << "Connection established!\n";
}
void TTCPClient::Send(const std::string& data) {
    if(send(Socket , data.c_str() , strlen( data.c_str() ) , 0) < 0) {
        throw std::runtime_error("Fail to send data: " + data);
    }
}

size_t TTCPClient::ReadBytes(char* buffer, size_t bufferSize) {
    return recv(Socket, buffer, bufferSize, 0);
}

TTCPClient::~TTCPClient() {
    close(Socket);
}

}