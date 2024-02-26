#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <signal.h>
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
display_banner(char playerSymbol, int playerNuggets, int unclaimedNuggets, char* additional)
{
    if (playerSymbol == '\0') {
        return;
    }
    
    move(0, 0);
    clrtoeol();
    
    char banner[200];
    sprintf(banner, "Player %c has %d nuggets (%d nuggets unclaimed). %s\n", playerSymbol, playerNuggets, unclaimedNuggets, additional);
    mvprintw(0, 0, "%s", banner);
    
    refresh(); 
}

void 
append_to_banner(char* message, int ncols) 
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
    move(y, x);

    printw(" %s", message);
    refresh();
}

void 
display_board(char* board, int nrows, int ncols) 
{
    int x = 0;
    int y = 1;

    for (int i = 0; i < strlen(board); i++) {
        char c = board[i];

        if (x >= ncols) {
            x = 0;
            y++;
        }

        if (y >= nrows) {
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