#pragma once

#include <netinet/in.h>
#include <string>
#include <netdb.h>
#include <cstring>
#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

namespace NCustom {
class TTCPClient {
public:
    TTCPClient(const std::string& host, size_t port);
    TTCPClient(const TTCPClient&) = delete;
    TTCPClient operator=(const TTCPClient&) = delete;

    void Send(const std::string& data);
    size_t ReadBytes(char* buffer, size_t bufferSize);
    ~TTCPClient();


private:
    int Socket = -1;
    struct sockaddr_in Server;
};
}