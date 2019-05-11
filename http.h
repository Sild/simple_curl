#pragma once

#include <functional>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <atomic>

namespace NCustom {
struct TUrl {
    std::string Protocol = "http";
    std::string Login;
    std::string Password;
    std::string Host;
    size_t Port = 80;
    std::string Path;
    std::string GetArgs;

};

using DataHandler = std::function<bool(const char* buf, size_t size)>;

class THttpClient {
private:
    static std::string BuildRequest(const TUrl &url);
    static void HandleData(const char* buffer, size_t size, const DataHandler& dataHandler);
    static void WriteProgress(size_t current, size_t total, size_t percent);
    static constexpr size_t MAX_HANDLE_ERRORS=5;
    static constexpr size_t BUFFER_SIZE = 65535;

public:
    void Get(const std::string &url, const DataHandler& dataHandler);
private:
    std::size_t TotalReceived;
    std::mutex DataMtx;
    std::condition_variable DataCondVar;
    std::queue<std::pair<char*, size_t>> DataPool;
    std::atomic_size_t DataSize = 0;
    std::atomic_bool JobDone = false;

};
}