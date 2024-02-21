#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <strings.h>
#include "gamemap.h"
#include "user.h"

const int MaxNameLength = 50;   // max number of chars in playerName
const int MaxPlayers = 26;      // maximum number of players
const int GoldTotal = 250;      // amount of gold in the game
const int GoldMinNumPiles = 10; // minimum number of gold piles
const int GoldMaxNumPiles = 30; // maximum number of gold piles


int main(int argc, char* argv)
{
  user_t* playerList[MaxPlayers];
  int current = 0;
  bool ok = message_loop(NULL, 0, NULL, NULL, handleMessage);

    // shut down the message module
  message_done();     
 return ok? 0 : 1; // status code depends on result of message_loop
}


