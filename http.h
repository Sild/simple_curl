#pragma once

#include <functional>
#include <map>

namespace NCustom {
struct TUrl {
    std::string Protocol = "http";
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
    static constexpr size_t MAX_HANDLE_ERRORS=5;
    static constexpr size_t BUFFER_SIZE = 65535;

public:
    void Get(const std::string &url, const DataHandler& dataHandler);
private:
    std::size_t TotalReceived;

};
}