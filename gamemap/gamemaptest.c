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

  // Load maps and output
  GameMap_t* map = loadMapFile("../maps/big.txt");
  printf("big.txt: %dx%d\n", getNumRows(map), getNumCols(map));
  for (int row = 0; row < getNumRows(map); row++) {
    for (int col = 0; col < getNumCols(map); col++) {
      printf("%c", getGrid(map)[row][col]);
    }
    printf("\n");
  }
  deleteGameMap(map);

  map = loadMapFile("../maps/hole.txt");
  printf("hole.txt: %dx%d\n", getNumRows(map), getNumCols(map));
  for (int row = 0; row < getNumRows(map); row++) {
    for (int col = 0; col < getNumCols(map); col++) {
      printf("%c", getGrid(map)[row][col]);
    }
    printf("\n");
  }
  deleteGameMap(map);

  map = loadMapFile("../maps/main.txt");
  printf("main.txt: %dx%d\n", getNumRows(map), getNumCols(map));
  for (int row = 0; row < getNumRows(map); row++) {
    for (int col = 0; col < getNumCols(map); col++) {
      printf("%c", getGrid(map)[row][col]);
    }
    printf("\n");
  }
  deleteGameMap(map);
  return 0;
}