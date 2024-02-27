#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <strings.h>
#include <time.h>
#include "../support/message.h"
#include "../gamemap/gamemap.h"
//#include "../user.h"

static const int MaxNameLength = 50;   // max number of chars in playerName
static const int MaxPlayers = 27;      // maximum number of players
static const int GoldTotal = 250;      // amount of gold in the game
static const int GoldMinNumPiles = 10; // minimum number of gold piles
static const int GoldMaxNumPiles = 30; // maximum number of gold piles

/****************** local types *********************/
typedef struct player {
  char characterID;
  GameMap_t* playerMap;
  int gold;
  char* name;
  int row;
  int col;
  addr_t playerAddress;
} player_t;

typedef struct goldPile {
  int row;
  int col;
  int amount;
} goldPile_t;

typedef struct game {
  int currentNumPlayers;
  int numGoldPiles;
  player_t** players;
  goldPile_t** goldPiles;
  GameMap_t* map;
} game_t;

//global types
game_t* game;

//function prototypes
void initializeGame();
static bool handleMessage(void* arg, const addr_t from, const char* message);
void distributeGold();
void spawnGold(int rol, int col);
void spawnPlayer(player_t* player, int row, int col);
void callCommand(char key);
void updatePlayerMaps();
void sendGridToClient(player_t* player);
player_t* playerJoin(addr_t address, char* name);
player_t* checkPlayerJoined(addr_t address);

int 
main(int argc, char* argv[])
{ 
  //player_t* playerList[MaxPlayers];
  int current = 0;


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
  //TO DO:
  /*
   * put functions here and other stuff to initialize the game 
   */
  //load create the game
  //load the map
  game = malloc(sizeof(game_t));
  if (game == NULL) {
    return;
  }
  game->map = loadMapFile("../maps/main.txt");
  game->players = malloc(MaxPlayers * sizeof(player_t*));
  if (game->players == NULL) {
    return;
  }
  game->currentNumPlayers = 0;
  distributeGold();
}

static bool
handleMessage(void* arg, const addr_t from, const char* message)
{
  char command[10]; //store the command
  char* name = malloc(100 * sizeof(char));
  char* okMessage = malloc(10 * sizeof(char)); //used to store message that contains client's charID (sent to client)

  //TO DO:
  /*
   * check who sent the message
   * check if they exist in the array (loop through and use )
   * 
   */
  printf("%s\n", message);
  player_t* player;
  if (sscanf(message, "PLAY %s", name) == 1) {
    player = playerJoin(from, name); 
    printf("%s joined the game with an id %c\n", player->name, player->characterID);
    sprintf(okMessage, "OK %c", player->characterID);
    message_send(player->playerAddress, okMessage);
    sendGridToClient(player);
  } else if (sscanf(message, "KEY %s", command) == 1) {
    player = checkPlayerJoined(from); 
    if (player == NULL) {
      return false; //don't do anything -- keep running
    }
    char key = command[0];
    printf("received key %c from %s\n", key, player->name);
    callCommand(key);
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
void callCommand(char key) 
{ 
  switch (key) {
    case 'Q':
      exit(1);
    case 'h':
      printf("command for moving left\n");
      break;
    case 'l':
      printf("command for moving right\n");
      break;
    case 'j':
      printf("command for moving down\n");
      break;
    case 'k':
      printf("command for moving up\n");
      break;
    case 'y':
      printf("command for moving diagonally up and left\n");
      break;
    case 'u':
      printf("command for moving diagonally up and right\n");
      break;
    case 'b':
      printf("command for moving diagonally down and left\n");
      break;
    case 'n':
      printf("command for moving diagonally down and right\n");
      break;
    default: 
      printf("not a valid command\n");
      break;
  }
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

    //update the map
    spawnGold(row, col);
  }
  //update all player maps
  updatePlayerMaps();

  free(indices);
  delete2DIntArr(roomCells, size);
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
  setCellType(game->map, player->characterID, row, col);
  //setCellType(player->playerMap, '@', row, col);
  sendGridToClient(player);
}
/*
 * Update all player maps with updated gameGrid
 */
void
updatePlayerMaps()
{
  for (int i = 0; i < game->currentNumPlayers; i++) {
    player_t* player = game->players[i];
    player->playerMap = game->map;
  }
}

/*
 * Sends the size of the grid and the grid to the client
 */

void
sendGridToClient(player_t* player) 
{
  //send the grid size to client
  char* sizeMessage = malloc(15 * sizeof(char));
  if (sizeMessage == NULL) {
    return;
  }
  int numRows = getNumRows(player->playerMap);
  int numCols = getNumCols(player->playerMap);
  //printf("%d %d", numRows, numCols); //testing
  sprintf(sizeMessage, "GRID %d %d", numRows, numCols);
  message_send(player->playerAddress, sizeMessage);

  //send the actual grid to the client
  char** grid = getGameGrid(player->playerMap);
  int size = strlen("DISPLAY ") + numRows * numCols + 1;
  char* gridMessage = malloc(size * sizeof(char));
  if (gridMessage == NULL) {
    return;
  }
  strcpy(gridMessage, "DISPLAY ");
  int pos = strlen(gridMessage);
  for (int i = 0; i < numRows; i++) {
    for (int j = 0; j < numCols; j++) {
      gridMessage[pos++] = grid[i][j];
    }
  }
  gridMessage[pos] = '\0';
  //printf("here\n");
  //printf("%s\n", gridMessage); //testing messages
  printMap(game->map);
  message_send(player->playerAddress, gridMessage);

  //free pointers
  free(sizeMessage);
  free(gridMessage);
}

/*
 * Create a new player and add it to the array of players
 */

player_t*
playerJoin(addr_t address, char* name) 
{
  //create the player and add them to the game
  int currentNumPlayers = game->currentNumPlayers;
  player_t* newPlayer = malloc(sizeof(player_t));
  if (newPlayer == NULL) {
    printf("error\n");
    return NULL;
  }
  if (currentNumPlayers < MaxPlayers) {
    //TO DO: INITIALIZE ATTRIBUTES OF PLAYER
    char id = 'A' + game->currentNumPlayers;
    newPlayer->characterID = id; //set id
    newPlayer->name = name; //set name
    newPlayer->playerAddress = address; //set address
    //CREATE THE PLAYER MAp
    game->players[currentNumPlayers] = newPlayer; //add to array
    game->currentNumPlayers++;

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

    //get row and col for index
    int row = roomCells[randomCell][0];
    int col = roomCells[randomCell][1];
    spawnPlayer(newPlayer, row, col);
    printf("new player has been added\n");

    delete2DIntArr(roomCells, numRoomCells);
  } else {
    //space is full
    free(newPlayer);
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
    addr_t playerAddress = player->playerAddress;
    if (message_eqAddr(playerAddress, address)) {
      printf("addresses are equal\n");
      return player;
    }
  }
  return NULL;
}

/*
 * Clean up the game by freeing any allocated memory
 */
void
cleanUpGame() 
{
  //free any dynamically allocated data for each player in the game
  for (int i = 0; i < game->currentNumPlayers; i++) {
    player_t* player = game->players[i];
    free(player->name);
    free(player->playerMap);
    free(player);
  }
  
  //free the gold piles
  for (int i = 0; i < game->numGoldPiles; i++) {
    free(game->goldPiles[i]);
  }

  //free the bigger structs
  free(game->players);
  free (game->goldPiles);
  free(game->map);

  //free the game itself
  free(game);
}
