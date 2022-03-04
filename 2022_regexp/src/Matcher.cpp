#include "Matcher.hpp"

#include <string.h>

Matcher::Matcher(const char* aFilter)
: m_FilterEmpty(true)
{
    SetFilter(aFilter);
}

bool Matcher::SetFilter(const char* aFilter) {
    if (aFilter == NULL) {
        m_FilterEmpty = true;
        return true;
    }
    return true;
}

bool Matcher::Complient(const char* aLine) {
    if (m_FilterEmpty)
        return true;
    return false;
}