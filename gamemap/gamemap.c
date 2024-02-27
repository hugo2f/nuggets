/*
 * GameMap_t.c    Hugo Fang    2/21/2024
 * 
 * defines the struct and functions related to storing and processing the game map
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "file.h"

/* Local types */
typedef struct GameMap {
  int numRows, numCols; // size of map
  char** grid; // terrain features
  char** gameGrid; // spectator view with players and gold
} GameMap_t;

/* Local consts */
// directions for changes in coordinates,
// starting with right and in clockwise order
static const int dr[] = {0, 1, 0, -1};
static const int dc[] = {1, 0, -1, 0};

// players can only see up to 5 units away, with diagonals
// counting as 1 unit
static const int sightRadius = 5;

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
void deleteGrid(char** grid, int numRows);
void delete2DIntArr(int** arr, int numRows);
int checkSquare(GameMap_t* map, int** visibleRegion, int idx,
                int row, int col, int radius);
static bool isVisible(GameMap_t* map, int r1, int c1, int r2, int c2);
static bool outOfMap(GameMap_t* map, int row, int col);
static bool isWall(char type);

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
    if (grid[row] != NULL) {
      free(grid[row]);
    }
  }
  // free grid pointer
  free(grid);
}

char getCellType(GameMap_t* map, int row, int col)
{
  if (outOfMap(map, row, col)) {
    return '\0';
  }
  return map->gameGrid[row][col];
}

void setCellType(GameMap_t* map, char type, int row, int col)
{
  // invalid coordinates
  if (outOfMap(map, row, col)) {
    return;
  }

  // can't change a wall or solid rock to another type
  char curType = map->grid[row][col];
  if (isWall(type) || curType == ' ') {
    return;
  }
  map->gameGrid[row][col] = type;
}

void restoreCell(GameMap_t* map, int row, int col)
{
  if (outOfMap(map, row, col)) {
    return;
  }
  map->gameGrid[row][col] = map->grid[row][col];
}

int** getVisibleRegion(GameMap_t* map, int row, int col)
{
  // parameter checks
  if (map == NULL || outOfMap(map, row, col)) {
    return NULL;
  }
  // a player would can only be in a room ('.') or passage cell ('#')
  char type = getCellType(map, row, col);
  if (type != '.' && type != '#') {
    return NULL;
  }

  int** visibleRegion = NULL;
  // at most map size x 2 if every cell is visible, +1 for (-1, -1)
  // to mark the end of the array
  int totalLength = map->numRows * map->numCols + 1;
  visibleRegion = malloc(totalLength * sizeof(int*));
  if (visibleRegion == NULL) {
    return NULL;
  }

  // TODO: delete after testing
  // // the player is always on visible spot
  // visibleRegion[0] = malloc(2 * sizeof(int));
  // if (visibleRegion[0] == NULL) {
  //   return NULL;
  // }
  // visibleRegion[0][0] = row;
  // visibleRegion[0][1] = col;
  int idx = 0; // start filling in later visible cells from 1
  int found = 1; // visible cells found in the previous checkSquare call
  // expand radius to check until no new visible cells are found
  for (int radius = 1; radius <= sightRadius && found > 0; radius++) {
    // check square around (row, col) of `radius`
    found = checkSquare(map, visibleRegion, idx, row, col, radius);
    // checkSquare returns -1 on memory allocation error
    if (found == -1) {
      delete2DIntArr(visibleRegion, totalLength);
    }
    idx += found;
  }

  visibleRegion[idx] = malloc(2 * sizeof(int));
  if (visibleRegion[idx] == NULL) {
    delete2DIntArr(visibleRegion, totalLength);
    return NULL;
  }
  visibleRegion[idx][0] = -1;
  visibleRegion[idx][1] = -1;
  return visibleRegion;
}

/*
 * Look for visible cells in a square around a cell, such that each cell
 * is at most `radius` cells away in either direction
 * 
 * Inputs:
 *   map to check in
 *   visibleRegion: 2d array to fill in with visible coordinates,
 *     terminated by (-1, -1)
 *   row, col: center of square
 *   length of square
 * 
 * Returns:
 *   number of visible cells found
 *   -1 if memory allocation error
 */
int checkSquare(GameMap_t* map, int** visibleRegion, int idx,
                int row, int col, int radius)
{
  if (map == NULL || visibleRegion == NULL || outOfMap(map, row, col)) {
    return 0;
  }

  int curIdx = idx;
  int curRow = row - radius, curCol = col - radius;
  int length = 2 * radius + 1;
  // printf("idx: %d, row: %d, col: %d, curRow: %d, curCol: %d, radius: %d\n",
  //        idx, row, col, curRow, curCol, radius);
  // go right, down, left, up, length - 1 steps each, to traverse the square
  // using `dr` and `dc`
  for (int d = 0; d < 4; d++) {
    for (int step = 0; step < length - 1; step++) {
      curRow += dr[d];
      curCol += dc[d];
      // printf("checking isVisible: %d, %d, %d, %d\n", row, col, curRow, curCol);
      if (isVisible(map, row, col, curRow, curCol)) {
        visibleRegion[curIdx] = malloc(2 * sizeof(int));
        if (visibleRegion[curIdx] == NULL) {
          return -1;
        }
        visibleRegion[curIdx][0] = curRow;
        visibleRegion[curIdx++][1] = curCol;
      }
    }
  }
  return curIdx - idx;

}

/*
 * Check if (r2, c2) is visible from (r1, c1). As per the requirements spec,
 * this is done by checking each intermediate row and col. An intermediate point
 * blocks vision if it intersects a gridpoint exactly and is not a room spot ('.'),
 * or if the line segment passes between a pair of map gridpoints, both of which
 * are not room spots.
 * 
 * Inputs:
 *   map to check in
 *   (r1, c1): starting cell
 *   (r2, c2): target cell
 * 
 * Returns:
 *   true if visible
 *   false otherwise
 */
bool isVisible(GameMap_t* map, int r1, int c1, int r2, int c2)
{
  if (map == NULL || outOfMap(map, r1, c1) || outOfMap(map, r2, c2)) {
    return false;
  }

  // TODO: delete prints after system testing
  // printf("start: %d, %d, end: %d, %d\n", r1, c1, r2, c2);
  int rowDiff = r2 - r1, colDiff = c2 - c1;
  int minRow, maxRow;
  if (r1 < r2) {
    minRow = r1;
    maxRow = r2;
  } else {
    minRow = r2;
    maxRow = r1;
  }
  // printf("intermediate rows\n");
  // for each intermediate row
  for (int row = minRow + 1; row < maxRow; row++) {
    // line between start and target cell intersects exactly on a col
    if (((row - r1) * colDiff) % rowDiff == 0) {
      int col = (row - r1) * colDiff / rowDiff + c1;
      // printf("exactly: %d, %d\n", row, col);
      if (map->grid[row][col] != '.') {
        return false;
      }
    } else { // line is between two columns
      // get col when the ray reaches `row`
      double col = (double) (row - r1) * colDiff / rowDiff + c1;
      // printf("not exactly: %d, %f\n", row, col);
      int colLeft = (int) col;
      if (map->grid[row][colLeft] != '.' && map->grid[row][colLeft + 1] != '.') {
        return false;
      }
    }
  }

  int minCol, maxCol;
  if (c1 < c2) {
    minCol = c1;
    maxCol = c2;
  } else {
    minCol = c2;
    maxCol = c1;
  }
  // printf("intermediate columns\n");
  // intermediate columns
  for (int col = minCol + 1; col < maxCol; col++) {
    if (((col - c1) * rowDiff) % colDiff == 0) {
      int row = (col - c1) * rowDiff / colDiff + r1;
      // printf("exactly: %d, %d\n", row, col);
      if (map->grid[row][col] != '.') {
        return false;
      }
    } else {
      double row = (double) (col - c1) * rowDiff / colDiff + r1;
      // printf("not exactly: %f, %d\n", row, col);
      int rowUp = (int) row;
      if (map->grid[rowUp][col] != '.' && map->grid[rowUp + 1][col] != '.') {
        return false;
      }
    }
  }
  // printf("can see: %d, %d\n", r2, c2);
  return true;
}

int** getRoomCells(GameMap_t* map)
{
  if (map == NULL) {
    return NULL;
  }

  // at most map size x 2 if every cell is room cell, +1 for (-1, -1)
  // to mark the end of the array
  int totalLength = map->numRows * map->numCols + 1;
  int** res = malloc(totalLength * sizeof(int*));
  if (res == NULL) {
    return NULL;
  }

  int idx = 0;

  // loop through the room
  for (int row = 0; row < map->numRows; row++) {
    for (int col = 0; col < map->numCols; col++) {
      if (map->grid[row][col] == '.') {
        // each row has two ints for (row, col)
        res[idx] = malloc(2 * sizeof(int));
        if (res[idx] == NULL) {
          delete2DIntArr(res, totalLength);
          return NULL;
        }
        res[idx][0] = row;
        res[idx++][1] = col;
      }
    }
  }
  res[idx] = malloc(2 * sizeof(int));
  if (res[idx] == NULL) {
    delete2DIntArr(res, totalLength);
    return NULL;
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
    // break on first NULL since arr is filled from lower indices
    // getting a NULL means a memory allocation error led to calling
    // delete2DIntArr before the arr was properly filled, in which case
    // the arr should be terminated with (-1, -1) and numRows should end
    // before NULLs
    if (arr[row] == NULL) {
      break;
    }
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
      printf("%c", map->gameGrid[row][col]);
    }
    printf("\n");
  }
}

/*
 * Helper function to validate a coordinate in a map
 *
 * Inputs:
 *   map to validate with
 *   row, col: coordinates
 * 
 * Returns:
 *   true if outside map
 *   false if in map
 */
bool outOfMap(GameMap_t* map, int row, int col)
{
  return (row < 0 || row >= map->numRows || col < 0 || col >= map->numCols);
}

bool isWall(char type)
{
  return (type == '+' || type == '-' || type == '|');
}