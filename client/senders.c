#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "message.h"
#include "clientdata.h"
#include "senders.h"
#include "graphics.h"

static void sendPlay(addr_t* serverp);
static void sendSpectate(addr_t* serverp);  

void
send_receipt(addr_t* serverp) 
{
    #ifdef MINISERVER_TEST
	if (client.state != CLIENT_PLAY && client.state != CLIENT_PRE_INIT) {
        message_send(*serverp, "RECEIVED");
    }
	#else
	;
	#endif
}

bool 
send_start(addr_t* serverp) 
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

void 
send_key(addr_t* serverp, char key) 
{
    if (client.state != CLIENT_PLAY) {
        return;
    }

    char message[6];
    snprintf(message, sizeof(message), "KEY %c", key);
    message_send(*serverp, message);
}

static void 
sendPlay(addr_t* serverp) 
{
    char message[MAXIMUM_NAME_LENGTH + 5];
    snprintf(message, sizeof(message), "PLAY %s", client.playerName);
    message_send(*serverp, message);
}

static void 
sendSpectate(addr_t* serverp) 
{
    message_send(*serverp, "SPECTATE");
}
