#pragma once
#include <time.h>
#include <cstdio>

namespace NSild {

struct TExecTime {
    TExecTime(const char* aId)
    : Id(aId)
    , ExecStart(clock())
    , Dumped(false) 
    {}

    void Dump() {
        printf("\033[0;32m");
        printf("Time taken for %s (seconds): %.10f\n", Id, static_cast<double>(clock() - ExecStart)/CLOCKS_PER_SEC);
        printf("\033[0m");
        Dumped = true;
    }

    ~TExecTime() {
        if(!Dumped) {
            Dump();
        }
    }
    const char* Id;
    const clock_t ExecStart;
    bool Dumped;
};

}
#define EXEC_TIME(x) const ::NSild::TExecTime& _exec_time_x = ::NSild::TExecTime(x); (void*)(&_exec_time_x);