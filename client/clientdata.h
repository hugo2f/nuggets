#ifndef CLIENT_DATA_H
#define CLIENT_DATA_H

#include <stdbool.h>
#include <stdio.h>

typedef enum {
    PRE_INIT,
    START_SENT,
    OK_RECEIVED,
    GRID_RECEIVED,
    STARTING_GOLD_REMAINING_RECEIVED,
    PLAY
} ClientState;

typedef struct {
    char* playerName;
    char playerSymbol;
    int nrowsMap;
    int ncolsMap;
    int nrowsScreen;
    int ncolsScreen;
    int state;
} ClientData;

extern ClientData client;

extern const int MAXIMUM_NAME_LENGTH;
extern const int MAXIMUM_GOLD;
extern const int MAXIMUM_MAP_SIZE;

extern const int BACKGROUND_COLOR;
extern const int FOREGROUND_COLOR;

extern const char* PLAYER_KEYSTROKES;
extern const char* SPECTATOR_KEYSTROKES;

#endif
