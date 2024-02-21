# CS50 Nuggets
## Design Spec
### team temporary, winter, 2024

Roles:

* Joe - Client
* Hugo - Map
* Jaysen + Colin - Server

According to the [Requirements Spec](REQUIREMENTS.md), the Nuggets game requires two standalone programs: a client and a server. Our design also includes a `map` module to encapsulate the data structure storing the game map. We describe each program and module separately. We do not describe the `support` library nor the modules that enable features that go beyond the spec. We avoid repeating information that is provided in the requirements spec.

## Client
The *client* acts in one of two modes:

 1. *spectator*, the passive spectator mode described in the requirements spec.
 2. *player*, the interactive game-playing mode described in the requirements spec.

### User interface

See the requirements spec for both the command-line and interactive UI. See under "Inputs and outputs" for display information. See graphics module function decomposition for more specific display design. 

### Inputs and outputs

| keystroke | Action                                      |
| --------- | ------------------------------------------- |
| `Q`       | quit the game                               |
| `h`       | move left, if possible                      |
| `l`       | move right, if possible                     |
| `j`       | move down, if possible                      |
| `k`       | move up, if possible                        |
| `y`       | move diagonally up and left, if possible    |
| `u`       | move diagonally up and right, if possible   |
| `b`       | move diagonally down and left, if possible  |
| `n`       | move diagonally down and right, if possible |

We maintain a log in stderr containing the following:
1. Client name
2. Client networking information (IP and port)
3. Client mode (spectator or player)
4. Time connected to server
5. Time disconnected from server
6. Any malformed packets received
7. Keystrokes (for replicating errors)
8. Error messages (including failed allocations, invalid function parameters, display too small for the grid, etc.)

We display the following on the console:
1. Status line (on the first line) of the form `Player %s has %d nuggets (%d nuggets unclaimed). %s`
2. Game grid (on the subsequent lines)
3. Game summary (instead of game grid) after game over

### Functional decomposition into modules

The main client script will use the following modules:
1. *message*, a UDP based messaging module containing functions to communicate with the server
2. *graphics*, contains functions for updating the status line, printing the grid, and printing the game over message 

Main Module:
* `parseArgs`
* `handlePlayerInput`
* `handleSpectatorInput`
* `handleMessage`
* `handleQuit`
* `handleError`
* `sendStartSpectator`
* `sendStartPlayer`
* `sendKey`
* `handleOkay`
* `handleGrid`
* `handleGold`
* `handleDisplay`

Graphics:
* `init_curses`
* `display_banner`
* `display_board`

Message: 
* (provided in support directory so not described in this design spec)

### Pseudocode for logic/algorithmic flow

#### main
The `main` function sets up the client, drives the loop, and exits cleanly.
```
execute from a command line per the requirement spec
initialize 'message' module via message_init
parse the command line via parseArgs
await and respond to server messages via message_loop
shut down message module via message_done
return status code according to message_loop result
```

#### parseArgs
The `parseArgs` function ensures that the command line arguments are valid, printing to stderr if not. It either updates variables dedicated to storing the validated arguments, or passes them directly to the functions that require them. 
```
if there are not three or four arguments
    print error
    exit with error code
attempt to pass args one and two to message_setAddr
if that fails
    print error
    exit with error code
if there are three arguments
    set player name to NULL
    call sendStartSpectator
    set handleInput function to handleSpectatorInput
else
    set player name to that argument
    call sendStartPlayer
    set handleInput function to handlePlayerInput
```
    
#### handlePlayerInput
The `handlePlayerInput` function responds to user input. It ignores garbage input, logs keystrokes, and sends data to the server.
```
perform defensive checks to ensure a valid correspondent
read a char from stdin
log char
if char represents a valid keystroke (see above)
    calls sendKey to send keystroke to server
    if char is Q
        calls sendKey(Q) to send quit message to server
        return true to stop looping
return false to continue looping
```
    
#### handleSpectatorInput
The `handleSpectatorInput` function responds to user input. It ignores garbage input, logs keystrokes, and sends data to the server. It only contains the functionality required for the spectator, not the player.
```
perform defensive checks to ensure a valid correspondent
read a char from stdin
log char
if char is Q
    calls sendKey(Q) to send quit message to server
    return true to stop looping
return false to continue looping
```

#### handleMessage
The `handleMessage` function responds to messages received by the server. It ignores and logs malformed packets, parses the message recevied, and calls upon the display module to update the display accordingly.
```
perform defensive checks to ensure non-NULL message
read first word of message
case "OK"
    calls handleOkay passing in rest of message
case "GRID"
    calls handleGrid passing in rest of message
case "GOLD"
    calls handleGold passing in rest of message
case "DISPLAY"
    calls handleDisplay passing in rest of message
case "QUIT"
    calls handleQuit passing in rest of message
case "ERROR" 
    calls handleError passing in rest of message
default
    ignores the message
```
    
        
#### handleQuit
The `handleQuit` function handles when a player quits from the application, but it still keeps track of the player's score.
```
perform defensive checks to make sure player exists
keeps the score kept in the scoreboard
frees all memory
```
    
#### handleError
The `handleError` function responds to errors within the server application. Logs these applications to stderr, and also sends a message to the individual clients of what went wrong. 
```
Recieves the char* error message
Logs this message to _stderr_
```
        
#### Helper functions

1. `sendStartSpectator` - used once at the start of the game, sends SPECTATOR to the server
2. `sendStartPlayer` - used once at the start of the game, sends PLAYER to the server 
4. `sendKey` - used after each keystroke, sends "KEY [key]" 
5. `handleOkay` - set player letter and allow client to await further communication
6. `handleGrid` - ensures the display is large enough for the grid (NR+1 x NC+1).
7. `handleGold` - performs validation and then calls graphics module to update gold count
8. `handleDisplay` - performs validation and then calls graphics module to update board

### graphics module 

#### init_curses
The `init_curses` function sets up the curses display, including the display size and colors.
```
initialize the screen
set the display size
ensure that keystrokes do not log to stdout
set up the colors
```

#### display_banner
The `display_banner` changes the banner at the top of the display. 
```
move to the top of the display
print out the new information
refresh
```

#### display_board
The `display_board` replaces the old board with the new board. 
```
loop through each character in the board grid
    replace it with the correct character in the new board
refresh
```
   
### Major data structures

The client uses no major data structures! 

---

## Server
### User interface

According to the requirements spec, the user interface is handled by the client. The client then sends the information to the server.

### Inputs and outputs

For inputs, the server takes in a map file and an optional seed.

The server outputs the port number for awaiting connections. 

The server logs every message sent to the server from a client and from the server to every client. 


### Functional decomposition into modules

* Game Module: 
* Network Module: The network module shall handle all network connections with clients (connection management and message communication)
* Player Module: Module that is used to keep track of player data (name, current score, current position, current visibility). It will also contain functions to initialize new players and delete an existing players.
* GameMap module: implements the data structure and functions related to the game map. 

Main Module:
* `parseArgs`

Game Module:
* `initializeGame`
* `loadMap`
* `spawnGold`
* `gameOver`
* `printScores`
* `sendSummaryToClients`
* `cleanUpGame`

Network Module
* `message_loop`
* `sendMessageToClients`
* `initalizeScoreboard`
* `updateScoreboard`

Player Module Functions:
* `initializePlayer`
* `movePlayer`
* `updatePlayerMap`
* `removePlayer`

### Pseudocode for logic/algorithmic flow

The server will run as follows:
```
execute from a command line per the requirement spec
call parseArgs
call initializeGame to set up data structures
initialize the 'message' module
print the port number on which we wait
call message_loop, to await clients
call gameOver to inform all clients the game has ended
clean up
```

#### parseArgs
```
Check to see if there are one or two arguments given
    if one arg given:
        set up port with given argument
        generate random seed 
    if two args given:
        set up port with given argument
        use given random seed
return whether it was successful
```

#### spawnGold
To prevent the unlikely (and unlucky) chance that randomly generated coordinates keep falling in non-room spots, we first create an array of empty spots in the map, then randomly choose spawnpoints from the array.
```
for each (row, col) pair in the map
    if the cell is an empty room spot
        store the (row, col) pair in an array
randomly divide total gold into piles
generate numGoldPiles distinct array indices
spawn gold piles at the corresponding cells
```

#### message_loop
```
while game is not over
    listen for messages from clients 
        parse the message
        update the player that sent the message accordingly,
        also update the gamestate
    send the update to all clients
end of loop
```

#### gameOver
```
compile and rank the scores 
send the game over summary to every client 
clean up the game 
exit
```

### Major data structures

#### player
We define a `player` struct to store each player's information, including:
1. `name`, a string storing a player chosen name
2. `ID`, a character (A-Z) representing the player in the map
3. `(row, col)`, a pair of coordinates storing the player's location in the map
4. `gold`, a number storing the gold a player has
5. `visibleMap`, a 2D array storing parts of the map that a player has seen or can currently see, not including players and gold piles (visible players and gold piles are processed in `getPlayerMap` in the `map` module)

---

## gamemap module

The `gamemap` module provides the data structure to store all map information. The contents will be stored as a 2D array of characters. Refer to the [Requirements Spec](REQUIREMENTS.md) for the types of characters in a map.

### Functional decomposition

We anticipate the following functions:
1. `loadMapFile`, which reads a map file and stores it in a `grid`
2. `deleteGameMap`, which frees all memory allocated for a GameMap
3. `deleteGrid`, which frees all memory allocated for a 2D `char` array
4. `getCellType`, which returns the type of cell corresponding to a coordinate in the map
5. `getPlayerMap`, which returns the map that is visible to a player, including visible players and gold piles
6. `getVisibleRegion`, which returns an array of the cells currently visible to a player
7. `getSpectatorMap`, which returns the map that a spectator sees, with all player and gold information

### Pseudocode for logic/algorithmic flow

#### loadMapFile
```
get number of rows and cols in the file
initialize 2D array of that size
for each line in the file
    for each character in the line
        store the character in the array
return the map
```

#### deleteGameMap/deleteGrid
Frees related memory for data structures used in the `gamemap` module. See the [implementation spec](./IMPLEMENTATION.md) for details.

#### getCellType
```
return the character stored in the map at a coordinate
```

#### getVisibleRegion
```
initialize an empty array of cells
for each cell in the map
    add the cell into the array if it is visible to the player
return the array
```


### Major data structures

#### grid & gameGrid
The `grid` will store a 2D array of characters representing the map, including solid rock, boundaries, empty room spots, and empty passage spots. It is basically an in-memory version of the map file.

The `gameGrid` is a copy of `grid`, but also stores where players and gold piles are. It contains all game information at each point in time, and is what the spectator sees.

---

## Testing Plan

In addition to the testing scenarios below, we will use Valgrind to ensure that there are no memory leaks or errors, except for those caused directly by curses. 

### client
1. Run with invalid IP and port and incorrect number of arguments. 
2. Send malformed packets and unexpected messages. 
3. Test with crazy keystroke combinations. 
4. Test in very small and very large windows.
5. Test resizing initially 
6. Potentially fuzz inputs

### server
1. Run with invalid ports and incorrect seed values and incorrect number of args 
2. Try with the miniclient
3. Send it malformed packets and unexpected messages
4. Log on too many users

### gamemap
1. Try to load and output from different map files, and compare the file vs. output
2. Simulate games with different numbers of players, and make sure the spectator sees everything