/* 
 * GameMap_t.h - header file for playground.c
 * 
 * Wrapper functions for printing strings to stdout and stderr,
 * with and without newlines 
 *
 * Hugo Fang, 2/21/2024
 */

#ifndef __GameMap_t_H__
#define __GameMap_t_H__

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

/* Local types */
typedef struct GameMap GameMap_t;

// Getters
int getNumRows(GameMap_t* map);
int getNumCols(GameMap_t* map);
char** getGrid(GameMap_t* map);

/*
 * Loads a map file into a GameMap_t
 * 
 * Inputs:
 *   mapFilePath: path to file to load
 * 
 * Returns:
 *   an initialized GameMap_t*
 *   NULL if errors reading mapFilePath or allocating memory
 * 
 * Caller needs to later GameMap_t_delete the returned pointer
 */
GameMap_t* loadMapFile(char* mapFilePath);

/*
 * Frees all memory allocated for a GameMap_t
 *
 * Inputs:
 *   map: the GameMap_t to delete 
 */
void deleteGameMap(GameMap_t* map);

/*
 * Frees all memory allocated for a 2D char array
 *
 * Inputs:
 *   grid: the 2D array to delete 
 *   numRows: number of rows in the grid
 */
void deleteGrid(char** grid, int numRows);

/*
 * Get the type of cell at a coordinate
 *
 * Inputs:
 *   map to get cell type from
 *   row, col: coordinates of target cell
 * 
 * Returns:
 *   the cell type
 *   '\0' if (row, col) is not in the map
 */
char getCellType(GameMap_t* map, int row, int col);

/*
 * For a given coordinate, get a boolean version of the map.
 * true means visible, false means not visible
 *
 * Inputs:
 *   map: GameMap_t*
 *   row, col: starting coordinate
 * 
 * Returns:
 *   2D numRows x numCols array of booleans
 * 
 * Caller needs to later call deleteGrid on the returned pointer
 */
bool** getVisibleRegion(GameMap_t* map, int row, int col);

/*
 * Print out a map (for testing)
 *
 * Inputs:
 *   map: GameMap_t*
 * 
 * Prints out "map is NULL" if a null pointer is passed in
 */
void printMap(GameMap_t* map);

#endif // __GameMap_t_H__
