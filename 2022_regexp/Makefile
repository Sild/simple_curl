CC=g++
CFLAGS=-c -Wall

all: clean main

main: main.o clogreader.o
	$(CC) clogreader.o main.o -o main
 
clogreader.o:
	$(CC) $(CFLAGS) ./src/CLogReader.cpp

main.o:
	$(CC) $(CFLAGS) ./src/main.cpp

clean:
	rm CLogReader.o main.o main
