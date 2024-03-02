/*
 * graphics.c
 *
 * Description: contains all the displaying and keyboard reading functions for the client. It initializes
 * curses, ensuring valid dimensions, displays information on a top banner depending on whether the client
 * is a spectator or player. It also displays various notifications on the banner (directly after it). It
 * reads keyboard input and provdes an exit function.
 * 
 * Author: Joseph Hirsh
 * Date: March 1st, 2024
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <signal.h>
#include <ncurses.h>
#include <sys/ioctl.h>
#include "graphics.h"
#include "clientdata.h"

// function prototypes
static void setupScreenSize(int nrows, int ncols);
static void appendToBanner(char* message);
static void moveToNormalBannerEnd();

/*
 * Initialize curses; see .h for more details. 
 */
bool
init_curses(int nrows, int ncols)
{
    // ensures window size is large enough, prompts user to expand it and waits if not
    setupScreenSize(nrows, ncols);
    
    // initialize the curses library
    initscr();

    // enable character input to be read one character at a time
    cbreak();

    // disable automatic echoing of characters typed by the user
    noecho();

    // enable non-blocking input mode
    nodelay(stdscr, TRUE);

    // enable the keypad for interpreting special keys
    keypad(stdscr, TRUE);

    // flush any pending input
    flushinp();

    // start color mode
    start_color();

    // initialize color pair one with white foreground and black background
    init_pair(1, FOREGROUND_COLOR, BACKGROUND_COLOR);

    // enable attributes with color pair 1
    attron(COLOR_PAIR(1));

    // return true to indicate successful initialization
    return true;
}

/*
 * Display map on screen; see .h for more details. 
 */
void 
display_map(char* map) 
{
    // starting cursor location
    int x = 0;
    int y = 1; // leaves top row for banner

    // iterates over map
    for (int i = 0; i < strlen(map); i++) {
        // gets current character
        char c = map[i];

        // if end of row reached, return
        if (x >= client.ncolsBoard) {
            x = 0;
            y++;
        }

        // if end of available board space reached, end
        if (y >= client.nrowsBoard + 1) {
            break;
        }

        // print character
        mvaddch(y, x, c);

        // advance
        x++;
    }

    // update display with new changes
    refresh();
}

void
display_player_banner(char playerSymbol, int playerNuggets, int unclaimedNuggets)
{
    if (playerSymbol == '\0') {
        return;
    }
    
    move(0, 0);
    clrtoeol();
    
    char banner[client.ncolsScreen];
    snprintf(banner, sizeof(banner), "Player %c has %d nuggets (%d nuggets unclaimed).", playerSymbol, playerNuggets, unclaimedNuggets);
    mvprintw(0, 0, "%s", banner);
    
    refresh(); 
}

void 
display_spectator_banner(const int remaining)
{
    move(0, 0);
    clrtoeol();
    mvprintw(0, 0, "Spectating: %d gold remaining.", remaining);
    refresh();
}

void 
indicate_invalid_key(const char key) 
{
    char message[20];
    snprintf(message, sizeof(message), "Invalid keystroke %c", key);

    remove_indicator();
    appendToBanner(message);
}

void
indicate_nuggets_collected_player(const int collected) 
{
    char message[45];
    snprintf(message, sizeof(message), "You collected %d nuggets!", collected);

    remove_indicator();
    appendToBanner(message);
}

void
indicate_nuggets_collected_spectator(const char symbol, const int collected)
{
    char message[45];
    snprintf(message, sizeof(message), "Player %c collected %d nuggets!", symbol, collected);

    remove_indicator();
    appendToBanner(message);
}

void 
remove_indicator()
{
    moveToNormalBannerEnd();
    
    for (int i = 0; i <= client.ncolsScreen; i++) {
        delch();
    }
    
    refresh();
}

char
get_character()
{
    return getch();
}

void
end_curses() 
{
    endwin();
}

static void 
setupScreenSize(int nrows, int ncols)
{
    struct winsize w;
    ioctl(0, TIOCGWINSZ, &w);
    int nrowsScreen = w.ws_row;
    int ncolsScreen = w.ws_col;

    if (nrows > nrowsScreen || ncols > ncolsScreen) {
        printf("Expand window to %d rows by %d columns\n", nrows, ncols);
        fflush(stdout);
    }
    
    while (nrows >= nrowsScreen || ncols >= ncolsScreen) {
        struct winsize w;
        ioctl(0, TIOCGWINSZ, &w);
        nrowsScreen = w.ws_row;
        ncolsScreen = w.ws_col;
    }

    client.nrowsScreen = nrowsScreen;
    client.ncolsScreen = ncolsScreen;
}

static void 
appendToBanner(char* message) 
{
    moveToNormalBannerEnd();

    printw("%s", message);

    refresh();
}

static void
moveToNormalBannerEnd() 
{
    move(0, 0);
    
    int y, x;
    getyx(stdscr, y, x);
    
    char ch;
    while ((ch = inch()) != '.') {
        x++;
        move(y, x);
    }

    x++;
    move(y, ++x);
}