.PHONY: test clean

LIBS = -lm

S = ../support
LLIBS = $S/support.a

CFLAGS = -Wall -pedantic -std=c11 -ggdb $(LOGGING) $(MEMTEST) -I$S $(FLAGS) 
CC = gcc

client: client.o graphics.o $(LLIBS)
	$(CC) $(CFLAGS) $^ $(LIBS) -o $@

client.o: graphics.h $S/message.h
graphics.o: graphics.h

test: client
	bash -v ./testing.sh

clean:
	rm -f core
	rm -f $(LIB) client *~ *.o