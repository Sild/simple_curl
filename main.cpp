#include <iostream>
#include <string>
#include <fstream>
#include <time.h>

#include "tools.h"
#include "http.h"



int main(int argc, char* argv[]) {
    if(argc != 2 && argc !=3) {
        NCustom::NTools::PrintUsage(argv[0]);
        return 1;
    }

    if(strcmp(argv[1], "-h") == 0) {
        NCustom::NTools::PrintUsage(argv[0]);
        return 0;
    }

    std::string url, fileName;
    NCustom::NTools::ParseArgs(argc, argv, url, fileName);

    std::ofstream file_out;
    try {
        file_out.open(fileName, std::ios::binary | std::ios::out);

        auto print_data = [&file_out](const char* buffer, size_t size) {
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

        std::cout << "Download done. Duration: " << time(NULL) - seconds_start << " seconds.\n";
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