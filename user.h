#ifndef __USER_H
#define __USER_H

#include <stdlib.h>
#include <stdio.h>
#include <strings.h>
#include <string.h>
#include <stdbool.h>
#include "gamemap.h"

/*
 * char characterID;
 * map_t* map;
 * int gold;
 * char* realName;
*/
typedef struct user user_t;


user_t* user_new(char ID, GameMap* map, int gold, char* realName, int row, int col);
user_t* getUserByID(user_t* user[], char ID);
void user_move(user_t* user, char command);
char getCharacterID(user_t* user);
void stealGold(user_t* user1, user_t* user2);
bool user_isActive(user_t* user);
void user_inActive(user_t* user);
int user_getGold(user_t* user);
void user_delete(user_t* user);
void moveDownRight(user_t* user, GameMap* map, user_t* user[]);
void moveDownLeft(user_t* user, GameMap* map, user_t* user[]);
void moveUpRight(user_t* user, GameMap* map, user_t* user[]);
void moveUpLeft(user_t* user, GameMap* map, user_t* user[]);
void moveUp(user_t* user, GameMap* map, user_t* user[]);
void moveDown(user_t* user, GameMap* map, user_t* user[]);
void moveLeft(user_t* user, GameMap* map, user_t* user[]);
void moveRight(user_t* user, GameMap* map, user_t* user[]);



#endif //__USER_H
