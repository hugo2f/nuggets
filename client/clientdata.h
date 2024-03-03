/*
 * validators.h
 *
 * Description: contains global variables and global constants for the entire client system.
 * 
 * Author: Joseph Hirsh
 * Date: March 2st, 2024
 *
 */

#ifndef _CLIENT_DATA_H_
#define _CLIENT_DATA_H_

#include <stdbool.h>
#include <stdio.h>

/*
 * Enum for possible client states, used to ensure messages are sent and received in the correct order. 
 */
typedef enum {
    PRE_INIT, // before client sends START
    START_SENT, // after client sends START, before client receives OK
    OK_RECEIVED, // after client receives OK, before client receives GRID
    GRID_RECEIVED, // after client receives GRID, before client receives GOLD_REMAINING
    GOLD_REMAINING_RECEIVED, // after client receives GOLD_REMAINING, before client receives PLAY
    PLAY // after client receives PLAY, before client receives QUIT
} ClientState;

/*
 * Type for all globally-scoped client data. 
 */
typedef struct {
    char* playerName; // name of player (can contain spaces, can be MAXIMUM_NAME_LENGTH long)
    char playerSymbol; // symbol (also can be thought of as ID) for player
    int nrowsMap; // number of rows of the map
    int ncolsMap; // number of columns of the map
    int nrowsScreen; // number of rows of the screen (terminal window)
    int ncolsScreen; // number of column of the screen (terminal window)
    int maximumGold; // maximum possible gold count in this game
    int state; // state the client is currently in (one of the enum values above)
} ClientData;

extern ClientData client; // globally-scoped client data

extern const int MAXIMUM_NAME_LENGTH; // maximum length of player name
extern const int MAXIMUM_GOLD; // maximum possible gold count in general for any game for this client
extern const int MAXIMUM_MAP_SIZE; // maximum size of map

extern const int BACKGROUND_COLOR; // color of display background
extern const int FOREGROUND_COLOR; // color of display foreground

extern const char* PLAYER_KEYSTROKES; // contains all valid keysrokes player can press
extern const char* SPECTATOR_KEYSTROKES; // contains all valid keysrokes spectator can press

#endif /* _CLIENT_DATA_H_ */
