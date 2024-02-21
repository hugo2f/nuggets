/*
 * gamemap.c    Hugo Fang    2/21/2024
 * 
 * defines the struct and functions related to storing and processing the game map
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "file.h"

typedef struct {
    int numRows, numCols; // size of map
    char** grid; // terrain features
    char** gameGrid; // also stores players and gold 
} GameMap;


GameMap* loadMapFile(char* mapFilePath)
{
  FILE* fp = fopen(mapFilePath, "r");
  if (fp == NULL) {
    return NULL;
  }
  GameMap* map = malloc(sizeof(GameMap));
  if (map == NULL) {
    fclose(fp);
    return NULL;
  }

  // initialize map variables
  int numRows = file_numLines(fp);
  int numCols = strlen(file_readLine(fp));
  rewind(fp);

  map->numRows = numRows;
  map->numCols = numCols;

  // malloc grid and gameGrid
  map->grid = malloc(numRows * sizeof(char*));
  if (map->grid == NULL) {
    return NULL;
  }
  map->gameGrid = malloc(numRows * sizeof(char*));
  if (map->gameGrid == NULL) {
    return NULL;
  }

  for (int row = 0; row < numRows; row++) {
    char* line = file_readLine(fp);
    map->grid[row] = malloc(numCols * sizeof(char));
    if (map->grid[row] == NULL) {
      return NULL;
    }
    map->gameGrid[row] = malloc(numCols * sizeof(char));
    if (map->gameGrid[row] == NULL) {
      return NULL;
    }

    for (int col = 0; col < numCols; col++) {
      // when loading a file, grid and gameGrid are the same
      // after the game starts, only gameGrid stores the players and gold
      map->grid[row][col] = line[col];
      map->gameGrid[row][col] = line[col];
    }
    free(line);
  }
  fclose(fp);
}

void GameMap_delete(GameMap* map)
{
  if (map == NULL) {
    return;
  }
  
  for (int row = 0; row < map->numRows; row++) {
    free(map->grid[row]);
    free(map->gameGrid[row]);
  }
  free(map->grid);
  free(map->gameGrid);
  free(map);
}
