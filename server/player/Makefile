# Makefile for the player module
# CS50 project 'Nuggets'
#
# Jaysen Quan, 2/27/2024
#

CFLAGS = -Wall -pedantic -std=c11 -ggdb -I../../support -I../../gamemap
CC = gcc
OBJS = player.o
LIB = player.a
LLIBS = ../../support/support.a ../../gamemap/gamemap.a

.PHONY: all clean lib

all: $(LIB)

# library of player object files
$(LIB): $(OBJS)
	ar cr $(LIB) $^

# object files depend on include files
player.o: player.h ../../gamemap/gamemap.h ../../support/message.h
	$(CC) $(CFLAGS) -c player.c -o player.o

# clean by removing object files, the library, and any temporary files
clean:
	rm -f player.a
	rm -f *~ *.o *.gch *.dSYM