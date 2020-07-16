#pragma once
#include <memory>
#include <thread>
#include <string>
#include <vector>
#include <fstream>
#include <libs/CRC.h>
#include "data.h"


template <typename T>
class DataChecker {
public:
    DataChecker(std::shared_ptr<ResourceManager<T>>& resourceManager, size_t threadCount, size_t blockCount, size_t blockSize):
            ThreadCount(threadCount),
            BlockCount(blockCount),
            BlockSize(blockSize),
            ErrorStream("/tmp/wrong_blocks.log", std::ofstream::out),
            Manager(resourceManager) {
    }

    void AsyncRun() {
        std::cout << "DataChecker start work" << std::endl;

        for(size_t i = 0; i < ThreadCount; i++) {
            ThreadPool.emplace_back([this]()->void{
                stringstream stream;
                std::size_t alreadyRead = 0;
                auto threadId = std::this_thread::get_id();
//                std::cout << "th=" << threadId << " running\n"; std::cout << stream.str() << endl;;
                while(alreadyRead < BlockCount) {
                    auto block = this->Manager->GetNext(threadId, alreadyRead);
                    if(!block) {
                        throw std::runtime_error("Something goes wrong, receive empty pointer in reader");
                    }

                    block->ValidateCRC32(
                            [this, &block]()->std::size_t {
                                unsigned char crcData[this->BlockSize];
                                for(std::size_t i = 0; i < this->BlockSize; i++) {
                                 crcData[i] = block->Data[i];
                                }
                                return CRC::Calculate(crcData, sizeof(crcData), CRC::CRC_32());
                            },
                            [this, &block]() {
                                std::cout << "Broken block detected with ptr=" << block << std::endl;
                                this->ErrorStream << "block with ptr=" << block << ":\n";
                                for(auto& e: block->Data) {
                                    this->ErrorStream << e;
                                }
                                this->ErrorStream << "\n\n";
                            }
                    );
                    ++alreadyRead;
                }
            });
        }
    }

    void WaitDone() {
        for(auto& th: ThreadPool) {
            th.join();
        }
        ThreadPool.clear();
        Manager->NotifyReadDone();
        std::cout << "DataChecker finish work" << std::endl;
    }

    ~DataChecker() {
        if(ThreadPool.size()) {
            WaitDone();
        }
    }
private:
    const size_t ThreadCount;
    const size_t BlockCount;
    const size_t BlockSize;
    std::ofstream ErrorStream;
    std::shared_ptr<ResourceManager<T>> Manager;
    std::vector<std::thread> ThreadPool;

};