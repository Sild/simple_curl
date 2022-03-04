#pragma once

// may be implemented inside CLogReader.cpp
class Matcher {
public:
    Matcher(const char* aFilter);
    // ~Matcher() {}; 

    bool SetFilter(const char* aFilter);
    bool Complient(const char* aLine); // null-tern aLine expected
private:
    bool m_FilterEmpty;
};