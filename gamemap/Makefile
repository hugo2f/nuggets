#
# Makefile for the gamemap module
# CS50 project 'Nuggets'
#
# Hugo Fang, 2/21/2024
#

LIB = ../support/support.a
# TESTS =

CFLAGS = -Wall -pedantic -std=c11 -ggdb
CC = gcc
MAKE = make
myvalgrind = valgrind --leak-check=full --show-leak-kinds=all

.PHONY: all test clean

all: gamemaptest

test: gamemaptest
	$(myvalgrind) ./gamemaptest

gamemaptest: gamemaptest.o gamemap.o file.o
	$(CC) $(CFLAGS) $^ -o $@

gamemap.o: file.h gamemap.h
file.o: file.h

clean:
	rm -f gamemaptest gamemap
	rm -f core
	rm -rf *~ *.o *.gch *.dSYM
