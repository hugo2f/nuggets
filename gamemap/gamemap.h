/* 
 * GameMap_t.h - header file for playground.c
 * 
 * Wrapper functions for printing strings to stdout and stderr,
 * with and without newlines 
 *
 * Hugo Fang, 2/21/2024
 */

#ifndef __GameMap_H__
#define __GameMap_H__

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

/* Local types */
typedef struct GameMap GameMap_t;

// Getters
int getNumRows(GameMap_t* map);
int getNumCols(GameMap_t* map);
char** getGrid(GameMap_t* map);
char** getGameGrid(GameMap_t* map);

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
 * Set the type of cell at a coordinate
 *
 * Inputs:
 *   map to update
 *   type of cell to set to
 *   row, col: coordinates of target cell
 */
void setCellType(GameMap_t* map, char type, int row, int col);

/*
 * (For use after a player moves from a cell)
 * Restore a cell type to the map terrain at the cell
 *
 * Inputs:
 *   map to update
 *   row, col: coordinates of target cell
 */
void restoreCell(GameMap_t* map, int row, int col);

/*
 * For a given coordinate, get a boolean version of the map.
 * true means visible, false means not visible
 *
 * Inputs:
 *   map: GameMap_t*
 *   row, col: starting coordinate
 * 
 * Returns:
 *   2D nx2 array of visible coordinates, terminated by (-1, -1)
 * 
 * Caller needs to later call deleteGrid on the returned pointer
 */
int** getVisibleRegion(GameMap_t* map, int row, int col);

/*
 * Return an array of room cell ('.') coordinates
 *
 * Inputs:
 *   map: GameMap_t*
 * 
 * Returns:
 *   2D nx2 array of coordinates, terminated by (-1, -1)
 *   NULL if map is NULL or memory allocation error
 * 
 * Example:
    int** roomCells = getRoomCells(map);
    int size = 0;
    for (int row = 0; roomCells[row][0] != -1; row++) {
      printf("%d, %d\n", roomCells[row][0], roomCells[row][1]);
      size++;
    }
    delete2DIntArr(roomCells, size + 1); // +1 for the (-1, -1) row
 * 
 * Caller needs to later call deleteGrid on the returned pointer
 */
int** getRoomCells(GameMap_t* map);

/*
 * Equivalent of deleteGrid, but for a 2d int array.
 * Helper function to free the 2d array returned by getRoomCells
 * 
 * Inputs:
 *   arr to delete
 *   numRows in arr
 */
void delete2DIntArr(int** arr, int numRows);

/*
 * Print out a map (for testing)
 *
 * Inputs:
 *   map: GameMap_t*
 * 
 * Prints out "map is NULL" if a null pointer is passed in
 */
void printMap(GameMap_t* map);

/*
 * Convert a GameMap_t.gameGrid into a string. The string will
 * contain a newline after every row, with last row terminated by '\0'
 *
 * Inputs:
 *   map to convert with
 * 
 * Returns:
 *   char* string of converted gameGrid
 *   NULL if map is NULL
 * 
 * Caller needs to free the returned pointer
 */
char* gridToString(GameMap_t* map);

#endif // __GameMap_H__
