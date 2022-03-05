#include <assert.h>
#include <stdio.h>

#include "benchmark.h"
#include "../src/Matcher.hpp"

int main() {
    Matcher sMatcher("1.*23.*");
    {
        EXEC_TIME("match_2000000_iterations_not_optimized");
        for (size_t i = 0; i < 2000000; i++)
        {
            sMatcher.Complient("123456789");
            sMatcher.Complient("wewtwmerkwegxsrt");
            sMatcher.Complient("ktk4588sdwer225wgsvxfbher4w3531341");
        }
    }
    printf("\033[0;32m");
    printf("perf_matcher tests done!\n");
    printf("\033[0m");
};