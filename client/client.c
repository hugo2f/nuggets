#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "message.h"
#include "graphics.h"

typedef bool (*InputHandler)(void* arg);

typedef struct gameState {
    char* playerName;
    char* playerSymbol;
    int playerGold;
    int goldRemaining;
    int nrows;
    int ncols;
    bool started;
} gameState_t;

void parseArgs(const int argc, char* argv[], addr_t* server, InputHandler* handleInput);
static bool handleSpectatorInput(void* arg);
static bool handlePlayerInput(void* arg);
static bool handleInputGeneric(void* server, const char validInputs[]);
static bool handleMessage(void* arg, const addr_t from, const char* message);
void sendStart(addr_t* serverp);
void sendPlay(addr_t* serverp);
void sendKey(addr_t* serverp, char message);
void sendSpectate(addr_t* serverp);
void indicateInvalidKey(char key);
void handleOkay(char* symbol);
void handleGrid(char* coordinates);
void handleGold(char* counts);
void handleDisplay(char* board);
void handleQuit(char* explanation);
void handleError(char* error);

struct gameState game = {NULL, NULL, 0, 0, 0, 0, false};

int 
main(const int argc, char* argv[]) 
{
    addr_t server;
    InputHandler handleInput;
    
    parseArgs(argc, argv, &server, &handleInput);
    bool messageLoopExitStatus = message_loop(&server, 0, NULL, handleInput, handleMessage);
    message_done();
  
    return messageLoopExitStatus? 0 : 1;
}

void 
parseArgs(const int argc, char* argv[], addr_t* serverp, InputHandler* handleInputp) 
{
    const char* program = argv[0];
    if (argc != 3 && argc != 4) {
        fprintf(stderr, "Usage: %s hostname port [player name]", program);
        exit(2);
    }

    if (message_init(stderr) == 0) {
        fprintf(stderr, "Could not init message module!");
        exit(3);
    }

    const char* serverHost = argv[1];
    const char* serverPort = argv[2];
    if (!message_setAddr(serverHost, serverPort, serverp)) {
        fprintf(stderr, "Could not create address from %s %s\n", serverHost, serverPort);
        exit(4);
    }

    if (argc == 3) {
        *handleInputp = handleSpectatorInput;
    } else {
        game.playerName = argv[3];
        *handleInputp = handlePlayerInput;
    }
}

static bool 
handleSpectatorInput(void* server) 
{
    return handleInputGeneric(server, "q");  
}

static bool 
handlePlayerInput(void* server) 
{
    return handleInputGeneric(server, "qhljkyunb");  
}

static bool 
handleInputGeneric(void* server, const char validInputs[])
{    
    addr_t* serverp = server;
    if (serverp == NULL) {
        fprintf(stderr, "InputHandler called with arg=NULL");
        return true;
    }
    
    if (!message_isAddr(*serverp)) {
        fprintf(stderr, "InputHandler called without a correspondent.");
        return true;
    }

    if (!game.started) {
        sendStart(serverp);
        return false;
    }
    
    char input = getchar();
    if (strchr(validInputs, input) != NULL) {
        sendKey(serverp, input);
    } else {
        indicateInvalidKey(input);
    }

    return false;
}

static bool
handleMessage(void* arg, const addr_t from, const char* message)
{
    if (message == NULL) {
        return false;
    }
    
    char messageType[10];
    char remainder[100];
    sscanf(message, "%s %99[^\n]", messageType, remainder);
    
    if (strcmp(messageType, "OK") == 0) {
        handleOkay(remainder);
    } else if (strcmp(messageType, "GRID") == 0) {
        handleGrid(remainder);
    } else if (strcmp(messageType, "GOLD") == 0) {
        handleGold(remainder);
    } else if (strcmp(messageType, "DISPLAY") == 0) {
        handleDisplay(remainder);
    } else if (strcmp(messageType, "QUIT") == 0) {
        handleQuit(remainder);
        return true;
    } else if (strcmp(messageType, "ERROR") == 0) {
        handleError(remainder);
    } else {
        fprintf(stderr, "%s is an invalid messageType", messageType);
    }

    return false;
}

void 
sendStart(addr_t* serverp) {
    if (game.playerName == NULL) {
        sendSpectate(serverp);
    } else {
        sendPlay(serverp);
    }
}

void
sendPlay(addr_t* serverp) {
    char message[100]; 
    sprintf(message, "PLAY %c", game.playerName);
    message_send(*serverp, message);
}

void
sendSpectate(addr_t* serverp) {
    message_send(*serverp, "SPECTATE");
}

void 
sendKey(addr_t* serverp, char key)
{
    char message[10]; 
    sprintf(message, "KEY %c", key);
    message_send(*serverp, message);
}

void
indicateInvalidKey(char key) 
{
    char message[100];
    sprintf(message, "Invalid keystroke %c", key);
    
    display_banner(game.playerSymbol, game.playerGold, game.goldRemaining, message); 
}

void 
handleOkay(char* symbol) 
{
    game.playerSymbol = symbol;
}

void 
handleGrid(char* coordinates) 
{
    int nrows, ncols;
    sscanf(coordinates, "%d %d", &nrows, &ncols);
    
    if (init_curses(nrows, ncols)) {
        game.nrows = nrows;
        game.ncols = ncols;
        game.started = true;
        return;
    } 

    while (!init_curses(nrows, ncols)) {
        printf("You must enlarge the window to at least %d %d!", nrows, ncols);
    }
}

void 
handleGold(char* counts) 
{
    int collected, current, remaining;
    sscanf(counts, "%d %d %d", &collected, &current, &remaining);

    char message[100];
    sprintf(message, "You collected %d nuggets!", collected);

    display_banner(game.playerSymbol, game.playerGold, game.goldRemaining, message); 
}

void 
handleDisplay(char* board) 
{
    display_board(board, game.nrows, game.ncols);
}

void 
handleQuit(char* explanation) 
{
    exit_curses();
    printf("%s", explanation);
    exit(0);
}

void 
handleError(char* error) 
{
    fprintf(stderr, "ERROR %s", error);
}
