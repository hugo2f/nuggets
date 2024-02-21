#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <strings.h>
#include "message.h"
#include "graphics.h"

typedef bool (*InputHandler)(void* arg);

void parseArgs(const int argc, char* argv[], addr_t* server, char** name, InputHandler* handleInput);
static bool handleSpectatorInput(void* arg);
static bool handlePlayerInput(void* arg);
static bool handleMessage(void* arg, const addr_t from, const char* message);
void sendKey(addr_t* serverp, char message);
void handleOkay(char* symbol);
void handleGrid(char* coordinates);
void handleGold(char* counts);
void handleDisplay(char* board);
void handleQuit(char* explanation);
void handleError(char* error);

int 
main(const int argc, char* argv[]) 
{
    addr_t server;
    char* playerName;
    InputHandler handleInput;
    
    parseArgs(argc, argv, &server, &playerName, &handleInput);
    bool messageLoopExitStatus = message_loop(&server, 0, NULL, handleInput, handleMessage);
    message_done();
  
    return messageLoopExitStatus? 0 : 1;
}

void 
parseArgs(const int argc, char* argv[], addr_t* serverp, char** playerNamep, InputHandler* handleInputp) 
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
        *playerNamep = NULL;
        *handleInputp = handleSpectatorInput;
    } else {
        *playerNamep = argv[3];
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
    
    char input = getchar();
    if (strchr(validInputs, input) != NULL) {
        sendKey(serverp, input);
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
sendKey(addr_t* serverp, char key)
{
    char message[10]; 
    sprintf(message, "KEY %s", key);
    message_send(*serverp, message);
}

void 
handleOkay(char* symbol) 
{

}

void 
handleGrid(char* coordinates) 
{

}

void 
handleGold(char* counts) 
{
    int collected, current, remaining;
    sscanf(counts, "%d %d %d", &collected, &current, &remaining);

    char message[100];
    sprintf(message, "You collected %d nuggets!", collected);
    
    display_banner("playerName", current, remaining, message); // replace with name variable 
}

void 
handleDisplay(char* board) 
{

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
    fprintf("ERROR %s", error);
}
