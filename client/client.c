#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <strings.h>
#include <ncurses.h>
#include "message.h"
#include "graphics.h"

typedef struct gameState {
    char* playerName;
    char* playerSymbol;
    int playerGold;
    int goldRemaining;
    int nrows;
    int ncols;
    int status;
} gameState_t;

static void parseArgs(const int argc, char* argv[], addr_t* serverp);
static bool handleInputGeneric(void* server);
static bool handleInputSpecific(addr_t* serverp, const char validInputs[]);
static bool handleMessage(void* arg, const addr_t from, const char* message);
static void sendStart(addr_t* serverp);
static void sendPlay(addr_t* serverp);
static void sendKey(addr_t* serverp, char message);
static void sendSpectate(addr_t* serverp);
static void sendReceipt(addr_t* serverp);
static void indicateInvalidKey(char key);
static void handleOkay(char* symbol);
static void handleGrid(char* coordinates);
static void handleGold(char* counts);
static void handleDisplay(char* board);
static void handleQuit(char* explanation);
static void handleError(char* error);

struct gameState game = {NULL, NULL, 0, 0, 0, 0, 0};

int 
main(const int argc, char* argv[]) 
{
    addr_t server;
    parseArgs(argc, argv, &server);
    bool messageLoopExitStatus = message_loop(&server, 0, NULL, handleInputGeneric, handleMessage);
    message_done();
  
    return messageLoopExitStatus? 0 : 1;
}

void 
parseArgs(const int argc, char* argv[], addr_t* serverp) 
{
    const char* program = argv[0];
    if (argc != 3 && argc != 4) {
        fprintf(stderr, "Usage: %s hostname port [player name]", program);
        exit(2);
    }

    if (message_init(NULL) == 0) {
        fprintf(stderr, "Could not init message module!");
        exit(3);
    }

    const char* serverHost = argv[1];
    const char* serverPort = argv[2];
    if (!message_setAddr(serverHost, serverPort, serverp)) {
        fprintf(stderr, "Could not create address from %s %s\n", serverHost, serverPort);
        exit(4);
    }

    if (argc == 4) {
        game.playerName = argv[3];
    }
}

static bool 
handleInputGeneric(void* server) 
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

    if (game.status == 0) {
        sendStart(serverp);
        game.status = 1;
        return false;
    } else if (game.status == 1) {
        sendReceipt(serverp); // for testing
        return false;
    } else if (game.status == 3) {
        if (game.playerName == NULL) {
            return handleInputSpecific(serverp, "q");  
        } else {
            return handleInputSpecific(serverp, "qhljkyunb");  
        }
    } else {
        return true;
    }
}

static bool 
handleInputSpecific(addr_t* serverp, const char validInputs[])
{    
    char input = getch();
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

static void 
sendStart(addr_t* serverp) 
{
    if (game.status != 0) {
        return;
    }
    
    if (game.playerName == NULL) {
        sendSpectate(serverp);
    } else {
        sendPlay(serverp);
    }
}

static void
sendPlay(addr_t* serverp) 
{
    char message[100]; 
    sprintf(message, "PLAY %s", game.playerName);
    message_send(*serverp, message);
}

static void
sendSpectate(addr_t* serverp) 
{
    message_send(*serverp, "SPECTATE");
}

static void 
sendKey(addr_t* serverp, char key)
{
    if (game.status != 3) {
        return;
    }
    
    char message[10]; 
    sprintf(message, "KEY %c", key);
    message_send(*serverp, message);
    display_banner(game.playerSymbol, game.playerGold, game.goldRemaining, NULL);
}

static void
sendReceipt(addr_t* serverp) {
    message_send(*serverp, "received");
}

static void
indicateInvalidKey(char key) 
{
    char message[100];
    sprintf(message, "Invalid keystroke %c", key);
    
    display_banner(game.playerSymbol, game.playerGold, game.goldRemaining, message); 
}

static void 
handleOkay(char* symbol) 
{
    if (game.status != 1) {
        return;
    }

    game.playerSymbol = symbol;
}

static void 
handleGrid(char* coordinates) 
{
    if (game.status != 1) {
        return;
    }
    
    int nrows, ncols;
    sscanf(coordinates, "%d %d", &nrows, &ncols);
    
    if (init_curses(nrows, ncols)) {
        game.nrows = nrows;
        game.ncols = ncols;
        game.status = 3;
        return;
    } 

    printf("You must enlarge the window to at least %d %d!", nrows, ncols);
    fflush(stdout);

    while (!init_curses(nrows, ncols));
}

static void 
handleGold(char* counts) 
{
    if (game.status != 3) {
        return;
    }
    
    int collected, current, remaining;
    sscanf(counts, "%d %d %d", &collected, &current, &remaining);

    char message[100];
    sprintf(message, "You collected %d nuggets!", collected);

    display_banner(game.playerSymbol, game.playerGold, game.goldRemaining, message); 
}

static void 
handleDisplay(char* board) 
{
    if (game.status != 3) {
        return;
    }

    display_board(board, game.nrows, game.ncols);
}

static void 
handleQuit(char* explanation) 
{
    if (game.status != 3) {
        return;
    }
    
    end_curses();
    printf("QUIT %s\n", explanation);
    fflush(stdout);
    exit(0);
}

static void 
handleError(char* error) 
{
    fprintf(stderr, "ERROR %s", error);
}
