#include "CLogReader.hpp"

#include <stdlib.h>
#include <string.h>

CLogReader::CLogReader()
: m_Filter(new char[1024])
, m_File(NULL)
{};

CLogReader::~CLogReader() {
    delete[] m_Filter;
    Close();
};

bool CLogReader::Open(const char* aPath) {
    Close();
    m_File = fopen(aPath, "r");
    return m_File != NULL;
}

void CLogReader::Close() {
    if (m_File != NULL)
        fclose(m_File);
    m_File = NULL;
}

bool CLogReader::SetFilter(const char* aFilter) {
    return true;
}

bool CLogReader::GetNextLine(char *aBuf, const int aBufSize) {
    if (m_File == NULL) {// how notificate that file was not open
        fprintf(stderr, "try to read closed file\n"); 
        return false;
    }

    char* sLine = NULL;
    size_t sLen = 0;
    ssize_t sRead;
    sRead = getline(&sLine, &sLen, m_File);
    if (sRead == -1)
    {
        fprintf(stderr, "EOF reached\n"); 
        Close(); // how to know about EOF?
        return false;
    }
    
    if (sRead >= aBufSize) {// >= to store null-term
        fprintf(stderr, "buff size too small: line_size >= size (%ld >=%d) \n", sRead, aBufSize); 
        return false; // how to know about low buffer?
    }

    memmove(aBuf, sLine, sRead + 1); // copy null-term to aBuf too
    free(sLine); // move sLine to class member
    return true;
}