# CS50 Nuggets
## Implementation Spec
### team temporary, winter, 2024

According to the [Requirements Spec](REQUIREMENTS.md), the Nuggets game requires two standalone programs: a client and a server.
Our design also includes a `map` module.
We describe each program and module separately.
We do not describe the `support` library nor the modules that enable features that go beyond the spec.
We avoid repeating information that is provided in the requirements spec.

## Plan for division of labor

* Joe - Client
* Hugo - Game Map
* Jaysen - Server
* Colin - Player


### Data structures

> For each new data structure, describe it briefly and provide a code block listing the `struct` definition(s).
> No need to provide `struct` for existing CS50 data structures like `hashtable`.

### Definition of function prototypes

> For function, provide a brief description and then a code block with its function prototype.
> For example:

A function to parse the command-line arguments, initialize the game struct, initialize the message module, and (BEYOND SPEC) initialize analytics module.

```c
static int parseArgs(const int argc, char* argv[]);
```
### Detailed pseudo code

> For each function write pseudocode indented by a tab, which in Markdown will cause it to be rendered in literal form (like a code block).
> Much easier than writing as a bulleted list!
> For example:

#### `parseArgs`:
```
validate commandline
initialize message module
print assigned port number
decide whether spectator or player
```
---

## Server

### Data structures

> For each new data structure, describe it briefly and provide a code block listing the `struct` definition(s).
> No need to provide `struct` for existing CS50 data structures like `hashtable`.

### Definition of function prototypes

> For function, provide a brief description and then a code block with its function prototype.
> For example:

A function to parse the command-line arguments, initialize the game struct, initialize the message module, and (BEYOND SPEC) initialize analytics module.

```c
static int parseArgs(const int argc, char* argv[]);
```
### Detailed pseudo code

> For each function write pseudocode indented by a tab, which in Markdown will cause it to be rendered in literal form (like a code block).
> Much easier than writing as a bulleted list!
> For example:

#### `parseArgs`:

	validate commandline
	verify map file can be opened for reading
	if seed provided
		verify it is a valid seed number
		seed the random-number generator with that seed
	else
		seed the random-number generator with getpid()


#### sendPlayerMap
```
get the cells visible to the player (getVisibleRegion)
for each cell in the visible region
    update the player's map with gameGrid (spectator view)
send player map to client
for each cell in the visible region
    restore the player's map to only contain terrain
```
---

## XYZ module

> For each module, repeat the same framework above.

### Data structures

### Definition of function prototypes

### Detailed pseudo code

## gamemap module

### Data structures
We store a terrain map `grid` and `gameGrid` with player and gold information in a struct as 2D `char**` arrays:
```c
typedef struct GameMap {
    int numRows, numCols; // size of map
    char** grid; // terrain features
    char** gameGrid; // spectator view with players and gold
} GameMap_t;
```

### Definition of function prototypes
```c
int getNumRows(GameMap_t* map);
int getNumCols(GameMap_t* map);
char** getGrid(GameMap_t* map);
char** getGameGrid(GameMap_t* map);
GameMap_t* loadMapFile(char* mapFilePath);
void deleteGameMap(GameMap_t* map);
void deleteGrid(char** grid, int numRows);
char getCellType(GameMap_t* map, int row, int col);
void setCellType(GameMap_t* map, char type, int row, int col);
void restoreCell(GameMap_t* map, int row, int col);
int** getVisibleRegion(GameMap_t* map, int row, int col);
static int checkSquare(GameMap_t* map, int** visibleRegion, int idx,
                int row, int col, int radius);
static bool isVisible(GameMap_t* map, int r1, int c1, int r2, int c2);
int** getRoomCells(GameMap_t* map);
void delete2DIntArr(int** arr, int numRows);
void printMap(GameMap_t* map);
char* gridToString(GameMap_t* map);
static bool outOfMap(GameMap_t* map, int row, int col);
static bool isWall(char type);
```

### Detailed pseudocode

Pseudocode for getters and setters are omitted, as they all follow the same format: verify parameters - get/set variables - return.

#### loadMapFile
```
verify mapFilePath points to a readable file
get number of lines in the file (numRows)
get length of the first line (numCols)
initialize a map with numRows, numCols, grid, and gameGrid
for each line in the file
    for each char in the line
        store the char in map->grid and map->gameGrid
        
    free line
```

#### deleteGameMap
```
call deleteGrid on map->grid and map->gameGrid
free(map)
```

#### deleteGrid
```
return if grid is NULL
for every row in the grid
    free grid[row]
free grid pointer
```

#### restoreCell
```
validate coordinates
restore gameGrid[row][col] to grid[row][col] (restore terrain at cell in gameGrid)
```

#### getVisibleRegion
```
return NULL if map is NULL, coordinates are out of map,
  or coordinates is not in a room or passage cell
allocate visibleRegion, a (map size + 1) by 2 array to return
initialize index to store next coordinate
for a radius starting from 1, loop until no new visible cells are found
    update visibleRegion with the cells in the square around the
      player, where all cells are at most `radius` cells away in
      either direction
terminate visibleRegion with (-1, -1)
return visibleRegion
```

#### checkSquare
```
validate parameters
initialize top left corner of square
initialize square length to be 2 * radius + 1
initialize curIdx to store next coordinate at
for each direction in clockwise order, starting from right
    go length - 1 steps in that direction
        if current cell is visible
            store current cell in visibleRegion
            increment curIdx
return curIdx - idx // number of visible cells in current square
```

#### isVisible
```
// change in row and col from start to target cell
initialize rowDiff and colDiff
for each row between start and target row, exclusive
    if line between start and target cell intersects exactly on a col
        if grid[row][col] is not a room cell
            return false
    else // line intersects between two cols
        get line's col at current row
        if grid[row][floor(col)] and grid[row][ceil(col)] are both
         not room cells
            return false

for each col between start and target col
    if line intersects exactly on a row
        if grid[row][col] is not a room cell
            return false
    else // intersects between two rows
        get line's row at current col
        if grid[floor(row)][col] and grid[ceil(row)][col] are both
         not room cells
            return false

return true
```

#### getRoomCells
```
return NULL if map is NULL
malloc res, a (map size + 1) by 2 array to return
initialize index to store next coordinate
for each row in the room
    for each cell in the row
         if cell is '.' (empty room spot), update res
         increment index
set res[idx][0] and res[idx][1] to (-1, -1) to mark end of res
return res
```

#### delete2DIntArr
```
return if arr is NULL
for each row in arr
    free row
free arr pointer
```

#### printMap
```
for each row in map->gameGrid
    for each cell in row
        print the cell
    print newline
```

#### gridToString
```
initialize totalLength to numRows * (numCols + 1)
malloc res to store totalLength chars
initialize curIndex // beginning of each gameGrid row in res
for each row in gameGrid
    copy gameGrid to &res[curIndex]
    curIndex += numCols
    set res[curIndex++] to '\n' // such that curIndex is at start of next row)
res[totalLength - 1] = '\0' // last row ended by '\0' instead of '\n'
```

---

## Player

> The player module decarles a struct for each person in the game. It also provides functions for movement, creation, deletion, stealing gold from players, and more

### Data structures
> The main data structure used was the declared player struct, which stores a character ID, playerMap, gameMap, gold amount, name, row, column, players IP address, and a boolean if they are active or not.

### Definition of function prototypes
```c
player_t* player_new(char ID, GameMap_t* map, char** grid, int gold, char* name, int row, int col, addr_t playerAddress);
void player_delete(player_t* player);
int getPlayerRow(player_t* player);
int getPlayerCol(player_t* player);
char** getPlayerMap(player_t* player);
char* getPlayerName(player_t* player);
int getPlayerGold(player_t* player);
addr_t getPlayerAddress(player_t* player);
player_t* getPlayerByID(player_t** players, char ID);
void addGold(player_t* player, int amount);
void stealGold(player_t* player1, player_t* player2);
void updatePlayerPosition(player_t* player);
char getCharacterID(player_t* player);
bool player_isActive(player_t* player);
void player_inActive(player_t* player);
int moveDownRight(player_t* player, player_t** players);
int moveDownLeft(player_t* player, player_t** players);
int moveUpRight(player_t* player, player_t** players);
int moveUpLeft(player_t* player, player_t** players);
int moveUp(player_t* player, player_t** players);
int moveDown(player_t* player, player_t** players);
int moveLeft(player_t* player, player_t** players);
int moveRight(player_t* player, player_t** players);
```
### Detailed pseudo code

#### player_new
    malloc the space for a new player module
    if successful:
        assign the memory space to hold the given parameters (accepts NULL params)
        return pointer to memory address
    else:
        return NULL
        
#### player_delete
    Frees all variables that were malloc when calling player_new
    Then frees the malloced player

#### getPlayerRow
    returns the row variable of the given player
    
#### getPlayerCol
    returns the column variable of the given player
    
#### getPlayerMap
    returns the GameMap variable of the given player
    
#### getPlayerName
    returns the name variable of the given player

#### getPlayerGold
    returns the gold variable of the given player

#### getPlayerAddress
    returns the IP address variable of the given player

#### getPlayerByID
    finds index in the array by subracting 'A' from the char given
    convert index to int
    return the ID at the index in the given array

#### addGold
    adds the given amount of gold to the player given
    
#### stealGold
    value = player2->gold
    stolen = 0;
    if value > 20:
        stolen = value - 10
    else if value > 10:
        stolen = value - 5
    else if value > 5:
        stolen = value - 2
    else if value >=1 
        stolen = value;
        
    player1->gold = player1->gold + stolen
    player2->gold = player2->gold - stolen
    
#### updatePlayerPosition
    player->row = given row
    player->col = given col
    
#### getCharacterID
    return given player's ID variable

#### player_isActive
    returns the boolean variable _active_ for the given player
    

#### player_inActive
    returns the IP address variable of the given player

#### moveDownRight
    finds index in the array by subracting 'A' from the char given
    convert index to int
    return the ID at the index in the given array
    

## Testing plan

### Unit testing

> How will you test each unit (module) before integrating them with a main program (client or server)?

#### gamemap
1. Try to load nonexistent map files
2. Try to load valid map files
3. Test `get/set/restoreCellType` on different invalid and valid coordinates
5. Test `getRoomCells` on different maps
6. Test `getVisibleRegion` on random coordinates in different maps
7. Compare the output from `printGrid` and `gridToString` (should be the same)
8. Run above tests with valgrind to check delete functions

### Integration testing

> How will you test the complete main programs: the server, and for teams of 4, the client?

### System testing

> For teams of 4: How will you test your client and server together?

---

## Limitations

> Bulleted list of any limitations of your implementation.
> This section may not be relevant when you first write your Implementation Plan, but could be relevant after completing the implementation.
