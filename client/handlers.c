/*
 * handlers.c
 *
 * Description: contains functions that handle individual commands, namely OK, GRID, GOLD, DISPLAY, QUIT,
 * and ERROR.
 * 
 * Author: Joseph Hirsh
 * Date: March 1st, 2024
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "clientdata.h"
#include "handlers.h"
#include "graphics.h"
#include "validators.h"

// function prototype
int parseGoldCounts(char* counts, int* collected, int* current, int* remaining); 

/*
 * Runs upon receiving message from server with the OK header; see .h for more details.
 */
void 
handle_okay(char* symbol) 
{
    int errors = 0; // stores number of errors so function can accumulate multiple errors before exiting
    
    // errors if client attempts to handle OK before it sends START 
    if (client.state != START_SENT) {
        fprintf(stderr, "Received OK again or prior to sending START\n");
        errors++;
    }

    // logs warning if the symbol received from server is more than one character
    if (strlen(symbol) > 1) {
        fprintf(stderr, "Received player symbol with multiple characters, attempting to use first\n");
    }

    // grabs the first character off the symbol string (which ideally would already be one character)
    char symbolCharacter = *symbol;
    
    // validates the symbol (ensures that it is alphabetic and captitalized)
    if (!validate_player_symbol(symbolCharacter)) {
        fprintf(stderr, "Received invalid player symbol\n");
        errors++;
    }

    // ceases execution if error encountered
    if (errors != 0) {
        return;
    }

    // otherwise, sets the client symbol
    client.playerSymbol = symbolCharacter;

    // and advances to the next client state 
    client.state = OK_RECEIVED;
}

/*
 * Runs upon receiving message from server with the GRID header; see .h for more details.
 */
void 
handle_grid(char* coordinates) 
{
    int errors = 0; // stores number of errors so function can accumulate multiple errors before exiting
    
    // errors if client attempts to handle GRID before it handles OK 
    if (client.state != OK_RECEIVED) {
        fprintf(stderr, "Received GRID prior to receiving OK or for a second time\n");
        errors++;
    }

    int nrows, ncols; // nrows = number of rows of game map, ncols = number of columns of game map
    
    // extracts values from the coordinates string, and ensures that two values were extracted, erroring if not
    if (sscanf(coordinates, "%d %d", &nrows, &ncols) != 2) {
        fprintf(stderr, "GRID message bad data\n");
        errors++;
    }

    // ceases execution if error encountered
    if (errors != 0) {
        return;
    }

    // otherwise, it initializes curses
    init_curses(nrows, ncols);

    // sets client nrows and client ncols which are global and used elswhere 
    client.nrowsMap = nrows;
    client.ncolsMap = ncols;

    // advance game state
    client.state = GRID_RECEIVED;
}

/*
 * Runs upon receiving message from server with the GOLD_REMAINING header; see .h for more details.
 */
void
handle_starting_gold_remaining(char* startingGoldRemainingString)
{
    int errors = 0; // stores number of errors so function can accumulate multiple errors before exiting
    
    // ensure that client already received GRID
    if (client.state != GRID_RECEIVED) {
        fprintf(stderr, "Received GOLD_REMAINING prior to GRID or for a second time\n");
        errors++;
    }
    
    // extract gold remaining number
    int startingGoldRemaining;
    if (sscanf(startingGoldRemainingString, "%d", &startingGoldRemaining) != 1) {
        fprintf(stderr, "GOLD_REMAINING message bad data\n");
        errors++;
    }

    if (!validate_gold_count(startingGoldRemaining, MAXIMUM_GOLD)) {
        fprintf(stderr, "Invalid 'startingGoldRemaining' gold count");
        errors++;
    }

    // cease execution if error occurred
    if (errors > 0) {
        return;
    }
    
    // depending on the client mode (player or spectator), display banner
    if (client.playerName != NULL) {
        display_player_banner(client.playerSymbol, 0, startingGoldRemaining);
    } else {
        display_spectator_banner(startingGoldRemaining);
    }
    
    // advance client state
    client.state = GOLD_REMAINING_RECEIVED;
}

/*
 * Runs upon receiving message from server with the GOLD header; see .h for more details.
 */
void 
handle_player_gold(char* counts) 
{
    int errors = 0; // stores number of errors so function can accumulate multiple errors before exiting

    // ensure that client is currently running a game session (not initializing)
    if (client.state != PLAY) {
        fprintf(stderr, "Received GOLD prior to game start\n");
        errors++;
    }

    // ensure that client is a player 
    if (client.playerName == NULL) {
        fprintf(stderr, "Received GOLD as Spectator\n");
        errors++;
    }

    // extracts values from the gold data string, and ensures that three values were extracted, erroring if not
    int collected, current, remaining;
    errors += parseGoldCounts(counts, &collected, &current, &remaining);

    // ceases execution if error occured
    if (errors > 0) {
        return;
    }
    
    // update banner with gold statistics
    display_player_banner(client.playerSymbol, current, remaining);
    
    // show message indicating that player collected nuggets if collected is not zero
    if (collected != 0) {
        indicate_nuggets_collected_player(collected);
    }
}

/*
 * Runs upon receiving message from server with the SPECTATOR_GOLD header; see .h for more details.
 */
void
handle_spectator_gold(char* collectionData)
{
    int errors = 0; // stores number of errors so function can accumulate multiple errors before exiting

    // ensure that client is currently running a game session (not initializing)
    if (client.state != PLAY) {
        fprintf(stderr, "Received SPECTATOR_GOLD prior to PLAY\n");
        errors++;
    }

    // ensure that client is a player 
    if (client.playerName != NULL) {
        fprintf(stderr, "Received SPECTATOR_GOLD as Player\n");
        errors++;
    }

    // extract symbol and everything else
    char counts[100];
    char collectorSymbol;
    if (sscanf(collectionData, "%c%[^\n]", &collectorSymbol, counts) != 2) {
        fprintf(stderr, "SPECTATOR_GOLD message bad data\n");
        return;
    }

    // ensure that the symbol is valid (capital and alphabetical)
    if (!validate_player_symbol(collectorSymbol)) {
        fprintf(stderr, "SPECTATOR_GOLD message contains invalid player symbol\n");
        errors++;
    }

    // extracts values from the gold data string, and ensures that three values were extracted, erroring if not
    int collected, current, remaining;
    errors += parseGoldCounts(counts, &collected, &current, &remaining);

    // ceases execution if error occured
    if (errors > 0) {
        return;
    }
    
    // update banner with gold statistics
    display_spectator_banner(remaining);
    
    // show message indicating that player collected nuggets
    indicate_nuggets_collected_spectator(collectorSymbol, collected);
}
/*
 * Runs upon receiving message from server with the STOLEN header; see .h for more details.
 */
void 
handle_stolen(char* stealData)
{
    int errors = 0; // stores number of errors so function can accumulate multiple errors before exiting

    // ensure that client is currently running a game session (not initializing)
    if (client.state != PLAY) {
        fprintf(stderr, "Received STOLEN prior to PLAY\n");
        errors++;
    }
    
    // extract stolenPlayerSymbol, stealerPlayerSymbol, and amountStolen from stealData
    char stolenPlayerSymbol, stealerPlayerSymbol;
    int amountStolen;
    if (sscanf(stealData, " %c %c %d", &stolenPlayerSymbol, &stealerPlayerSymbol, &amountStolen) != 3) {
        fprintf(stderr, "STOLEN message bad data\n");
        return;
    }

    // ensure amountStolen is valid
    if (!validate_gold_count(amountStolen, client.maximumGold)) {
        fprintf(stderr, "Invalid 'amountStolen' gold count\n");
        errors++;
    }

    // ensure stolenPlayerSymbol is valid
    if (!validate_player_symbol(stolenPlayerSymbol)) {
        fprintf(stderr, "Invalid 'stolenPlayerSymbol' (the player stolen from)\n");
        errors++;
    }

    // ensure stealerPlayerSymbol is valid
    if (!validate_player_symbol(stealerPlayerSymbol)) {
        fprintf(stderr, "Invalid 'stealerPlayerSymbol' (the player who stole)\n");
        errors++;
    }
    
    // ensure stolenPlayerSymbol and stealerPlayerSymbol are not the same player
    if (stolenPlayerSymbol == stealerPlayerSymbol) {
        fprintf(stderr, "Someone cannot steal from themselves\n");
        errors++;
    }

    // cease execution if error occurred
    if (errors > 0) {
        return;
    }

    // display different indication message according to if...
    if (client.playerName != NULL) {
        // someone stole from you or
        if (client.playerSymbol == stolenPlayerSymbol) {
            indicate_someone_stole_nuggets_from_you(stealerPlayerSymbol, amountStolen);
        } 
        // you stole from someone else or
        else if (client.playerSymbol == stealerPlayerSymbol) {
            indicate_you_stole_nuggets_from_someone(stolenPlayerSymbol, amountStolen);
        }
    } 
    // you are a spectator
    else {
        indicate_nuggets_stolen_spectator(stolenPlayerSymbol, stealerPlayerSymbol, amountStolen);
    }
}

/*
 * Runs upon receiving message from server with the DISPLAY header; see .h for more details.
 */
void 
handle_display(char* map) 
{
    // ensure that DISPLAY recevied either after GRID received or a during game session
    if (client.state != GOLD_REMAINING_RECEIVED && client.state != PLAY) {
        fprintf(stderr, "Received DISPLAY prior to receiving GOLD_REMAINING\n");
        return;
    }

    // prints map to display
    display_map(map);

    // advance client status iff game not yet started
    if (client.state != PLAY) {
        client.state = PLAY;
    }
}

/*
 * Runs upon receiving message from server with the QUIT header; see .h for more details.
 */
void 
handle_quit(char* explanation) 
{
    end_curses();
    printf("%s\n", explanation);
    fflush(stdout);
    exit(EXIT_SUCCESS);
}

/*
 * Runs upon receiving message from server with the ERROR header; see .h for more details.
 */
void 
handle_error(char* error) 
{
    fprintf(stderr, "ERROR %s\n", error);
}

int 
parseGoldCounts(char* counts, int* collected, int* current, int* remaining) 
{
    int errors = 0; // stores number of errors so function can accumulate multiple errors before exiting

    // extracts collected, current, and remaining from counts into "collected", "current", and "remaining" parameters
    if (sscanf(counts, "%d %d %d", collected, current, remaining) != 3) {
        fprintf(stderr, "Gold data missing\n");
        errors++;
        return errors;
    }
    
    // errors if the collected gold count received is unrealistic 
    if (!validate_gold_count(*collected, client.maximumGold)) {
        fprintf(stderr, "Invalid 'collected' gold count\n");
        errors++;
    }

    // errors if the current gold count received is unrealistic  
    if (!validate_gold_count(*current, client.maximumGold)) {
        fprintf(stderr, "Invalid 'current' gold count\n");
        errors++;
    }

    // errors if the remaining gold count received is unrealistic 
    if (!validate_gold_count(*remaining, client.maximumGold)) {
        fprintf(stderr, "Invalid 'remaining' gold count\n");
        errors++;
    }

    return errors;
}