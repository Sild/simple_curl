CC=g++
CFLAGS=-c -Wall
SRCDIR=./src
TESTDIR=./tests

all: clean test

test: perf matches

clogreader.o:
	$(CC) $(CFLAGS) $(SRCDIR)/CLogReader.cpp

perf: clogreader.o perf.o
	$(CC) clogreader.o perf.o -o perf && ./perf 2>/dev/null

perf.o:
	$(CC) $(CFLAGS) $(TESTDIR)/perf.cpp

matches: clogreader.o matches.o
	$(CC) clogreader.o matches.o -o matches && ./matches

matches.o:
	$(CC) $(CFLAGS) $(TESTDIR)/matches.cpp

clean:
	rm -f CLogReader.o perf.o perf matches.o matches
