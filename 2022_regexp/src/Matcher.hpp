#pragma once

// may be implemented inside CLogReader.cpp
// support ^$ regexp-features
// also support ? as one_any_char, * as any_any_char
class Matcher {
public:
    Matcher(const char* aFilter);
    ~Matcher(); 

    bool SetFilter(const char* aFilter);
    bool Complient(const char* aLine); // null-tern aLine expected
private:
    bool m_FilterEmpty;
    char* m_Filter;
};