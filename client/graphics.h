#ifndef _GRAPHICS_H_
#define _GRAPHICS_H_

#include <stdbool.h>

bool init_curses(int nrows, int ncols);

void display_map(char* map);

void display_player_banner(char playerSymbol, int playerNuggets, int unclaimedNuggets);

void display_spectator_banner();

void indicate_invalid_key(const char key);

void indicate_nuggets_collected(const int collected);

void remove_indicator();

char get_character();

void end_curses();

#endif
