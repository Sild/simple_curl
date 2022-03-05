#include "CLogReader.hpp"

#include <stdlib.h>
#include <string.h>

CLogReader::CLogReader()
: m_LineBuffer(NULL)
, m_File(NULL)
, m_Matcher(NULL)
{};

CLogReader::~CLogReader() {
    free(m_LineBuffer);
    Close();
};

bool CLogReader::Open(const char* aPath) {
    Close(); // should I warn if any file already opened?
    m_File = fopen(aPath, "r");
    return m_File != NULL;
}

void CLogReader::Close() {
    if (m_File != NULL)
        fclose(m_File);
    m_File = NULL;
}

bool CLogReader::SetFilter(const char* aFilter) {
    // should I also reset m_File?
    return m_Matcher.SetFilter(aFilter);
}

bool CLogReader::GetNextLine(char *aBuf, const int aBufSize) {
    if (m_File == NULL) {// how notificate that file was not open
        fprintf(stderr, "try to read closed file\n"); 
        return false;
    }

    size_t sLen = 0;
    ssize_t sRead = 0;
    bool sFound = false;
    while (!sFound) {
        sRead = getline(&m_LineBuffer, &sLen, m_File);
        if (sRead == -1) {
            fprintf(stderr, "EOF reached\n"); 
            Close(); // how to know about EOF?
            break;
        }

        if (m_LineBuffer[sRead - 1] == '\n')
        {
            m_LineBuffer[sRead - 1] = '\0';
            --sRead;
        }
        
        if (sRead >= aBufSize) { // getline also store the \n. will replace it by \0
            fprintf(stderr, "buff size too small: line_size >= size (%ld>=%d) \n", sRead, aBufSize); 
            break; // how to know about low buffer? should I keep line to rerun GetNextLine?
        }

        sFound = LineMatch(m_LineBuffer);
    }

    if (sFound)
        memmove(aBuf, m_LineBuffer, sRead + 1); // copy null-term to aBuf too
    
    return sFound;
}

bool CLogReader::LineMatch(const char* aBuffer)
{
    return m_Matcher.Complient(aBuffer);
}