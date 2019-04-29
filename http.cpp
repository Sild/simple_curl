#include "http.h"
#include <string>
#include <map>
#include <sstream>
#include <functional>
#include <condition_variable>

namespace {

NCustom::TUrl BuildUrl(const std::string& url_string) {
    NCustom::TUrl url;

    auto protocol_end_pos = url_string.find("://");
    if(protocol_end_pos != std::string::npos) {
        url.Protocol = url_string.substr(0, protocol_end_pos);
    }
    auto auth_end_pos = url_string.find('@');
    if(auth_end_pos != std::string::npos) {
        auto login_end_pos = url_string.find(':', protocol_end_pos + 3);
        if(login_end_pos != std::string::npos) {
            if(login_end_pos > auth_end_pos) {
                url.Login = url_string.substr(protocol_end_pos + 3, auth_end_pos - protocol_end_pos - 3);
            } else {
                url.Login = url_string.substr(protocol_end_pos + 3, login_end_pos - protocol_end_pos - 3);
                url.Password = url_string.substr(login_end_pos + 1, auth_end_pos - login_end_pos - 1);
            }
        }

    }
    auto host_start_pos = auth_end_pos == std::string::npos ? protocol_end_pos + 3 : auth_end_pos + 1;
    auto port_start_pos = url_string.find(':', host_start_pos);
    if(port_start_pos != std::string::npos) {
        url.Host = url_string.substr(host_start_pos, port_start_pos - host_start_pos);
        auto port_end_pos = url_string.find('/', host_start_pos);
        if(port_end_pos != std::string::npos) {
            url.Port = std::stol(url_string.substr(port_start_pos + 1, port_end_pos - port_start_pos - 1));
        } else {
            url.Port = std::stol(url_string.substr(port_start_pos + 1, url_string.size()));
        }
    } else {
        auto host_end_pos = url_string.find('/', host_start_pos);
        if(host_end_pos != std::string::npos) {
            url.Host = url_string.substr(host_start_pos, host_end_pos - host_start_pos);
        } else {
            url.Host = url_string.substr(host_start_pos, url_string.size() - host_start_pos);
        }

    }
    auto path_start_pos = url_string.find('/', host_start_pos);
    if(path_start_pos == std::string::npos) {
        return url;
    }
    auto path_end_pos = url_string.find('/', path_start_pos);
    if(path_end_pos == std::string::npos) {
        url.Path = url_string.substr(path_start_pos + 1, url_string.size() - path_start_pos - 1);
        return url;
    }
    url.Path = url_string.substr(path_start_pos + 1, path_end_pos - path_start_pos - 1);

    auto start_params_pos = url_string.find('?', path_start_pos) + 1;
    auto separator_pos = 0;
    std::string key, value;
    for(size_t cur_pos = start_params_pos; cur_pos < url_string.size(); ++cur_pos) {
        if(url_string[cur_pos] == '=') {
            separator_pos = cur_pos;
        }
        if(url_string[cur_pos] == '&') {
            if(separator_pos == 0) {
                continue;
            }
            key = url_string.substr(start_params_pos, separator_pos - start_params_pos);
            value = url_string.substr(separator_pos + 1, cur_pos - separator_pos - 1);
            if(!key.empty() && !value.empty()) {
                url.GetArgs.insert(std::make_pair(key, value));
            }
            separator_pos = 0;
            start_params_pos = cur_pos + 1;
        }
    }
    if(separator_pos != 0) {
        key = url_string.substr(start_params_pos, separator_pos - start_params_pos);
        value = url_string.substr(separator_pos + 1, url_string.size() - separator_pos - 1);
        if(!key.empty() && !value.empty()) {
            url.GetArgs.insert(std::make_pair(key, value));
        }
    }
    return url;
}

size_t ExtractContentLength(const char* c_str) {
    auto buffer_str = std::string(c_str);
    auto content_len_start_pos = buffer_str.find("Content-Length: ");
    if(content_len_start_pos == std::string::npos) {
        throw std::runtime_error("Unknown Content-Length. Is it binary file?");
    }
    auto next_line_start = buffer_str.find("\r\n", content_len_start_pos);
    if(next_line_start == std::string::npos || content_len_start_pos + 16 >= next_line_start) {
        throw std::runtime_error("Invalid headers");
    }
    return std::stol(buffer_str.substr(content_len_start_pos + 16, next_line_start - content_len_start_pos - 16));
}

char* StripHeaders(char* buffer, size_t& size) {
    for(size_t i = 3; i < size; ++i) {
        if(buffer[i] == '\n' && buffer[i - 1] == '\r' && buffer[i -2] == '\n' && buffer[i - 3] == '\r') {
            size -= (i + 1);
            return buffer + i + 1;
        }
    }
}

// stackoverflow-drive-development https://stackoverflow.com/questions/180947/base64-decode-snippet-in-c
static const std::string BASE64_CHARS ="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
std::string Base64Encode(const std::string& source) {
    auto bytes_to_encode = source.c_str();
    auto in_len = source.size();
    std::string ret;
    int i = 0;
    int j = 0;
    unsigned char char_array_3[3];
    unsigned char char_array_4[4];

    while (in_len--) {
        char_array_3[i++] = *(bytes_to_encode++);
        if (i == 3) {
            char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
            char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
            char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
            char_array_4[3] = char_array_3[2] & 0x3f;

            for(i = 0; (i <4) ; i++)
                ret += BASE64_CHARS[char_array_4[i]];
            i = 0;
        }
    }

    if (i)
    {
        for(j = i; j < 3; j++)
            char_array_3[j] = '\0';

        char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
        char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
        char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
        char_array_4[3] = char_array_3[2] & 0x3f;

        for (j = 0; (j < i + 1); j++)
            ret += BASE64_CHARS[char_array_4[j]];

        while((i++ < 3))
            ret += '=';

    }

    return ret;
}

}

namespace NCustom {
const size_t THttpClient::MaxThreadCount = std::thread::hardware_concurrency();

void THttpClient::HandleData(char *buffer, size_t size, const NCustom::DataHandler& dataHandler) {
    size_t error_counter = 0;
    while(!dataHandler(buffer, size) && ++error_counter < MaxHandleError);
    if(error_counter >= MaxHandleError) {
        throw std::runtime_error("Too many fails in dataHandler");
    }
}

void THttpClient::Get(const std::string &url_string, const DataHandler& dataHandler) {
    const auto url = BuildUrl(url_string);
    const auto& invalidReason = url.Validate();
    if(!invalidReason.empty()) {
        throw std::runtime_error("Wrong url: " + invalidReason);
    }
    TCPCli.Connect(url.Host, url.Port);
    TCPCli.Send(BuildRequest(url));
    auto buffer = BufferPool.top();

    size_t segmentId = 0;
    auto received = TCPCli.ReadBytes(buffer, BufferSize - 1, segmentId);

    buffer[received] = '\0';
    auto contentLength = ExtractContentLength(buffer);
    std::cerr << "Downloading " << contentLength << " bytes...";

    auto noHeaderBuffer = StripHeaders(buffer, received);
    HandleData(noHeaderBuffer, received, dataHandler);
    NextSegmentId = 1;
    TotalReceived.store(received);

    for(size_t i = 0; i < MaxThreadCount - 1; i++) {
        ThreadPool.emplace([this, contentLength, i]() {
            size_t tcpCounter = 0;
            while(this->TotalReceived < contentLength) {
                std::unique_lock<std::mutex> BufferPoolLock(BufferPoolMtx);
                if(BufferPool.empty()) {
                    BufferPoolCond.wait(BufferPoolLock, [this](){return !this->BufferPool.empty();});
                }
                auto buffer = BufferPool.top();
                BufferPool.pop();
                BufferPoolLock.unlock();
                auto received = TCPCli.ReadBytes(buffer, this->BufferSize, tcpCounter);
                std::lock_guard<std::mutex> DataPoolLock(DataPoolMtx);
                DataPool.insert(std::make_pair(tcpCounter, std::make_pair(buffer, received)));
                DataPoolCond.notify_one();
            }
        });
    }

    while(this->TotalReceived < contentLength) {
        std::unique_lock<std::mutex> DataPoolLock(DataPoolMtx);
        auto nextDataPtr = DataPool.find(NextSegmentId);
        while(nextDataPtr == DataPool.end()) {
            DataPoolCond.wait(DataPoolLock);
            nextDataPtr = DataPool.find(NextSegmentId);
        }
        auto data = nextDataPtr->second.first;
        auto size = nextDataPtr->second.second;
        this->TotalReceived.fetch_add(size);
        HandleData(data, size, dataHandler);
        DataPool.erase(NextSegmentId++);
        DataPoolLock.unlock();

        std::lock_guard<std::mutex> BufferPoolLock(BufferPoolMtx);
        BufferPool.push(data);
        BufferPoolCond.notify_one();
     }

    while(!ThreadPool.empty()) {
        ThreadPool.top().join();
        ThreadPool.pop();
    }
    TCPCli.Disconnect();

}

std::string THttpClient::BuildRequest(const NCustom::TUrl& url) {
    std::stringstream stream;
    stream <<
    "GET /" << url.Path << " HTTP/1.1\r\n"
    "Host: " << url.Host << "\r\n"
    "Connection: keep-alive\r\n"
    "Upgrade-Insecure-Requests: 1\r\n";
    if(!url.Login.empty()) {
        stream << "Authorization: Basic " << Base64Encode((url.Login + ":" + url.Password).c_str()) << "\r\n";
    }
    stream << "User-Agent: Simple-Curl\r\n\r\n";
    return stream.str();
}
}

