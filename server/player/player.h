#ifndef __PLAYER_H__
#define __PLAYER_H__

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct player player_t;

/*
 * 
 */
player_t* player_new(char ID, GameMap_t* map, char** grid, int gold, char* name, int row, int col, addr_t playerAddress);

/*
 * Deletes the contents of a player, frees the player struct itself
 */
void player_delete(player_t* player);

/*
 * Return player row
 */
int getPlayerRow(player_t* player);

/*
 * Return player col
 */
int getPlayerCol(player_t* player);

/*
 * Returns the map of a player
 */
char** getPlayerMap(player_t* player);

/*
 * Returns the name of a player
 */
char* getPlayerName(player_t* player);

/*
 * Returns the amount of gold that a player has 
 */

int getPlayerGold(player_t* player);
/*
 * Returns the address of a player
 */
addr_t getPlayerAddress(player_t* player);
/*
 * 
 */
player_t* getPlayerByID(player_t** players, char ID);
/*
 
 */

bool getPlayerActive(player_t* player);

/*
 
 */
void setPlayerInactive(player_t* player);

/*
 
 */
void addGold(player_t* player, int amount);
/*
 
 */
void stealGold(player_t* player1, player_t* player2);

/*
 *
 */
void updatePlayerPosition(player_t* player);
/*
 * 
 */
char getCharacterID(player_t* player);
/*
 * 
 */

int moveDownRight(player_t* player, player_t** players);
/*
 * 
 */
int moveDownLeft(player_t* player, player_t** players);
/*
 * 
 */
int moveUpRight(player_t* player, player_t** players);
/*
 * 
 */
int moveUpLeft(player_t* player, player_t** players);
/*
 * 
 */
int moveUp(player_t* player, player_t** players);
/*
 * 
 */
int moveDown(player_t* player, player_t** players);
/*
 * 
 */
int moveLeft(player_t* player, player_t** players);
/*
 * 
 */
int moveRight(player_t* player, player_t** players);


#endif // __GameMap_H__