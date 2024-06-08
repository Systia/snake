#include "snake.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

void resizeMultipleCoordinates(MultipleCoordinates *coordinates,
                               int newNumElem) {
    Coordinate *newArr =
        (Coordinate *)calloc(newNumElem, sizeof(coordinates->coordinates[0]));
    memcpy(newArr, coordinates->coordinates,
           coordinates->numCoordinates * sizeof(coordinates->coordinates[0]));
    free(coordinates->coordinates);
    coordinates->coordinates = newArr;
}

int generateNumber(int max) {
    return rand() % (max + 1);
}

Coordinate generateCoordinate(const Coordinate *maxCoordinate) {
    return (Coordinate){.x = generateNumber(maxCoordinate->x),
                        .y = generateNumber(maxCoordinate->y)};
}

int isCollidingCoordinate(const Coordinate *coordinate,
                          const MultipleCoordinates *checkCoordinates) {
    for (int i = 0; i < checkCoordinates->numCoordinates; i++) {
        if (memcmp(coordinate, &checkCoordinates->coordinates[i],
                   sizeof(Coordinate)) == 0)
            return i;
    }

    return -1;
}

void spawnApple(const MultipleCoordinates *apples, int idx,
                const MultipleCoordinates *snake,
                const Coordinate *maxCoordinate) {
    bool isColliding;

    do {
        apples->coordinates[idx] = generateCoordinate(maxCoordinate);
        // TODO: check if the spawned apple is colliding with other apples
        //
        isColliding =
            isCollidingCoordinate(&apples->coordinates[idx], snake) >= 0 ||
            isCollidingCoordinate(&apples->coordinates[idx], apples) < idx;

        if (idx < apples->numCoordinates - 1) {
            isColliding |=
                isCollidingCoordinate(
                    &apples->coordinates[idx],
                    &((MultipleCoordinates){
                        .coordinates = apples->coordinates + idx + 1,
                        .numCoordinates = apples->numCoordinates - idx - 1})) >
                idx;
        }
    } while (isColliding || apples->coordinates[idx].x % 2 == 1);
}

SnakeGame initSnakeGame(Coordinate startPoint,
                        const Coordinate *maxCoordinate) {
    srand(100);
    SnakeGame game;

    game.snake.body.coordinates =
        (Coordinate *)malloc(sizeof(Coordinate) * SNAKE_START_LENGTH);
    game.snake.body.numCoordinates = SNAKE_START_LENGTH;

    if (startPoint.x % 2 == 1)
        startPoint.x++;

    for (int i = 0; i < SNAKE_START_LENGTH; i++) {
        game.snake.body.coordinates[i].x = startPoint.x - i * 2;
        game.snake.body.coordinates[i].y = startPoint.y;
    }

    game.snake.direction = RIGHT;

    game.apples.coordinates =
        (Coordinate *)malloc(sizeof(Coordinate) * MAX_APPLES);
    game.apples.numCoordinates = MAX_APPLES;

    for (int i = 0; i < MAX_APPLES; i++) {
        spawnApple(&game.apples, i, &game.snake.body, maxCoordinate);
    }

    game.score = 0;

    return game;
}

void freeSnakeGame(SnakeGame *game) {
    free(game->snake.body.coordinates);
    game->snake.body.coordinates = NULL;
    game->snake.body.numCoordinates = 0;

    free(game->apples.coordinates);
    game->apples.coordinates = NULL;
    game->apples.numCoordinates = 0;
}

void snakeMove(Snake *snake) {
    snake->body.coordinates[snake->body.numCoordinates - 1] =
        snake->body.coordinates[snake->body.numCoordinates - 2];

    for (int i = snake->body.numCoordinates - 2; i >= 1; i--) {
        snake->body.coordinates[i] = snake->body.coordinates[i - 1];
    }

    switch (snake->direction) {
    case UP:
        snake->body.coordinates[0].y--;
        break;

    case RIGHT:
        snake->body.coordinates[0].x += 2;
        break;

    case DOWN:
        snake->body.coordinates[0].y++;
        break;

    case LEFT:
        snake->body.coordinates[0].x -= 2;
        break;
    }
}
