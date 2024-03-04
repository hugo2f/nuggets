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
void
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
        if (x >= client.ncolsMap) {
            x = 0;
            y++;
        }

        // if end of available board space reached, end
        if (y >= client.nrowsMap + 1) {
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

/*
 * Displays banner for client in player mode; see .h for more details. 
 */
void
display_player_banner(char playerSymbol, int playerNuggets, int unclaimedNuggets)
{
    // ensure the player symbol is not the null terminator (this should really never happen due to other 
    // defensive code)
    if (playerSymbol == '\0') {
        return;
    }
    
    
    move(0, 0); // move to top left of the screen
    clrtoeol(); // clear to end of line
    
    // create banner string
    char banner[client.ncolsScreen];
    snprintf(banner, sizeof(banner), "Player %c has %d nuggets (%d nuggets unclaimed).", playerSymbol, playerNuggets, unclaimedNuggets);
    
    // print banner string 
    mvprintw(0, 0, "%s", banner);
    
    // update display with new changes
    refresh(); 
}

/*
 * Displays banner for client in spectator mode; see .h file for more details.
 */
void 
display_spectator_banner(const int unclaimedNuggets)
{
    
    move(0, 0); // move to top left of the screen
    clrtoeol(); // clear to end of line
    mvprintw(0, 0, "Spectating: %d nuggets unclaimed.", unclaimedNuggets); // print banner string 
    refresh(); // update display with new changes
}

/*
 * Displays message after basic banner indicating invalid key press; see .h for more details. 
 */
void 
indicate_invalid_key(const char key) 
{
    // create invalid keystroke indicator message
    char message[20];
    snprintf(message, sizeof(message), "Invalid keystroke %c", key);

    // display it on first line after the basic banner
    remove_indicator(); // clears any current indicator messages
    appendToBanner(message);
}

/*
 * Displays message after basic banner indicating that you nuggets collected; see .h for more details. 
 */
void
indicate_nuggets_collected_player(const int collected) 
{
    // create you collected nuggets indicator message
    char message[50];
    snprintf(message, sizeof(message), "You collected %d nuggets!", collected);

    // display it on first line after the basic banner
    remove_indicator(); // clears any current indicator messages
    appendToBanner(message);
}

/*
 * Displays message after basic banner indicating someone else collected nuggets; see .h for more details. 
 */
void
indicate_nuggets_collected_spectator(const char symbol, const int collected)
{
    // create someone else collected nuggets indicator message
    char message[50];
    snprintf(message, sizeof(message), "Player %c collected %d nuggets!", symbol, collected);

    // display it on first line after the basic banner
    remove_indicator();  // clears any current indicator messages
    appendToBanner(message);
}

/*
 * Displays message after basic banner indicating you stole nuggets from someone; see .h for more details. 
 */
void 
indicate_you_stole_nuggets_from_someone(const char stolenPlayerSymbol, const int amount)
{
    // create you stole nuggets from someone indicator message
    char message[50];
    snprintf(message, sizeof(message), "You stole %d nuggets from player %c!", amount, stolenPlayerSymbol);

    // display it on first line after the basic banner
    remove_indicator();  // clears any current indicator messages
    appendToBanner(message);

}

/*
 * Displays message after basic banner indicating someone stole nuggets from you; see .h for more details. 
 */
void 
indicate_someone_stole_nuggets_from_you(const char stealerPlayerSymbol, const int amount)
{
    // create someone stole nuggets from you indicator message
    char message[50];
    snprintf(message, sizeof(message), "Player %c stole %d nuggets from you!", stealerPlayerSymbol, amount);

    // display it on first line after the basic banner
    remove_indicator();  // clears any current indicator messages
    appendToBanner(message);
}

/*
 * Displays message after basic banner indicating someone stole nuggets from someone else; see .h for more details. 
 */
void 
indicate_nuggets_stolen_spectator(const char stolenPlayerSymbol, const char stealerPlayerSymbol, const int amount)
{
    // create someone stole nuggets from someone else indicator message
    char message[50];
    snprintf(message, sizeof(message), "Player %c stole %d nuggets from player %c!", stealerPlayerSymbol, amount, stolenPlayerSymbol);

    // display it on first line after the basic banner
    remove_indicator();  // clears any current indicator messages
    appendToBanner(message);
}

/*
 * Removes whatever special indicator message is added after basic banner; see .h for more details.
 */
void 
remove_indicator()
{
    // moves cursor to banner end
    moveToNormalBannerEnd();
    
    // deletes everything after until end of screen
    for (int i = 0; i <= client.ncolsScreen; i++) {
        delch();
    }
    
    // update display with new changes
    refresh();
}

/*
 * Gets keypress; see .h for more details.
 */
char
get_character()
{
    return getch();
}

/*
 * Shuts down all graphics; see .h for more details.
 */
void
end_curses() 
{
    endwin();
}

/*
 * Ensures window size is sufficently large to accomodate the board, prompts user to expand window and
 * waits if not.
 */
static void 
setupScreenSize(int nrows, int ncols)
{
    // checks the size of the terminal window
    struct winsize w;
    ioctl(0, TIOCGWINSZ, &w); // aquire the terminal window size
    int nrowsScreen = w.ws_row; // number of rows in the terminal window
    int ncolsScreen = w.ws_col; // number of columns in the terminal window

    // If the expected size is larger than the actual terminal window size, print a message
    if (nrows > nrowsScreen || ncols > ncolsScreen) {
        printf("Expand window to %d rows by %d columns (current: %d x %d)\n",
               nrows, ncols, nrowsScreen, ncolsScreen); // prompts user to expand window
        fflush(stdout); // flush the output buffer
    }

    // loop continuously checking the window size until it is large enough
    while (nrows >= nrowsScreen || ncols >= ncolsScreen) {
        struct winsize w;
        ioctl(0, TIOCGWINSZ, &w); // reaquire the terminal window size
        nrowsScreen = w.ws_row; // update nrowsScreen for next check
        ncolsScreen = w.ws_col; // update ncolsScreen for next check
    }

    // after the loop, update the screen dimensions in the client structure
    client.nrowsScreen = nrowsScreen;
    client.ncolsScreen = ncolsScreen;
}

/*
 * Adds text to the end of the banner.
 */
static void 
appendToBanner(char* message) 
{
    // moves cursor to banner end
    moveToNormalBannerEnd();

    // prints message there
    printw("%s", message);

    // update display with new changes
    refresh();
}

/*
 * Moves cursor to end of banner (such that the next print or delete starts there).
 */
static void
moveToNormalBannerEnd() 
{
    // move to top left of the screen
    move(0, 0);
    
    // start x and y at zero
    int x = 0;
    int y = 0;

    // walk to end of banner where a period indicates the end
    char ch;
    while ((ch = inch()) != '.') {
        x++;
        move(y, x);
    }

    // take another two steps forward;
    x++;
    move(y, ++x);
}