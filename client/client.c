#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <strings.h>
#include <ncurses.h>
#include "message.h"
#include "graphics.h"

typedef struct clientData {
    char* playerName;
    char playerSymbol;
    int nrows;
    int ncols;
    int state;
} clientData_t;

static void parseArgs(const int argc, char* argv[], addr_t* serverp);
static bool handleInputGeneric(void* server);
static bool handleInputSpecific(addr_t* serverp, const char validInputs[]);
static bool handleMessage(void* arg, const addr_t from, const char* message);
static void nextClientState();
static bool sendStart(addr_t* serverp);
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

struct clientData client = {NULL, '\0', 0, 0, 0};

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
        fprintf(stderr, "Usage: %s hostname port [player name]\n", program);
        exit(2);
    }

    if (message_init(NULL) == 0) {
        fprintf(stderr, "Could not init message module!\n");
        exit(3);
    }

    const char* serverHost = argv[1];
    const char* serverPort = argv[2];
    if (!message_setAddr(serverHost, serverPort, serverp)) {
        fprintf(stderr, "Could not create address from %s %s\n", serverHost, serverPort);
        exit(4);
    }

    if (argc == 4) {
        client.playerName = argv[3];
    }
}

static bool 
handleInputGeneric(void* server) 
{
    addr_t* serverp = server;
    if (serverp == NULL) {
        fprintf(stderr, "InputHandler called with arg=NULL\n");
        return true;
    }
    
    if (!message_isAddr(*serverp)) {
        fprintf(stderr, "InputHandler called without a correspondent\n");
        return true;
    }

    if (client.state == 0) {
        return sendStart(serverp);
    } else if (client.state == 1) {
        sendReceipt(serverp);
        return false;
    } else if (client.state == 2) {
        if (client.playerName == NULL) {
            return handleInputSpecific(serverp, "q");  
        } else {
            return handleInputSpecific(serverp, "qhljkyunb");  
        }
    } else {
        fprintf(stderr, "Impossible client state");
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
    if (message == NULL) return false;
    
    char messageType[10];
    char remainder[100];
    if (sscanf(message, "%s %99[^\n]", messageType, remainder) != 2) {
        fprintf(stderr, "Received message with invalid format\n");
        return false;
    }
    
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
        fprintf(stderr, "%s is an invalid message header\n", messageType);
    }

    return false;
}

static void
nextClientState() 
{
    if (client.state != 2) {
        client.state += 1;
    } else {
        fprintf(stderr, "Attempting to transition to next client state amid game phase");
    }
}

static bool 
sendStart(addr_t* serverp) 
{
    if (client.state != 0) {
        fprintf(stderr, "Sent START not during client pre-initialization phase");
        return true;
    }

    if (client.playerName == NULL) {
        sendSpectate(serverp);
    } else {
        sendPlay(serverp);
    }

    nextClientState();
    return false;
}

static void
sendPlay(addr_t* serverp) 
{
    char message[100]; 
    sprintf(message, "PLAY %s", client.playerName);
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
    if (client.state != 2) {
        return;
    }
    
    char message[10]; 
    sprintf(message, "KEY %c", key);
    message_send(*serverp, message);
}

static void
sendReceipt(addr_t* serverp) {
    message_send(*serverp, "received");
}

static void
indicateInvalidKey(char key) 
{
    if (client.state != 3) {
        return;
    }
    
    char message[100];
    sprintf(message, "Invalid keystroke %c", key);
    
    append_to_banner(message, client.ncols);
}

static void 
handleOkay(char* symbol) 
{
    if (client.state != 1) {
        fprintf(stderr, "Received OK not during client initialization phase\n");
        return;
    }

    char symbolCharacter = *symbol;
    client.playerSymbol = symbolCharacter;
}

static void 
handleGrid(char* coordinates) 
{
    if (client.state != 1) {
        fprintf(stderr, "Received GRID not during client initialization phase\n");
        return;
    }
    
    int nrows, ncols;
    if (sscanf(coordinates, "%d %d", &nrows, &ncols) != 2) {
        fprintf(stderr, "GRID message missing data\n");
        return;
    }
    
    if (!init_curses(nrows, ncols)) {
        printf("You must enlarge the window to at least %d %d!\n", nrows, ncols);
        fflush(stdout);
    }
    
    while (!init_curses(nrows, ncols));
    
    client.nrows = nrows;
    client.ncols = ncols;
        
    display_banner(client.playerSymbol, 0, 0, 0); 

    nextClientState();
}

static void 
handleGold(char* counts) 
{
    if (client.state != 2) {
        fprintf(stderr, "Received GOLD not during client phase\n");
        return;
    }
    
    int collected, current, remaining;
    if (sscanf(counts, "%d %d %d", &collected, &current, &remaining) != 3) {
        fprintf(stderr, "GOLD message missing data\n");
        return;
    }

    char message[100];
    sprintf(message, "You collected %d nuggets!", collected);

    display_banner(client.playerSymbol, current, remaining, message); 
}

static void 
handleDisplay(char* board) 
{
    if (client.state != 2) {
        fprintf(stderr, "Received DISPLAY not during client phase\n");
        return;
    }
    
    display_board(board, client.nrows, client.ncols);
}

static void 
handleQuit(char* explanation) 
{    
    end_curses();
    printf("QUIT %s\n", explanation);
    fflush(stdout);
    exit(0);
}

static void 
handleError(char* error) 
{
    fprintf(stderr, "ERROR %s\n", error);
}
