#include "Matcher.hpp"

#include <string.h>

namespace {
    // https://www.cs.princeton.edu/courses/archive/spr09/cos333/beautiful.html
    bool matchstar(int c, char *regexp, const char *text);

    /* matchhere: search for regexp at beginning of text */
    bool matchhere(char *regexp, const char *text)
    {
        if (regexp[0] == '\0')
            return true;
        if (regexp[1] == '*')
            return matchstar(regexp[0], regexp+2, text);
        if (regexp[0] == '$' && regexp[1] == '\0')
            return *text == '\0';
        if (*text!='\0' && (regexp[0]=='.' || regexp[0]==*text))
            return matchhere(regexp+1, text+1);
        return false;
    }

    /* matchstar: search for c*regexp at beginning of text */
    bool matchstar(int c, char *regexp, const char *text)
    {
        do {    /* a * matches zero or more instances */
            if (matchhere(regexp, text))
                return true;
        } while (*text != '\0' && (*text++ == c || c == '.'));
        return false;
    }

    bool match(char* regexp, const char* text) {
        if (regexp[0] == '^')
            return matchhere(regexp + 1, text);

        do {    /* must look even if string is empty */
            if (matchhere(regexp, text))
                return true;
        } while (*text++ != '\0');

        return false;
    }
}


Matcher::Matcher(const char* aFilter)
: m_FilterEmpty(true)
, m_Filter(NULL)
{
    SetFilter(aFilter);
}

Matcher::~Matcher() {
    delete[] m_Filter;
}

bool Matcher::SetFilter(const char* aFilter) {
    if (aFilter == NULL) {
        delete[] m_Filter;
        m_Filter = NULL;
        m_FilterEmpty = true;
        return true;
    }
    
    // may be optimized to reuse old memory, then should handle aFilter == NULL withour delete[]
    {
        delete[] m_Filter;
        m_Filter = new char[strlen(aFilter)+ 1];
        strcpy(m_Filter, aFilter);
        m_FilterEmpty = false;
    }
    return true;
}

bool Matcher::Complient(const char* aLine, bool aOptimized) {
    if (m_FilterEmpty)
        return true;

    return aOptimized? match(m_Filter, aLine) :  match(m_Filter, aLine);
}