CC=g++
CFLAGS=-c -Wall
SRCDIR=./src
TESTDIR=./tests

all: clean test

test: clean test_matcher test_perf_matcher test_perf test_clogreader 

clogreader.o: matcher.o
	$(CC) $(CFLAGS) $(SRCDIR)/CLogReader.cpp -o clogreader.o

matcher.o:
	$(CC) $(CFLAGS) $(SRCDIR)/Matcher.cpp -o matcher.o

test_perf: clean clogreader.o test_perf.o matcher.o
	$(CC) clogreader.o test_perf.o matcher.o -o test_perf && ./test_perf 2>/dev/null

test_perf.o:
	$(CC) $(CFLAGS) $(TESTDIR)/perf.cpp -o test_perf.o

test_clogreader: clean clogreader.o matcher.o test_clogreader.o 
	$(CC) clogreader.o test_clogreader.o matcher.o -o test_clogreader && ./test_clogreader 2>/dev/null

test_clogreader.o:
	$(CC) $(CFLAGS) $(TESTDIR)/clogreader.cpp -o test_clogreader.o

test_matcher: clean test_matcher.o matcher.o
	$(CC) matcher.o test_matcher.o -o test_matcher && ./test_matcher 2>/dev/null

test_matcher.o:
	$(CC) $(CFLAGS) -Wno-non-literal-null-conversion $(TESTDIR)/matcher.cpp -o test_matcher.o

test_perf_matcher: clean test_perf_matcher.o matcher.o
	$(CC) matcher.o test_perf_matcher.o -o test_perf_matcher && ./test_perf_matcher 2>/dev/null

test_perf_matcher.o:
	$(CC) $(CFLAGS) $(TESTDIR)/perf_matcher.cpp -o test_perf_matcher.o

clean:
	rm -f clogreader.o matcher.o test_perf.o test_perf test_perf_matcher test_perf_matcher.o test_clogreader.o test_clogreader test_matcher.o test_matcher
