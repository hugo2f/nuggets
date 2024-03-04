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
* Hugo - GameMap
* Jaysen & Colin - Server & Player

---

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
player_t* player_new(char ID, GameMap_t* map, char** grid, int gold,
                     char* name, int row, int col, addr_t playerAddress);
void player_delete(player_t* player);
player_t* getPlayerByID(player_t** players, char ID);
char* getPlayerName(player_t* player);
int getPlayerGold(player_t* player);
char** getPlayerMap(player_t* player);
int getPlayerRow(player_t* player);
int getPlayerCol(player_t* player);
addr_t getPlayerAddress(player_t* player);
bool getPlayerActive(player_t* player);
char* getStealMessage(player_t*player);
void setPlayerInactive(player_t* player);
void updateGoldDisplay(player_t* player, int pileAmount, int goldRemaining);
void addGold(player_t* player, int amount);
void stealGold(player_t* player1, player_t* player2, int goldRemaining);
void updatePlayerPosition(player_t* player);
char getCharacterID(player_t* player);
int moveDownRight(player_t* player, player_t** players, int goldRemaining);
int moveDownLeft(player_t* player, player_t** players, int goldRemaining);
int moveUpRight(player_t* player, player_t** players, int goldRemaining);
int moveDownRight(player_t* player, player_t** players, int goldRemaining);
int moveUp(player_t* player, player_t** players, int goldRemaining);
int moveDown(player_t* player, player_t** players, int goldRemaining);
int moveLeft(player_t* player, player_t** players, int goldRemaining);
int moveRight(player_t* player, player_t** players, int goldRemaining);
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

#### getStealMessage
    returns the steal message for a player (server sends this to client)

#### updateGoldDisplay
    sends a message to the client to update it's banner to reflect changes in gold

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
    grid = player->playerMap
    numRows = getNumRows(player->gameMap)
    numCols = getNumCols(player->gameMap)
    row = player->row
    col = player->col
    for (int row = 0; row < numRows; row++): 
      for (int col = 0; col < numCols; col++): 
        if grid[row][col] != ' ':
          grid[row][col] = getCellTerrain(player->gameMap, row, col)
          
    visibleRegion = getVisibleRegion(player->gameMap, row, col)
    if visibleRegion == NULL:
      printf("can't move there\n")
      return

    int size = 0
    for (row = 0; visibleRegion[row][0] != -1; row++):
      visibleRow = visibleRegion[row][0]
      visibleCol = visibleRegion[row][1]
      grid[visibleRow][visibleCol] = getCellType(player->gameMap, visibleRow, visibleCol)
      size++

    grid[row][col] = '@'
    delete2DIntArr(visibleRegion, size)
    
#### getCharacterID
    return given player's ID variable

#### getPlayerActive
    returns the boolean variable _active_ for the given player

#### setPlayerInactive
    player->active = false

#### moveDownRight
    flag = 0
    if player != NULL and not a wall at the updated position:
      if player is active:
        if the cell is an available spot
          set the cell to the player ID
          restore the map at the old position
          player->row = player->row +1
          player->col = player->col +1
        else if updated position is another player:
          player2 = getPlayerbyID
          swap players rows and columns
          set maps to reflect the players' new coordinates
          player1 stealGold from player 2
          flag = 2;
        else if updated position is gold
          set the position to player
          restore old cell
          player->row = player->row +1
          player->col = player->col +1
          flag = 1
      updatePlayerPosition(player);
      return flag;
    return 3;
    
#### moveDownLeft
    flag = 0
    if player != NULL and not a wall at the updated position:
      if player is active:
        if the cell is an available spot
          set the cell to the player ID
          restore the map at the old position
          player->row = player->row -1
          player->col = player->col +1
        else if updated position is another player:
          player2 = getPlayerbyID
          swap players rows and columns
          set maps to reflect the players' new coordinates
          player1 stealGold from player 2
          flag = 2
        else if updated position is gold
          set the position to player
          restore old cell
          player->row = player->row -1
          player->col = player->col +1
          flag = 1
      updatePlayerPosition(player);
      return flag;
    return 3;
    
#### moveUpRight
    flag = 0
    if player != NULL and not a wall at the updated position:
      if player is active:
        if the cell is an available spot
          set the cell to the player ID
          restore the map at the old position
          player->row = player->row +1
          player->col = player->col -1
        else if updated position is another player:
          player2 = getPlayerbyID
          swap players rows and columns
          set maps to reflect the players' new coordinates
          player1 stealGold from player 2
          flag = 2;
        else if updated position is gold
          set the position to player
          restore old cell
          player->row = player->row +1
          player->col = player->col -1
          flag = 1
      updatePlayerPosition(player);
      return flag;
    return 3;

#### moveUpLeft
    flag = 0
    if player != NULL and not a wall at the updated position:
      if player is active:
        if the cell is an available spot
          set the cell to the player ID
          restore the map at the old position
          player->row = player->row -1
          player->col = player->col -1
        else if updated position is another player:
          player2 = getPlayerbyID
          swap players rows and columns
          set maps to reflect the players' new coordinates
          player1 stealGold from player 2
          flag = 2;
        else if updated position is gold
          set the position to player
          restore old cell
          player->row = player->row -1
          player->col = player->col -1
          flag = 1
      updatePlayerPosition(player);
      return flag;
    return 3;

#### moveUp
    flag = 0
    if player != NULL and not a wall at the updated position:
      if player is active:
        if the cell is an available spot
          set the cell to the player ID
          restore the map at the old position
          player->row = player->row -1
        else if updated position is another player:
          player2 = getPlayerbyID
          swap players rows and columns
          set maps to reflect the players' new coordinates
          player1 stealGold from player 2
          flag = 2;
        else if updated position is gold
          set the position to player
          restore old cell
          player->row = player->row -1
          flag = 1
      updatePlayerPosition(player);
      return flag;
    return 3;
    
#### moveDown
    flag = 0
    if player != NULL and not a wall at the updated position:
      if player is active:
        if the cell is an available spot
          set the cell to the player ID
          restore the map at the old position
          player->row = player->row +1
        else if updated position is another player:
          player2 = getPlayerbyID
          swap players rows and columns
          set maps to reflect the players' new coordinates
          player1 stealGold from player 2
          flag = 2;
        else if updated position is gold
          set the position to player
          restore old cell
          player->row = player->row +1
          flag = 1
      updatePlayerPosition(player);
      return flag;
    return 3;

#### moveLeft
    flag = 0
    if player != NULL and not a wall at the updated position:
      if player is active:
        if the cell is an available spot
          set the cell to the player ID
          restore the map at the old position
          player->row = player->col-1
        else if updated position is another player:
          player2 = getPlayerbyID
          swap players rows and columns
          set maps to reflect the players' new coordinates
          player1 stealGold from player 2
          flag = 2;
        else if updated position is gold
          set the position to player
          restore old cell
          player->row = player->col -1
          flag = 1
      updatePlayerPosition(player);
      return flag;
    return 3;

#### moveRight
    flag = 0
    if player != NULL and not a wall at the updated position:
      if player is active:
        if the cell is an available spot
          set the cell to the player ID
          restore the map at the old position
          player->row = player->col+1
        else if updated position is another player:
          player2 = getPlayerbyID
          swap players rows and columns
          set maps to reflect the players' new coordinates
          player1 stealGold from player 2
          flag = 2'
        else if updated position is gold
          set the position to player
          restore old cell
          player->row = player->col +1
          flag = 1
      updatePlayerPosition(player);
      return flag;
    return 3;

## Server

### Data structures
> Uses the player, client, and gameMap module. There is a game struct, which holds currentNumPlayers, numGoldPiles, goldRemaining, an array of players, an array of gold piles, a map, and a boolean if there is a spectator. The one game instance is a global variable. There also is a struct for gold piles which hold row, col, and amount.

### Definition of function prototypes

```c 
void initializeGame();
static bool handleMessage(void* arg, const addr_t from, const char* message);
void updateSpectatorDisplay();
void removeSpectator();
bool spectatorActive();
void distributeGold();
void sendStartingGold(player_t* player);
void collectGold(player_t* player);
void sendGoldUpdate(player_t* player, int pileAmount);
void spawnGold(int rol, int col);
void spawnPlayer(player_t* player, int row, int col);
void callCommand(player_t* player, char key);
void sendGrid(player_t* player, bool isSpectator);
void sendDisplay(player_t* player, bool isSpectator);
char** initializePlayerMap(int row, int col);
void updateCurrentPlayerVision();
player_t* spectatorJoin(addr_t address, char* name);
player_t* playerJoin(addr_t address, char* name);
player_t* checkPlayerJoined(addr_t address);
void playerQuit(player_t* player);
void spectatorQuit(player_t* spectator);
void sendGameSummary();
void cleanUpGame();

void initializeGame();
static bool handleMessage(void* arg, const addr_t from, const char* message);
void updateSpectatorDisplay();
void removeSpectator();
bool spectatorActive();
void distributeGold();
void sendStartingGold(player_t* player);
void collectGold(player_t* player);
void sendGoldUpdate(player_t* player, int pileAmount);
void spawnGold(int rol, int col);
void spawnPlayer(player_t* player, int row, int col);
void callCommand(player_t* player, char key);
void sendGrid(player_t* player, bool isSpectator);
void sendDisplay(player_t* player, bool isSpectator);
char** initializePlayerMap(int row, int col);
void updateCurrentPlayerVision();
player_t* spectatorJoin(addr_t address, char* name);
player_t* playerJoin(addr_t address, char* name);
player_t* checkPlayerJoined(addr_t address);
void playerQuit(player_t* player);
void spectatorQuit(player_t* spectator);
void sendGameSummary();
void cleanUpGame();
```

### Detailed pseudo code

#### initializeGame
    malloc memory for the game
    if game == NULL:
      return
    game->map = loadMapFile("../maps/main.txt")
    game->players = malloc(MaxPlayers * sizeof(player_t*))
    if game->players == NULL:
      return;
    game->currentNumPlayers = 0
    game->spectatorJoined = false
    distributeGold()

#### handleMessage
    char command[10]
    char name[100]
    char* okMessage = malloc(10 * sizeof(char)) 
    print the message to the console
    declare a player variable
    if message starts with "PLAY" and can extract a name:
      Call playerJoin function with 'from' and the extracted 'name'
      Get player's address, ID, and name
      Print a message indicating the player joined the game
      Create an "OK" message with the player's ID
      Send the "OK" message to the player's address
      Send the player's grid and display information
      Update the spectator display
      Free memory allocated for the "OK" message
    else if message starts with "KEY" and can extract a command:
      call checkPlayerJoined function with 'from'
      if the player is not found:
        Return false and keep running
      Extract the key from the command
      Get the player's name
      Print a message indicating a key was received from the player
      Call callCommand function with the player and the extracted key
    else if message == "SPECATE":
      Create a spectator name
      Call spectatorJoin function with 'from' and the spectator name
      Get the spectator's address
      Send an "OK A" message to the spectator
      Send the spectator's grid and display information
    else:
      Create an invalid message indicating the message format is invalid
      Send the invalid message to 'from'
    return false 

#### updateSpectatorDisplay
    spectator = game->players[MaxPlayers-1]
    if spectator is active:
      sendDisplay to the spectator

#### updateCurrentPlayerVision
    loop through players in game
    if player is active
    send display update

#### spectatorActive
    spectator = game->players[MaxPlayers-1]
    if spectator == NULL:
      return false
    return true;

#### distributeGold
    seed the random number generator
    game->numGoldPiles = GoldMinNumPiles + rand() % (GoldMaxNumPiles - GoldMinNumPiles + 1)
    malloc the space for gameGoldPiles
    for (size = 0; roomCells[size][0] != -1; size++)
    if size < game->numGoldPiles:
      print to stderr
      return;
    malloc space for indices
    if indices == NULL
      print to stderr
      return;
    for (i = 0; i < game->numGoldPiles; i++):
      indices[i] = i
    for (int i = game->numGoldPiles; i < size; i++):
      j = random number
      if j < game->numGoldPiles:
        indices[j] = i;
    for (int i = 0; i < game->numGoldPiles; i++):
      int roomCellIndex = indices[i]
      int row = roomCells[roomCellIndex][0]
      int col = roomCells[roomCellIndex][1]
      create the goldpile object
      if goldPile != NULL:
        goldPile->amount = 0
        goldPile->row = row
        goldPile->col = col
        game->goldPiles[i] = goldPile
     spawnGold(row, col);
    game->goldRemaining = GoldTotal
    for (int i = 0; i < GoldTotal; i++):
      int index = rand() % game->numGoldPiles
      goldPile_t* goldPile = game->goldPiles[index]
      goldPile->amount++
    free all malloc'd memory

#### sendStartingGold
    sends a message to the client with the starting amount of gold
    
#### collectGold
    row = getPlayerRow(player)
    col = getPlayerCol(player)

    for i = 0 to game->numGoldPiles - 1:
        goldPile = game->goldPiles[i]
        pileAmount = goldPile->amount
        goldRow = goldPile->row
        goldCol = goldPile->col
    
        if row == goldRow and col == goldCol:
            addGold(player, pileAmount)
        goldMessage = "GOLD " + pileAmount + " " + getPlayerGold(player) + " " + game->goldRemaining
        playerAddress = getPlayerAddress(player)
        message_send(playerAddress, goldMessage)
        game->goldRemaining -= pileAmount
        if spectatorActive():
            playerID = getCharacterID(player)
            spectatorMessage = "SPECTATOR_GOLD " + playerID + " " + pileAmount + " " + getPlayerGold(player) + " " + game->goldRemaining
            spectator = game->players[MaxPlayers-1]
            spectatorAddress = getPlayerAddress(spectator)
            message_send(spectatorAddress, spectatorMessage)
        goldPile->amount = 0
        free(goldMessage)
        break

#### spawnGold
    setCellType at row and column to '*'
    
#### spawnPlayer
    id = getCharacterID(player)
    setCellType at row and column to id
    playerMap = get player's map
    set playerMap at row, col to '@'
    send display to the player

#### callCommand
    int atGold = 0;
    switch (key) {
      case 'Q':
        if sent by spectator
          spectatorQuit
        else
          playerQuit
        break;
      case 'h':
        atGold = moveLeft(player, game->players, game->goldRemaining);    
        break;
      case 'l':
        atGold = moveRight(player, game->players, game->goldRemaining);
        break;
      case 'j':
        atGold = moveDown(player, game->players, game->goldRemaining);
        break;
      case 'k':
        atGold = moveUp(player, game->players, game->goldRemaining);
        break;
      case 'y':
        atGold = moveUpLeft(player, game->players, game->goldRemaining);
        break;
      case 'u':
        atGold = moveUpRight(player, game->players, game->goldRemaining);
        break;
      case 'b':
        atGold = moveDownLeft(player, game->players, game->goldRemaining);
        break;
      case 'n':
        atGold = moveDownRight(player, game->players, game->goldRemaining);
        break;
      case 'H':
        while(moveLeft(player, game->players, game->goldRemaining) != 3);
        break;
      case 'L':
        while(moveRight(player, game->players, game->goldRemaining) != 3);
      break;
      case 'J':
        while(moveDown(player, game->players, game->goldRemaining) != 3);
        break;
      case 'K':
        while(moveUp(player, game->players, game->goldRemaining) != 3);
        break;
      case 'Y':
        while(moveUpLeft(player, game->players, game->goldRemaining) != 3);
        break;
      case 'U':
        while(moveUpRight(player, game->players, game->goldRemaining) != 3)
        break;
      case 'B':
        while(moveDownLeft(player, game->players, game->goldRemaining) != 3);
        break;
      case 'N':
        while(moveDownRight(player, game->players, game->goldRemaining) != 3);
        break;    
      default: 
        printf("not a valid command\n");
        break;
    }
    if atGold = 1
      collectGold(player)
    if atGold = 2
      send stealMessage to player
      if spectator is active
        send stealMessage to spectator
  if key not 'Q'
    update all player vision
    update spectator display

#### sendGrid
    malloc memory for sizeMessage
    if (sizeMessage == NULL)
      return
    numRows = rows in game->map
    numCols = cols in game->map
    if (isSpectator):
      print message to sizeMessage
    else 
      print message to sizeMessage
    addr_t address = get player's address
    send the message out
    free mallocs

#### sendDisplay
    if isSpectator == false:
      update player's position
    numRows, numCols = get rows and cols from map
    address = player's address
    if (isSpectator == false):
      grid = getPlayerMap(player)
    else:
      grid = getGameGrid(game->map)
    send message
    update grid message
    print Map
    send the message
    free malloc'd memory

#### initializePlayerMap
    malloc size for grid
    if (grid == NULL) 
      return NULL
    for (int row = 0; row < numRows; row++):
      malloc and set grid[row]

    visibleRegion = getPlayer's visible region
    if (visibleRegion == NULL):
      printf("ERROR\n")
      return NULL
    int size = 0
    for (int row = 0; visibleRegion[row][0] != -1; row++):
      int visibleRow = visibleRegion[row][0]
      int visibleCol = visibleRegion[row][1]
      grid[visibleRow][visibleCol] = getCellType(game->map, visibleRow, visibleCol)
      size++
    delete visibleRegion
    return grid

#### sendGoldUpdate
    loop through players
    if active
      check player who called this function
        send player who called this function gold update with pile amount
      send all other players remaining gold to update their banner

#### spectatorJoin
    if (game->spectatorJoined):
        return NULL;
      spectator = new player
      if (spectator == NULL):
        print to stderr
        return NULL
      put spectator at the end of the game's player array
      return spectator;
    
#### playerJoin
      if (currentNumPlayers < MaxPlayers-1):
        id = next char in the Alhpabet
        seed the random number generator
        roomCells = get room cells in game->map
        find number of room cells
        seed random number generator
        randomCell = rand() % numRoomCells
        get row and col for index the player is spawned at 
        create player map
        initialize the player
        create the new player with the relevant information
        if (newPlayer == NULL):
          print to stderr
          return NULL
        spawn the player in
        add player to array of players after their setup is done
        update game variables
      else:
        return NULL
      return newPlayer
    
#### checkPlayerJoined
    for each current player
      if the current player is the desired player
        print addresses are equal
        return player 
    return NULL

#### playerQuit 
    changes player's ID on gameGrid back to terrain
    sets player's active status to false 
    update active player's vision to reflect this change
    send QUIT message to player

#### spectatorQuit
    set spectator active status to false 
    send QUIT message to spectator
    delete the specatator

#### sendGameSummary
    sends a formatted summary of all players and their gold totals when game is over (all gold collected)

#### cleanUpGame
    for each current player:
      delete the player
  
    for each gold pile:
        delete the goldPile

    free the players
    free the gold piles
    free the gameMap
    free the game
    
## Testing plan

1. We will test the server as a systems test. We can utilize the miniclient initially to test sending and receiving messages.
2. When the actual client is up and running, we will use that client. 
3. Use print statements in server to check whether or not the message the client receives is the same

## Client

### Data structures
> There is one ClientData client initialized

### Definition of function prototypes
```c 
static void parseArgs(int argc, char* argv[], addr_t* serverp);
static bool respondToInput(void* server);
static bool handleMessage(void* arg, const addr_t from, const char* message);
static void setPlayerName(char* name);
static int getMapSize(); 
static void unitTest();
```

### Detailed pseudo code

#### parseArgs
    program = argv[0]
    if argc is not 3 and not 4:
        print error message and exit 2
    if message_init(NULL) is 0:
        print error message and exit 3
    serverHost = argv[1]
    serverPort = argv[2]
    if not message_setAddr(serverHost, serverPort, serverp):
        print error and exit 4
    if serverp is NULL or not message_isAddr(*serverp):
        print error and exit 5
    if argc is 4:
        set player name to argv[3]
    run unit tests (this does not do anything except in the test builds)
    start server

#### respondToInput
    if client is not playing:
        return false
    if server == NULL OR message address not valid:
        print to stderr and return true
    get validInputs
    input = get_character()
    if (strchr(validInputs, input) != NULL):
        sendKey
        removeFromBanner
    else:
        indicate_invalid_key(input);
    return false;
    
#### handleMessage
    if message is NULL:
        print an error message
        return false
    if cannot properly scan message:
        print error message
        return false
    if 'OK'
        handle_okay
    else if 'GRID':
        handle_grid
    else if 'GOLD':
        handle_gold
    else if 'QUIT':
        handle_quit
    else if 'ERROR':
        handle_error
    else if 'DISPLAY':
        get mapsize
        retrieve the map
        deal with unsuccessful case
        handle_display
    else:
        print error message
    return false


#### setPlayerName
    if client.playerName is not NULL:
        Print "You cannot reset player name" to stderr
        Return

    create a character array named name with size MAXIMUM_NAME_LENGTH
    set name[0] to '\0' to ensure that the buffer is empty
    set currentNameSize to 0

    for each argument in argv starting from index 3:
        set argument to the current argument
        set argumentLength to the length of argument

        if (currentNameSize + argumentLength + 1) is less than MAXIMUM_NAME_LENGTH:
            concatenate argument and a space to name using strncat, ensuring not to exceed MAXIMUM_NAME_LENGTH
            update currentNameSize accordingly
        else:
            concatenate as much of the argument as possible to name without exceeding MAXIMUM_NAME_LENGTH
            break the loop

    set client.playerName to name

#### getMapSize
    set mapsize to rows times columns
    if mapsize is still zero set it to MAXIMUM_MAP_SIZe
    return mapsize

#### unitTest
    if UNIT_TEST is defined (via preprocessor directives):
        print "Running test build" to stderr

        open "testcommands.txt" file for reading as testCommandsFile
        if testCommandsFile is not opened successfully:
            print "Error opening testcommands.txt file, continuing to normal execution" to stderr
            return

        while there are lines in testCommandsFile:
            read a line from testCommandsFile into command
            print "\nSERVER COMMAND\n" + command + "\nCLIENT OUTPUT" to stdout
            handle message with parameters (NULL, from, command)
            print "\n" to stdout

        close testCommandsFile
        print "TESTING COMPLETE" to stdout
        exit with status 0

    else:
        print "Running release build" to stderr

## Handlers Module

#### handle_okay
    initialize errors to 0

    if client.state is not START_SENT:
        print "Received OK again or prior to sending START" to stderr
        increment errors by 1

    if length of symbol is greater than 1:
        print "Received player symbol with multiple characters, attempting to use first" to stderr

    set symbolCharacter to the first character of symbol

    if symbolCharacter is not a valid alphabetic and capitalized symbol:
        print "Received invalid player symbol" to stderr
        increment errors by 1

    if errors is not equal to 0:
        return

    set client.playerSymbol to symbolCharacter
    set client.state to OK_RECEIVED

#### handle_grid
    initialize errors to 0

    if client.state is not OK_RECEIVED:
        print "Received GRID prior to receiving OK or for a second time" to stderr
        increment errors by 1

    initialize nrows and ncols variables

    extract integer values from the coordinates string into nrows and ncols
    if the number of successfully extracted values is not 2:
        print "GRID message bad data" to stderr
        increment errors by 1

    if errors is not equal to 0:
        return

    initialize curses
    set client.nrowsMap to nrows
    set client.ncolsMap to ncols
    set client.state to GRID_RECEIVED

#### handle_gold_remaining

    initialize errors to 0

    if client.state is not GRID_RECEIVED:
        print "Received GOLD_REMAINING prior to GRID or for a second time" to stderr
        increment errors by 1

    initialize startingGoldRemaining variable

    extract an integer value from startingGoldRemainingString into startingGoldRemaining
    if the number of successfully extracted values is not 1:
        print "GOLD_REMAINING message bad data" to stderr
        increment errors by 1

    if startingGoldRemaining is not a valid gold count:
        print "Invalid 'startingGoldRemaining' gold count" to stderr
        increment errors by 1

    if errors is greater than 0:
        return

    if client.playerName is not NULL:
        display player banner with parameters (client.playerSymbol, 0, startingGoldRemaining)
    else:
        display spectator banner with parameter (startingGoldRemaining)

    set client.state to GOLD_REMAINING_RECEIVED

#### handle_player_gold

    initialize errors to 0

    if client.state is not PLAY:
        print "Received GOLD prior to game start" to stderr
        increment errors by 1

    if client.playerName is NULL:
        print "Received GOLD as Spectator" to stderr
        increment errors by 1

    initialize collected, current, and remaining variables

    parse gold counts from counts string into collected, current, and remaining
    add the number of errors returned by parseGoldCounts to errors

    if errors is greater than 0:
        return

    update player banner with parameters (client.playerSymbol, current, remaining)

    if collected is not equal to 0:
        indicate nuggets collected player with parameter (collected)

#### handle_spectator_gold
    initialize errors to 0

    if client.state is not PLAY:
        print "Received SPECTATOR_GOLD prior to PLAY" to stderr
        increment errors by 1

    if client.playerName is not NULL:
        print "Received SPECTATOR_GOLD as Player" to stderr
        increment errors by 1

    initialize counts, collectorSymbol variables

    extract symbol and gold data from collectionData string into collectorSymbol and counts
    if the number of successfully extracted values is not 2:
        print "SPECTATOR_GOLD message bad data" to stderr
      return

    if collectorSymbol is not a valid alphabetic and capitalized symbol:
        print "SPECTATOR_GOLD message contains invalid player symbol" to stderr
        increment errors by 1

    initialize collected, current, and remaining variables

    parse gold counts from counts string into collected, current, and remaining
    add the number of errors returned by parseGoldCounts to errors

    if errors is greater than 0:
        return

    update spectator banner with parameter (remaining)

    indicate nuggets collected spectator with parameters (collectorSymbol, collected)

#### handle_stolen
    initialize errors to 0

    if client.state is not PLAY:
        print "Received STOLEN prior to PLAY" to stderr
        increment errors by 1

    initialize stolenPlayerSymbol, stealerPlayerSymbol, and amountStolen variables

    extract stolenPlayerSymbol, stealerPlayerSymbol, and amountStolen from stealData
    if the number of successfully extracted values is not 3:
        print "STOLEN message bad data" to stderr
        return

    if amountStolen is not a valid gold count:
        print "Invalid 'amountStolen' gold count" to stderr
        increment errors by 1

    if stolenPlayerSymbol is not a valid alphabetic and capitalized symbol:
        print "Invalid 'stolenPlayerSymbol' (the player stolen from)" to stderr
        increment errors by 1

    if stealerPlayerSymbol is not a valid alphabetic and capitalized symbol:
        print "Invalid 'stealerPlayerSymbol' (the player who stole)" to stderr
        increment errors by 1

    if stolenPlayerSymbol is the same as stealerPlayerSymbol:
        print "Someone cannot steal from themselves" to stderr
        increment errors by 1

    if errors is greater than 0:
        return

    if client.playerName is not NULL:
        if client.playerSymbol is equal to stolenPlayerSymbol:
            indicate someone stole nuggets from you with parameters (stealerPlayerSymbol, amountStolen)
        else if client.playerSymbol is equal to stealerPlayerSymbol:
            indicate you stole nuggets from someone with parameters (stolenPlayerSymbol, amountStolen)
    else:
      indicate nuggets stolen spectator with parameters (stolenPlayerSymbol, stealerPlayerSymbol, amountStolen)

#### handle_display
    if client.state is not GOLD_REMAINING_RECEIVED and client.state is not PLAY:
        print "Received DISPLAY prior to receiving GOLD_REMAINING" to stderr
        return

    display map using function display_map with parameter map

    if client.state is not PLAY:
        set client.state to PLAY

#### handle_quit
    end curses
    print quit explanation
    flush stdout
    exit program

#### handle_error
    print error message parameter

#### parseGoldCounts
    initialize errors to 0

    extract collected, current, and remaining from counts into collected, current, and remaining parameters
    if the number of successfully extracted values is not 3:
        print "Gold data missing" to stderr
        increment errors by 1
        return errors

    if collected is not a valid gold count:
        print "Invalid 'collected' gold count" to stderr
        increment errors by 1

    if current is not a valid gold count:
        print "Invalid 'current' gold count" to stderr
        increment errors by 1

    if remaining is not a valid gold count:
        print "Invalid 'remaining' gold count" to stderr
        increment errors by 1

    return errors

## Senders Module

#### send_receipt:
    if client.state is not PLAY and client.state is not PRE_INIT:
        send "RECEIVED" message to server using message_send

#### send_start:
    if client.state is not PRE_INIT:
        print "Sent START again" to stderr
        return

    if client.playerName is NULL:
        sendSpectate(serverp)
    else:
        sendPlay(serverp)

    set client.state to START_SENT

#### send_key:
    if client.state is not PLAY:
        return

    create message using key
    send message to server using message_send

#### sendPlay:
    create message containing "PLAY" followed by client.playerName
    send message to server using message_send

#### sendSpectate:
    send "SPECTATE" message to server using message_send

## Graphics Module

#### init_curses:
    if window size is smaller than required:
        prompt user to expand window
    while window size is still smaller than required:
        continue prompting user to expand window
    initialize curses library
    set input mode to read one character at a time
    disable automatic echoing of characters typed by the user
    enable non-blocking input mode
    enable interpretation of special keys
    flush any pending input
    start color mode
    initialize color pair one with specified foreground and background colors

#### display_map:
    set initial cursor position
    iterate over characters in the map:
        if end of row is reached:
            move to the next row
        if end of available board space is reached:
            break
        print character at current position
        move to the next position
    update display

#### display_player_banner:
    if playerSymbol is not null:
        clear current line
        create banner string with player information
        print banner string at top left corner
        update display

#### function display_spectator_banner:
    clear current line
    print spectator banner with unclaimed nuggets information at top left corner
    update display

#### indicate_invalid_key:
    create message indicating invalid key press
    remove any existing indicator messages
    append message to banner
    update display

#### indicate_nuggets_collected_player:
    create message indicating collected nuggets
    remove any existing indicator messages
    append message to banner
    update display

#### indicate_nuggets_collected_spectator:
    create message indicating someone else collected nuggets
    remove any existing indicator messages
    append message to banner
    update display

#### indicate_you_stole_nuggets_from_someone:
    create message indicating you stole nuggets
    remove any existing indicator messages
    append message to banner
    update display

#### indicate_someone_stole_nuggets_from_you:
    create message indicating someone stole nuggets from you
    remove any existing indicator messages
    append message to banner
    update display

#### indicate_nuggets_stolen_spectator:
    create message indicating someone stole nuggets from someone else
    remove any existing indicator messages
    append message to banner
    update display

#### remove_indicator:
    move cursor to end of banner
    delete all characters until end of screen
    update display

#### get_character:
    return character from keyboard input

#### end_curses:
    end curses library

## Validators Module

#### validate_gold_count
    if count is greater than or equal to zero and count is less than or equal to maximum:
        return true
    else:
        return false

#### validate_player_symbol
    if symbol is between 'A' and 'Z' inclusive:
        return true
    else:
        return false

#### validate_stdin_character
    if stdinCharacter is not equal to EOF:
        return true
    else:
        return false


## Testing plan

- We will test the server as a systems test. We can utilize the miniclient initially to test sending and receiving messages.
- When the actual client is up and running, we will use that client. 
- Use print statements in server to check whether or not the message the client receives is the same


### Unit testing

#### Client Unit Testing

The Client's main operations are tested via the unitTest function in the Client driver. The tester can compile the Client in unit test mode. This essentially creates a client.c test executable as opposed to a client.c release-ready playable executable. The unitTest function reads a long list of malformed messages into the handleMessage function, which either errors or delegates responsiblity for responding to the various handlers. The unitTest function prints each malformed command with the logged error message next to it. Therefore, we are able to test if the client is robust with respect to the various ways in which the server may send it invalid data. We decided to use preporcessor definitions in the client.c program, instead of creating an individual  test program, to control for the way in which the client sets up the global variables and parses the arguments. Otherwise, we would need to redundantly replicate all this code in the individual test file. 

The cases we tested included the following among others:
1. malformed message headers
2. messages with numbers that are too large
3. messages sent in the wrong order
4. messages missing data 
5. messages with too much data
6. messages with missing dilineators


#### gamemap
1. Try to load nonexistent map files
2. Try to load valid map files
3. Test `get/set/restoreCellType` on different invalid and valid coordinates
5. Test `getRoomCells` on different maps
6. Test `getVisibleRegion` on random coordinates in different maps
7. Compare the output from `printGrid` and `gridToString` (should be the same)
8. Run above tests with valgrind to check delete functions

### Integration testing

#### Client Manual Integration Testing (against miniserver)

We tested ALL the client functionality against the miniserver, including sending, graphics, receiving, and validating data. To test with the miniserver, we needed to call a sendReceipt function between receiving the next initialization command. We do not compile this in the release executable either, again by setting a preprocessor directive for testing mode. 

The cases we tested included the following among others:
1. everything in the list above under (unit testing)
3. DISPLAY commands with maps that are way too large
4. checking that the graphics module responds appropriately when the terminal size is too large for the grid 
5. High volumes of nonsense data

^^^ We did many if not all of these tests under Valgrind as well ^^^

### System testing

We run the server and then connect all of our computers to the server. We test with multiple wondows via localhost, as well as with multiple computers via the Darmtouth network. We play the game not just to win, but
also to test possible interactions and edge cases that we think might break our code.  

---

## Limitations

1. Constrained to 26 players (allowed by `REQUIREMENTS.md`)
2. Players cannot quit and rejoin (allowed by `REQUIREMENTS.md`)
3. One spectator at a time (allowed by `REQUIREMENTS.md`)
4. Some constant buffer sizes and associated memory protection theoretically truncate the values that the client can receive such that the client behavior might not align with the server's expectations of it, but these buffers are so big that this should never be a problem unless the server is greatly malfunctioning. 
