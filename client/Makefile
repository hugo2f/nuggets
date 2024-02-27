.PHONY: test clean

S = ../support
LLIBS = $(S)/support.a
LIBS = -lm -lcurses
MINICLIENT_TEST=-DMINICLIENT_TEST

CFLAGS = -Wall -pedantic -std=c11 -ggdb $(LOGGING) $(MINICLIENT_TEST) -I$(S) $(FLAGS)

CC = gcc

client: client.o graphics.o $(LLIBS)
	$(CC) $^ $(LIBS) -o $@

client.o: graphics.h $(S)/message.h
graphics.o: graphics.h

test: client
	bash -v ./testing.sh

clean:
	rm -f core client *.o
