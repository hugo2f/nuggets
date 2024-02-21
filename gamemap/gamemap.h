/* 
 * gamemap.h - header file for playground.c
 * 
 * Wrapper functions for printing strings to stdout and stderr,
 * with and without newlines 
 *
 * Hugo Fang, 2/21/2024
 */

#ifndef __GAMEMAP_H__
#define __GAMEMAP_H__

#include <stdio.h>
#include <stdlib.h>

/* Local types */
typedef struct GameMap;

/*
 * Loads a map file into a GameMap
 * 
 * Inputs:
 *   mapFilePath: path to file to load
 * 
 * Returns:
 *   an initialized GameMap*
 *   NULL if errors reading mapFilePath or allocating memory
 * 
 * Caller needs to later GameMap_delete the returned pointer
 */
GameMap* loadMapFile(char* mapFilePath);

/*
 * Frees all memory allocated for a GameMap
 *
 * Inputs:
 *   map: the GameMap to delete 
 */
void GameMap_delete(GameMap* map);


#endif // __GAMEMAP_H__
