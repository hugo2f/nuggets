#include <stdlib.h>
#include <stdio.h>
#include <strings.h>
#include <string.h>
#include <stdbool.h>
#include "../../support/message.h"
#include "../../gamemap/gamemap.h"
#include "player.h"
#include <ctype.h>

typedef struct player {
  char characterID;
  GameMap_t* gameMap; //store a pointer to the map object of the entire game
  char** playerMap;
  int gold;
  char* name;
  int row;
  int col;
  addr_t playerAddress;
  bool active;
} player_t;

//function prototypes
player_t* player_new(char ID, GameMap_t* map, char** grid, int gold, char* name, int row, int col, addr_t address);
void player_delete(player_t* player);
player_t* getPlayerByID(player_t** players, char ID);
char* getPlayerName(player_t* player);
int getPlayerGold(player_t* player);
char** getPlayerMap(player_t* player);
int getPlayerRow(player_t* player);
int getPlayerCol(player_t* player);
addr_t getPlayerAddress(player_t* player);
bool getPlayerActive();
void setPlayerInactive(player_t* player);
void addGold(player_t* player, int amount);
void stealGold(player_t* player1, player_t* player2);
void updatePlayerPosition(player_t* player);
char getCharacterID(player_t* player);
int moveDownRight(player_t* player, player_t** players);
int moveDownLeft(player_t* player, player_t** players);
int moveUpRight(player_t* player, player_t** players);
int moveDownRight(player_t* player, player_t** players);
int moveUp(player_t* player, player_t** players);
int moveDown(player_t* player, player_t** players);
int moveLeft(player_t* player, player_t** players);
int moveRight(player_t* player, player_t** players);

/*
 * Initializes a player and their data
 */
player_t* player_new(char ID, GameMap_t* map, char** grid, int gold, char* name, int row, int col, addr_t address)
{
  player_t* player = malloc(sizeof(player_t));
  if (player == NULL) {
    return NULL;
  }
  player->characterID = ID;
  player->gameMap = map;
  player->playerMap = grid;
  player->gold = gold;
  player->name = name;
  player->row = row;
  player->col = col;
  player->playerAddress = address;
  player->active = true;
  return player;
}

/*
 * Deletes a player
 */
void player_delete(player_t* player) 
{
  if (player->name != NULL) {
    free(player->name);
    player->name = NULL;
  }
  if (player->playerMap != NULL) {
    deleteGrid(player->playerMap, getNumRows(player->gameMap));
    player->playerMap = NULL;
  }
  free(player);
}

/*
 * Return the player's name
 */
char* 
getPlayerName(player_t* player) 
{
  return player->name;  
}

/*
 * Returns the amount of gold that a player has 
 */
int
getPlayerGold(player_t* player)
{
  return player->gold;
}

/*
 * Returns the address of a player
 */
addr_t
getPlayerAddress(player_t* player) 
{
  return player->playerAddress;
}

/*
 * Return a player's map
 */
char**
getPlayerMap(player_t* player) 
{
  return player->playerMap;
}

/*
 * Return the row of a player
 */
int
getPlayerRow(player_t* player) 
{
  return player->row;
}

/*
 * Return the col of a player
 */
int
getPlayerCol(player_t* player) 
{
  return player->col;
}

/*
 * Returns a player based on their ID
 */
player_t* getPlayerByID(player_t** players, char ID)
{
  char index = ID - 'A';
  player_t* returner = players[(int)index];
  if(returner != NULL){
    return returner;
  }
  return NULL;
}

/*
 * Returns whether a player is active or not 
 */

bool
getPlayerActive(player_t* player) 
{
  return player->active;  
}

/*
 * Add gold to a player's amount
 */
void 
addGold(player_t* player, int amount)
{
  player->gold += amount;
}

void stealGold(player_t* player1, player_t* player2)
{
  int value = player2->gold;
  int stolen = 0;
  if(value > 20){
    stolen = value - 10;
  }
  else if(value > 10){
    stolen = value - 5;
  }
  else if(value > 5){
    stolen = value - 2;
  }
  else if(value >=1){
    stolen = value;
  }
  player1->gold = player1->gold + stolen;
  player2->gold = player2->gold - stolen;
}

/*
 * Updates a player's visibility and position
 */
void
updatePlayerPosition(player_t* player) 
{
  //set the visible region

  if (player == NULL) {
    return;
  }
  
  char** grid = player->playerMap;
  int numRows = getNumRows(player->gameMap);
  int numCols = getNumCols(player->gameMap);

  int playerRow = player->row;
  int playerCol = player->col;

  for (int row = 0; row < numRows; row++) {
    for (int col = 0; col < numCols; col++) {
      if (grid[row][col] != ' ') {
        grid[row][col] = getCellTerrain(player->gameMap, row, col);
      }
    }
  }

  int** visibleRegion = getVisibleRegion(player->gameMap, playerRow, playerCol);
  if (visibleRegion == NULL) {
    printf("can't move there\n");
    return;
  }
  int size = 0;
  for (int row = 0; visibleRegion[row][0] != -1; row++) {
    int visibleRow = visibleRegion[row][0];
    int visibleCol = visibleRegion[row][1];
    grid[visibleRow][visibleCol] = getCellType(player->gameMap, visibleRow, visibleCol);
    size++;
  }
  //set the player's location
  grid[playerRow][playerCol] = '@';
  delete2DIntArr(visibleRegion, size+1);
}
      
char getCharacterID(player_t* player)
{
  return player->characterID;
}

void setPlayerInactive(player_t* player)
{
  player->active = false;
}

/*
 * Moves a player down and right
 * Returns 0 if regular movement, 1 if the player stepped on gold,
 * 2 if we hit a wall (this is for move as far as you can movement)
 */

int moveDownRight(player_t* player, player_t** players)
{
  int flag = 0;
  if(player != NULL && !isWall(getCellType(player->gameMap, player->row+1, player->col+1))) {
    if(getPlayerActive(player)){
      if(getCellType(player->gameMap, player->row +1, player->col +1) == '#' ||
          getCellType(player->gameMap, player->row +1, player->col +1) == '.'){
        setCellType(player->gameMap, player->characterID, player->row +1, player->col +1);
        restoreCell(player->gameMap, player->row, player->col);
        player->row = player->row +1;
        player->col = player->col +1;
      }
      else if(isalpha(getCellType(player->gameMap, player->row +1, player->col+1))){
        player_t* player2 = getPlayerByID(players, getCellType(player->gameMap, player->row +1, player->col +1));
        int tempRow = player->row;
        int tempCol = player->col;
        player->row = player2->row;
        player->col = player2->col;
        player2->row = tempRow;
        player2->col = tempCol;
        setCellType(player->gameMap, player->characterID, player->row, (player->col));
        setCellType(player->gameMap, player2->characterID, player2->row, (player2->col));
        stealGold(player, player2);
      }
      else if(getCellType(player->gameMap, player->row +1, player-> col+1) == '*'){
        setCellType(player->gameMap, player->characterID, player->row+1, player->col +1);
        restoreCell(player->gameMap, player->row, player->col);
        player->row = player->row+1;
        player->col = player->col +1;
        flag = 1;
      }
    }
    updatePlayerPosition(player);
    return flag;
  }
  return 2;
}

/*
 * Moves a player down and left
 * Returns 0 if regular movement, 1 if the player stepped on gold,
 * 2 if we hit a wall (this is for move as far as you can movement)
 */

int moveDownLeft(player_t* player, player_t** players)
{
  int flag = 0;
  if(player != NULL && !isWall(getCellType(player->gameMap, player->row+1, player->col-1))){
    if(getPlayerActive(player)){
      if(getCellType(player->gameMap, player->row +1, player->col -1) == '#' ||
          getCellType(player->gameMap, player->row +1, player->col -1) == '.'){
        setCellType(player->gameMap, player->characterID, player->row +1, player->col -1);
        restoreCell(player->gameMap, player->row, player->col);
        player->row = player->row +1;
        player->col = player->col -1;
      }
      else if(isalpha(getCellType(player->gameMap, player->row +1, player->col-1))){
        player_t* player2 = getPlayerByID(players, getCellType(player->gameMap, player->row +1, player->col -1));
        int tempRow = player->row;
        int tempCol = player->col;
        player->row = player2->row;
        player->col = player2->col;
        player2->row = tempRow;
        player2->col = tempCol;
        setCellType(player->gameMap, player->characterID, player->row  , (player->col));
        setCellType(player->gameMap, player2->characterID, player2->row  , (player2->col));
        stealGold(player, player2);
      }
      else if(getCellType(player->gameMap, player->row +1, player-> col-1) == '*'){
        setCellType(player->gameMap, player->characterID, player->row+1, player->col -1);
        restoreCell(player->gameMap, player->row, player->col);
        player->row = player->row+1;
        player->col = player->col -1;
        flag = 1;
      }
    }
    updatePlayerPosition(player);
    return flag;
  }
  return 2;
}

/*
 * Moves a player up and right
 * Returns 0 if regular movement, 1 if the player stepped on gold,
 * 2 if we hit a wall (this is for move as far as you can movement)
 */

int moveUpRight(player_t* player, player_t** players)
{
  int flag = 0;
  if(player != NULL && !isWall(getCellType(player->gameMap, player->row-1, player->col+1))){
    if(getPlayerActive(player)){
      if(getCellType(player->gameMap, player->row -1, player->col +1) == '#' ||
          getCellType(player->gameMap, player->row -1, player->col +1) == '.'){
        setCellType(player->gameMap, player->characterID, player->row -1, player->col +1);
        restoreCell(player->gameMap, player->row, player->col);
        player->row = player->row -1;
        player->col = player->col +1;
      }
      else if(isalpha(getCellType(player->gameMap, player->row -1, player->col+1))){
        player_t* player2 = getPlayerByID(players, getCellType(player->gameMap, player->row - 1, player->col +1));
        int tempRow = player->row;
        int tempCol = player->col;
        player->row = player2->row;
        player->col = player2->col;
        player2->row = tempRow;
        player2->col = tempCol;
        setCellType(player->gameMap, player->characterID, player->row , (player->col));
        setCellType(player->gameMap, player2->characterID, player2->row , (player2->col));
        stealGold(player, player2);
      }
      else if(getCellType(player->gameMap, player->row -1, player-> col+1) == '*'){
        setCellType(player->gameMap, player->characterID, player->row-1, player->col +1);
        restoreCell(player->gameMap, player->row, player->col);
        player->row = player->row-1;
        player->col = player->col +1;
        flag = 1;
      }
    }
    updatePlayerPosition(player);
    return flag;
  }
  return 2;
} 

/*
 * Moves a player up and left
 * Returns 0 if regular movement, 1 if the player stepped on gold,
 * 2 if we hit a wall (this is for move as far as you can movement)
 */

int moveUpLeft(player_t* player, player_t** players)
{
  int flag = 0;
  if(player != NULL && !isWall(getCellType(player->gameMap, player->row-1, player->col-1))){
    if(getPlayerActive(player)){
      if(getCellType(player->gameMap, player->row -1, player->col -1) == '#' ||
          getCellType(player->gameMap, player->row -1, player->col -1) == '.'){
        setCellType(player->gameMap, player->characterID, player->row -1, player->col -1);
        restoreCell(player->gameMap, player->row, player->col);
        player->row = player->row -1;
        player->col = player->col -1;
      }
      else if(isalpha(getCellType(player->gameMap, player->row -1, player->col-1))){
        player_t* player2 = getPlayerByID(players, getCellType(player->gameMap, player->row - 1, player->col -1));
        int tempRow = player->row;
        int tempCol = player->col;
        player->row = player2->row;
        player->col = player2->col;
        player2->row = tempRow;
        player2->col = tempCol;
        setCellType(player->gameMap, player->characterID, player->row, (player->col));
        setCellType(player->gameMap, player2->characterID, player2->row , (player2->col));
        stealGold(player, player2);
      }
      else if(getCellType(player->gameMap, player->row -1, player-> col-1) == '*'){
        setCellType(player->gameMap, player->characterID, player->row-1, player->col -1);
        restoreCell(player->gameMap, player->row, player->col);
        player->row = player->row-1;
        player->col = player->col -1;
        flag = 1;
      }
    }
    updatePlayerPosition(player);
    return flag;
  }
  return 2;
} 

/*
 * Moves a player up
 * Returns 0 if regular movement, 1 if the player stepped on gold,
 * 2 if we hit a wall (this is for move as far as you can movement)
 */

int moveUp(player_t* player, player_t** players)
{
  int flag = 0;
  if(player != NULL && !isWall(getCellType(player->gameMap, player->row-1, player->col))){
    if(getPlayerActive(player)){
      if(getCellType(player->gameMap, player->row -1, player->col) == '#' ||
          getCellType(player->gameMap, player->row -1, player->col) == '.'){
        setCellType(player->gameMap, player->characterID, player->row -1, player->col);
        restoreCell(player->gameMap, player->row, player->col);
        player->row = player->row -1;
      }
      else if(isalpha(getCellType(player->gameMap, player->row -1, player->col))){
        player_t* player2 = getPlayerByID(players, getCellType(player->gameMap, player->row - 1, player->col));
        int tempRow = player->row;
        player->row = player2->row;
        player2->row = tempRow;
        setCellType(player->gameMap, player->characterID, player->row , (player->col));
        setCellType(player->gameMap, player2->characterID, player2->row , (player2->col));
        stealGold(player, player2);
      }
      else if(getCellType(player->gameMap, player->row -1, player-> col) == '*'){
        setCellType(player->gameMap, player->characterID, player->row-1, player->col);
        restoreCell(player->gameMap, player->row, player->col);
        player->row = player->row-1;
        flag = 1;
      } 
    }
    updatePlayerPosition(player);
    return flag;
  }
  return 2;
}

/*
 * Moves a player down
 * Returns 0 if regular movement, 1 if the player stepped on gold,
 * 2 if we hit a wall (this is for move as far as you can movement)
 */

int moveDown(player_t* player, player_t** players)
{
  int flag = 0;
  if(player != NULL && !isWall(getCellType(player->gameMap, player->row+1, player->col))){
    if(getPlayerActive(player)){
      if(getCellType(player->gameMap, player->row +1, player->col) == '#' ||
          getCellType(player->gameMap, player->row +1, player->col) == '.'){
        setCellType(player->gameMap, player->characterID, player->row +1, player->col);
        restoreCell(player->gameMap, player->row, player->col);
        player->row = player->row +1;
      }
      else if(isalpha(getCellType(player->gameMap, player->row +1, player->col))){
        player_t* player2 = getPlayerByID(players, getCellType(player->gameMap, player->row + 1, player->col));
        int tempRow = player->row;
        player->row = player2->row;
        player2->row = tempRow;
        setCellType(player->gameMap, player->characterID, player->row , (player->col));
        setCellType(player->gameMap, player2->characterID, player2->row , (player2->col));
        stealGold(player, player2);
      }
      else if(getCellType(player->gameMap, player->row +1, player-> col) == '*'){
        setCellType(player->gameMap, player->characterID, player->row+1, player->col);
        restoreCell(player->gameMap, player->row, player->col);
        player->row = player->row+1;
        flag = 1;
      } 
    }
    updatePlayerPosition(player);
    return flag;
  }
  return 2;
}

/*
 * Moves a player left
 * Returns 0 if regular movement, 1 if the player stepped on gold,
 * 2 if we hit a wall (this is for move as far as you can movement)
 */

int moveLeft(player_t* player, player_t** players)
{
  int flag = 0;
  if(player != NULL && !isWall(getCellType(player->gameMap, player->row, player->col-1))) {
    if(getPlayerActive(player)){
      if(getCellType(player->gameMap, player->row, player->col-1) == '#' ||
          getCellType(player->gameMap, player->row, player->col-1) == '.'){
        setCellType(player->gameMap, player->characterID, player->row, player->col-1);
        restoreCell(player->gameMap, player->row, player->col);
        player->col = player->col - 1;
      }
      else if(isalpha(getCellType(player->gameMap, player->row, player->col-1))){
        player_t* player2 = getPlayerByID(players, getCellType(player->gameMap, player->row, player->col-1));
        int tempRow = player->col;
        player->col = player2->col;
        player2->col = tempRow;
        setCellType(player->gameMap, player->characterID, player->row , (player->col));
        setCellType(player->gameMap, player2->characterID, player2->row , (player2->col));
        stealGold(player, player2);
      }
      else if(getCellType(player->gameMap, player->row, player->col-1) == '*'){
        setCellType(player->gameMap, player->characterID, player->row, player->col-1);
        restoreCell(player->gameMap, player->row, player->col);
        player->col = player->col-1;
        flag = 1;
      } 
    }
    updatePlayerPosition(player);
    return flag;
  }
  return 2;
}

/*
 * Moves a player right
 * Returns 0 if regular movement, 1 if the player stepped on gold,
 * 2 if we hit a wall (this is for move as far as you can movement)
 */

int moveRight(player_t* player, player_t* players[])
{
  int flag = 0;
  if(player != NULL && !isWall(getCellType(player->gameMap, player->row, player->col+1))) {
    if(getPlayerActive(player)){
      if(getCellType(player->gameMap, player->row, player->col+1) == '#' ||
          getCellType(player->gameMap, player->row, player->col+1) == '.'){
        setCellType(player->gameMap, player->characterID, player->row , player->col+1);
        restoreCell(player->gameMap, player->row, player->col);
        player->col = player->col + 1;
      }
      else if(isalpha(getCellType(player->gameMap, player->row , player->col+1))){
        player_t* player2 = getPlayerByID(players, getCellType(player->gameMap, player->row , player->col+1));
        int tempRow = player->col;
        player->col = player2->col;
        player2->col = tempRow;
        setCellType(player->gameMap, player->characterID, player->row , (player->col));
        setCellType(player->gameMap, player2->characterID, player2->row , (player2->col));
        stealGold(player, player2);
      }
      else if(getCellType(player->gameMap, player->row, player-> col+1) == '*'){
        setCellType(player->gameMap, player->characterID, player->row, player->col+1);
        restoreCell(player->gameMap, player->row, player->col);
        player->col = player->col+1;
        flag = 1;
      } 
    }
    updatePlayerPosition(player);
    return flag;
  }
  return 2;
}