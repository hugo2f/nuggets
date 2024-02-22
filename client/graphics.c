#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ncurses.h>

bool
init_curses(int nrows, int ncols)
{
    initscr();
    int nrowsScreen, ncolsScreen;
    getmaxyx(stdscr, nrowsScreen, ncolsScreen);

    if (nrows >= nrowsScreen || ncols >= ncolsScreen) {
        return false;
    }

    cbreak();
    noecho();

    start_color();
    init_pair(1, COLOR_WHITE, COLOR_BLACK);
    attron(COLOR_PAIR(1));

    return true;
}

void
display_banner(const char* playerName, int playerNuggets, int unclaimedNuggets, char* additional)
{
    move(0, 0);
    clrtoeol();
    
    char banner[200];
    sprintf(banner, "Player %s has %d nuggets (%d nuggets unclaimed). %s", playerName, playerNuggets, unclaimedNuggets, additional);
    mvprintw(0, 0, "%s", banner);
    
    refresh();
    
}

// TODO: see if this can be done using mods
void
display_board(char* board, int nrows, int ncols)
{
    int x = 0;
    int y = 0;
    
    for (int i = 0; i < strlen(board); i++) {
        char c = board[i];

        if (x >= ncols) {
            x = 0;
            y++;
        }

        if (y >= nrows) {
            clear();
            x = 0;
            y = 0;
        }

        mvaddch(y, x, c);
        refresh();
        x++;
    }
}

void
end_curses() 
{
    endwin();
}