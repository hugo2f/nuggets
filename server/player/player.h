/*
 * Header file for the player module 
 */

#ifndef __PLAYER_H__
#define __PLAYER_H__

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct player player_t;

/*
 * Creates a new player with specified information
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
 * Returns a player based on their ID
 */
player_t* getPlayerByID(player_t** players, char ID);

/*
 * Returns whether a player is active or not
 */

bool getPlayerActive(player_t* player);

/*
 * Returns the "stealMessage"
 * This is used because when a player steals gold from another player,
 * We also need to send that update to the spectator, which is accessed in the server
 * The server uses this function to get the message to send to the spectator
 */
char* getStealMessage(player_t* player);
/*
 * Set's a player's status to inactive
 */
void setPlayerInactive(player_t* player);

/*
 * Update's a player's banner to reflect changes in gold
 */

void updateGoldDisplay(player_t* player, int pileAmount, int goldRemaining);

/*
 * Adds gold to a player
 */

void addGold(player_t* player, int amount);

/*
 * Method for when one player steals gold from another
 */

void stealGold(player_t* player1, player_t* player2, int goldRemaining);

/*
 * Update's a player's position and vision on the map
 */

void updatePlayerPosition(player_t* player);

/*
 * Returns a player's character ID 
 */

char getCharacterID(player_t* player);

/*
 * Moves a player down and right
 * Returns 0 if regular movement, 1 if the player stepped on gold,
 * 2 if a player stole gold, 3 if we hit a wall 
 * (this is for move as far as you can movement)
 */

int moveDownRight(player_t* player, player_t** players, int goldRemaining);

/*
 * Moves a player down and left
 * Returns 0 if regular movement, 1 if the player stepped on gold,
 * 2 if a player stole gold, 3 if we hit a wall 
 * (this is for move as far as you can movement)
 */

int moveDownLeft(player_t* player, player_t** players, int goldRemaining);

/*
 * Moves a player up and right
 * Returns 0 if regular movement, 1 if the player stepped on gold,
 * 2 if a player stole gold, 3 if we hit a wall 
 * (this is for move as far as you can movement)
 */

int moveUpRight(player_t* player, player_t** players, int goldRemaining);

/*
 * Moves a player up and right
 * Returns 0 if regular movement, 1 if the player stepped on gold,
 * 2 if a player stole gold, 3 if we hit a wall 
 * (this is for move as far as you can movement)
 */

int moveUpLeft(player_t* player, player_t** players, int goldRemaining);

/*
 * Moves a player up
 * Returns 0 if regular movement, 1 if the player stepped on gold,
 * 2 if a player stole gold, 3 if we hit a wall 
 * (this is for move as far as you can movement)
 */

int moveUp(player_t* player, player_t** players, int goldRemaining);

/*
 * Moves a player down
 * Returns 0 if regular movement, 1 if the player stepped on gold,
 * 2 if a player stole gold, 3 if we hit a wall 
 * (this is for move as far as you can movement)
 */

int moveDown(player_t* player, player_t** players, int goldRemaining);

/*
 * Moves a player left
 * Returns 0 if regular movement, 1 if the player stepped on gold,
 * 2 if a player stole gold, 3 if we hit a wall 
 * (this is for move as far as you can movement)
 */

int moveLeft(player_t* player, player_t** players, int goldRemaining);

/*
 * Moves a player right
 * Returns 0 if regular movement, 1 if the player stepped on gold,
 * 2 if a player stole gold, 3 if we hit a wall 
 * (this is for move as far as you can movement)
 */

int moveRight(player_t* player, player_t** players, int goldRemaining);


#endif // __GameMap_H__