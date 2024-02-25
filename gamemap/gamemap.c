/*
 * GameMap_t.c    Hugo Fang    2/21/2024
 * 
 * defines the struct and functions related to storing and processing the game map
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "file.h"

/* Local types */
typedef struct GameMap {
  int numRows, numCols; // size of map
  char** grid; // terrain features
  char** gameGrid; // spectator view with players and gold
} GameMap_t;

/* Local functions */

// Getters
int getNumRows(GameMap_t* map)
{
  if (map == NULL) {
    return 0;
  }
  return map->numRows;
}

int getNumCols(GameMap_t* map)
{
  if (map == NULL) {
    return 0;
  }
  return map->numCols;
}

char** getGrid(GameMap_t* map)
{
  if (map == NULL) {
    return 0;
  }
  return map->grid;
}

char** getGameGrid(GameMap_t* map)
{
  if (map == NULL) {
    return 0;
  }
  return map->gameGrid;
}

// Helper functions
static void deleteGrid(char** grid, int numRows);
void delete2DIntArr(int** arr, int numRows);

GameMap_t* loadMapFile(char* mapFilePath)
{
  FILE* fp = fopen(mapFilePath, "r");
  if (fp == NULL) {
    return NULL;
  }
  GameMap_t* map = malloc(sizeof(GameMap_t));
  if (map == NULL) {
    fclose(fp);
    return NULL;
  }

  // initialize map variables
  int numRows = file_numLines(fp);
  char* line = file_readLine(fp);
  int numCols = strlen(line);
  free(line);
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
      break;
    }
    map->gameGrid[row] = malloc(numCols * sizeof(char));
    if (map->gameGrid[row] == NULL) {
      break;
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

void deleteGameMap(GameMap_t* map)
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
  // free grid pointer
  free(grid);
}

char getCellType(GameMap_t* map, int row, int col)
{
  if (row < 0 || row >= map->numRows || col < 0 || col >= map->numCols) {
    return '\0';
  }
  return map->grid[row][col];
}

void setCellType(GameMap_t* map, char type, int row, int col)
{
  if (row < 0 || row >= map->numRows || col < 0 || col >= map->numCols) {
    return;
  }
  map->gameGrid[row][col] = type;
}

void restoreCell(GameMap_t* map, int row, int col)
{
  if (row < 0 || row >= map->numRows || col < 0 || col >= map->numCols) {
    return;
  }
  map->gameGrid[row][col] = map->grid[row][col];
}

int** getVisibleRegion(GameMap_t* map, int row, int col)
{
  // TODO
  return NULL;
}

int** getRoomCells(GameMap_t* map)
{
  if (map == NULL) {
    return NULL;
  }

  // at most room size x 2 if every cell is room cell, +1 for (-1, -1)
  // to mark the end of the array
  int totalLength = map->numRows * map->numCols + 1;
  int** res = malloc(totalLength * sizeof(int*));
  if (res == NULL) {
    return NULL;
  }

  int idx = 0;
  // each row will have 2 ints
  res[0] = malloc(2 * sizeof(int));
  if (res[0] == NULL) {
    return NULL;
  }

  // loop through the room
  for (int row = 0; row < map->numRows; row++) {
    for (int col = 0; col < map->numCols; col++) {
      if (map->grid[row][col] == '.') {
        res[idx][0] = row;
        res[idx++][1] = col;

        // malloc the next row
        res[idx] = malloc(2 * sizeof(int));
        if (res[idx] == NULL) {
          delete2DIntArr(res, totalLength * sizeof(int*));
          return NULL;
        }
      }
    }
  }
  res[idx][0] = -1;
  res[idx][1] = -1;
  return res;
}

void delete2DIntArr(int** arr, int numRows)
{
  if (arr == NULL) {
    return;
  }
  for (int row = 0; row < numRows; row++) {
    free(arr[row]);
  }
  free(arr);
}

void printMap(GameMap_t* map)
{
  if (map == NULL) {
    printf("map is NULL\n");
    return;
  }

  for (int row = 0; row < map->numRows; row++) {
    for (int col = 0; col < map->numCols; col++) {
      printf("%c", map->grid[row][col]);
    }
    printf("\n");
  }
}

char* gridToString(GameMap_t* map)
{
  if (map == NULL) {
    return NULL;
  }

  int numRows = map->numRows, numCols = map->numCols;
  int totalLength = numRows * (numCols + 1);
  char* res = malloc(totalLength * sizeof(char));
  if (res == NULL) {
    return NULL;
  }

  int curIndex = 0;
  for (int row = 0; row < numRows; row++) {
    strncpy(&res[curIndex], map->gameGrid[row], numCols);
    curIndex += numCols;
    res[curIndex++] = '\n';
  }
  res[totalLength - 1] = '\0';
  return res;
}