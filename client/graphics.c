#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <signal.h>
#include <ncurses.h>
#include <sys/ioctl.h>
#include "graphics.h"

typedef struct {
    int nrowsScreen;
    int ncolsScreen;
    int nrows;
    int ncols;
} Display;

static void setupScreenSize(int nrows, int ncols);
static void appendToBanner(char* message);
static void moveToNormalBannerEnd();

Display display = {0, 0, 0, 0};

bool
init_curses(int nrows, int ncols)
{
    setupScreenSize(nrows, ncols);
    
    initscr();

    cbreak();
    noecho();
    nodelay(stdscr, TRUE);
    keypad(stdscr, TRUE);
    flushinp();

    start_color();
    init_pair(1, COLOR_WHITE, COLOR_BLACK);
    attron(COLOR_PAIR(1));

    return true;
}

void 
display_map(char* map) 
{
    int x = 0;
    int y = 1;

    for (int i = 0; i < strlen(map); i++) {
        char c = map[i];

        if (x >= display.ncols) {
            x = 0;
            y++;
        }

        if (y >= display.nrows + 1) {
            break;
        }

        mvaddch(y, x, c);
        x++;
    }

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
    
    char banner[display.ncolsScreen];
    snprintf(banner, sizeof(banner), "Player %c has %d nuggets (%d nuggets unclaimed).", playerSymbol, playerNuggets, unclaimedNuggets);
    mvprintw(0, 0, "%s", banner);
    
    refresh(); 
}

void 
display_spectator_banner()
{
    move(0, 0);
    clrtoeol();
    mvprintw(0, 0, "Spectating");
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
indicate_nuggets_collected(const int collected) 
{
    char message[45];
    snprintf(message, sizeof(message), "You collected %d nuggets!", collected);

    remove_indicator();
    appendToBanner(message);
}

void 
remove_indicator()
{
    moveToNormalBannerEnd();
    
    for (int i = 0; i <= display.ncolsScreen; i++) {
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

    display.nrowsScreen = nrowsScreen;
    display.ncolsScreen = ncolsScreen;
    display.nrows = nrows;
    display.ncols = ncols;
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