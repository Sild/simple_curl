// quetions:
// 1) when I should return false on setFilter? If filter already set? Or if it's invalid (by supported semantic)?
// 2) if second, should I reset the file reading before return the new match file?
// 3) why interface not support RAII?
// 4) should it work in concurrent aplication?
// 5) interface is unobvious. why bool return everywhere?

#include <stdio.h>
#include <stdlib.h>
#include <iostream>

class CLogReader
{
public:
	CLogReader()
	: m_Filter(new char[1024])
	, m_File(NULL)
	{};

	~CLogReader() {
		delete[] m_Filter;
		Close();
	};

	bool Open(const char* aPath) {
		Close();
		m_File = fopen(aPath, "r");
		return m_File != NULL;
	}

	void Close() {
		if (m_File != NULL)
			fclose(m_File);
		m_File = NULL;
	}

	bool SetFilter(const char* aFilter) {
		return true;
	}

	bool GetNextLine(char *aBuf, const int aBufSize) {
		if (m_File == NULL) // how notificate that file was not open
			return false;

		char* sLine = NULL;
		size_t sLen = 0;
		ssize_t sRead;
		sRead = getline(&sLine, &sLen, m_File);
		if (sRead == -1)
		{
			Close(); // how to know about EOF?
			return false;
		}
		
		if (sRead >= aBufSize) // >= to store null-term
			return false; // how to know about low buffer?

		memmove(aBuf, sLine, sRead + 1); // copy null-term to aBuf too
		free(sLine); // move sLine to class member
		return true;
	}

private:
	char* m_Filter;
	FILE* m_File;

};

int main()
{
	CLogReader sCLogReader;
	sCLogReader.Open("./tests/test1.data");
	int sBufSize = 3;
	char* sBuf = new char[sBufSize];
	while (sCLogReader.GetNextLine(sBuf, sBufSize))
		printf("read line: %s", sBuf);

	delete[] sBuf;
}
