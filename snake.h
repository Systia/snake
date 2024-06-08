#ifndef SNAKE_H
#define SNAKE_H

#include <stdbool.h>
#include <stddef.h>

#ifndef SNAKE_START_LENGTH
#define SNAKE_START_LENGTH 4
#endif
#ifndef MAX_APPLES
#define MAX_APPLES 4
#endif

typedef enum { UP, RIGHT, DOWN, LEFT } Direction;

typedef struct {
    int x;
    int y;
} Coordinate;

typedef struct {
    Coordinate *coordinates;
    int numCoordinates;
} MultipleCoordinates;

typedef struct {
    MultipleCoordinates body;
    Direction direction;
} Snake;

typedef struct {
    Snake snake;
    MultipleCoordinates apples;
    int score;
    Coordinate maxCoordinate;
} SnakeGame;

void resizeMultipleCoordinates(MultipleCoordinates *coordinates,
                               int newNumElem);
int generateNumber(int max);
Coordinate generateCoordinate(const Coordinate *maxCoordinate);
int isCollidingCoordinate(const Coordinate *coordinate,
                          const MultipleCoordinates *checkCoordinates);
void spawnApple(const MultipleCoordinates *apples, int idx,
                const MultipleCoordinates *snake,
                const Coordinate *maxCoordinate);
SnakeGame initSnakeGame(Coordinate startPoint, const Coordinate *maxCoordinate);
void freeSnakeGame(SnakeGame *game);
void snakeMove(Snake *snake);

#endif // SNAKE_H
