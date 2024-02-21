#include <stdlib.h>
#include <stdio.h>
#include <strings.h>
#include <string.h>
#include <stdbool.h>
#include "gamemap.h" 

typedef struct user {
  char characterID;
  GameMap* map;
  int gold;
  char* realName;
} user_t;

user_t* createUser(char ID, GameMap* map, int gold, char* realName)
{
  user_t* user = malloc(sizeof(user_t));
  if(user != NULL){
    user->characterID = ID;
    user->map = map;
    user->gold = gold;
    user->realName = realName;
    return user;
  }
  return NULL;
}
