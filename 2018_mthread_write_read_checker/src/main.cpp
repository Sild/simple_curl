// Потоки А-1..А-M генерируют блоки случайных данных. Количество потоков, блоков и размер блока задается параметрами командной строки. Количество блоков может быть очень большим.
// Потоки Б-1..Б-N вычисляют CRC32 (можно использовать готовую реализацию) для блоков сгенерированных потоками A. Количество потоков задается параметром командной строки.
// Когда все потоки Б вычислят CRC32 для какого-то блока, надо сравнить полученные значения и если они не совпадают записать блок в файл и вывести сообщение в std::cout.
// Потоки A и Б должны работать параллельно.

#include <iostream>
#include <string>
#include "argparse.h"
#include "data.h"
#include "write.h"
#include "read.h"

int main(int argc, char** argv) {
    size_t genThCount = 0;
    size_t blockCount = 0;
    size_t blockSize = 0;
    size_t calcThCount = 0;
    if(!parseArgs(argc, argv, genThCount, blockCount, blockSize, calcThCount)) {
        std::cerr << "Fail to parse all required arguments" << std::endl;
        return 1;
    }
    std::cout << "Args parsed: " << endl
        << "  genThCount=" << genThCount << ";\n"
        << "  blockCount=" << blockCount << ";\n"
        << "  blockSize=" << blockSize << ";\n"
        << "  calcThCount=" << calcThCount << ";" << std::endl;

    auto managerPtr = std::make_shared<ResourceManager<std::vector<unsigned char>>>(blockCount, blockSize, calcThCount);
    DataWriter<std::vector<unsigned char>> dataGen(managerPtr, genThCount, blockCount, blockSize);
    DataChecker<std::vector<unsigned char>> checker(managerPtr, calcThCount, blockCount, blockSize);
    dataGen.AsyncRun();
    checker.AsyncRun();
//    std::cout << "main thread continue ..." << std::endl;
    dataGen.WaitDone();
    checker.WaitDone();
    std::cout << "App finish work." << std::endl;
    return 0;
}