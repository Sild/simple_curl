#include <assert.h>
#include <string.h>

#include "../src/CLogReader.hpp"

namespace
{
    static CLogReader sCLogReader;
    static int BUF_SIZE = 1024;
    static char* BUF = new char[BUF_SIZE];
}

void test_no_filter()
{
    sCLogReader.Open("./tests/data/match1.data");
    assert(sCLogReader.GetNextLine(BUF, BUF_SIZE));
    assert(strcmp(BUF, "234\n") == 0);
    assert(sCLogReader.GetNextLine(BUF, BUF_SIZE));
    assert(strcmp(BUF, "567\n") == 0);
    assert(sCLogReader.GetNextLine(BUF, BUF_SIZE));
    assert(strcmp(BUF, "89") == 0);
    assert(!sCLogReader.GetNextLine(BUF, BUF_SIZE));
}

void test_low_buffer()
{
    sCLogReader.Open("./tests/data/match1.data");
    BUF_SIZE = 1;
    assert(!sCLogReader.GetNextLine(BUF, BUF_SIZE));

    sCLogReader.Open("./tests/data/match1.data");
    BUF_SIZE = 2;
    assert(!sCLogReader.GetNextLine(BUF, BUF_SIZE));

    sCLogReader.Open("./tests/data/match1.data");
    BUF_SIZE = 3;
    assert(!sCLogReader.GetNextLine(BUF, BUF_SIZE)); // still not enough because of \n

    sCLogReader.Open("./tests/data/match1.data");
    BUF_SIZE = 4;
    assert(!sCLogReader.GetNextLine(BUF, BUF_SIZE)); // still not enough because of null-term

    sCLogReader.Open("./tests/data/match1.data");
    BUF_SIZE = 5;
    assert(sCLogReader.GetNextLine(BUF, BUF_SIZE)); // success!
    assert(strcmp(BUF, "234\n") == 0);
}

int main()
{
    test_no_filter();
    test_low_buffer();
    printf("\033[0;32m");
    printf("clogreader tests passed!\n");
    printf("\033[0m");
}