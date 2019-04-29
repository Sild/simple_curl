#include <iostream>
#include <string>
#include "http.h"
#include <fstream>
#include <time.h>

void print_usage(const std::string& app_name) {
    std::cout << "Usage: " << app_name << " {url} // download file by given url\n";
    std::cout << "Usage: " << app_name << " -h  // print help" << std::endl;
}

void parseArgs(int argc, char** argv, std::string& url, std::string& fileName) {
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
int main(int argc, char* argv[]) {
    if(argc != 2 && argc !=3) {
        print_usage(argv[0]);
        return 1;
    }

    if(strcmp(argv[1], "-h") == 0) {
        print_usage(argv[0]);
        return 0;
    }

    std::string url, fileName;
    parseArgs(argc, argv, url, fileName);

    std::ofstream file_out;
    try {
        file_out.open(fileName, std::ios::binary | std::ios::out);

        auto print_data = [&file_out](char* buffer, size_t size) {
            try {
                file_out.write(buffer, size);
                return true;
            } catch(...) {
                return false;
            }
        };

        NCustom::THttpClient httpClient;
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