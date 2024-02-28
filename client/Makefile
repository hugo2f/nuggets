.PHONY: test clean

S = ../support
LLIBS = $(S)/support.a
LIBS = -lm -lcurses
MINISERVER_TEST=-DMINISERVER_TEST

CFLAGS = -Wall -pedantic -std=c11 -ggdb $(LOGGING) $(MINISERVER_TEST) -I$(S) $(FLAGS)

CC = gcc

client: client.o graphics.o $(LLIBS)
	$(CC) $^ $(LIBS) -o $@

client.o: graphics.h $(S)/message.h
graphics.o: graphics.h

test: client
	bash -v ./testing.sh

clean:
	rm -f core client *.o
