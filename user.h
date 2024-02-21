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


user_t* createUser(char ID, GameMap* map, int gold, char* realName, int row, int column);

#endif //__USER_H
