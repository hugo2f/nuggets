#
# Makefile for the gamemap module
# CS50 project 'Nuggets'
#
# Hugo Fang, 2/21/2024
#

LIB = gamemap.a
# TESTS =

CFLAGS = -Wall -pedantic -std=c11 -ggdb
CC = gcc
MAKE = make
myvalgrind = valgrind --leak-check=full --show-leak-kinds=all

.PHONY: all test clean

all: $(LIB) gamemaptest

# library and executables
$(LIB): gamemap.o file.o
	ar cr $(LIB) $^

gamemaptest: gamemaptest.o $(LIB)
	$(CC) $(CFLAGS) $^ -o $@

# objects
gamemap.o: gamemap.h file.h
file.o: file.h

test: gamemaptest
	$(myvalgrind) ./gamemaptest

clean:
	rm -f gamemaptest gamemap.a
	rm -f core
	rm -rf *~ *.o *.gch *.dSYM
