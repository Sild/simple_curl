#include <string>
#include <sstream>
#include <thread>
#include <queue>
#include <atomic>

#include "http.h"
#include "tools.h"
#include "base64.h"
#include "cycle_buffer.h"


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

std::string TUrl::Validate() const  {
    if(Protocol != "http") {
        return "Invalid protocol";
    }
    if(Host.empty()) {
        return "Fail to parse hostname";
    }
    return "";
}


void THttpClient::Reset() {
    TotalReceived = 0;
    ContentLength = 0;
    MetaHandled = false;
    DataSize.store(0);
    JobDone.store(false);
}

// return input buffer if headers end not-found
// return pointer to body begin if headers end found.
// also set-up Content-Length if find it.
char* THttpClient::HandleMeta(char* buffer, size_t& size) {
    auto contentLenStartPos = strstr(buffer, "Content-Length: ");
    if(contentLenStartPos == NULL && ContentLength == 0) {
        return buffer;
    } else if(contentLenStartPos != NULL) {
        auto contentLenEndPos = strstr(contentLenStartPos + 2, "\r\n");
        if(contentLenEndPos != NULL) {
            ContentLength = std::stol(std::string(contentLenStartPos + 16));
        }
    }
    auto headerEndPos = strstr(buffer, "\r\n\r\n");
    if(headerEndPos != NULL) {
        MetaHandled = true;
        size -= (headerEndPos + 4 - buffer);
        return headerEndPos + 4;
    } else {
        return buffer;
    }
}

void THttpClient::GetHeaders(TTCPClient& tcpClient, const DataHandler& dataHandler) {
    Reset();

    char* usefulData = nullptr;
    TCycleBuffer<BUFFER_SIZE * 2> cycleBuffer;
    char buffer[BUFFER_SIZE * 2];
    size_t received = 0;
    while(!MetaHandled) {
        received = tcpClient.ReadBytes(buffer, BUFFER_SIZE - 1);
        if(received == 0) {
            throw std::runtime_error("Fail to detect headers in server response.");
        }
        cycleBuffer.Append(buffer, received);
        received = cycleBuffer.GetData(buffer, BUFFER_SIZE * 2);
        buffer[received] = '\0';
        usefulData = HandleMeta(buffer, received);
    }

    if(ContentLength == 0) {
        throw std::runtime_error("Unknown Content-Length. Is it a binary file?\n");
    }
    TotalReceived = received;
    usefulData[received] = '\0';
    THttpClient::HandleData(usefulData, received, dataHandler);
}


void THttpClient::Get(const std::string &url_string, const DataHandler& dataHandler) {
    const auto url = NTools::BuildUrl(url_string);
    const auto invalidReason = url.Validate();
    if(!invalidReason.empty()) {
        throw std::runtime_error("Wrong url: " + invalidReason);
    }
    TTCPClient tcpClient(url.Host, url.Port);
    tcpClient.Send(THttpClient::BuildRequest(url));

    GetHeaders(tcpClient, dataHandler);

    std::cout << "Downloading " << ContentLength << " bytes...\n";

    char bufferFirst[BUFFER_SIZE], bufferSecond[BUFFER_SIZE];
    size_t received = 0;
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
            THttpClient::HandleData(data, size, dataHandler);

            DataPool.pop();
            DataSize.fetch_sub(1);
            lock.unlock();
        }
    });


    size_t percent = 0;
    size_t newPercent = 0;
    THttpClient::WriteProgress(TotalReceived, ContentLength, TotalReceived * 100 / ContentLength);
    char* actualBuffer = bufferFirst;
    while(TotalReceived < ContentLength) {
        while(DataSize == 2) {
            DataCondVar.notify_one();
        }
        received = tcpClient.ReadBytes(actualBuffer, THttpClient::BUFFER_SIZE);

        auto lock = std::unique_lock(DataMtx);
        DataPool.emplace(actualBuffer, received);
        lock.unlock();
        DataSize.fetch_add(1);
        DataCondVar.notify_one();

        TotalReceived += received;
        newPercent = TotalReceived * 100 / ContentLength;
        if(newPercent != percent) {
            percent = newPercent;
            THttpClient::WriteProgress(TotalReceived, ContentLength, percent);
        }
        actualBuffer = actualBuffer == bufferFirst ? bufferSecond : bufferFirst;

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

