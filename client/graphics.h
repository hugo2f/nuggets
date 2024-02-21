#ifndef _GRAPHICS_H_
#define _GRAPHICS_H_

void init_curses(int nrows, int ncols);

void display_banner(const char* playerName, int playerNuggets, int unclaimedNuggets, char* additional);

void display_board(char* board, int nrows, int ncols);

void exit_curses();

#endif