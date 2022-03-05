#include <stdio.h>
#include "CLogReader.hpp"

int main(int argc, char *argv[]) {
    if (argc != 3)
    {
        fprintf(stderr, "Usage: ./app {file_path} {regexp}\n");
        return 1;
    }

    const char* sFile = argv[1];
    const char* aFilter = argv[2];

    const int sBuffSize = 4096;
    char* sBuf = new char[sBuffSize];

    CLogReader sCLogReader;
    if (!sCLogReader.SetFilter(aFilter)) {
        fprintf(stderr, "Fail to SetFilter for '%s'", aFilter);
        return 2;
    }

    if (!sCLogReader.Open(sFile)) {
        fprintf(stderr, "Fail to Open file '%s'", sFile);
        return 3;
    }

    while (sCLogReader.GetNextLine(sBuf, sBuffSize)) {
        printf("\033[0;32m");
        printf("%s\n", sBuf);
        printf("\033[0m");
    }
    return 0;
}