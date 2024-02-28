.PHONY: test clean

OBJS = client.o graphics.o validators.o indicators.o senders.o handlers.o

S = ../support
LLIBS = $(S)/support.a
LIBS = -lm -lcurses
MINISERVER_TEST=-DMINISERVER_TEST

CFLAGS = -Wall -pedantic -std=c11 -ggdb $(LOGGING) $(MINISERVER_TEST) -I$(S) $(FLAGS)

CC = gcc

client: $(OBJS) $(LLIBS) 
	$(CC) $^ $(LIBS) -o $@

client.o: graphics.h validators.h indicators.h senders.h handlers.h clientdata.h $(S)/message.h
graphics.o: graphics.h
validators.o: validators.h
indicators.o: indicators.h
senders.o: senders.h $(S)/message.h
handlers.o: handlers.h graphics.h validators.h

test: client
	bash -v ./testing.sh

clean:
	rm -f core client *.o
