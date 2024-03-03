/*
 * handlers.h
 *
 * Description: contains interface for handler functions.
 * 
 * Author: Joseph Hirsh
 * Date: March 2st, 2024
 * 
 */

#ifndef _HANDLERS_H_
#define _HANDLERS_H_

/*
 * Handles messages of the form "OK [allCapsCharacter]"
 *
 * Runs in CLIENT_START_SENT state. 
 * 
 * Robust with respect to allCapsCharacter being a string: it attempts to make the first character of that
 * string the player symbol.
 * 
 * Handlers sets the player symbol to [allCapsCharacter] and advances client state.
 */
void handle_okay(char* symbol); 

/*
 * Handles messages of the form "GRID [nrows] [ncols]"
 *
 * Runs in CLIENT_OKAY_RECEIVED state. 
 * 
 * Handler initializes curses display, prints the status banner according to the client type, and 
 * advances the client state.
 */
void handle_grid(char* coordinates); 

/*
 * Handles messages of the form "GOLD [collected] [current] [remaining]"
 *
 * Runs in CLIENT_PLAY state for player. 
 * 
 * Handler updates banner according to new values and prints indicator alerting player to the collected 
 * gold.
 */
void handle_gold(char* counts);

/*
 * Handles messages of the form "SPECTATOR_GOLD [playerSymbol] [collected] [current] [remaining]"
 *
 * Runs in CLIENT_PLAY state for spectator. 
 * 
 * Handler updates banner according to new values and prints indicator alerting spectator to the collected 
 * gold. This function runs every time any player collects.
 */
void handle_spectator_gold(char* collectionData);

/*
 * Handles messages of the form "DISPLAY [map]"
 *
 * Runs in CLIENT_PLAY or CLIENT_GRID_RECEIVED states. 
 * 
 * Handler displays map and advances state if the current state is not already CLIENT_PLAY. 
 */
void handle_display(char* map); 

/*
 * Handles messages of the form "QUIT [explanation]"
 *
 * Runs in any state. 
 * 
 * Handler ends curses, prints explanation, and then exits program. 
 */
void handle_quit(char* explanation);

/*
 * Handles messages of the form "ERROR [explanation]"
 *
 * Runs in any state. 
 * 
 * Handler prints error. 
 */
void handle_error(char* error); 

#endif
