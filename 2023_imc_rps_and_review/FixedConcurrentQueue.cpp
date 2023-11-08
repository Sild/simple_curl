// clang++ -std=c++17 FixedConcurrentQueue.cpp
#include <cmath>
#include <functional>
#include <iostream>
#include <mutex>
#include <stdexcept>
#include <thread>

namespace {
    template<uint64_t x>
    constexpr uint64_t closestExponentOf2() {
        if (x == 0 || x == std::numeric_limits<uint64_t>::max()) {
            static_assert("incorrect value");
        }
        uint64_t res = 1;
        while (res < x) res <<= 1;
        return res;
    }
}

// seems about 10-20% slower then the quick-fixed original, but it's hard to compare because of UB in originals
// https://quick-bench.com/q/89s8bkNjUCs5bl71E8ydH1gn8mo
template<typename T, uint64_t SIZE = 4096, uint64_t MAX_SPIN_ON_BUSY = 40000000>
class ConcurrentQueue {
private:
    static constexpr uint64_t mRingModMask = closestExponentOf2<SIZE>() - 1;
    static constexpr uint64_t mSize = closestExponentOf2<SIZE>();

    T mMem[mSize];
    std::mutex mMtx;
    std::atomic<uint64_t> mReadPtr = 0;
    std::atomic<uint64_t> mWritePtr = 0;

public:
    bool pop(T& dest) {
        if (mReadPtr != mWritePtr) {
            dest = std::move(mMem[mReadPtr & mRingModMask]);
            mReadPtr++; // can't increment it in previous line because then producer can overwrite the value
            return true;
        }
        return false;
    }

    uint64_t size() const {
        return mWritePtr - mReadPtr;
    }

    bool busyWaitForPush() {
        uint64_t start = 0;
        while (size() == mSize) {
            if (start++ > MAX_SPIN_ON_BUSY) {
                return false;
            }
        }
        return true;
    }

    void push(const T& pItem) {
        auto lock = std::unique_lock(mMtx);
        if (size() == mSize && !busyWaitForPush()) { // quick check without func call first
            throw std::runtime_error("Concurrent queue full cannot write to it!");
        }
        mMem[mWritePtr & mRingModMask] = pItem;
        mWritePtr++;
    }

    void push(T&& pItem) {
        auto lock = std::unique_lock(mMtx);
        if (size() == mSize && !busyWaitForPush()) { // quick check without func call first
            throw std::runtime_error("Concurrent queue full cannot write to it!");
        }
        mMem[mWritePtr & mRingModMask] = std::move(pItem);
        mWritePtr++;
    }
};

// tests for closestExponentOf2()
// static_assert(closestExponentOf2<0>() == 0); // should not compile
// static_assert(closestExponentOf2<std::numeric_limits<uint64_t>::max()>() == 0); // should not compile, but max() is not constexpr so ¯\_(ツ)_/¯
static_assert(closestExponentOf2<1>() == 1);
static_assert(closestExponentOf2<2>() == 2);
static_assert(closestExponentOf2<4>() == 4);
static_assert(closestExponentOf2<5>() == 8);
static_assert(closestExponentOf2<4096>() == 4096);

void reproduceWrongSize() {
    using Functor = std::function<void()>;

    ConcurrentQueue<Functor*, 4096> queue;

    std::atomic_bool producerDone = false;
    std::thread producer([ & ] {
        uint64_t counter = 0;
        while (counter++ < 4090) {
            queue.push(new Functor([ = ] {}));
        }
        producerDone = true;
    });

    std::thread consumer([ & ] {
        uint64_t consumeCounter = 0;
        while (!producerDone) {
            std::cout << "unsynced count=" << queue.size() << "\n";
        }
        std::cout << "synced count=" << queue.size() << std::endl;
    });
    // I'm not sure it's a honest test...
    // unsynced count=2048
    // unsynced count=2054
    // synced count=4090

    consumer.join();
    producer.join();
}

void reproduceBrokenLink() {
    // task return some number to check
    using Functor = std::function<int()>;

    ConcurrentQueue<Functor*, 1> queue;
    queue.push(new Functor([ = ] {return 1;}));

    // get task
    Functor* receivedTask = nullptr;
    queue.pop(receivedTask);

    queue.push(new Functor([ = ] {return 2;}));

    //execute received task
    auto res = (*receivedTask)();
    assert(res == 1);
}

int main(int, char**) {
    reproduceBrokenLink();
    // reproduceWrongSize();
    // return 0;
    using Functor = std::function<void()>;
    ConcurrentQueue<Functor*> queue;

    std::thread consumer([ & ] {
        Functor* task = nullptr;
        while (true) {
            if (queue.pop(task)) {
                (*task)();
                delete task;
            }
        }
    });

    std::thread producer1([ & ] {
        uint64_t counter = 0;
        while (true) {
            auto taskId = counter++;
            auto newTask = new Functor([ = ] {
                std::cout << "Running task1 " << taskId << std::endl << std::flush;
            });
            queue.push(newTask);
        }
    });
    std::thread producer2([ & ] {
        uint64_t counter = 0;
        while (true) {
            auto taskId = counter++;
            auto newTask = new Functor([ = ] {
                std::cout << "Running task2 " << taskId << std::endl << std::flush;
            });
            queue.push(newTask);
        }
    });

    consumer.join();
    producer1.join();
    producer2.join();
    return 0;
}
