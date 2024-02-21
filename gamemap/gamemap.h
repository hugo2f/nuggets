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
#include <stdbool.h>

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
void deleteGameMap(GameMap* map);

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
 *   NULL if (row, col) is not in the map
 */
char getCellType(GameMap* map, int row, int col);

/*
 * Get the map that should be displayed to a player, which
 * consists of the regions of the map it has seen before,
 * and the region it currently sees, including other players
 * and gold that is visible.
 *
 * Inputs:
 *   TODO: player struct?
 * 
 * Returns:
 * 
 * 
 * Caller needs to call deleteGrid on the returned pointer
 */
char** getPlayerMap();

/*
 * For a given coordinate, get a boolean version of the map.
 * true means visible, false means not visible
 *
 * Inputs:
 *   map: GameMap*
 *   row, col: starting coordinate
 * 
 * Returns:
 *   2D numRows x numCols array of booleans
 * 
 * Caller needs to later call deleteGrid on the returned pointer
 */
bool** getVisibleRegion(GameMap* map, int row, int col);

/*
 * Returns an updated gameGrid
 *
 * Inputs:
 *   map: GameMap*
 * 
 * Returns:
 *   a 2D array containing the room features, players, and gold piles
 *   NULL if map is NULL
 */
char** getSpectatorMap(GameMap* map);

#endif // __GAMEMAP_H__
