# Makefile for CS50 Nuggets game
#
# Team temporary, 2/21/2024

.PHONY: all clean

all:
	make -C support
	make -C gamemap
	make -C client
	make -C server/player
	make -C server

clean:
	rm -f *~
	make -C support clean
	make -C gamemap clean
	make -C client clean
	make -C server clean
	make -C server/player clean
