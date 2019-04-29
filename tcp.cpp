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

bool TTCPClient::Connect(const std::string& host, size_t port) {
    std::cerr << "Try to get new connection...\n";
    if(Socket != -1) {
        throw std::logic_error("Try to create a second connection. Please close the first one.");
    }
    if(Socket == -1) {
        Socket = socket(AF_INET , SOCK_STREAM , 0);
        if (Socket == -1)  {
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
            Server.sin_addr = *addr_list[i];
            break;
        }
    } else {
        Server.sin_addr.s_addr = inet_addr( host.c_str() );
    }
    Server.sin_family = AF_INET;
    Server.sin_port = htons(port);
    if (connect(Socket , (struct sockaddr *)&Server , sizeof(Server)) < 0) {
        std::cerr << "Fail to create a connection with remote Host\n";
        return false;
    }
    std::cerr << "Connection established!\n";
    return true;

}
bool TTCPClient::Send(const std::string& data) {
    if(Socket != -1) {
        if( send(Socket , data.c_str() , strlen( data.c_str() ) , 0) < 0) {
            std::cerr << "Send failed : " << data << "\n";
            return false;
        }
    } else {
        return false;
    }
}

size_t TTCPClient::ReadBytes(char* buffer, size_t bufferSize, size_t& segmentCounter) {
    if(Socket == -1) {
        throw std::logic_error("Try to read from closed socket");
    }
    std::lock_guard<std::mutex> Lock(ReadMtx);
    segmentCounter = SegmentCounter++;
    auto received = recv(Socket, buffer, bufferSize, 0);
    return received;
}


void TTCPClient::Disconnect() {
    if(Socket != -1) {
        close(Socket);
    }
    Socket = -1;
}

}