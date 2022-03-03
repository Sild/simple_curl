#include "benchmark.h"
#include "../src/CLogReader.hpp"



int main() {
    CLogReader sCLogReader;
    int sBufSize = 1024;
    char* sBuf = new char[sBufSize];
    {
        EXEC_TIME("read_file_1000_iterations");
        for (size_t i = 0; i < 1000; i++)
        {
            sCLogReader.Open("./tests/data/perf.data");
            while (sCLogReader.GetNextLine(sBuf, sBufSize));
        }
    }


}