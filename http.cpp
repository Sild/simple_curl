#include "http.h"
#include <string>
#include <map>
#include <sstream>
#include <functional>
namespace {
NCustom::Url BuildUrl(const std::string& url_string) {
    NCustom::Url url;

    auto protocol_end_pos = url_string.find("://");
    if(protocol_end_pos != std::string::npos) {
        url.protocol = url_string.substr(0, protocol_end_pos);
    }
    auto auth_end_pos = url_string.find('@');
    if(auth_end_pos != std::string::npos) {
        auto login_end_pos = url_string.find(':', protocol_end_pos + 3);
        if(login_end_pos != std::string::npos) {
            if(login_end_pos > auth_end_pos) {
                url.login = url_string.substr(protocol_end_pos + 3, auth_end_pos - protocol_end_pos - 3);
            } else {
                url.login = url_string.substr(protocol_end_pos + 3, login_end_pos - protocol_end_pos - 3);
                url.password = url_string.substr(login_end_pos + 1, auth_end_pos - login_end_pos - 1);
            }
        }

    }
    auto host_start_pos = auth_end_pos == std::string::npos ? protocol_end_pos + 3 : auth_end_pos + 1;
    auto port_start_pos = url_string.find(':', host_start_pos);
    if(port_start_pos != std::string::npos) {
        url.host = url_string.substr(host_start_pos, port_start_pos - host_start_pos);
        auto port_end_pos = url_string.find('/', host_start_pos);
        if(port_end_pos != std::string::npos) {
            url.port = std::stol(url_string.substr(port_start_pos + 1, port_end_pos - port_start_pos - 1));
        } else {
            url.port = std::stol(url_string.substr(port_start_pos + 1, url_string.size()));
        }
    } else {
        auto host_end_pos = url_string.find('/', host_start_pos);
        if(host_end_pos != std::string::npos) {
            url.host = url_string.substr(host_start_pos, host_end_pos - host_start_pos);
        } else {
            url.host = url_string.substr(host_start_pos, url_string.size() - host_start_pos);
        }

    }
    auto path_start_pos = url_string.find('/', host_start_pos);
    if(path_start_pos == std::string::npos) {
        return url;
    }
    auto path_end_pos = url_string.find('/', path_start_pos);
    if(path_end_pos == std::string::npos) {
        url.path = url_string.substr(path_start_pos + 1, url_string.size() - path_start_pos - 1);
        return url;
    }
    url.path = url_string.substr(path_start_pos + 1, path_end_pos - path_start_pos - 1);

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
                url.get_args.insert(std::make_pair(key, value));
            }
            separator_pos = 0;
            start_params_pos = cur_pos + 1;
        }
    }
    if(separator_pos != 0) {
        key = url_string.substr(start_params_pos, separator_pos - start_params_pos);
        value = url_string.substr(separator_pos + 1, url_string.size() - separator_pos - 1);
        if(!key.empty() && !value.empty()) {
            url.get_args.insert(std::make_pair(key, value));
        }
    }
    return url;
}

size_t ExtractContentLength(const char* c_str) {
    auto buffer_str = std::string(c_str);
    auto content_len_start_pos = buffer_str.find("Content-Length: ");
    if(content_len_start_pos == std::string::npos) {
        throw std::runtime_error("Fail to detect content-length. Is it binary file?");
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
}

namespace NCustom {
const size_t HttpClient::MaxThreadCount = std::thread::hardware_concurrency();

void HttpClient::HandleData(char *buffer, size_t size, const NCustom::DataHandler& data_handler) {
    size_t error_counter = 0;
    while(!data_handler(buffer, size) && ++error_counter < MaxHandleError);
    if(error_counter >= MaxHandleError) {
        throw std::runtime_error("Too many fails in data_handler");
    }
}

void HttpClient::Get(const std::string &url_string, const DataHandler& data_handler) {
    const auto url = BuildUrl(url_string);
    if(!url.IsValid()) {
        throw std::runtime_error("incorrect url");
    }
    TCPCli.Connect(url.host, url.port);
    TCPCli.Send(BuildRequest(url));
    char buffer[BufferSize];
    auto received = TCPCli.ReadBytes(buffer, BufferSize - 1);

    buffer[received] = '\0';
    auto content_length = ExtractContentLength(buffer);
    std::cerr << "Downloading of " << content_length << " bytes";

    auto no_header_buffer = StripHeaders(buffer, received);
    HandleData(no_header_buffer, received, data_handler);

    size_t total_received = received;
    while(total_received < content_length && received > 0) {
        received = TCPCli.ReadBytes(buffer, BufferSize);
        total_received += received;
        HandleData(buffer, received, data_handler);
    }
    TCPCli.Disconnect();

}

std::string HttpClient::BuildRequest(const NCustom::Url& url) {
    std::stringstream stream;
    stream <<
    "GET /" << url.path << " HTTP/1.1\r\n"
    "Host: " << url.host << "\r\n"
    "Connection: keep-alive\r\n"
    "Upgrade-Insecure-Requests: 1\r\n"
    "User-Agent: Simple-Curl\r\n\r\n";
    return stream.str();
}
}

