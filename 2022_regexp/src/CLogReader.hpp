#pragma once

#include <stdio.h>

// quetions:
// 1) when I should return false on setFilter? If filter already set? Or if it's invalid (by supported semantic)?
// 2) if second, should I reset the file reading before return the new match file?
// 3) why interface not support RAII?
// 4) should it work in concurrent aplication?
// 5) interface is unobvious. why bool return everywhere?

class CLogReader
{
public:
	CLogReader();
	~CLogReader();
	bool Open(const char* aPath);
	void Close();
	bool SetFilter(const char* aFilter);
	bool GetNextLine(char *aBuf, const int aBufSize);
private:
    bool LineMatch(const char* aBuf); // expect null-term buffer
    char* m_Filter;
    char* m_LineBuffer;
    FILE* m_File;
};