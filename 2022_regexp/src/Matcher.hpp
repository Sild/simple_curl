#pragma once

// may be implemented inside CLogReader.cpp
// support ^$.* regexp-features
class Matcher {
public:
    Matcher(const char* aFilter);
    ~Matcher(); 

    bool SetFilter(const char* aFilter);
    bool Complient(const char* aLine, bool aOptimized=true); // null-tern aLine expected
private:
    bool m_FilterEmpty;
    char* m_Filter;
};