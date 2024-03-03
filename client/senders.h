/*
 * senders.h
 *
 * Description: contains interface for sender functions.
 * 
 * Author: Joseph Hirsh
 * Date: March 2st, 2024
 * 
 */

#ifndef _SENDERS_H_
#define _SENDERS_H_

#include <stdbool.h>
#include "message.h"

/*
 * Only runs during client initialization phase such that the miniserver can send many messages in a
 * row without waiting for player input. The preprocessor directives allow the client to compile with
 * and without this functionality. Compile with this functionality when testing against miniserver, 
 * otherwise compile without. 
 *
 * Sends "RECEIPT". 
 *
 * Requires serverp and returns void
 */
void send_receipt(addr_t* serverp); 

/*
 * Sends "PLAY [playerName]" or "SPECTATE" according to client type and advances client state. 
 *
 * Requires serverp and returns void
 */
void send_start(addr_t* serverp); 

/*
 * Sends "KEY [keystroke]". 
 *
 * Requires serverp and returns void
 */
void send_key(addr_t* serverp, char key); 

#endif

