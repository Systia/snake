#!/usr/bin/env bash

set -xe

clang -Werror -Wall -Wextra -Wpedantic -fsanitize=address -ggdb3 -o main main.c snake.c -lncurses
clang -Werror -Wall -Wextra -Wpedantic -O3 -o release main.c snake.c -lncurses
