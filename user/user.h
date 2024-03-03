#ifndef __USER_H
#define __USER_H

#include <stdlib.h>
#include <stdio.h>
#include <strings.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include "gamemap.h"

/*
 * char characterID;
 * map_t* map;
 * int gold;
 * char* realName;
*/
typedef struct user user_t;


user_t* user_new(char ID, GameMap_t* map, int gold, char* realName, int row, int col);
user_t* getUserByID(user_t* users[], char ID);
void user_move(user_t* user, char command, GameMap_t* map, user_t* users[]);
char getCharacterID(user_t* user);
void stealGold(user_t* user1, user_t* user2);
bool user_isActive(user_t* user);
void user_inActive(user_t* user);
int user_getGold(user_t* user);
void user_delete(user_t* user);
void moveDownRight(user_t* user, GameMap_t* map, user_t* users[]);
void moveDownLeft(user_t* user, GameMap_t* map, user_t* users[]);
void moveUpRight(user_t* user, GameMap_t* map, user_t* users[]);
void moveUpLeft(user_t* user, GameMap_t* map, user_t* users[]);
void moveUp(user_t* user, GameMap_t* map, user_t* users[]);
void moveDown(user_t* user, GameMap_t* map, user_t* users[]);
void moveLeft(user_t* user, GameMap_t* map, user_t* users[]);
void moveRight(user_t* user, GameMap_t* map, user_t* users[]);



#endif //__USER_H
