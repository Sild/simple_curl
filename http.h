#pragma once

#include "tcp.h"
#include <map>
#include <thread>

namespace NCustom {
struct Url {
    std::string protocol;
    std::string login;
    std::string password;
    std::string host;
    size_t port = 80;
    std::string path;
    std::map<std::string, std::string> get_args;
    bool IsValid() const {
        return protocol == "http" && !host.empty();
    }
};

using DataHandler = std::function<bool(char* buf, size_t size)>;

class HttpClient {
public:
    void Get(const std::string &url, const DataHandler& data_handler);

private:
    static std::string BuildRequest(const Url &url);
    static void HandleData(char* buffer, size_t size, const DataHandler& data_handler);
    TCPClient TCPCli;
    size_t BufferSize=65536;
    static constexpr size_t MaxHandleError=5;
    static const size_t MaxThreadCount;

};
}