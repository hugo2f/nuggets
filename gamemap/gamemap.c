/*
 * GameMap_T.c    Hugo Fang    2/21/2024
 * 
 * defines the struct and functions related to storing and processing the game map
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "file.h"

typedef struct GameMap {
    int numRows, numCols; // size of map
    char** grid; // terrain features
    char** gameGrid; // also stores players and gold 
} GameMap_T;

GameMap_T* loadMapFile(char* mapFilePath)
{
  FILE* fp = fopen(mapFilePath, "r");
  if (fp == NULL) {
    return NULL;
  }
  GameMap_T* map = malloc(sizeof(GameMap_T));
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
  return map;
}

void deleteGameMap_T(GameMap_T* map)
{
  if (map == NULL) {
    return;
  }

  deleteGrid(map->grid, map->numRows);
  deleteGrid(map->gameGrid, map->numRows);
  free(map);
}

void deleteGrid(char** grid, int numRows)
{
  if (grid == NULL) {
    return;
  }

  // free every row
  for (int row = 0; row < numRows; row++) {
    free(grid[row]);
  }
  // free pointer
  free(grid);
}

char getCellType(GameMap_T* map, int row, int col)
{
  if (row < 0 || row >= map->numRows || col < 0 || col >= map->numCols) {
    return NULL;
  }
  return map->gameGrid[row][col];
}

char** getPlayerMap()
{
  // TODO
  return NULL; 
}

char** getVisibleRegion(GameMap_T* map, int row, int col)
{
  // TODO
  return NULL;
}

char** getSpectatorMap(GameMap_T* map)
{
  if (map == NULL) {
    return NULL;
  }
  return map->gameGrid;
}

void movePlayer(GameMap_T* map)
{

}

void spawnGold(GameMap_T* map)
{
  
}