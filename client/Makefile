#
# Makefile for client
#
# Description: builds cleint and the requisite graphics, validators, senders, and handlers 
# Author: Joseph Hirsh
# Date: March 2nd, 2024
# 

# all, test, and clean not file targets
.PHONY: all test clean

# define object files
OBJS = client.o graphics.o validators.o senders.o handlers.o

# set up library variables and linker flags
S = ../support
LLIBS = $(S)/support.a
LIBS = -lm -lcurses

# flag for testing on the miniserver (comment out for final build)
MINISERVER_TEST=-DMINISERVER_TEST

# flag for running unit tests (comment out for final build)
#UNIT_TEST=-DUNIT_TEST

# compilation flags (used by default)
CFLAGS = -Wall -pedantic -std=c11 -ggdb $(MINISERVER_TEST) $(UNIT_TEST) -I$(S) $(FLAGS)

# compiler is gcc
CC = gcc

# client is default make
all: client

# compile client
client: $(OBJS) $(LLIBS) 
	$(CC) $^ $(LIBS) -o $@

# recipes for object files
client.o: graphics.h validators.h senders.h handlers.h clientdata.h $(S)/message.h
graphics.o: graphics.h
validators.o: validators.h
senders.o: senders.h $(S)/message.h
handlers.o: handlers.h graphics.h validators.h

# runs testing script
test: client
	bash -v ./testing.sh

# cleans core dumps, object files, executables
clean:
	rm -f core client graphics vmalidators senders handlers *.o
