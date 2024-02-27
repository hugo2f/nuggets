#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ncurses.h>
#include "message.h"
#include "graphics.h"

enum ClientState {
    CLIENT_PRE_INIT,
    CLIENT_START_SENT,
    CLIENT_OKAY_RECEIVED,
    CLIENT_GRID_RECEIVED,
    CLIENT_PLAY
};

typedef struct {
    char* playerName;
    char playerSymbol;
    int nrows;
    int ncols;
    int state;
} ClientData;

static void parseArgs(int argc, char* argv[], addr_t* serverp);
static void setPlayerName(char* name);
static bool handleInput(void* server);
static void handleClientTypeSpecificInput(addr_t* serverp, const char validInputs[]);
static bool handleMessage(void* arg, const addr_t from, const char* message);
static void sendReceipt(addr_t* serverp);
static bool sendStart(addr_t* serverp);
static void sendPlay(addr_t* serverp);
static void sendSpectate(addr_t* serverp);
static void sendKey(addr_t* serverp, char message);
static void handleOkay(char* symbol);
static void handleGrid(char* coordinates);
static void handleGold(char* counts);
static void handleDisplay(char* map);
static void handleQuit(char* explanation);
static void handleError(char* error);
static void indicateInvalidKey(char key);
static void indicateNuggetsCollected(int collected);
static int getMapSize(); 

ClientData client = {NULL, '\0', 0, 0, CLIENT_PRE_INIT};

int 
main(int argc, char* argv[]) 
{
    addr_t server;
    parseArgs(argc, argv, &server);

    bool messageLoopExitStatus = message_loop(&server, 0, NULL, handleInput, handleMessage);

    message_done();
    return messageLoopExitStatus ? EXIT_SUCCESS : 1;
}

void 
parseArgs(int argc, char* argv[], addr_t* serverp) 
{
    const char* program = argv[0];
    if (argc != 3 && argc != 4) {
        fprintf(stderr, "Usage: %s hostname port [player name]\n", program);
        exit(2);
    }

    if (message_init(NULL) == 0) {
        fprintf(stderr, "Could not initialize message module\n");
        exit(3);
    }

    const char* serverHost = argv[1];
    const char* serverPort = argv[2];
    if (!message_setAddr(serverHost, serverPort, serverp)) {
        fprintf(stderr, "Could not create address from serverHost=%s and serverPort=%s\n", serverHost, serverPort);
        exit(4);
    }

    if (serverp == NULL || !message_isAddr(*serverp)) {
        fprintf(stderr, "Invalid server address\n");
        exit(5);
    }

    if (argc == 4) {
        setPlayerName(argv[3]);
    }

    sendStart(serverp);
}

static void
setPlayerName(char* name) 
{
    if (client.playerName != NULL) {
        fprintf(stderr, "You cannot reset player name");
        return;
    }

    if (strlen(name) > 50) {  // CHANGE
        name[50] = '\0';  // CHANGE
    }

    client.playerName = name;
}

static bool 
handleInput(void* server) 
{    
    addr_t* serverp = server;
    if (serverp == NULL || !message_isAddr(*serverp)) {
        fprintf(stderr, "Invalid server address\n");
        return true;
    }

    if (client.playerName == NULL) {
        handleClientTypeSpecificInput(serverp, "q");
    } else {
        handleClientTypeSpecificInput(serverp, "qhljkyunb");
    }

    return false;
}

static void 
handleClientTypeSpecificInput(addr_t* serverp, const char validInputs[]) 
{
    int input = getch();
    if (strchr(validInputs, input) != NULL) {
        sendKey(serverp, input);
    } else {
        indicateInvalidKey(input);
    }
}

static bool 
handleMessage(void* arg, const addr_t from, const char* message) 
{
    if (message == NULL) {
        fprintf(stderr, "Obtained NULL message");
        sendReceipt(&from); // REMOVE LATER
        return false;
    }

    char messageType[10];
    char remainder[100];
    if (sscanf(message, "%9s %99[^\n]", messageType, remainder) != 2) {
        fprintf(stderr, "Received message with invalid format\n");
        sendReceipt(&from); // REMOVE LATER
        return false;
    }

    if (strcmp(messageType, "OK") == 0) {
        handleOkay(remainder);
    } else if (strcmp(messageType, "GRID") == 0) {
        handleGrid(remainder);
    } else if (strcmp(messageType, "GOLD") == 0) {
        handleGold(remainder);
    } else if (strcmp(messageType, "QUIT") == 0) {
        handleQuit(remainder);
    } else if (strcmp(messageType, "ERROR") == 0) {
        handleError(remainder);
    } else if (strcmp(messageType, "DISPLAY") == 0) {
        int mapsize = getMapSize();
        
        char map[mapsize];
        if (sscanf(message, "%*s %99[^\n]", map) != 1) {
            fprintf(stderr, "Failed to retrieve map from DISPLAY message\n");
            sendReceipt(&from); // REMOVE LATER
            return false;
        }

        handleDisplay(map);  
    } else {
        fprintf(stderr, "%s is an invalid message header\n", messageType);
    }
    
    sendReceipt(&from); // REMOVE LATER
    return false;
}

static void
sendReceipt(addr_t* serverp) 
{
    if (client.state != CLIENT_PLAY && client.state != CLIENT_PRE_INIT) {
        message_send(*serverp, "RECEIVED");
    }
}

static bool 
sendStart(addr_t* serverp) 
{
    if (client.state != CLIENT_PRE_INIT) {
        fprintf(stderr, "Sent START again\n");
        return true;
    }

    if (client.playerName == NULL) {
        sendSpectate(serverp);
    } else {
        sendPlay(serverp);
    }

    client.state = CLIENT_START_SENT;
    return false;
}

static void 
sendPlay(addr_t* serverp) 
{
    char message[100];  // CHANGE
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
    if (client.state != CLIENT_PLAY) {
        return;
    }

    char message[10];  // CHANGE
    sprintf(message, "KEY %c", key);
    message_send(*serverp, message);

    remove_from_banner();
}

static void 
handleOkay(char* symbol) 
{
    if (client.state != CLIENT_START_SENT) {
        fprintf(stderr, "Received OK prior to sending START\n");
        return;
    }

    client.playerSymbol = *symbol;

    client.state = CLIENT_OKAY_RECEIVED;
}

static void 
handleGrid(char* coordinates) 
{
    if (client.state != CLIENT_OKAY_RECEIVED) {
        fprintf(stderr, "Received GRID prior to receiving OK\n");
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

    display_banner(client.playerSymbol, 0, 0);

    client.state = CLIENT_GRID_RECEIVED;
}

static void 
handleGold(char* counts) 
{
    if (client.state != CLIENT_PLAY) {
        fprintf(stderr, "Received GOLD prior to game start\n");
        return;
    }

    int collected, current, remaining;
    if (sscanf(counts, "%d %d %d", &collected, &current, &remaining) != 3) {
        fprintf(stderr, "GOLD message missing data\n");
        return;
    }

    display_banner(client.playerSymbol, current, remaining);
    indicateNuggetsCollected(collected);
}

static void 
handleDisplay(char* map) 
{
    if (client.state != CLIENT_GRID_RECEIVED && client.state != CLIENT_PLAY) {
        fprintf(stderr, "Received DISPLAY prior to receiving GRID\n");
        return;
    }

    display_map(map);

    if (client.state != CLIENT_PLAY) {
        client.state = CLIENT_PLAY;
    }
}

static void 
handleQuit(char* explanation) 
{
    end_curses();
    printf("QUIT %s\n", explanation);
    fflush(stdout);
    exit(EXIT_SUCCESS);
}

static void 
handleError(char* error) 
{
    fprintf(stderr, "ERROR %s\n", error);
}

static void 
indicateInvalidKey(char key) 
{
    if (client.state != CLIENT_PLAY) {
        return;
    }

    char message[20];  // CHANGE
    sprintf(message, "Invalid keystroke %c", key);

    remove_from_banner();
    append_to_banner(message);
}

static void
indicateNuggetsCollected(int collected) 
{
    char message[100]; // CHANGE
    sprintf(message, "You collected %d nuggets!", collected);

    append_to_banner(message);
}

static int
getMapSize() 
{
    int mapsize = client.ncols * client.nrows;
    mapsize = (mapsize == 0) ? 1000 : mapsize;
    return mapsize;
}