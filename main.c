#include <ncurses.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "snake.h"

#define ESC 0x1b
#define MAX_SCORE_STR_SIZE 32

#define TITLE "Snake"
#define SNAKE_CHAR '*'
#define APPLE_CHAR 'O'

#define DELAY 150000 // high number <==> low speed && low number <==> high speed

typedef struct {
    int x, y;
    int width, height;
} WindowInfo;

const char *getScoreStr(int score);
bool isInsideWindow(WINDOW *win, Coordinate coordinate);
WINDOW *createBorder(int xWIn, int yWin, int widthWin, int heightWin);
void drawBorder(WINDOW *border);
void drawGameInfo(int score);
void drawGame(WINDOW *gameWindow, SnakeGame game);
WINDOW *drawGameOver(int score);

int main(void) {
    initscr();
    cbreak();
    noecho();
    curs_set(0);

    start_color();
    init_pair(1, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(2, COLOR_GREEN, COLOR_BLACK);
    init_pair(3, COLOR_RED, COLOR_BLACK);

    const int winWidth = getmaxx(stdscr);
    const int winHeight = getmaxy(stdscr);
    const Coordinate maxCoordinate = {
        .x = winWidth - 4 - 1,
        .y = winHeight - 6 - 1,
    };

    Coordinate startPoint = {
        .x = maxCoordinate.x / 4,
        .y = maxCoordinate.y / 2,
    };

    drawGameInfo(0);

    WINDOW *gameBorder =
        createBorder(2, 4, maxCoordinate.x + 1, maxCoordinate.y + 1);
    drawBorder(gameBorder);

    WINDOW *gameWindow = newwin(maxCoordinate.y + 1, maxCoordinate.x + 1, 4, 2);
    keypad(gameWindow, true);
    nodelay(gameWindow, true);
    wrefresh(gameWindow);

    SnakeGame game = initSnakeGame(startPoint, &maxCoordinate);
    drawGame(gameWindow, game);

    while (1) {
        int c = wgetch(gameWindow);

        if (c == ESC || c == 'Q' || c == 'q') {
            break;
        } else if (game.snake.direction != DOWN &&
                   (c == KEY_UP || c == 'W' || c == 'w')) {
            game.snake.direction = UP;
        } else if (game.snake.direction != LEFT &&
                   (c == KEY_RIGHT || c == 'D' || c == 'd')) {
            game.snake.direction = RIGHT;
        } else if (game.snake.direction != UP &&
                   (c == KEY_DOWN || c == 'S' || c == 's')) {
            game.snake.direction = DOWN;
        } else if (game.snake.direction != RIGHT &&
                   (c == KEY_LEFT || c == 'A' || c == 'a')) {
            game.snake.direction = LEFT;
        }

        Coordinate oldTail =
            game.snake.body.coordinates[game.snake.body.numCoordinates - 1];
        snakeMove(&game.snake);

        // Check if the head of the snake is not colliding with an apple or the
        // itself
        if (isInsideWindow(gameWindow, game.snake.body.coordinates[0]) &&
            isCollidingCoordinate(
                &game.snake.body.coordinates[0],
                &((MultipleCoordinates){
                    .coordinates = game.snake.body.coordinates + 1,
                    .numCoordinates = game.snake.body.numCoordinates - 1})) ==
                -1) {
            int idx;

            if ((idx = isCollidingCoordinate(&game.snake.body.coordinates[0],
                                             &game.apples)) >= 0) {
                spawnApple(&game.apples, idx, &game.snake.body, &maxCoordinate);

                resizeMultipleCoordinates(&game.snake.body,
                                          game.snake.body.numCoordinates + 1);
                game.snake.body.coordinates[game.snake.body.numCoordinates] =
                    oldTail;
                game.snake.body.numCoordinates++;
                game.score++;

                drawGameInfo(game.score);
                drawBorder(gameBorder);
            }
        } else {
            WINDOW *gameOverWindow = drawGameOver(game.score);
            bool doQuit = false;
            bool doContinue = false;

            do {
                c = wgetch(gameOverWindow);

                if (c == 'C' || c == 'c') {
                    doContinue = true;
                } else if (c == 'Q' || c == 'q') {
                    doQuit = true;
                }

                if (doContinue || doQuit)
                    delwin(gameOverWindow);
            } while (!doContinue && !doQuit);

            if (doContinue) {
                freeSnakeGame(&game);
                game = initSnakeGame(startPoint, &maxCoordinate);
                game.snake.direction = RIGHT;

                drawGameInfo(game.score);
                drawBorder(gameBorder);
            } else if (doQuit)
                break;
        }

        drawGame(gameWindow, game);
    };

    delwin(gameBorder);
    delwin(gameWindow);
    endwin();

    freeSnakeGame(&game);

    return 0;
}

const char *getScoreStr(int score) {
    static char str[MAX_SCORE_STR_SIZE];
    snprintf(str, sizeof(str), "Score: %d", score);
    return str;
}

bool isInsideWindow(WINDOW *win, Coordinate coordinate) {
    return coordinate.x >= 0 && coordinate.y >= 0 &&
           coordinate.x <= getmaxx(win) - 1 && coordinate.y <= getmaxy(win) - 1;
}

WINDOW *createBorder(int xWIn, int yWin, int widthWin, int heightWin) {
    return newwin(heightWin + 2, widthWin + 2, yWin - 1, xWIn - 1);
}

void drawBorder(WINDOW *border) {
    box(border, ACS_VLINE, ACS_HLINE);
    wrefresh(border);
}

void drawGameInfo(int score) {
    clear();
    mvprintw(1, getmaxx(stdscr) / 2 - strlen(TITLE) / 2, TITLE);
    mvprintw(1, getmaxx(stdscr) - strlen(getScoreStr(score)) - 2, "%s",
             getScoreStr(score));
    refresh();
}

void drawGame(WINDOW *gameWindow, SnakeGame game) {
    wclear(gameWindow);

    wattron(gameWindow, COLOR_PAIR(1));
    mvwaddch(gameWindow, game.snake.body.coordinates[0].y,
             game.snake.body.coordinates[0].x, '*');

    wattron(gameWindow, COLOR_PAIR(2));
    for (int i = 1; i < game.snake.body.numCoordinates; i++) {
        mvwaddch(gameWindow, game.snake.body.coordinates[i].y,
                 game.snake.body.coordinates[i].x, '*');
    }

    wattron(gameWindow, COLOR_PAIR(3));
    for (int i = 0; i < game.apples.numCoordinates; i++) {
        mvwaddch(gameWindow, game.apples.coordinates[i].y,
                 game.apples.coordinates[i].x, 'O');
    }

    wrefresh(gameWindow);
    usleep(DELAY);
}

WINDOW *drawGameOver(int score) {
    const char *msg = "Game Over!";
    const char *submsg = getScoreStr(score);
    const char *actionMsg = "Continue (C)? Exit (Q)?";
    const int offset = 4;
    const int winWidth = getmaxx(stdscr);
    const int winHeight = getmaxy(stdscr);

    WindowInfo winInfo;

    winInfo.x = winWidth - winWidth / 2 - strlen(actionMsg) / 2 - offset;
    winInfo.width = strlen(actionMsg) + offset * 2;
    winInfo.height = offset * 3;
    winInfo.y = winHeight / 2 - offset * 3 / 2;

    WINDOW *border =
        createBorder(winInfo.x, winInfo.y, winInfo.width, winInfo.height);
    drawBorder(border);
    wrefresh(border);

    WINDOW *window =
        newwin(winInfo.height, winInfo.width, winInfo.y, winInfo.x);
    keypad(window, true);

    mvwprintw(window, offset, winInfo.width / 2 - strlen(msg) / 2, "%s", msg);
    mvwprintw(window, offset + 1, winInfo.width / 2 - strlen(submsg) / 2, "%s",
              submsg);
    mvwprintw(window, offset + 3, winInfo.width / 2 - strlen(actionMsg) / 2,
              "%s", actionMsg);
    wrefresh(window);

    delwin(border);

    return window;
}
