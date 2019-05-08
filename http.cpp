#include <string>
#include <sstream>
#include <thread>
#include <queue>
#include <atomic>

#include "http.h"
#include "tools.h"
#include "base64.h"
#include "tcp.h"


namespace NCustom {

void THttpClient::HandleData(char *buffer, size_t size, const NCustom::DataHandler& dataHandler) {
    size_t errorCounter = 0;
    while(!dataHandler(buffer, size) && ++errorCounter < MAX_HANDLE_ERRORS);
    if(errorCounter >= MAX_HANDLE_ERRORS) {
        throw std::runtime_error("Too many fails in dataHandler");
    }
}

void THttpClient::Get(const std::string &url_string, const DataHandler& dataHandler) {
    const auto url = NTools::BuildUrl(url_string);
    const auto invalidReason = url.Validate();
    if(!invalidReason.empty()) {
        throw std::runtime_error("Wrong url: " + invalidReason);
    }
    TTCPClient tcpClient(url.Host, url.Port);
    tcpClient.Send(BuildRequest(url));

    char buffer[BUFFER_SIZE];
    auto received = tcpClient.ReadBytes(buffer, BUFFER_SIZE - 1);

    buffer[received] = '\0';
    auto contentLength = NTools::ExtractContentLength(buffer);
    std::cout << "Downloading " << contentLength << " bytes...\n";

    auto noHeaderBuffer = NTools::StripHeaders(buffer, received);
    HandleData(noHeaderBuffer, received, dataHandler);
    TotalReceived = received;

    while(this->TotalReceived < contentLength) {
        received = tcpClient.ReadBytes(buffer, THttpClient::BUFFER_SIZE);
        auto handlerThread = std::thread(HandleData, buffer, received, dataHandler);
        handlerThread.join();
        this->TotalReceived += received;
    }


}

std::string THttpClient::BuildRequest(const NCustom::TUrl& url) {
    std::stringstream stream;
    stream <<
    "GET /" << url.Path << " HTTP/1.1\r\n"
    "Host: " << url.Host << "\r\n"
    "Connection: keep-alive\r\n"
    "Upgrade-Insecure-Requests: 1\r\n";
    if(!url.Login.empty()) {
        stream << "Authorization: Basic " << NBase64::Base64Encode(url.Login + ":" + url.Password) << "\r\n";
    }
    stream << "User-Agent: Simple-Curl\r\n\r\n";
    return stream.str();
}
}

