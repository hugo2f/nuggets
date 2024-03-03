/*
 * validators.h
 *
 * Description: contains interface for the graphics module.
 * 
 * Author: Joseph Hirsh
 * Date: March 2st, 2024
 *
 */

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
 * Requires unclaimedNuggets parameter, which stand for the amount of nuggets remaining. 
 * 
 * This function displays a banner at the top of the ncurses display of the form "Spectating: 
 * [unclaimedNuggets] nuggets unclaimed." 
 */
void display_spectator_banner(const int unclaimedNuggets);

/*
 * Displays mesage at banner end indicating that the user pressed an invalid key.
 *
 * Require key parameter, the key character that was invalid. 
 * 
 * This function displays an indicator of the following form "Invalid keystroke [character]" 
 */
void indicate_invalid_key(const char key);

/*
 * Displays mesage at banner end indicating that the client player collected nuggets.
 *
 * Require collected parameter, the amount of nuggets collected. 
 * 
 * This function displays an indicator of the following form "You collected [collected] nuggets!" 
 */
void indicate_nuggets_collected_player(const int collected);

/*
 * Displays mesage at banner end indicating that a player collected nuggets.
 *
 * Require symbol and collected parameters, the symbol of the player that collected nuggets and the amount 
 * of nuggets collected. 
 * 
 * This function displays an indicator of the following form "Player [symbol] collected [collected] 
 * nuggets!" 
 */
void indicate_nuggets_collected_spectator(const char symbol, const int collected);

/*
 * Displays mesage at banner end indicating that a client player stole nuggets from another player.
 *
 * Requires stolenPlayerSymbol and stolenAmount, the symbol of the player that client player stole from and the
 * amount. 
 * 
 * This function displays an indicator of the following form "You stole [amount] nuggets from player 
 * [stolenPlayerSymbol]" 
 */
void indicate_you_stole_nuggets_from_someone(const char stolenPlayerSymbol, const int stolenAmount);

/*
 * Displays mesage at banner end indicating that someone stole from client player.
 *
 * Requires stealerPlayerSymbol and stolenAmount, the symbol of the player stole from client player and the
 * amount. 
 * 
 * This function displays an indicator of the following form "Player [stealerPlayerSymbol] stole [stolenAmount]
 * nuggets from you!" 
 */
void indicate_someone_stole_nuggets_from_you(const char stealerPlayerSymbol, const int stolenAmount);

/*
 * Displays mesage at banner end indicating that someone stole from someone else.
 *
 * Requires stolenPlayerSymbol, stealerPlayerSymbol, and stolenAmount, the symbol of the player who was stolen from
 * the synbol of the player who stole, and the ammount stolen. 
 * 
 * This function displays an indicator of the following form "Player [stealerPlayerSymbol] stole [stolenAmount] 
 * nuggets from player [stolenPlayerSymbol]!" 
 */
void indicate_nuggets_stolen_spectator(const char stolenPlayerSymbol, const char stealerPlayerSymbol, const int stolenAmount);

/*
 * Removes indicator message.
 *
 * This function uses a helper function to move the cursor to the period at the end of the base banner.
 * It then marches down the rest of the line deleteing everything. This function is called within and
 * outside the graphics module. 
 */
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

#endif /* _GRAPHICS_H_ */
