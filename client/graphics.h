#ifndef _GRAPHICS_H_
#define _GRAPHICS_H_

bool init_curses(int nrows, int ncols);

void display_banner(char playerSymbol, int playerNuggets, int unclaimedNuggets, char* additional);

void display_map(char* map);

void append_to_banner(char* message);

void remove_from_banner();

void end_curses();

void clear_keystroke_buffer();

#endif