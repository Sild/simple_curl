#pragma once

#include "tcp.h"
#include <map>
#include <thread>
#include <stack>
#include <unordered_map>
#include <mutex>
#include <condition_variable>

namespace NCustom {
struct TUrl {
    std::string Protocol;
    std::string Login;
    std::string Password;
    std::string Host;
    size_t Port = 80;
    std::string Path;
    std::map<std::string, std::string> GetArgs;
    std::string Validate() const {
        if(Protocol != "http") {
            return "Invalid protocol";
        }
        if(Host.empty()) {
            return "Fail to parse hostname";
        }
        return "";
    }
};

using DataHandler = std::function<bool(char* buf, size_t size)>;

class THttpClient {
private:
    static std::string BuildRequest(const TUrl &url);
    static void HandleData(char* buffer, size_t size, const DataHandler& dataHandler);
    static constexpr size_t MaxHandleError=5;
    static constexpr size_t BufferSize = 65536;
    static const size_t MaxThreadCount;

public:
    void Get(const std::string &url, const DataHandler& dataHandler);
    THttpClient(): TotalReceived() {
        for(size_t i = 0; i < MaxThreadCount - 1; i++) {
            BufferPool.push(new char[BufferSize]);
        }
    }
    ~THttpClient() {
        while(!BufferPool.empty()) {
            delete[] BufferPool.top();
            BufferPool.pop();
        }
    }
private:
    TTCPClient TCPCli;
    std::stack<std::thread> ThreadPool;
    std::stack<char*> BufferPool;
    std::unordered_map<size_t, std::pair<char*, size_t>> DataPool;
    std::mutex DataPoolMtx;
    std::mutex BufferPoolMtx;
    std::condition_variable DataPoolCond;
    std::condition_variable BufferPoolCond;

    size_t NextSegmentId = 0;
    std::atomic_size_t TotalReceived;
};
}