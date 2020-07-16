#pragma once
#include <getopt.h>
#include <iostream>
void printHelp() {
        std::cout <<
                  "--gen-th-count <n>    : data generator thread count\n"
                  "--block-num-count <n>\n"
                  "--block-size <n>      : block size in bytes\n"
                  "--calc-th-count <n>   : CRC32 calc thread count\n"
                  "--help                : Show help"
                  << std::endl;
        exit(1);
};

bool isArgsCorrect(size_t genThCount,
                   size_t blockCountNum,
                   size_t blockSize,
                   size_t calcThCount) {
    return genThCount && blockCountNum && blockSize && calcThCount;
}

bool parseArgs(
        int argc,
        char **& argv,
        size_t& genThCount,
        size_t& blockCountNum,
        size_t& blockSize,
        size_t& calcThCount) {
    const char* const short_opts = "g:n:s:c:h";
    const option long_opts[] = {
            {"gen-th-count", required_argument, nullptr, 'g'},
            {"block-count", required_argument, nullptr, 'n'},
            {"block-size", required_argument, nullptr, 's'},
            {"calc-th-count", required_argument, nullptr, 'c'},
            {"help", no_argument, nullptr, 'h'},
            {nullptr, no_argument, nullptr, 0}
    };
    while (true) {
        const auto opt = getopt_long(argc, argv, short_opts, long_opts, nullptr);
        if (-1 == opt)
            break;
        switch (opt) {
            case 'g':
                genThCount = std::stoul(optarg);
                break;
            case 'n':
                blockCountNum = std::stoul(optarg);
                break;
            case 's':
                blockSize = std::stoul(optarg);
                break;
            case 'c':
                calcThCount = std::stoul(optarg);
                break;
            case 'h': // -h or --help
                printHelp();
                exit(0);
            default:
                printHelp();
                exit(1);
        }
    }
    return isArgsCorrect(genThCount, blockCountNum, blockCountNum, calcThCount);
}