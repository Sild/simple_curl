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

bool TCPClient::Connect(const std::string& host, size_t port) {
    std::cerr << "Try to get new connection...\n";
    if(sock != -1) {
        throw std::logic_error("Try to create a second connection. Please close the first one.");
    }
    if(sock == -1) {
        sock = socket(AF_INET , SOCK_STREAM , 0);
        if (sock == -1)  {
            std::cerr << "Fail to create a socket\n";
            return false;
        }
    }
    if(inet_addr(host.c_str()) == -1) {
        struct hostent *he;
        struct in_addr **addr_list;
        if ( (he = gethostbyname( host.c_str() ) ) == NULL) {
            std::cerr << "Failed to resolve hostname\n";
            return false;
        }
        addr_list = (struct in_addr **) he->h_addr_list;
        for(int i = 0; addr_list[i] != NULL; i++) {
            server.sin_addr = *addr_list[i];
            break;
        }
    } else {
        server.sin_addr.s_addr = inet_addr( host.c_str() );
    }
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0) {
        std::cerr << "Fail to create a connection with remote host\n";
        return false;
    }
    std::cerr << "Connection established!\n";
    return true;

}
bool TCPClient::Send(const std::string& data) {
    if(sock != -1) {
        if( send(sock , data.c_str() , strlen( data.c_str() ) , 0) < 0) {
            std::cerr << "Send failed : " << data << "\n";
            return false;
        }
    } else {
        return false;
    }
}

size_t TCPClient::ReadBytes(char* buf, size_t buf_size) {
    if(sock == -1) {
        throw std::logic_error("Try to read from closed socket");
    }
    auto received = recv(sock, buf, buf_size, 0);
    return received;
}


void TCPClient::Disconnect() {
    if(sock != -1) {
        close(sock);
    }
    sock = -1;
}

}