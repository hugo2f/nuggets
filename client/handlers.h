/*
 * handlers.h
 *
 * Description: contains interface for the handler module.
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
 * Runs in START_SENT state. 
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
 * Runs in OK_RECEIVED state. 
 * 
 * Handler initializes curses display, sets client nrowsBoard and ncolsBoard, and advances the client 
 * state.
 */
void handle_grid(char* coordinates); 

/*
 * Handles messages of the form "STARTING_GOLD_REMAINING"
 *
 * Runs in GRID_RECEIVED state
 * 
 * Handler prints banner according to gold remaining and advances the client state.
 * 
 */
void handle_gold_remaining(char* startingGoldRemainingString);

/*
 * Handles messages of the form "GOLD [collected] [current] [remaining]"
 *
 * Runs in PLAY state for player. 
 * 
 * Handler updates banner according to new values and prints indicator alerting player to the collected 
 * gold.
 */
void handle_player_gold(char* counts);

/*
 * Handles messages of the form "SPECTATOR_GOLD [playerSymbol] [collected] [current] [remaining]"
 *
 * Runs in PLAY state for spectator. 
 * 
 * Handler updates banner according to new values and prints indicator alerting spectator to the collected 
 * gold. This function runs every time any player collects.
 */
void handle_spectator_gold(char* collectionData);

/*
 * Handles messages of the form "DISPLAY [map]"
 *
 * Runs in PLAY or GRID_RECEIVED states. 
 * 
 * Handler displays map and advances state if the current state is not already PLAY. 
 */
void handle_display(char* map); 

/*
 * Handles messages of the form "STOLEN [stealerPlayerID] [stolenPlayerID] [amount]"
 *
 * Runs in PLAY state. 
 * 
 * Handler displays banner according to if you stole from them, they stole from you, or you are a 
 * spectator.
 */
void handle_stolen(char* stealData);

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

#endif /* _HANDLERS_H_ */
