/*
 * senders.c
 *
 * Description: contains functions that send commands, namely PLAY, SPECTATE, KEY, and RECEIPT.
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
#include "clientdata.h"
#include "senders.h"

// function prototypes
static void sendPlay(addr_t* serverp);
static void sendSpectate(addr_t* serverp);  

/*
 * Sends "RECEIVED" to server; see .h for more details. 
 */
void
send_receipt(addr_t* serverp) 
{  
    // we only require this method when testing against the miniserver, thus this method uses
    // a preprocessor directive such that the client can be compiled for miniserver testing or as the final
    // product
    #ifdef MINISERVER_TEST
	// we only send receipts during initialization
    if (client.state != PLAY && client.state != PRE_INIT) {
        message_send(*serverp, "RECEIVED");
    }
	#else
	;
	#endif /* MINISERVER_TEST */
}

/*
 * Sends different start message to server depending on client type; see .h for more details. 
 */
void 
send_start(addr_t* serverp) 
{
    // ensures that we are not sending start a second time
    if (client.state != PRE_INIT) {
        fprintf(stderr, "Sent START again\n");
        return;
    }

    // sends start message to server depending on client type
    if (client.playerName == NULL) {
        sendSpectate(serverp);
    } else {
        sendPlay(serverp);
    }

    // advance client state
    client.state = START_SENT;
}

/*
 * Sends key to server; see .h for more details. 
 */
void
send_key(addr_t* serverp, char key) 
{
    // ensures client is currently running a game session
    if (client.state != PLAY) {
        return;
    }

    // create key send message
    char message[6];
    snprintf(message, sizeof(message), "KEY %c", key);
    
    // send message to server
    message_send(*serverp, message);
}

/*
 * Sends play message to server (the player start message); see .h for more details. 
 */
static void 
sendPlay(addr_t* serverp) 
{
    // check to make sure playerName is not NULL
    if (client.playerName == NULL) {
        fprintf(stderr, "Cannot send NULL playerName");
        return;
    }
    
    // check to make sure playerName is not too large
    int playerNameLength = strlen(client.playerName);
    if (playerNameLength > MAXIMUM_NAME_LENGTH) {
        fprintf(stderr, "Player name '%s' is too long", client.playerName);
        return;
    }

    // create PLAY message
    char message[MAXIMUM_NAME_LENGTH + 10];
    snprintf(message, sizeof(message), "PLAY %s", client.playerName);
    
    // send message to server
    message_send(*serverp, message);
}

/*
 * Sends spectate message to server (the spectator start message); see .h for more details. 
 */
static void 
sendSpectate(addr_t* serverp) 
{
    // send spectator start message to server
    message_send(*serverp, "SPECTATE");
}
