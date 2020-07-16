#pragma once
#include <memory>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <iostream>
#include <thread>
#include <queue>
#include <map>
#include <exception>
#include <sstream>
#include <functional>
using namespace std;
atomic_size_t BlocksCounter(1);  // For broken blocks emulation
template<typename T>
struct Block {
    Block(T&& data):
            Data(move(data)),
            Next(nullptr),
            CRC32(0),
            ValidationFailed(false),
            FirstCheck(true) {
    }

    void ValidateCRC32(const function<size_t()>& crc32Func, const function<void()>& loggerFunc) {
        {
            std::lock_guard<std::mutex> lg(ValidateMtx);
            if(ValidationFailed) {
                return;
            }
            if(FirstCheck) {
                CRC32 = crc32Func();
                FirstCheck = false;
                return;
            }
        }
        auto crc32 = crc32Func();
        //Broken blocks emulation. Every 37 check will be failed.
        if (BlocksCounter++ % 37 == 0) {
            crc32 = 17;
        }
        lock_guard<mutex> lg(ErrorMtx);
        if(crc32 == CRC32 || ValidationFailed) {
            return;
        }
        ValidationFailed = true;
        loggerFunc();
    }

    T Data;
    shared_ptr<Block<T>> Next;
    size_t CRC32;
    atomic_bool ValidationFailed;
    bool FirstCheck;
    std::mutex ValidateMtx;
    std::mutex ErrorMtx;
};

template<typename T>
class ResourceManager {
private:
    constexpr static size_t MAX_MEMORY_USAGE_BYTES = 2000; // 2Kb
public:
    using SharedBlockPtr = shared_ptr<Block<T>>;
    ResourceManager(size_t blockCount, size_t blockSize, size_t readThCount):
            RequiredBlocksCount(blockCount),
            MaxBlocksCountPerTime(MAX_MEMORY_USAGE_BYTES / blockSize),
            ReadThCount(readThCount),
            CurrentBlocksCount(0),
            JobDone(false),
            FirstItem(true) {
        cout << "MaxBlockPerTime=" << MaxBlocksCountPerTime << endl;
        if (MaxBlocksCountPerTime < 1) {
            throw runtime_error("Block size is too large, not enough memory.");
        }
    }

    void AddNext(T&& data) {
        unique_lock<mutex> addLock(AddMtx);
        if(CurrentBlocksCount >= MaxBlocksCountPerTime) {
            AddCond.wait(addLock, [&]() {return CurrentBlocksCount < MaxBlocksCountPerTime;});
        }
        CurrentBlocksCount++;
        addLock.unlock();
        auto block = make_shared<Block<T>>(move(data));
        {
            lock_guard<mutex> lg(BlockMapMtx);
            BlocksStats[block] = 0;
        }
        lock_guard<mutex> lg(ReadMtx);
        if (FirstItem) {
            First = block;
            Last = First;
            FirstItem = false;
        } else {
            Last->Next = block;
            Last = block;
        }
        ReadCond.notify_all();
    }

    void NotifyWriteDone() {
        lock_guard<mutex> lg(ReadMtx);
        JobDone = true;
        Last.reset();
        ReadCond.notify_all();
    }

    void NotifyReadDone() {
        ReadStats.clear();
    }

    SharedBlockPtr GetNext(const thread::id& threadId, size_t curblocknum) {
        auto readStat = ReadStats.end();
        {
            lock_guard<mutex> lg(ReadMapMtx);
            readStat = ReadStats.find(threadId);
        }
        if(readStat == ReadStats.end()) {
            unique_lock<mutex> readLock(ReadMtx);
            if(!First) {
                ReadCond.wait(readLock, [&](){return First;});
            }
            readLock.unlock();
            {
                lock_guard<mutex> lg(ReadMapMtx);
                ReadStats[threadId] = First;
            }
            UpdateBlocksStat(First, threadId);
            return ReadStats[threadId];
        }

        unique_lock<mutex> readLock(ReadMtx);
        if(!readStat->second->Next && !JobDone) {
            ReadCond.wait(readLock, [&](){return readStat->second->Next || JobDone;});
        }
        readLock.unlock();
        {
            lock_guard<mutex> lg(ReadMapMtx);
            readStat->second = readStat->second->Next;
        }
        UpdateBlocksStat(readStat->second, threadId);
        return readStat->second;
    }

private:
    bool UpdateBlocksStat(const SharedBlockPtr& key, const thread::id& threadId) {
        {
            lock_guard<mutex> lg(BlockMapMtx);
            auto blockStat = BlocksStats.find(key);
            if(blockStat == BlocksStats.end()) {
                throw runtime_error("Some error happen. Fail to find blockStat for existing block.");
            }
            blockStat->second++;
            if(blockStat->second != ReadThCount) {
                return true;
            }
            BlocksStats.erase(blockStat);
        }

        if(First == key) {
            First.reset();
        }

        lock_guard<mutex> lg(AddMtx);
        CurrentBlocksCount--;
        AddCond.notify_one();
        return true;
    }
private:
    const size_t RequiredBlocksCount;
    const size_t MaxBlocksCountPerTime;
    const size_t ReadThCount;
    atomic_size_t CurrentBlocksCount;
    atomic_bool JobDone;
    atomic_bool FirstItem;

    mutex AddMtx;
    mutex ReadMtx;
    mutex BlockMapMtx;
    mutex ReadMapMtx;
    condition_variable AddCond;
    condition_variable ReadCond;

    SharedBlockPtr First;
    SharedBlockPtr Last;
    map<thread::id, SharedBlockPtr> ReadStats;
    map<SharedBlockPtr, size_t> BlocksStats;



};