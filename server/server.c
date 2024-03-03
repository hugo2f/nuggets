#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <strings.h>
#include <time.h>
#include "../support/message.h"
#include "../gamemap/gamemap.h"
#include "player/player.h"

static const int MaxPlayers = 27;      // maximum number of players
static const int GoldTotal = 250;      // amount of gold in the game
static const int GoldMinNumPiles = 1; // minimum number of gold piles
static const int GoldMaxNumPiles = 2; // maximum number of gold piles

/****************** local types *********************/
typedef struct goldPile {
  int row;
  int col;
  int amount;
} goldPile_t;

typedef struct game {
  int currentNumPlayers;
  int numGoldPiles;
  int goldRemaining; 
  player_t** players;
  goldPile_t** goldPiles;
  GameMap_t* map;
  bool spectatorActive;
} game_t;

//global types
game_t* game;
bool gameOver = false;

//function prototypes
void initializeGame();
static bool handleMessage(void* arg, const addr_t from, const char* message);
void updateSpectatorDisplay();
void removeSpectator();
bool spectatorActive();
void distributeGold();
void collectGold(player_t* player);
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

int 
main(int argc, char* argv[])
{ 
  // initialize the message module (without logging)
  int myPort = message_init(NULL);
  if (myPort == 0) {
    return 2; // failure to initialize message module
  } else {
    printf("serverPort=%d\n", myPort);
  }

  // check arguments (there should be none)
  const char* program = argv[0];
  if (argc != 1) {
    fprintf(stderr, "usage: %s\n", program);
    return 3; // bad commandline
  }

  initializeGame();

  // Loop, waiting for input or for messages; provide callback functions.
  // We use the 'arg' parameter to carry a pointer to 'server'.
  bool ok = message_loop(NULL, 0, NULL, NULL, handleMessage);

  // shut down the message module
  message_done();
  
  return ok? 0 : 1; // status code depends on result of message_loop
}

/*
 * Initialize the main elements of the game 
 */
void 
initializeGame() 
{
  game = malloc(sizeof(game_t));
  if (game == NULL) {
    return;
  }
  game->map = loadMapFile("../maps/fewspots.txt");
  game->players = malloc(MaxPlayers * sizeof(player_t*));
  for (int i = 0; i < MaxPlayers; i++) {
    game->players[i] = NULL;
  }   
  if (game->players == NULL) {
    return;
  }
  game->currentNumPlayers = 0;
  game->spectatorActive = false;
  distributeGold();
}

static bool
handleMessage(void* arg, const addr_t from, const char* message)
{
  char command[20]; //store the command
  char name[30];

  player_t* player;
  if (sscanf(message, "PLAY %s", name) == 1) {
    char* playerName = malloc(30 * sizeof(char));
    strcpy(playerName, name);
    player = playerJoin(from, playerName); 
    addr_t playerAddress = getPlayerAddress(player);
    char playerID = getCharacterID(player);
    char* okMessage = malloc(10 * sizeof(char)); //used to store message that contains client's charID (sent to client)
    sprintf(okMessage, "OK %c", playerID);
    message_send(playerAddress, okMessage);
    sendGrid(player, false);
    sendDisplay(player, false);
    updateCurrentPlayerVision();
    updateSpectatorDisplay();
    free(okMessage);
  } else if (sscanf(message, "KEY %s", command) == 1) {
    player = checkPlayerJoined(from); 
    if (player == NULL) {
      //if the spectator is in the game and none of the players sent the message,
      //we know the messaage game from the spectator
      if (game->spectatorActive) {
        player = game->players[MaxPlayers-1];
      } else {
        return false; //don't do anything -- keep running
      }
    }
    char key = command[0];
    callCommand(player, key);
  } else if (strcmp(message, "SPECTATE") == 0) {
    // check if there is already a spectator in the game
    // if so, remove them and replace
    if (game->spectatorActive) {
      removeSpectator();
    }
    char* spectatorName = malloc(strlen("SPECTATOR")+1 * sizeof(char));
    strcpy(spectatorName, "SPECTATOR");
    player = spectatorJoin(from, spectatorName);
    addr_t spectatorAddress = getPlayerAddress(player);
    message_send(spectatorAddress, "OK A");
    sendGrid(player, true);
    sendDisplay(player, true);
  } else {
    char invalidMessage[100];
    sprintf(invalidMessage, "Invalid message format: %s", message);
    message_send(from, invalidMessage);
  }
  //server keeps running
  return false;
}

/*
 * Switch statement that calls command based on the client's input
 */
void callCommand(player_t* player, char key) 
{ 
  int atGold = 0;
  switch (key) {
    case 'Q':
      //check if its the spectator
      if (game->spectatorActive) {
        player_t* spectator = game->players[MaxPlayers-1];
        addr_t spectatorAddress = getPlayerAddress(spectator);
        addr_t playerAddress = getPlayerAddress(player);
        if (message_eqAddr(playerAddress, spectatorAddress)) {
          printf("here lol\n");
          spectatorQuit(spectator);
        }
      } else {
        playerQuit(player);
      }
      break;
    case 'h':
      atGold = moveLeft(player, game->players);    
      break;
    case 'l':
      atGold = moveRight(player, game->players);
      break;
    case 'j':
      printf("command for moving down\n");
      atGold = moveDown(player, game->players);
      break;
    case 'k':
      printf("command for moving up\n");
      atGold = moveUp(player, game->players);
      break;
    case 'y':
      atGold = moveUpLeft(player, game->players);
      break;
    case 'u':
      atGold = moveUpRight(player, game->players);
      break;
    case 'b':
      atGold = moveDownLeft(player, game->players);
      break;
    case 'n':
      atGold = moveDownRight(player, game->players);
      break;
    case 'H':
      while(moveLeft(player, game->players) != 2);
      break;
    case 'L':
      while(moveRight(player, game->players) != 2);
     break;
    case 'J':
      while(moveDown(player, game->players) != 2);
      break;
    case 'K':
      while(moveUp(player, game->players) != 2);
      break;
    case 'Y':
      break;
    case 'U':
      break;
    case 'B':
      break;
    case 'N':
      break;    
    default: 
      printf("not a valid command\n");
      break;
  }
  if (atGold == 1) {
    collectGold(player);
  }
  //send to client
  if (key != 'Q') {
    sendDisplay(player, false); 
    updateSpectatorDisplay();
  }
}

/*
 * Update the spectator's display
 */
void
updateSpectatorDisplay() 
{
  //send update to spectator
  if (game->spectatorActive) {
    player_t* spectator = game->players[MaxPlayers-1];
    sendDisplay(spectator, true);
  }
}

/* 
 * If another client joins as a spectator, kick the old one
 */
void
removeSpectator() 
{
  player_t* currSpectator = game->players[MaxPlayers-1];
  spectatorQuit(currSpectator);
}

/*
 * Randomly distributes the gold throughout the map
 */
void distributeGold() 
{
  srand(time(NULL)); //seed the random number generator

  //generate random number of gold piles
  game->numGoldPiles = GoldMinNumPiles + rand() % (GoldMaxNumPiles - GoldMinNumPiles + 1);
  game->goldPiles = malloc(game->numGoldPiles * sizeof(goldPile_t*));

  //TO DO:
  /*
   * get array with valid spots (coordinates)
   * select numGoldPiles items from that array
   * reservoir sampling 
   * update those positions in the map with gold 
   * store the gold pile in the array of goldPiles
   * to distribute the gold "fairly" 
   * for every piece of gold, randomly put it in a pile 
   */
  int** roomCells = getRoomCells(game->map);
  int size = 0;
  // get number of room cells
  for (size = 0; roomCells[size][0] != -1; size++) {
  }
  if (size < game->numGoldPiles) {
    fprintf(stderr, "not enough room cells to distribute gold\n");
    return;
  }
  // reservoir sampling to get indices
  int* indices = malloc(game->numGoldPiles * sizeof(int));
  if (indices == NULL) {
    fprintf(stderr, "distributeGold memory allocation failed\n");
    return;
  }
  for (int i = 0; i < game->numGoldPiles; i++) {
    indices[i] = i;
  }

  for (int i = game->numGoldPiles; i < size; i++) {
    int j = rand() % (i + 1);

    if (j < game->numGoldPiles) {
        indices[j] = i;
    }
  }

  // TODO: go through indices and spawn gold piles at roomCells[indices[i]] = (row, col)
  for (int i = 0; i < game->numGoldPiles; i++) {
    int roomCellIndex = indices[i];
    int row = roomCells[roomCellIndex][0];
    int col = roomCells[roomCellIndex][1];

    //create the goldpile object
    goldPile_t* goldPile = malloc(sizeof(goldPile_t));
    if (goldPile != NULL) {
      goldPile->amount = 0;
      goldPile->row = row;
      goldPile->col = col;
      game->goldPiles[i] = goldPile;
    }

    //update the map
    spawnGold(row, col);
  }

  // distribute the gold fairly
  // for every piece of gold, randomly choose
  // a pile to put it in
  game->goldRemaining = GoldTotal;
  for (int i = 0; i < GoldTotal; i++) {
    int index = rand() % game->numGoldPiles;
    goldPile_t* goldPile = game->goldPiles[index];
    goldPile->amount++;
  }

  free(indices);
  delete2DIntArr(roomCells, size+1);
}

void 
collectGold(player_t* player) 
{
  //loop through the piles to see which one was collected
  int row = getPlayerRow(player);
  int col = getPlayerCol(player);

  for (int i = 0; i < game->numGoldPiles; i++) {
    goldPile_t* goldPile = game->goldPiles[i];
    int pileAmount = goldPile->amount;
    int goldRow = goldPile->row;
    int goldCol = goldPile->col;
    if (row == goldRow && col == goldCol) {
      addGold(player, pileAmount); //update player gold amount 
      char* goldMessage = malloc(50 * sizeof(char)); 
      int currPlayerGold = getPlayerGold(player);
      game->goldRemaining -= pileAmount;
      sprintf(goldMessage, "GOLD %d %d %d", pileAmount, currPlayerGold, game->goldRemaining);
      addr_t playerAddress = getPlayerAddress(player);
      message_send(playerAddress, goldMessage);
      //check if the spectator is present, if so we need to update their banner
      if (game->spectatorActive) {
        char playerID = getCharacterID(player);
        sprintf(goldMessage, "SPECTATOR_GOLD %c %d %d %d", playerID, pileAmount, currPlayerGold, game->goldRemaining);
        player_t* spectator = game->players[MaxPlayers-1];
        addr_t spectatorAddress = getPlayerAddress(spectator);
        message_send(spectatorAddress, goldMessage);
      }  
      free(goldMessage); 
      //check if all piles have been collected
      if (game->goldRemaining == 0) {
        printf("sending summary\n");
        sendGameSummary();
      }
      break;
    }
  }
}

/*
 * Given a row and col, put gold on the map
 * We assume that row col are valid "room cells" that are within
 * the bounds of the map
 */
void
spawnGold(int row, int col) 
{
  // get the current map
  setCellType(game->map, '*', row, col);
}

void
spawnPlayer(player_t* player, int row, int col) 
{
  char id = getCharacterID(player);
  setCellType(game->map, id, row, col);
  char** playerMap = getPlayerMap(player);
  playerMap[row][col] = '@';
}

/*
 * Sends the size of the grid to the client
 */

void
sendGrid(player_t* player, bool isSpectator) 
{
  //send the grid size to client
  char* sizeMessage = malloc(30 * sizeof(char));
  if (sizeMessage == NULL) {
    return;
  }
  int numRows = getNumRows(game->map);
  int numCols = getNumCols(game->map);
  //printf("%d %d", numRows, numCols); //testing
  /*
  if (isSpectator) {
    sprintf(sizeMessage, "SPECTATOR_GRID %d %d", numRows, numCols);
  } else {
    sprintf(sizeMessage, "GRID %d %d", numRows, numCols);
  }
  */
  sprintf(sizeMessage, "GRID %d %d", numRows, numCols);
  addr_t address = getPlayerAddress(player);
  message_send(address, sizeMessage);
  free(sizeMessage);
}

/*
 * This function is specifically used to call all
 * existing players to update their vision whenever a new 
 * player joins or when a player quits.
 */
void
updateCurrentPlayerVision()
{
  for (int i = 0; i < game->currentNumPlayers; i++) {
    player_t* player = game->players[i];
    bool playerActive = getPlayerActive(player);
    if (playerActive) {
      sendDisplay(player, false);
    }
  }
}

/*
 * Sends the map to the client (player)
 */
void 
sendDisplay(player_t* player, bool isSpectator)
{
  //send the actual grid to the client
  //char** grid = getGameGrid(game->map); //TESTING 
    
  //update the player's position on the map
  if (isSpectator == false) {
    updatePlayerPosition(player);
  }

  int numRows = getNumRows(game->map);
  int numCols = getNumCols(game->map);
  addr_t address = getPlayerAddress(player);

  char** grid; 
  if (isSpectator == false) {
    grid = getPlayerMap(player);
  } else {
    grid = getGameGrid(game->map);
  }
  int size = strlen("DISPLAY\n") + numRows * numCols + 1;
  char* gridMessage = malloc(size * sizeof(char));
  if (gridMessage == NULL) {
    return;
  }
  strcpy(gridMessage, "DISPLAY\n");
  int pos = strlen(gridMessage);
  for (int i = 0; i < numRows; i++) {
    for (int j = 0; j < numCols; j++) {
      gridMessage[pos++] = grid[i][j];
    }
  }
  gridMessage[pos] = '\0';
  //printf("here\n");
  //printf("%s\n", gridMessage); //testing messages
  //printMap(game->map);
  message_send(address, gridMessage);

  //free pointers
  free(gridMessage);
}

/*
 * Initializes the player map based on their starting visible region
 */
char**
initializePlayerMap(int row, int col) 
{
  int numRows = getNumRows(game->map);
  int numCols = getNumCols(game->map);

  //create an empty grid
  char** grid = malloc(numRows * sizeof(char*));
  if (grid == NULL) {
    return NULL;
  }

  for (int row = 0; row < numRows; row++) {
    grid[row] = malloc(numCols * sizeof(char));
    memset(grid[row], ' ', numCols);
  }
  

  //set the visible region
  int** visibleRegion = getVisibleRegion(game->map, row, col);
  if (visibleRegion == NULL) {
    printf("ERROR\n");
    return NULL;
  }
  int size = 0;
  for (int row = 0; visibleRegion[row][0] != -1; row++) {
    int visibleRow = visibleRegion[row][0];
    int visibleCol = visibleRegion[row][1];
    grid[visibleRow][visibleCol] = getCellType(game->map, visibleRow, visibleCol);
    size++;
  }
  delete2DIntArr(visibleRegion, size+1);
  return grid;
}

/*
 * Create a spectator
 */

player_t* 
spectatorJoin(addr_t address, char* name)
{
  //check if a specatator has already joined
  if (game->spectatorActive) {
    return NULL;
  }
  player_t* spectator; 
  //initialize spectator's information
  spectator = player_new('A', game->map, NULL, 0, name, 0, 0, address);
  if (spectator == NULL) {
    fprintf(stderr, "error initializing spectator\n");
    return NULL;
  }
  game->players[MaxPlayers-1] = spectator; //put them at the end of the array
  game->spectatorActive = true;
  return spectator;
}

/*
 * Create a new player and add it to the array of players
 */

player_t*
playerJoin(addr_t address, char* name) 
{
  //create the player and add them to the game
  player_t* newPlayer;
  int currentNumPlayers = game->currentNumPlayers;
  if (currentNumPlayers < MaxPlayers-1) {
    //initialize player's information
    char id = 'A' + game->currentNumPlayers;

    //spawn the player 
    srand(time(NULL)); //seed the random number generator
    int** roomCells = getRoomCells(game->map);
    // get number of room cells
    int numRoomCells = 0;
    while (roomCells[numRoomCells][0] != -1) {
      numRoomCells++;
    }

    srand(time(NULL)); //seed random number generator
    int randomCell = rand() % numRoomCells;

    //get row and col for index the player is spawned at 
    int row = roomCells[randomCell][0];
    int col = roomCells[randomCell][1];

    //create player map
    char** playerMap = initializePlayerMap(row, col);
    //initialize the player
    newPlayer = player_new(id, game->map, playerMap, 0, name, row, col, address);
    if (newPlayer == NULL) {
      fprintf(stderr, "error initializing player\n");
      return NULL;
    }
    spawnPlayer(newPlayer, row, col);
    printf("new player has been added\n");

    delete2DIntArr(roomCells, numRoomCells+1);
    // add player to array of players after their setup is done
    game->players[currentNumPlayers] = newPlayer; 
    game->currentNumPlayers++;
  } else {
    //space is full
    return NULL;
  }
  return newPlayer;
}

/*
 * Check if a player exists, if they do then return that player
 */

player_t* 
checkPlayerJoined(addr_t address) 
{
  //check if the player has already joined the gmae
  for (int i = 0; i < game->currentNumPlayers; i++) {
    player_t* player = game->players[i];
    addr_t playerAddress = getPlayerAddress(player);
    if (message_eqAddr(playerAddress, address)) {
      return player;
    }
  }
  return NULL;
}

/*
 * Removes player from map and makes their status inactive
 */
void
playerQuit(player_t* player) 
{
  //remove player from map
  int playerRow = getPlayerRow(player);
  int playerCol = getPlayerCol(player);

  restoreCell(game->map, playerRow, playerCol); //turns the playerID back to the terrain

  //make player inactive
  setPlayerInactive(player);

  //update all the ACTIVE player's vision 
  updateCurrentPlayerVision();

  //send quit message to client 
  addr_t playerAddress = getPlayerAddress(player);
  message_send(playerAddress, "QUIT Thanks for playing!");
}

/* 
 * Remove spectator 
 */
void
spectatorQuit(player_t* spectator) {
  game->spectatorActive = false;

  addr_t spectatorAddress = getPlayerAddress(spectator);
  message_send(spectatorAddress, "QUIT Thanks for watching!");
  player_delete(spectator);
}

/* 
 * Sends the summary of the game to the clients 
 */
void
sendGameSummary() 
{
  char* gameOverMessage = malloc(1000 * sizeof(char));
  strcpy(gameOverMessage, "QUIT GAME OVER:\n");
  
  //create the messsage
  char buffer[25];
  int offset = strlen(gameOverMessage);
  for (int i = 0; i < game->currentNumPlayers; i++) {
    player_t* player = game->players[i];
    char playerID = getCharacterID(player);
    int playerGold = getPlayerGold(player);
    char* playerName = getPlayerName(player);
    int len = sprintf(buffer, "%c          %d %s\n", playerID, playerGold, playerName);
    strcpy(gameOverMessage + offset, buffer); // append the line
    offset += len; // move offset over for next append
  }
  printf("test: %s\n", gameOverMessage);

  //send the message to active players
  for (int i = 0; i < game->currentNumPlayers; i++) {
    player_t* player = game->players[i];
    bool playerActive = getPlayerActive(player);
    addr_t playerAddress = getPlayerAddress(player);
    if (playerActive) {
      message_send(playerAddress, gameOverMessage);
    }
  }
  free(gameOverMessage);
  cleanUpGame();
}


/*
 * Clean up the game by freeing any allocated memory
 */
void
cleanUpGame() 
{
  //free any dynamically allocated data for each player that joined the game
  for (int i = 0; i < game->currentNumPlayers; i++) {
    player_t* player = game->players[i];
    if (player != NULL) {
      player_delete(player);
    }
  }

  //free the spectator if they are in the game
  if(game->spectatorActive) {
    player_t* spectator = game->players[MaxPlayers-1];
    spectatorQuit(spectator);
  }

  free(game->players);
  
  //free the gold piles
  for (int i = 0; i < game->numGoldPiles; i++) {
    if (game->goldPiles[i] != NULL) {
      free(game->goldPiles[i]);
    }
  }
  free(game->goldPiles);

  //free the bigger structs
  deleteGameMap(game->map);

  //free the game itself
  
  free(game);
  message_done();
  exit(0);
}
