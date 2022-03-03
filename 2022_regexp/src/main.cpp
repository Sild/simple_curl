#include <stdio.h>
#include "CLogReader.hpp"

int main()
{
	CLogReader sCLogReader;
	sCLogReader.Open("./tests/test1.data");
	int sBufSize = 5;
	char* sBuf = new char[sBufSize];
	while (sCLogReader.GetNextLine(sBuf, sBufSize))
		printf("read line: %s", sBuf);

	delete[] sBuf;
}
