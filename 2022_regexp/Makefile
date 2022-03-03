CC=g++
CFLAGS=-c -Wall
SRCDIR=./src
TESTDIR=./tests

all: clean main

main: main.o clogreader.o
	$(CC) clogreader.o main.o -o main
 
clogreader.o:
	$(CC) $(CFLAGS) $(SRCDIR)/CLogReader.cpp

main.o:
	$(CC) $(CFLAGS) $(SRCDIR)/main.cpp

perf: clogreader.o perf.o
	$(CC) clogreader.o perf.o -o perf && ./perf 2>/dev/null

perf.o:
	$(CC) $(CFLAGS) $(TESTDIR)/perf.cpp

clean:
	rm -f CLogReader.o main.o main perf.o perf
