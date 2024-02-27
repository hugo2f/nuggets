#
# Makefile for the server module
# CS50 project 'Nuggets'
#
# Jaysen Quan, 2/21/2024
#

CFLAGS = -Wall -pedantic -std=c11 -ggdb -I../support -I../gamemap
CC = gcc
OBJS = server.o 

LIBS = 
LLIBS = ../support/support.a ../gamemap/gamemap.a
# TESTS =

MAKE = make
myvalgrind = valgrind --leak-check=full --show-leak-kinds=all

.PHONY: all test clean

all: server

server: $(OBJS)
	$(CC) $(CFLAGS) $^ $(LLIBS) $(LIBS) -o $@


clean:
	rm -f server
	rm -f core
	rm -rf *~ *.o *.gch *.dSYM
