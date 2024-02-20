#ifndef _GRAPHICS_H_
#define _GRAPHICS_H_

static void init_curses(int nrows, int ncols);

static void display_banner(const char* playerName, int playerNuggets, int unclaimedNuggets, char* additional);

static void display_board(char* board, int nrows, int ncols);

#endif