#pragma once

#include <netinet/in.h>
#include <string>
#include <netdb.h>
#include <cstring>
#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <atomic>
#include <mutex>

namespace NCustom {
class TTCPClient {
public:
    TTCPClient() = default;
    TTCPClient(const TTCPClient&) = delete;
    TTCPClient operator=(const TTCPClient&) = delete;

    bool Connect(const std::string& host, size_t port);
    bool Send(const std::string& data);
    size_t ReadBytes(char* buffer, size_t bufferSize, size_t& segmentCounter);
    void Disconnect();
    ~TTCPClient() {
        Disconnect();
    }


private:
    int Socket = -1;
    struct sockaddr_in Server;
    size_t SegmentCounter = 0;
    std::mutex ReadMtx;
};
}