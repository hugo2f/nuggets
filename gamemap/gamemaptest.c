/*
 * gamemaptest.c    Hugo Fang    2/21/2024
 * 
 * defines unit tests for the gamemap module
 * 
 * Usage:
 *   ./gamemaptest
 * 
 * Errors:
 *   errno 1 if arguments are provided
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "gamemap.h"

void displayVisible(GameMap_t* map, int** visibleRegion, int startRow, int startCol);

int main(const int argc, char* argv[])
{
  if (argc != 1) {
    fprintf(stderr, "Usage: %s\n", argv[0]);
    return 1;
  }

  GameMap_t* map = NULL;

  /* Load maps and output with printGrid vs. gridToString */

  // // Invalid map
  // map = loadMapFile("../maps/asdf.txt");
  // printf("asdf.txt: %dx%d\n", getNumRows(map), getNumCols(map));
  // printMap(map);
  // deleteGameMap(map);

  // // Valid maps
  // map = loadMapFile("../maps/hole.txt");
  // printf("hole.txt: %dx%d\n", getNumRows(map), getNumCols(map));
  // printMap(map);
  // printf("----------\n");
  // char* gridString = gridToString(map);
  // printf("%s\n", gridString);
  // free(gridString);
  // deleteGameMap(map);
  // printf("----------\n");

  // map = loadMapFile("../maps/main.txt");
  // printf("main.txt: %dx%d\n", getNumRows(map), getNumCols(map));
  // printMap(map);
  // printf("----------\n");
  // gridString = gridToString(map);
  // printf("%s\n", gridString);
  // free(gridString);
  // deleteGameMap(map);
  // printf("----------\n");

  // /* get/set/restoreCellType */

  // map = loadMapFile("../maps/fewspots.txt");
  // printf("\nTesting get/set/restoreCellType on fewspots.txt\n");
  // printMap(map);

  // // get outside of map
  // char type = getCellType(map, -1, 5);
  // if (type == '\0') {
  //   printf("getting (-1, 5) returned '\\0'\n");
  // }

  // type = getCellType(map, 5, -1);
  // if (type == '\0') {
  //   printf("getting (5, -1) returned '\\0'\n");
  // }

  // type = getCellType(map, 100, 5);
  // if (type == '\0') {
  //   printf("getting (100, 5) returned '\\0'\n");
  // }

  // type = getCellType(map, 5, 100);
  // if (type == '\0') {
  //   printf("getting (5, 100) returned '\\0'\n");
  // }

  // // get inside of map
  // type = getCellType(map, 5, 5);
  // printf("getting (5, 5) returned '%c'\n", type);

  // type = getCellType(map, 7, 10);
  // printf("getting (10, 20) returned '%c'\n", type);

  // // set outside of map
  // setCellType(map, '@', -1, -1);
  // setCellType(map, '@', 100, 100);

  // // try to set wall cell (shouldn't do anything)
  // setCellType(map, 'A', 7, 10);

  // // set valid cell
  // setCellType(map, 'B', 8, 10);
  // setCellType(map, 'C', 9, 9);

  // printf("\nMap after setting:\n");
  // printMap(map);

  // // restore outside of map
  // restoreCell(map, -1, -1);
  // restoreCell(map, 100, 100);


  // // restore inside of map
  // restoreCell(map, 8, 10);
  // restoreCell(map, 9, 9);

  // printf("\nMap after restoring:\n");
  // printMap(map);
  // deleteGameMap(map);

  /* getVisibleRegion */
  map = loadMapFile("../maps/hole.txt");
  // map = loadMapFile("./temporary.txt");
  printf("\ngetVisibleRegion on hole.txt (%dx%d)\n", getNumRows(map), getNumCols(map));
  printMap(map);

  while (1) {
    int row, col;
    if (scanf("%d %d", &row, &col) == EOF) {
      break;
    }
    int** visibleRegion = getVisibleRegion(map, row, col);
    displayVisible(map, visibleRegion, row, col);
  }
  deleteGameMap(map);

  /* getRoomCells */
  // int** roomCells = getRoomCells(map);
  // size = 0;
  // for (int row = 0; roomCells[row][0] != -1; row++) {
  //   printf("%d, %d\n", roomCells[row][0], roomCells[row][1]);
  //   size++;
  // }
  // delete2DIntArr(roomCells, size + 1); // +1 for the last (-1, -1) row

  return 0;
}

void displayVisible(GameMap_t* map, int** visibleRegion, int startRow, int startCol)
{
  if (visibleRegion == NULL) {
    printf("nothing found\n");
    return;
  }
  int size = 0;
  int numRows = getNumRows(map), numCols = getNumCols(map);
  char** grid = malloc(numRows * sizeof(char*));
  if (grid == NULL) {
    return;
  }

  for (int row = 0; row < numRows; row++) {
    grid[row] = malloc(numCols * sizeof(char));
    memset(grid[row], ' ', numCols);
  }

  for (int row = 0; visibleRegion[row][0] != -1; row++) {
    int visibleRow = visibleRegion[row][0], visibleCol = visibleRegion[row][1];
    grid[visibleRow][visibleCol] = getCellType(map, visibleRow, visibleCol);
    size++;
  }
  
  grid[startRow][startCol] = '@';
  printf("Found %d visible cells\n", size);
  for (int row = 0; row < numRows; row++) {
    for (int col = 0; col < numCols; col++) {
      printf("%c", grid[row][col]);
    }
    printf("\n");
  }
  deleteGrid(grid, numRows);
  delete2DIntArr(visibleRegion, size + 1); // +1 for the last (-1, -1) row
}