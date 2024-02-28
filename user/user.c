#include <stdlib.h>
#include <stdio.h>
#include <strings.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include "gamemap.h" 
#include "user.h"


typedef struct user {
  char characterID;
  GameMap_t* map;
  int gold;
  char* realName;
  int row;
  int col;
  bool active;
} user_t;
/*
static void moveLeft(user_t*);
static void moveRight(user_t*);
static void moveUp(user_t*);
static void moveDown(user_t*);
*/
user_t* user_new(char ID, GameMap_t* map, int gold, char* realName, int row, int col)
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

user_t* getUserByID(user_t* users[], char ID)
{
  char index = ID - 'A';
  int index2 = index - '0';
  user_t* returner = users[index2];
  if(returner != NULL){
    return returner;
  }
  return NULL;
}

void stealGold(user_t* user1, user_t* user2)
{
  int value = user2->gold;
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
  user1->gold = user1->gold + stolen;
  user2->gold = user2->gold - stolen;
}


void user_move(user_t* user, char command, GameMap_t* map, user_t* users[])
{
  if(user != NULL){
    if(user->active){
      if(command == 'h'){
        moveLeft(user, map, users);
      }
      else if(command == 'l'){
        moveRight(user, map, users);
      }
      else if(command == 'j'){
        moveDown(user, map, users);
      }
      else if(command == 'k'){
        moveUp(user, map, users);
      }
      else if(command == 'y'){
        moveUpLeft(user, map, users);
      }
      else if(command == 'u'){
        moveUpRight(user, map, users);
      }
      else if(command == 'b'){
        moveDownLeft(user, map, users);
      }
      else if(command == 'n'){
        moveDownRight(user, map, users);
      }
    }
  }
} 

      
char getCharacterID(user_t* user)
{
  return user->characterID;
}

bool user_isActive(user_t* user)
{
  return user->active;
}

void user_inActive(user_t* user)
{
  user->active = false;
}

int user_getGold(user_t* user)
{
  return user->gold;
}

void user_delete(user_t* user)
{
  if(user!= NULL){  
    free(user);
  }
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

//TODO add in correct numerical value instead of 225. Add in gold + player swap functionality
static void moveRight(user_t* user, GameMap_t* grid)
{
  if(user != NULL && ((user->col)+1) <= 50 ){
    if(getCellType(grid, user->row, user->col +1) == '#' ||getCellType(grid, user->row, user->col +1) == '.'  ||getCellType(grid, user->row, user->col +1)  == '*' ){
      setCellType(grid, user->characterID, user->row, (user->col)+1);
      retoreCell(grid, user->row, (user->col));
      user->col = user->col + 1;
    }
  }
}

static void moveLeft(user_t* user, GameMap_t* grid)
{
  if(user != NULL && ((user->col)-1) >= 0 ){
    if(getCellType(grid, user->row, user->col - 1) == '#' ||getCellType(grid, user->row, user->col - 1) == '.'  ||getCellType(grid, user->row, user->col - 1)  == '*' ){
      setCellType(grid, user->characterID, user->row, (user->col)-1);
      restoreCell(grid, user->row, (user->col));
      user->col = user->col - 1;
    }
  }
}

static void moveUp(user_t* user, GameMap_t* grid)
{
  if(user != NULL && ((user->row)-1) >= 0 ){
    if(getCellType(grid, user->row-1, user->col) == '#' ||getCellType(grid, user->row-1, user->col) == '.'  ||getCellType(grid, user->row-1, user->col)  == '*' ){
      setCellType(grid, user->characterID, user->row -1 , (user->col));
      restoreCell(grid, user->row, (user->col));
      user->row = user->row - 1;
    }
  }
}

static void moveDown(user_t* user, GameMap_t* grid)
{
  if(user != NULL && ((user->row)+1) <= 50 ){
    if(getCellType(grid, user->row+1, user->col) == '#' ||getCellType(grid, user->row+1, user->col) == '.'  ||getCellType(grid, user->row+1, user->col)  == '*' ){
      setCellType(grid, user->characterID, user->row + 1 , (user->col));
      restoreCell(grid, user->row, (user->col));
      user->row = user->row + 1;
    }
  }
}



static void moveUpLeft(user_t* user, GameMap* map)
{
  if(user != NULL && ((user->row)-1)>=0 && (user->col) - 1 >=0){
    if(user_isActive(user)){
      if(getCellType(grid, user->row -1, user->col -1) == '#' ||getCellType(grid, user->row -1, user->col -1) == '.'  ||getCellType(grid, user->row -1, user->col -1 )  == '*' ){
        setCellType(grid, user->characterID, user->row - 1 , (user->col)-1);
        restoreCell(grid, user->row, (user->col));
        user->row = user->row - 1;
        user->col - user->col - 1;
      }
    }
  }
}
static void moveUpRight(user_t* user, GameMap* map)
{
  if(user != NULL && ((user->row)-1)>=0 && (user->col) - 1 >=0){
    if(user_isActive(user)){
      if(getCellType(grid, user->row -1, user->col -1) == '#' ||getCellType(grid, user->row -1, user->col -1) == '.'  ||getCellType(grid, user->row -1, user->col -1 )  == '*' ){
        setCellType(grid, user->characterID, user->row - 1 , (user->col)-1);
        restoreCell(grid, user->row, (user->col));
        user->row = user->row - 1;
        user->col - user->col - 1;
      }
    }
  }
}
static void moveUpLeft(user_t* user, GameMap* map)
{
  if(user != NULL && ((user->row)-1)>=0 && (user->col) - 1 >=0){
    if(user_isActive(user)){
      if(getCellType(grid, user->row -1, user->col -1) == '#' ||getCellType(grid, user->row -1, user->col -1) == '.'  ||getCellType(grid, user->row -1, user->col -1 )  == '*' ){
        setCellType(grid, user->characterID, user->row - 1 , (user->col)-1);
        restoreCell(grid, user->row, (user->col));
        user->row = user->row - 1;
        user->col - user->col - 1;
      }
    }
  }
}
static void moveUpLeft(user_t* user, GameMap* map)
{
  if(user != NULL && ((user->row)-1)>=0 && (user->col) - 1 >=0){
    if(user_isActive(user)){
      if(getCellType(grid, user->row -1, user->col -1) == '#' ||getCellType(grid, user->row -1, user->col -1) == '.'  ||getCellType(grid, user->row -1, user->col -1 )  == '*' ){
        setCellType(grid, user->characterID, user->row - 1 , (user->col)-1);
        restoreCell(grid, user->row, (user->col));
        user->row = user->row - 1;
        user->col - user->col - 1;
      }
    }
  }
}
static void moveDownLeft(user_t* user, GameMap* map)
{
  if(user != NULL && ((user->row)+1)<=50 && (user->col) - 1 >=0){
    if(user_isActive(user)){
      if(getCellType(grid, user->row +1, user->col -1) == '#' ||getCellType(grid, user->row +1, user->col -1) == '.'  ||getCellType(grid, user->row +1, user->col -1 )  == '*' ){
        setCellType(grid, user->characterID, user->row + 1 , (user->col)-1);
        restoreCell(grid, user->row, user->col);
        user->row = user->row + 1;
        user->col - user->col - 1;
      }
    }
  }
} 
*/
void moveDownRight(user_t* user, GameMap_t* grid, user_t* users[])
{
  if(user != NULL && ((user->row)+1) <= getNumRows(grid) && ((user->col) +1 ) <= getNumCols(grid)){
    if(user_isActive(user)){
      if(getCellType(grid, user->row +1, user->col +1) == '#' ||
          getCellType(grid, user->row +1, user->col +1) == '.'){
        setCellType(grid, user->characterID, user->row +1, user->col +1);
        restoreCell(grid, user->row, user->col);
        user->row = user->row +1;
        user->col = user->col +1;
      }
      else if(isalpha(getCellType(grid, user->row +1, user->col+1))){
        user_t* user2 = getUserByID(users, getCellType(grid, user->row +1, user->col +1));
        int tempRow = user->row;
        int tempCol = user->col;
        user->row = user2->row;
        user->col = user2->col;
        user2->row = tempRow;
        user2->col = tempCol;
        setCellType(grid, user->characterID, user->row, (user->col));
        setCellType(grid, user2->characterID, user2->row, (user2->col));
        stealGold(user, user2);
      }
      else if(getCellType(grid, user->row +1, user-> col+1) == '*'){
        setCellType(grid, user->characterID, user->row+1, user->col +1);
        restoreCell(grid, user->row, user->col);
        user->row = user->row+1;
        user->col = user->col +1;
        user->gold = user->gold +1;
      }
    }
  }
}


void moveDownLeft(user_t* user, GameMap_t* grid, user_t* users[])
{
  if(user != NULL && ((user->row)+1) <= getNumRows(grid) && ((user->col) -1 ) >=0){
    if(user_isActive(user)){
      if(getCellType(grid, user->row +1, user->col -1) == '#' ||
          getCellType(grid, user->row +1, user->col -1) == '.'){
        setCellType(grid, user->characterID, user->row +1, user->col -1);
        restoreCell(grid, user->row, user->col);
        user->row = user->row +1;
        user->col = user->col -1;
      }
      else if(isalpha(getCellType(grid, user->row +1, user->col-1))){
        user_t* user2 = getUserByID(users, getCellType(grid, user->row +1, user->col -1));
        int tempRow = user->row;
        int tempCol = user->col;
        user->row = user2->row;
        user->col = user2->col;
        user2->row = tempRow;
        user2->col = tempCol;
        setCellType(grid, user->characterID, user->row  , (user->col));
        setCellType(grid, user2->characterID, user2->row  , (user2->col));
        stealGold(user, user2);
      }
      else if(getCellType(grid, user->row +1, user-> col-1) == '*'){
        setCellType(grid, user->characterID, user->row+1, user->col -1);
        restoreCell(grid, user->row, user->col);
        user->row = user->row+1;
        user->col = user->col -1;
        user->gold = user->gold +1;
      }
    }
  }
}

void moveUpRight(user_t* user, GameMap_t* grid, user_t* users[])
{
  if(user != NULL && ((user->row)-1) >=0 && ((user->col) +1 ) <= getNumCols(grid)){
    if(user_isActive(user)){
      if(getCellType(grid, user->row -1, user->col +1) == '#' ||
          getCellType(grid, user->row -1, user->col +1) == '.'){
        setCellType(grid, user->characterID, user->row -1, user->col +1);
        restoreCell(grid, user->row, user->col);
        user->row = user->row -1;
        user->col = user->col +1;
      }
      else if(isalpha(getCellType(grid, user->row -1, user->col+1))){
        user_t* user2 = getUserByID(users, getCellType(grid, user->row - 1, user->col +1));
        int tempRow = user->row;
        int tempCol = user->col;
        user->row = user2->row;
        user->col = user2->col;
        user2->row = tempRow;
        user2->col = tempCol;
        setCellType(grid, user->characterID, user->row , (user->col));
        setCellType(grid, user2->characterID, user2->row , (user2->col));
        stealGold(user, user2);
      }
      else if(getCellType(grid, user->row -1, user-> col+1) == '*'){
        setCellType(grid, user->characterID, user->row-1, user->col +1);
        restoreCell(grid, user->row, user->col);
        user->row = user->row-1;
        user->col = user->col +1;
        user->gold = user->gold +1;
      }
    }
  }
}

void moveUpLeft(user_t* user, GameMap_t* grid, user_t* users[])
{
  if(user != NULL && ((user->row)-1) >=0 && ((user->col) -1 ) >=0){
    if(user_isActive(user)){
      if(getCellType(grid, user->row -1, user->col -1) == '#' ||
          getCellType(grid, user->row -1, user->col -1) == '.'){
        setCellType(grid, user->characterID, user->row -1, user->col -1);
        restoreCell(grid, user->row, user->col);
        user->row = user->row -1;
        user->col = user->col -1;
      }
      else if(isalpha(getCellType(grid, user->row -1, user->col-1))){
        user_t* user2 = getUserByID(users, getCellType(grid, user->row - 1, user->col -1));
        int tempRow = user->row;
        int tempCol = user->col;
        user->row = user2->row;
        user->col = user2->col;
        user2->row = tempRow;
        user2->col = tempCol;
        setCellType(grid, user->characterID, user->row, (user->col));
        setCellType(grid, user2->characterID, user2->row , (user2->col));
        stealGold(user, user2);
      }
      else if(getCellType(grid, user->row -1, user-> col-1) == '*'){
        setCellType(grid, user->characterID, user->row-1, user->col -1);
        restoreCell(grid, user->row, user->col);
        user->row = user->row-1;
        user->col = user->col -1;
        user->gold = user->gold +1;
      }
    }
  }
} 


void moveUp(user_t* user, GameMap_t* grid, user_t* users[])
{
  if(user != NULL && ((user->row)-1) >=0){
    if(user_isActive(user)){
      if(getCellType(grid, user->row -1, user->col) == '#' ||
          getCellType(grid, user->row -1, user->col) == '.'){
        setCellType(grid, user->characterID, user->row -1, user->col);
        restoreCell(grid, user->row, user->col);
        user->row = user->row -1;
      }
      else if(isalpha(getCellType(grid, user->row -1, user->col))){
        user_t* user2 = getUserByID(users, getCellType(grid, user->row - 1, user->col));
        int tempRow = user->row;
        user->row = user2->row;
        user2->row = tempRow;
        setCellType(grid, user->characterID, user->row , (user->col));
        setCellType(grid, user2->characterID, user2->row , (user2->col));
        stealGold(user, user2);
      }
      else if(getCellType(grid, user->row -1, user-> col) == '*'){
        setCellType(grid, user->characterID, user->row-1, user->col);
        restoreCell(grid, user->row, user->col);
        user->row = user->row-1;
        user->gold = user->gold +1;
      }
    }
  }
}

void moveDown(user_t* user, GameMap_t* grid, user_t* users[])
{
  if(user != NULL && ((user->row)+1) <= getNumRows(grid)){
    if(user_isActive(user)){
      if(getCellType(grid, user->row +1, user->col) == '#' ||
          getCellType(grid, user->row +1, user->col) == '.'){
        setCellType(grid, user->characterID, user->row +1, user->col);
        restoreCell(grid, user->row, user->col);
        user->row = user->row +1;
      }
      else if(isalpha(getCellType(grid, user->row +1, user->col))){
        user_t* user2 = getUserByID(users, getCellType(grid, user->row + 1, user->col));
        int tempRow = user->row;
        user->row = user2->row;
        user2->row = tempRow;
        setCellType(grid, user->characterID, user->row , (user->col));
        setCellType(grid, user2->characterID, user2->row , (user2->col));
        stealGold(user, user2);
      }
      else if(getCellType(grid, user->row +1, user-> col) == '*'){
        setCellType(grid, user->characterID, user->row+1, user->col);
        restoreCell(grid, user->row, user->col);
        user->row = user->row+1;
        user->gold = user->gold +1;
      }
    }
  }
}
void moveLeft(user_t* user, GameMap_t* grid, user_t* users[])
{
  if(user != NULL && ((user->col)-1) >=0){
    if(user_isActive(user)){
      if(getCellType(grid, user->row, user->col-1) == '#' ||
          getCellType(grid, user->row, user->col-1) == '.'){
        setCellType(grid, user->characterID, user->row, user->col-1);
        restoreCell(grid, user->row, user->col);
        user->col = user->col -1;
      }
      else if(isalpha(getCellType(grid, user->row, user->col-1))){
        user_t* user2 = getUserByID(users, getCellType(grid, user->row, user->col-1));
        int tempRow = user->col;
        user->col = user2->col;
        user2->col = tempRow;
        setCellType(grid, user->characterID, user->row , (user->col));
        setCellType(grid, user2->characterID, user2->row , (user2->col));
        stealGold(user, user2);
      }
      else if(getCellType(grid, user->row, user->col-1) == '*'){
        setCellType(grid, user->characterID, user->row, user->col-1);
        restoreCell(grid, user->row, user->col);
        user->col = user->col-1;
        user->gold = user->gold +1;
      }
    }
  }
}
void moveRight(user_t* user, GameMap_t* grid, user_t* users[])
{
  if(user != NULL && ((user->col)+1) <=getNumCols(grid)){
    if(user_isActive(user)){
      if(getCellType(grid, user->row, user->col+1) == '#' ||
          getCellType(grid, user->row, user->col+1) == '.'){
        setCellType(grid, user->characterID, user->row , user->col+1);
        restoreCell(grid, user->row, user->col);
        user->row = user->col +1;
      }
      else if(isalpha(getCellType(grid, user->row , user->col+1))){
        user_t* user2 = getUserByID(users, getCellType(grid, user->row , user->col+1));
        int tempRow = user->col;
        user->col = user2->col;
        user2->col = tempRow;
        setCellType(grid, user->characterID, user->row , (user->col));
        setCellType(grid, user2->characterID, user2->row , (user2->col));
        stealGold(user, user2);
      }
      else if(getCellType(grid, user->row, user-> col+1) == '*'){
        setCellType(grid, user->characterID, user->row, user->col+1);
        restoreCell(grid, user->row, user->col);
        user->col = user->col+1;
        user->gold = user->gold +1;
      }
    }
  }
}



