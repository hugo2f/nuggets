/*
 * Description: contains the main control flow for the client, defining the global data, receiving 
 * input from the user and messages from the server, delegating the responses to various command 
 * handlers and message senders (organized as such in seperate modules), as well as running tests if it 
 * is in test mode. 
 * 
 * Author: Joseph Hirsh
 * Date: March 1st, 2024
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "message.h"
#include "graphics.h"
#include "senders.h"
#include "handlers.h"
#include "validators.h"
#include "clientdata.h"

// function prototypes
static void parseArgs(int argc, char* argv[], addr_t* serverp);
static bool respondToInput(void* server);
static bool handleMessage(void* arg, const addr_t from, const char* message);
static void setPlayerName(const int argc, char* argv[]);

#ifdef MINISERVER_TEST
static int getMapSize();
#endif

void unitTest(const addr_t from);

// global constants; see .h for more details.
const int MAXIMUM_NAME_LENGTH = 50;
const int MAXIMUM_GOLD = 1000;
const int MAXIMUM_MAP_SIZE = 2500;
const int FOREGROUND_COLOR = 7;
const int BACKGROUND_COLOR = 0;
const char* PLAYER_KEYSTROKES = "qQhHlLjJkKyYuUnNbB";
const char* SPECTATOR_KEYSTROKES = "qQ";

// project-wide global client struct; see .h for more details.
ClientData client = {NULL, '\0', 0, 0, 0, 0, MAXIMUM_GOLD, PRE_INIT};

int 
main(int argc, char* argv[]) 
{    
    addr_t server;
    parseArgs(argc, argv, &server); // sets up server and sets player name

    // uses message module to start a communicaation loop with the server
    bool messageLoopExitStatus = message_loop(&server, 0, NULL, respondToInput, handleMessage);

    // shut down message module
    message_done();

    // free client.playerName which we allocated via the set name function
    free(client.playerName);

    // return error code corresponding to message loop exit status
    return messageLoopExitStatus ? EXIT_SUCCESS : 1;
}

/*
 * Use IP adress and port to attempt server setup and set player name. 
 */
void 
parseArgs(int argc, char* argv[], addr_t* serverp) 
{    
    // verifies correct number of arguments 
    const char* program = argv[0];
    if (argc < 3) {
        fprintf(stderr, "Usage: %s hostname port [player name]\n", program);
        exit(2);
    }

    // attempts initialize message module, errors and exits if it cannot
    if (message_init(NULL) == 0) {
        fprintf(stderr, "Could not initialize message module\n");
        exit(3);
    }

    // attempts to construct server adress, errors and exits if it cannot
    const char* serverHost = argv[1];
    const char* serverPort = argv[2];
    if (!message_setAddr(serverHost, serverPort, serverp)) {
        fprintf(stderr, "Could not create address from serverHost=%s and serverPort=%s\n", serverHost, serverPort);
        exit(4);
    }

    // verifies server adress is valid, errors and exits if not
    if (serverp == NULL || !message_isAddr(*serverp)) {
        fprintf(stderr, "Invalid server address\n");
        exit(5);
    }

    // if there is a fourth argument, use it to set player name
    if (argc > 3) {
        setPlayerName(argc, argv);
    }

    unitTest(*serverp);
    
    // send start message to kick off communication with the server 
    send_start(serverp);
}

/*
 * Responds to user input (keystrokes)
 */
static bool 
respondToInput(void* server) 
{    
    // only respond to user input when a game is in session
    if (client.state != PLAY) {
        return false;
    }
    
    // ensure server adress is valid (this should very rarely be an issue, given that we check this in 
    // parseArgs as well)
    addr_t* serverp = server;
    if (serverp == NULL || !message_isAddr(*serverp)) {
        fprintf(stderr, "Invalid server address\n");
        return true;
    }

    // define valid inputs depending on cllient mode (whether it is a Spectator or Player)
    const char* functionalInputs = (client.playerName == NULL) ? SPECTATOR_KEYSTROKES : PLAYER_KEYSTROKES;

    // get keystroke
    int input = get_character();

    // validate keystroke as not EOF
    if (!validate_stdin_character(input)) {
        send_key(serverp, 'q');
        return false; // continue message loop (we stop when we receive QUIT, not by our own volition)
    }
    
    // if keystroke does something, send it to server, else indicate that it is invalid
    if (strchr(functionalInputs, input) != NULL) {
        send_key(serverp, input);
        remove_indicator(); // removes any indicator present on banner line
    } else if (client.playerName != NULL) {
        indicate_invalid_key(input);
    }

    return false; // continue message loop 
}

/*
 * Responds to server messages
 */
static bool 
handleMessage(void* arg, const addr_t from, const char* message) 
{
    // check if message is NULL, if so, error and continue
    if (message == NULL) {
        fprintf(stderr, "Obtained NULL message");
        send_receipt((addr_t *)&from); // sends receipt message (only to miniserver - see function definition)
        return false; // continue message loop 
    }
    
    // extract the message header (the first word) and everything else
    char messageHeader[25];
    char remainder[100];
    if (sscanf(message, "%24s %99[^\n]", messageHeader, remainder) != 2) {
        fprintf(stderr, "Received message with invalid format\n");
        send_receipt((addr_t *)&from);
        return false; // continue message loop 
    }

    // depending on the message header, call the corresponding function and pass in remainder
    if (strcmp(messageHeader, "OK") == 0) {
        handle_okay(remainder);
    } else if (strcmp(messageHeader, "GRID") == 0) {
        handle_grid(remainder);
    } else if (strcmp(messageHeader, "GOLD") == 0) {
        handle_player_gold(remainder);
    } else if (strcmp(messageHeader, "QUIT") == 0) {
        // we handle the quit parsing seperately as the "remaining" equivelent requires much more memory
        char* skip = "QUIT ";
        char* found;

        // if 'skip' is found within 'message'
        found = strstr(message, skip);
        if (found != NULL) {
            // set 'quit' pointer to the position in 'message' right after the 'skip' substring
            char* quit = found + strlen(skip);
            handle_quit(quit);
        } else {
            fprintf(stderr, "Malformed QUIT message\n");
        }
    } else if (strcmp(messageHeader, "ERROR") == 0) {
        handle_error(remainder);
    } else if (strcmp(messageHeader, "SPECTATOR_GOLD") == 0) {
        handle_spectator_gold(remainder);
    } else if (strcmp(messageHeader, "DISPLAY") == 0) {
        // we handle the display parsing seperately as the "remaining" equivelent requires much more memory
        #ifdef MINISERVER_TEST
        int mapsize = getMapSize(); // get map size

        // get format string that limits command length (to avoid stack smashing), error and continue upon failure
        char format[mapsize];
        if (snprintf(format, mapsize, "DISPLAY%%%d[^\n]", mapsize - 1) < 0) {
            fprintf(stderr, "Failed to create map format string\n");
            send_receipt((addr_t *)&from);
            return false; // continue message loop 
        }
        
        // extract map from message, error and continue upon failure 
        char map[mapsize];
        if (sscanf(message, format, map) != 1) {
            fprintf(stderr, "Failed to retrieve map from DISPLAY message\n");
            send_receipt((addr_t *)&from);
            return false; // continue message loop 
        }

        handle_display(map);
        #else
        char* skip = "DISPLAY\n"; // notice that the header here is formatted slightly different
        char* found;

        // if 'skip' is found within 'message'
        found = strstr(message, skip);
        if (found != NULL) {
            // set 'map' pointer to the position in 'message' right after the 'skip' substring
            char* map = found + strlen(skip);
            handle_display(map);
        } else {
            fprintf(stderr, "Malformed DISPLAY message\n");
        }
        #endif
    } else if (strcmp(messageHeader, "GOLD_REMAINING") == 0) {
        handle_gold_remaining(remainder);
    } else if (strcmp(messageHeader, "STOLEN") == 0) {
        handle_stolen(remainder);
    } else {
        fprintf(stderr, "%s is an invalid message header\n", messageHeader); // bad message header
    }
    
    send_receipt((addr_t *)&from);
    return false; // continue message loop 
}

#include <stdlib.h> // for malloc and free

/*
 * Sets playerName in clientData structure ensuring correct length
 */
static void
setPlayerName(const int argc, char* argv[]) 
{
    // ensures we are not setting a player name we already set 
    if (client.playerName != NULL) {
        fprintf(stderr, "You cannot reset player name");
        return;
    }

    // create name character array
    char name[MAXIMUM_NAME_LENGTH + 1];
    
    // ensure that there is nothing in memory name included in array
    name[0] = '\0';

    int currentNameSize = 0; // will keep track of how full the buffer is
    
    // iterate through each argument
    for (int i = 3; i < argc; i++) {
        char* argument = argv[i]; // get argument
        int argumentLength = strlen(argument); // get argument size

        // if adding the argument and a space will not exceed the buffer, add it and a space
        if (currentNameSize + argumentLength + 1 < MAXIMUM_NAME_LENGTH) {
            strncat(name, argument, MAXIMUM_NAME_LENGTH - currentNameSize - 1);
            strncat(name, " ", MAXIMUM_NAME_LENGTH - currentNameSize - 1);
            currentNameSize = currentNameSize + argumentLength + 1;
        } 
        // otherwise, add however much is possible and break
        else {
            strncat(name, argv[i], MAXIMUM_NAME_LENGTH - currentNameSize);
            break;
        }
    }

    // null-terminate the string
    name[MAXIMUM_NAME_LENGTH] = '\0';

    // allocate memory for client.playerName and copy name into it
    client.playerName = malloc(strlen(name) + 1);
    if (client.playerName == NULL) {
        fprintf(stderr, "Memory allocation failed");
        return;
    }
    
    strcpy(client.playerName, name);
}



/*
 * Calculates the map size (area), returns maximum possible map size (for memory safety), if calculation
 * obtains zero
 */
#ifdef MINISERVER_TEST
static int
getMapSize() 
{
    int mapsize = client.ncolsMap * client.nrowsMap;
    mapsize = (mapsize == 0) ? MAXIMUM_MAP_SIZE : mapsize;
    return mapsize;
}
#endif
/*
 * Runs a series of commands to stress test the client's message receive functionality (its primary tasks).
 * 
 * Note that this is not a comprehensive test of the message receive, but it verifies that the cleint is 
 * robust with respect to the most frequent ways in which it might receive malformed messages. 
 */
void
unitTest(const addr_t from)
{
    #ifdef UNIT_TEST // if we are in test mode
    fprintf(stderr, "Running test build\n"); // indicate that we are in test mode on log

    char command[500]; // adjust the size as needed according to testcommands.txt content

    // open the file
    FILE* testCommandsFile = fopen("testcommands.txt", "r");
    
    // ensure file opened successfully
    if (testCommandsFile == NULL) {
        fprintf(stderr, "Error opening testcommands.txt file, continuing to normal execution\n");
        return;
    }

    // read each line in the file
    while (fgets(command, sizeof(command), testCommandsFile)) {
        printf("\nSERVER COMMAND\n%s\nCLIENT OUTPUT\n", command);
        fflush(stdout);
        handleMessage(NULL, from, command);
        printf("\n");
        fflush(stdout);
    }

    // close the file
    fclose(testCommandsFile);

    // indicate tests over
    printf("TESTING COMPLETE\n");
    exit(0);
    #else
    fprintf(stderr, "Running release build\n"); // indicate that we are in release mode on log
    #endif
}
