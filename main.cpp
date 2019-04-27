#include <iostream>
#include <string>
#include "http.h"
#include <fstream>
#include <time.h>

//static std::string url = "http://releases.ubuntu.com/19.04/ubuntu-19.04-desktop-amd64.iso";
static std::string url = "http://releases.ubuntu.com/18.04.2/ubuntu-18.04.2-live-server-amd64.iso";
//static std::string url = "http://mi01ht.market.yandex.net:3131/marketindexer/show.py";


int main() {
    std::string fname = "tmp";
    auto fname_start_pos = url.rfind('/');
    if(fname_start_pos != std::string::npos) {
        fname = url.substr(fname_start_pos + 1, url.size()  - fname_start_pos);
    }
    std::ofstream file_out;
    try {
        file_out.open(fname, std::ios::binary | std::ios::out);

        auto print_data = [&file_out](char* buffer, size_t size) {
            try {
                file_out.write(buffer, size);
                return true;
            } catch(...) {
                return false;
            }
        };

        NCustom::HttpClient httpClient;
        time_t seconds_start = time(nullptr);
        httpClient.Get(url, print_data);

        std::cerr << "Download done. Duration: " << time(NULL) - seconds_start << " seconds.\n";
    } catch(std::exception& e) {
        std::cerr << e.what() << "\n";
        file_out.close();
        return 1;
    } catch(...) {
        std::cerr << "Unknown error.\n";
        file_out.close();
        return 1;
    }
    return 0;
}