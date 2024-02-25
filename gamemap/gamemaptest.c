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

#include "gamemap.h"

int main(const int argc, char* argv[])
{
  if (argc != 1) {
    fprintf(stderr, "Usage: %s\n", argv[0]);
    return 1;
  }

  /* Load maps and output */

  // Invalid map
  GameMap_t* map = loadMapFile("../maps/asdf.txt");
  printf("asdf.txt: %dx%d\n", getNumRows(map), getNumCols(map));
  printMap(map);
  deleteGameMap(map);

  // Valid maps
  map = loadMapFile("../maps/hole.txt");
  printf("hole.txt: %dx%d\n", getNumRows(map), getNumCols(map));
  printMap(map);
  printf("----------\n");
  char* gridString = gridToString(map);
  printf("%s\n", gridString);
  free(gridString);
  deleteGameMap(map);
  printf("----------\n");

  map = loadMapFile("../maps/main.txt");
  printf("main.txt: %dx%d\n", getNumRows(map), getNumCols(map));
  printMap(map);
  printf("----------\n");
  gridString = gridToString(map);
  printf("%s\n", gridString);
  free(gridString);
  deleteGameMap(map);
  printf("----------\n");

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
  int** visibleRegion = getVisibleRegion(map, 5, 5);
  int size = 0;
  for (int row = 0; visibleRegion[row][0] != -1; row++) {
    printf("%d, %d\n", visibleRegion[row][0], visibleRegion[row][1]);
    size++;
  }
  delete2DIntArr(visibleRegion, size + 1); // +1 for the last (-1, -1) row

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