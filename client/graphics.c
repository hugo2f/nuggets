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

static void setupScreenSize();
static void moveToNormalBannerEnd();

Display display = {0, 0, 0};

bool
init_curses(int nrows, int ncols)
{
    initscr();

    setupScreenSize(nrows, ncols);

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

static void 
setupScreenSize(int nrows, int ncols)
{
    int nrowsScreen = 0; 
    int ncolsScreen = 0;

    while (nrows >= nrowsScreen || ncols >= ncolsScreen) {
        struct winsize w;
        ioctl(0, TIOCGWINSZ, &w);
        nrowsScreen = w.ws_row;
        ncolsScreen = w.ws_col;
        
        endwin();
        clear();
        refresh();
        printw("Please expand your terminal window and press any key to procede\n");
        refresh();
        getch();
    }

    display.nrowsScreen = nrowsScreen;
    display.ncolsScreen = ncolsScreen;
    display.nrows = nrows;
    display.ncols = ncols;
}

void
display_banner(char playerSymbol, int playerNuggets, int unclaimedNuggets)
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
append_to_banner(char* message) 
{
    moveToNormalBannerEnd();

    printw("%s", message);

    refresh();
}

void 
remove_from_banner()
{
    moveToNormalBannerEnd();
    
    for (int i = 0; i <= display.ncolsScreen; i++) {
        delch();
    }
    
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
end_curses() 
{
    endwin();
}