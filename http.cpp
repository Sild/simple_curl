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

void THttpClient::HandleData(const char *buffer, size_t size, const NCustom::DataHandler& dataHandler) {
    size_t errorCounter = 0;
    while(!dataHandler(buffer, size) && ++errorCounter < MAX_HANDLE_ERRORS);
    if(errorCounter >= MAX_HANDLE_ERRORS) {
        throw std::runtime_error("Too many fails in dataHandler");
    }
}

void THttpClient::WriteProgress(size_t current, size_t total, size_t percent) {
    std::cout <<  "Progress: " << percent << "%: " << current << " / " << total << "\n";

}

void THttpClient::Get(const std::string &url_string, const DataHandler& dataHandler) {
    const auto url = NTools::BuildUrl(url_string);
    const auto invalidReason = NTools::ValidateUrl(url);
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



    auto writeThread = std::thread([this, dataHandler]() mutable {
        char* data = nullptr;
        size_t size = 0;
        while(!JobDone) {
            auto lock = std::unique_lock(DataMtx);
            if(DataPool.empty()) {
                DataCondVar.wait(lock, [this](){return !this->DataPool.empty() || this->JobDone;});
            }
            if(DataPool.empty()) {
                continue;
            }

            std::tie(data, size) = DataPool.front();
            this->HandleData(data, size, dataHandler);

            DataPool.pop();
            DataSize.fetch_sub(1);
            lock.unlock();
        }
    });

    char altBuffer[BUFFER_SIZE];
    size_t percent = 0;
    size_t newPercent = 0;
    WriteProgress(0, 0, 0);
    char* actualBuffer = buffer;
    while(this->TotalReceived < contentLength) {
        while(DataSize == 2) {
            DataCondVar.notify_one();
        }
        received = tcpClient.ReadBytes(actualBuffer, THttpClient::BUFFER_SIZE);

        auto lock = std::unique_lock(DataMtx);
        this->DataPool.emplace(actualBuffer, received);
        lock.unlock();
        DataSize.fetch_add(1);
        DataCondVar.notify_one();

        this->TotalReceived += received;
        newPercent = this->TotalReceived * 100 / contentLength;
        if(newPercent != percent) {
            percent = newPercent;
            WriteProgress(this->TotalReceived, contentLength, percent);
        }
        actualBuffer = actualBuffer == buffer ? altBuffer : buffer;

    }
    JobDone.store(true);
    DataCondVar.notify_one();
    writeThread.join();
}

std::string THttpClient::BuildRequest(const NCustom::TUrl& url) {
    std::stringstream stream;
    stream <<
    "GET /" << url.Path << "?" << url.GetArgs << " HTTP/1.1\r\n"
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

