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

void send_receipt(addr_t* serverp); 

void send_start(addr_t* serverp); 

void send_key(addr_t* serverp, char key); 

#endif

