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

.PHONY: all clean

all: gamemaptest

gamemap: gamemaptest.o gamemap.o file.o
	$(CC) $(CFLAGS) $^ -o $@

gamemap.o: gamemap.h
file.o: file.h

clean:
	rm -f gamemaptest gamemap
	rm -f core
	rm -rf *~ *.o *.gch *.dSYM
