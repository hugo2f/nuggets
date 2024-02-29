#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "message.h"
#include "indicators.h"
#include "graphics.h"
#include "senders.h"
#include "handlers.h"
#include "clientdata.h"

static void parseArgs(int argc, char* argv[], addr_t* serverp);
static bool respondToInput(void* server);
static bool handleMessage(void* arg, const addr_t from, const char* message);

static void setPlayerName(char* name);
static int getMapSize(); 

ClientData client = {NULL, '\0', 0, 0, CLIENT_PRE_INIT};

const int MAXIMUM_NAME_LENGTH = 50;
const int MAXIMUM_GOLD = 1000;
const int MAXIMUM_MAP_SIZE = 2500;
const char* PLAYER_KEYSTROKES = "qQhHlLjJkKyYuUnNbB";
const char* SPECTATOR_KEYSTROKES = "qQ";

int 
main(int argc, char* argv[]) 
{
    addr_t server;
    parseArgs(argc, argv, &server);

    bool messageLoopExitStatus = message_loop(&server, 0, NULL, respondToInput, handleMessage);

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

    send_start(serverp);
}

static bool 
respondToInput(void* server) 
{    
    if (client.state != CLIENT_PLAY) {
        return false;
    }
    
    addr_t* serverp = server;
    if (serverp == NULL || !message_isAddr(*serverp)) {
        fprintf(stderr, "Invalid server address\n");
        return true;
    }

    const char* validInputs = (client.playerName == NULL) ? SPECTATOR_KEYSTROKES : PLAYER_KEYSTROKES;

    int input = get_character();
    
    if (strchr(validInputs, input) != NULL) {
        send_key(serverp, input);
        remove_from_banner();
    } else if (client.playerName != NULL) {
        indicate_invalid_key(input);
    }

    return false;
}

static bool 
handleMessage(void* arg, const addr_t from, const char* message) 
{
    if (message == NULL) {
        fprintf(stderr, "Obtained NULL message");
        send_receipt((addr_t *)&from);
        return false;
    }

    char messageHeader[10];
    char remainder[100];
    if (sscanf(message, "%9s %99[^\n]", messageHeader, remainder) != 2) {
        fprintf(stderr, "Received message with invalid format\n");
        send_receipt((addr_t *)&from);
        return false;
    }

    if (strcmp(messageHeader, "OK") == 0) {
        handle_okay(remainder);
    } else if (strcmp(messageHeader, "GRID") == 0) {
        handle_grid(remainder);
    } else if (strcmp(messageHeader, "GOLD") == 0) {
        handle_gold(remainder);
    } else if (strcmp(messageHeader, "QUIT") == 0) {
        handle_quit(remainder);
    } else if (strcmp(messageHeader, "ERROR") == 0) {
        handle_error(remainder);
    } else if (strcmp(messageHeader, "DISPLAY") == 0) {
        int mapsize = getMapSize();
        
        char map[mapsize];
        if (sscanf(message, "%*s %[^\n]", map) != 1) {
            fprintf(stderr, "Failed to retrieve map from DISPLAY message\n");
            send_receipt((addr_t *)&from);
            return false;
        }

        handle_display(map);  
    } else {
        fprintf(stderr, "%s is an invalid message header\n", messageHeader);
    }
    
    send_receipt((addr_t *)&from);
    return false;
}

static void
setPlayerName(char* name) 
{
    if (client.playerName != NULL) {
        fprintf(stderr, "You cannot reset player name");
        return;
    }

    if (strlen(name) > MAXIMUM_NAME_LENGTH) {
        name[MAXIMUM_NAME_LENGTH] = '\0';
    }

    client.playerName = name;
}

static int
getMapSize() 
{
    int mapsize = client.ncols * client.nrows;
    mapsize = (mapsize == 0) ? MAXIMUM_MAP_SIZE : mapsize;
    return mapsize;
}