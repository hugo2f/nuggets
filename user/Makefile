.PHONY: user clean

S = ../gamemap
LLIBS = $(S)/gamemap.o
CFLAGS = -Wall -pedantic -std=c11 -ggdb -I$(S) $(FLAGS) 
CC = gcc

user: user.o $(LLIBS)
	$(CC) $^ $(LIBS) -o $@

user.o: user.c user.h
gamemap.o: ../gamemap/gamemap.h ../gamemap/gamemap.c

clean:
	rm -f core client *.o
