#pragma once

#include <iostream>
#include <cstring>
#include "http.h"

namespace NCustom::NTools {

static void PrintUsage(const std::string &app_path) {
    auto start_pos = app_path.rfind('/');
    std::string app_name;
    if(start_pos != std::string::npos) {
        app_name = app_path.substr(start_pos);
    } else {
        app_name = app_path;
    }
    std::cout << "Usage: ." << app_name << " {url}        // download file by given url\n";
    std::cout << "Usage: ." << app_name << " {url} {path} // download file by given url to specified path\n";
    std::cout << "Usage: ." << app_name << " -h           // print help" << std::endl;
}

static void ParseArgs(int argc, char **argv, std::string &url, std::string &fileName) {
    url = argv[1];
    fileName = "tmp";
    if(argc == 3) {
        fileName = argv[2];
    } else {
        auto fileNameStartPos = url.rfind('/');
        if(fileNameStartPos != std::string::npos && fileNameStartPos < url.size() - 1) {
            fileName = url.substr(fileNameStartPos + 1, url.size()  - fileNameStartPos);
        }
    }
}

static NCustom::TUrl BuildUrl(const std::string& url_string) {
    NCustom::TUrl url;
    auto protocol_end_pos = url_string.find("://");
    bool protocolSpecified = false;
    if(protocol_end_pos != std::string::npos) {
        protocolSpecified = true;
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
    auto host_start_pos = auth_end_pos == std::string::npos ? (protocolSpecified ? protocol_end_pos + 3 : 0): auth_end_pos + 1;
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
            host_end_pos = url_string.find('?', host_start_pos);
            if(host_end_pos != std::string::npos) {
                url.Host = url_string.substr(host_start_pos, host_end_pos - host_start_pos);
            } else {
                url.Host = url_string.substr(host_start_pos, url_string.size() - host_start_pos);
            }
        }

    }
    auto path_start_pos = url_string.find('/', host_start_pos);
    if(path_start_pos != std::string::npos) {
        auto path_end_pos = url_string.find('/', path_start_pos);
        if(path_end_pos == std::string::npos) {
            url.Path = url_string.substr(path_start_pos + 1, url_string.size() - path_start_pos - 1);
            return url;
        }
        url.Path = url_string.substr(path_start_pos + 1, path_end_pos - path_start_pos - 1);
    }


    auto start_params_pos = url_string.find('?');
    if(start_params_pos == std::string::npos) {
        return url;
    }
    auto end_param_pos = url_string.find('#', start_params_pos);
    if(end_param_pos == std::string::npos) {
        url.GetArgs = url_string.substr(start_params_pos + 1, url_string.size() - start_params_pos - 1);
    } else {
        url.GetArgs = url_string.substr(start_params_pos + 1, end_param_pos - start_params_pos - 1);
    }
    std::cout << url.GetArgs << std::endl;
    return url;
}

static size_t ExtractContentLength(const char* c_str) {
    auto buffer_str = std::string(c_str);
    auto content_len_start_pos = buffer_str.find("Content-Length: ");
    if(content_len_start_pos == std::string::npos) {
        throw std::runtime_error("Unknown Content-Length. Is it a binary file?\n");
    }
    auto next_line_start = buffer_str.find("\r\n", content_len_start_pos);
    if(next_line_start == std::string::npos || content_len_start_pos + 16 >= next_line_start) {
        throw std::runtime_error("Invalid headers");
    }
    return std::stol(buffer_str.substr(content_len_start_pos + 16, next_line_start - content_len_start_pos - 16));
}

static char* StripHeaders(char* buffer, size_t& size) {
    auto headerEndPos = strstr(buffer, "\r\n\r\n");
    if(headerEndPos != NULL) {
        size -= (headerEndPos + 4 - buffer);
        return headerEndPos + 4;
    }
    return buffer;
}

std::string ValidateUrl(const TUrl& url)  {
    if(url.Protocol != "http") {
        return "Invalid protocol";
    }
    if(url.Host.empty()) {
        return "Fail to parse hostname";
    }
    return "";
}

}
