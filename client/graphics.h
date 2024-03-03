#ifndef _GRAPHICS_H_
#define _GRAPHICS_H_

/*
 * Initializes curses.
 * 
 * First, it sets up the screen size by calling a helper function setupScreenSize. This ensures that the
 * terminal window is the correct size for the game. It also calls various ncurses functions to set the
 * colors, keystroke settings, and other display settings.
 * 
 * Requires nrows and ncols parameters, the number of rows of the board and the number of cols of the board 
 * respectively.
 */
void init_curses(int nrows, int ncols);

/*
 * Displays map on curses display.
 *
 * Requires map parameter. This map is just a string with no new line characters. This function knows the
 * map dimensions, so it knows when to return to the next line.
 */
void display_map(char* map);

/*
 * Displays banner for client in player mode.
 *
 * Requires playerSymbol, playerNuggets, and unclaimedNuggets parameters, which stand for the clients 
 * symbol, the amount of nuggets this cleint has, and the amount of nuggets remaining. 
 * 
 * This function displays a banner at the top of the ncurses display of the form "Player [playerSymbol] 
 * has [playerNuggets] nuggets ([unclaimedNuggets] nuggets unclaimed)." 
 */
void display_player_banner(char playerSymbol, int playerNuggets, int unclaimedNuggets);

/*
 * Displays banner for client in spectator mode.
 *
 * Requires  unclaimedNuggets parameter, which stand for the amount of nuggets remaining. 
 * 
 * This function displays a banner at the top of the ncurses display of the form "Spectating: 
 * [unclaimedNuggets] nuggets unclaimed." 
 */
void display_spectator_banner(const int unclaimedNuggets);

void indicate_invalid_key(const char key);

void indicate_nuggets_collected_player(const int collected);

void indicate_nuggets_collected_spectator(const char symbol, const int collected);

void remove_indicator();

/*
 * Gets keypress; just a wrapper for getch so all curses code remains in one module.
 *
 * Returns char
 */
char get_character();

/*
 * Ends curses; just a wrapper for endwin so all curses code remains in one module.
 *
 * Returns char
 */
void end_curses();

#endif
