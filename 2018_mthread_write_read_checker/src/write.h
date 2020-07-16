#pragma once
#include <memory>
#include <thread>
#include <string>
#include <random>
#include <vector>
#include "data.h"
#include <climits>
#include <algorithm>
#include <functional>

template <typename T>
class DataWriter {
private:
public:
    DataWriter(std::shared_ptr<ResourceManager<T>>& resourceManager, size_t threadCount, size_t blockCount, size_t blockSize):
        ThreadCount(threadCount),
        BlockCount(blockCount),
        BlockSize(blockSize),
        GeneratedBlockCount(0),
        Manager(resourceManager) {
    }

    void AsyncRun() {
        std::cout << "DataWriter start work" << std::endl;
        for(size_t i = 0; i < ThreadCount; i++) {
            ThreadPool.emplace_back([this](){
                while(this->GeneratedBlockCount++ < BlockCount) {
                    this->Manager->AddNext(GenData());
                }
            });
        }
    }
    void WaitDone() {
        for(auto& th: ThreadPool) {
            th.join();
        }
        Manager->NotifyWriteDone();
        ThreadPool.clear();
        std::cout << "DataWriter finish work" << std::endl;
    }

    ~DataWriter() {
        if(ThreadPool.size()) {
            WaitDone();
        }
    }
private:
    T GenData() {
        using random_bytes_engine = std::independent_bits_engine<std::default_random_engine, CHAR_BIT, unsigned char>;
        random_bytes_engine rbe;
        std::vector<unsigned char> data(BlockSize);
        std::generate(begin(data), end(data), std::ref(rbe));
        return data;
    }

private:
    const size_t ThreadCount;
    const size_t BlockCount;
    const size_t BlockSize;
    std::atomic_size_t GeneratedBlockCount;
    std::shared_ptr<ResourceManager<T>> Manager;
    std::vector<std::thread> ThreadPool;
};