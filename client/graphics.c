#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>

static void
init_curses(int nrows, int ncols)
{
    initscr();
    getmaxyx(stdscr, nrows, ncols);

    cbreak();
    noecho();

    start_color();
    init_pair(1, COLOR_WHITE, COLOR_BLACK);
    attron(COLOR_PAIR(1));
}

static void
display_banner(const char* playerName, int playerNuggets, int unclaimedNuggets, char* additional)
{
    move(0, 0);
    clrtoeol();

    mvprintw(0, 0, "Player %s has %d nuggets (%d nuggets unclaimed). %s", playerName, playerNuggets, unclaimedNuggets, additional);

    refresh();
}

static void
display_board(char* board, int nrows, int ncols)
{
    for (int y = 0; y < nrows; y++) {
        for (int x = 0; x < ncols; x++) {
            move(y + 1, x);
            addch(board[y * nrows + x]);
        }
    }

    refresh();
}