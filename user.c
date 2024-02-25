#include <stdlib.h>
#include <stdio.h>
#include <strings.h>
#include <string.h>
#include <stdbool.h>
#include "gamemap.h" 
#include "user.h"


typedef struct user {
  char characterID;
  GameMap* map;
  int gold;
  char* realName;
  int row;
  int col;
  bool active;
} user_t;

static void moveLeft(user_t*);
static void moveRight(user_t*);
static void moveUp(user_t*);
static void moveDown(user_t*);

user_t* user_new(char ID, GameMap* map, int gold, char* realName, int row, int col)
{
  user_t* user = malloc(sizeof(user_t));
  if(user != NULL){
    user->characterID = ID;
    user->map = map;
    user->gold = gold;
    user->realName = realName;
    user->row = row;
    user->col = col;
    user->active = true;
    return user;
  }
  return NULL;
}

void user_move(user_t* user, char command)
{
  if(user != NULL){
    if(user->active){
      if(command == 'h'){
        moveLeft(user);
      }
      else if(command == 'l'){
        moveRight(user);
      }
      else if(command == 'j'){
        moveDown(user);
      }
      else if(command == 'k'){
        moveUp(user);
      }
      else if(command == 'y'){
        moveUp(user);
        moveLeft(user);
      }
      else if(command == 'u'){
        moveUp(user);
        moveRight(user);
      }
      else if(command == 'b'){
        moveDown(user);
        moveLeft(user);
      }
      else if(command == 'n'){
        moveDown(user);
        moveRight(user);
      }
    }
  }
}      
      
char getCharacterID(user)
{
  return user->characterID;
}
/*
static void moveLeft(user_t* user, GameMap_t* grid)
{
  if(user != NULL && ((user->col)-1)>=0){
    if(grid[user->row][(user->col)-1] == '#' || grid[user->row][(user->col)-1] == '*' || grid[user->row][(user->col)-1] == '.'){
      grid[user->row][(user->col)-1] = user->characterID;
      user->col = user->col - 1;
    }
  }
}
*/
//TODO add in correct numerical value instead of 225. Add in gold + player swap functionality
static void moveRight(user_t* user, GameMap_t* grid)
{
  if(user != NULL && ((user->col)+1) <= 50 ){
    if(getCellType(grid, user->row, user->col +1) == '#' ||getCellType(grid, user->row, user->col +1) == '.'  ||getCellType(grid, user->row, user->col +1)  == '*' ){
      setCellType(grid, user->characterID, user->row, (user->col)+1);
      setCellType(grid, '.', user->row, (user->col));
      user->col = user->col + 1;
    }
  }
}

static void moveLeft(user_t* user, GameMap_t* grid)
{
  if(user != NULL && ((user->col)-1) >= 0 ){
    if(getCellType(grid, user->row, user->col - 1) == '#' ||getCellType(grid, user->row, user->col - 1) == '.'  ||getCellType(grid, user->row, user->col - 1)  == '*' ){
      setCellType(grid, user->characterID, user->row, (user->col)-1);
      setCellType(grid, '.', user->row, (user->col));
      user->col = user->col - 1;
    }
  }
}

static void moveUp(user_t* user, GameMap_t* grid)
{
  if(user != NULL && ((user->row)-1) >= 0 ){
    if(getCellType(grid, user->row-1, user->col) == '#' ||getCellType(grid, user->row-1, user->col) == '.'  ||getCellType(grid, user->row-1, user->col)  == '*' ){
      setCellType(grid, user->characterID, user->row -1 , (user->col));
      setCellType(grid, '.', user->row, (user->col));
      user->row = user->row - 1;
    }
  }
}

static void moveDown(user_t* user, GameMap_t* grid)
{
  if(user != NULL && ((user->row)+1) <= 50 ){
    if(getCellType(grid, user->row+1, user->col) == '#' ||getCellType(grid, user->row+1, user->col) == '.'  ||getCellType(grid, user->row+1, user->col)  == '*' ){
      setCellType(grid, user->characterID, user->row + 1 , (user->col));
      setCellType(grid, '.', user->row, (user->col));
      user->row = user->row + 1;
    }
  }
}



static void moveUp(user_t* user, GameMap* map);
static void moveDown(user_t* user, GameMap* map);
