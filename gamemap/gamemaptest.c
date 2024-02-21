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

  // Invalid map
  GameMap_t* map = loadMapFile("../maps/asdf.txt");
  printf("asdf.txt: %dx%d\n", getNumRows(map), getNumCols(map));
  printMap(map);
  deleteGameMap(map);

  map = loadMapFile("../maps/hole.txt");
  printf("hole.txt: %dx%d\n", getNumRows(map), getNumCols(map));
  printMap(map);
  deleteGameMap(map);

  map = loadMapFile("../maps/main.txt");
  printf("main.txt: %dx%d\n", getNumRows(map), getNumCols(map));
  printMap(map);
  deleteGameMap(map);

  //
  return 0;
}