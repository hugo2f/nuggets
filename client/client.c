#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ncurses.h>
#include "message.h"
#include "graphics.h"

enum ClientState {
    CLIENT_PRE_INIT,
    CLIENT_INIT,
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
static bool handleInput(void* server);
static bool handleInputSpecific(addr_t* serverp, const char validInputs[]);
static bool handleMessage(void* arg, const addr_t from, const char* message);
static void sendReceipt(addr_t* serverp);
static bool sendStart(addr_t* serverp);
static void sendPlay(addr_t* serverp);
static void sendSpectate(addr_t* serverp);
static void sendKey(addr_t* serverp, char message);
static void indicateInvalidKey(char key);
static void handleOkay(char* symbol);
static void handleGrid(char* coordinates);
static void handleGold(char* counts);
static void handleDisplay(char* board);
static void handleQuit(char* explanation);
static void handleError(char* error);

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
        fprintf(stderr, "Could not initialize message module!\n");
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
handleInput(void* server) 
{
    addr_t* serverp = server;
    if (serverp == NULL || !message_isAddr(*serverp)) {
        fprintf(stderr, "Invalid server address\n");
        return true;
    }

    switch (client.state) {
        case CLIENT_PRE_INIT:
            return sendStart(serverp);
        case CLIENT_INIT:
            sendReceipt(serverp);
            return false;
        case CLIENT_PLAY:
            if (client.playerName == NULL) {
                return handleInputSpecific(serverp, "q");
            } else {
                return handleInputSpecific(serverp, "qhljkyunb");
            }
        default:
            fprintf(stderr, "Unknown client state\n");
            return true;
    }
}

static bool 
handleInputSpecific(addr_t* serverp, const char validInputs[]) 
{
    int input = getch();
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
    if (sscanf(message, "%9s %99[^\n]", messageType, remainder) != 2) {
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
sendReceipt(addr_t* serverp) 
{
    message_send(*serverp, "RECEIVED");
}

static bool 
sendStart(addr_t* serverp) 
{
    if (client.state != CLIENT_PRE_INIT) {
        fprintf(stderr, "Sent START not during client pre-initialization phase\n");
        return true;
    }

    if (client.playerName == NULL) {
        sendSpectate(serverp);
    } else {
        sendPlay(serverp);
    }

    client.state = CLIENT_INIT;
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
    if (client.state != CLIENT_PLAY) {
        return;
    }

    char message[10];
    sprintf(message, "KEY %c", key);
    message_send(*serverp, message);

    remove_from_banner();
}

static void 
indicateInvalidKey(char key) 
{
    if (client.state != CLIENT_PLAY) {
        return;
    }

    char message[100];
    sprintf(message, "Invalid keystroke %c", key);

    append_to_banner(message);
}

static void 
handleOkay(char* symbol) 
{
    if (client.state != CLIENT_INIT) {
        fprintf(stderr, "Received OK not during client initialization phase\n");
        return;
    }

    client.playerSymbol = *symbol;
}

static void 
handleGrid(char* coordinates) 
{
    if (client.state != CLIENT_INIT) {
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

    client.state = CLIENT_PLAY;
}

static void 
handleGold(char* counts) 
{
    if (client.state != CLIENT_PLAY) {
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
    if (client.state != CLIENT_PLAY) {
        fprintf(stderr, "Received DISPLAY not during client phase\n");
        return;
    }

    display_board(board);
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
