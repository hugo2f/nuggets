#ifndef CLIENT_DATA_H
#define CLIENT_DATA_H

#include <stdbool.h>

typedef enum {
    CLIENT_PRE_INIT,
    CLIENT_START_SENT,
    CLIENT_OKAY_RECEIVED,
    CLIENT_GRID_RECEIVED,
    CLIENT_PLAY
} ClientState;

typedef struct {
    char* playerName;
    char playerSymbol;
    int nrows;
    int ncols;
    int state;
} ClientData;

extern ClientData client;

extern const int MAXIMUM_NAME_LENGTH;
extern const int MAXIMUM_GOLD;
extern const int MAXIMUM_MAP_SIZE;

#endif /* CLIENT_DATA_H */
