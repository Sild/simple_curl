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
class TCPClient {
public:

    bool Connect(const std::string& host, size_t port);
    bool Send(const std::string& data);
    size_t ReadBytes(char* buf, size_t buf_size);
    void Disconnect();
    ~TCPClient() {
        Disconnect();
    }

private:
    int sock = -1;
    struct sockaddr_in server;

};
}