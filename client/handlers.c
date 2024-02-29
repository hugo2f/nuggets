#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "clientdata.h"
#include "handlers.h"
#include "graphics.h"
#include "validators.h"
#include "indicators.h"


void 
handle_okay(char* symbol) 
{
    int errors = 0;
    if (client.state != CLIENT_START_SENT) {
        fprintf(stderr, "Received OK again or prior to sending START\n");
        errors++;
    }

    if (strlen(symbol) > 1) {
        fprintf(stderr, "Received player symbol with multiple characters, attempting to use first\n");
    }

    char symbolCharacter = *symbol;
    
    if (!validate_player_symbol(symbolCharacter)) {
        fprintf(stderr, "Received invalid player symbol\n");
        errors++;
    }

    if (errors != 0) {
        return;
    }

    client.playerSymbol = symbolCharacter;

    client.state = CLIENT_OKAY_RECEIVED;
}

void 
handle_grid(char* coordinates) 
{
    int errors = 0;
    if (client.state != CLIENT_OKAY_RECEIVED) {
        fprintf(stderr, "Received GRID prior to receiving OK\n");
        errors++;
    }

    int nrows, ncols;
    if (sscanf(coordinates, "%d %d", &nrows, &ncols) != 2) {
        fprintf(stderr, "GRID message missing data\n");
        errors++;
    }

    if (errors != 0) {
        return;
    }

    init_curses(nrows, ncols);

    client.nrows = nrows;
    client.ncols = ncols;

    if (client.playerName != NULL) {
        display_player_banner(client.playerSymbol, 0, 0);
    } else {
        display_spectator_banner();
    }

    client.state = CLIENT_GRID_RECEIVED;
}

void 
handle_gold(char* counts) 
{
    if (client.state != CLIENT_PLAY) {
        fprintf(stderr, "Received GOLD prior to game start\n");
        return;
    }

    int collected, current, remaining;
    if (sscanf(counts, "%d %d %d", &collected, &current, &remaining) != 3) {
        fprintf(stderr, "GOLD message missing data\n");
        return;
    }

    int errors = 0;
    if (!validate_gold_count(collected, MAXIMUM_GOLD)) {
        fprintf(stderr, "Invalid 'collected' gold count %d\n", collected);
        errors++;
    }

    if (!validate_gold_count(current, MAXIMUM_GOLD)) {
        fprintf(stderr, "Invalid 'current' gold count %d\n", current);
        errors++;
    }

    if (!validate_gold_count(remaining, MAXIMUM_GOLD)) {
        fprintf(stderr, "Invalid 'remaining' gold count %d\n", remaining);
        errors++;
    }

    if (errors != 0) {
        return;
    }

    display_player_banner(client.playerSymbol, current, remaining);
    indicate_nuggets_collected(collected);
}

void 
handle_display(char* map) 
{
    if (client.state != CLIENT_GRID_RECEIVED && client.state != CLIENT_PLAY) {
        fprintf(stderr, "Received DISPLAY prior to receiving GRID\n");
        return;
    }

    display_map(map);

    if (client.state != CLIENT_PLAY) {
        client.state = CLIENT_PLAY;
    }
}

void 
handle_quit(char* explanation) 
{
    end_curses();
    printf("QUIT %s\n", explanation);
    fflush(stdout);
    exit(EXIT_SUCCESS);
}

void 
handle_error(char* error) 
{
    fprintf(stderr, "ERROR %s\n", error);
}