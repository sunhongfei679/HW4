#include "snake.h"
#include <stdlib.h>

bool snakeInit(Snake *snake, int rows, int cols, size_t length) {
    if (rows < MIN_BOARD_SIZE || cols < MIN_BOARD_SIZE || length < 1 ||
        cols - 2 * BOARD_WALL_THICKNESS < 1 || length > (size_t)(cols - 2 * BOARD_WALL_THICKNESS)) {
        return false;
    }
    SnakeNode *firstNode = NULL;
    SnakeNode *lastNode = NULL;
    size_t num = 0;
    for (size_t i = 0; i < length; i++) {
        SnakeNode *node = malloc(sizeof(SnakeNode));
        if (node == NULL) {
            // If the allocation fails,release all created nodes.num represents the number of nodes.
            SnakeNode *current = firstNode;
            for (size_t j = 0; j < num; j++) {
                SnakeNode *next = current->m_next;
                free(current);
                current = next;
            }
            *snake = (Snake){0};
            return false;
        }
        node->m_pos.m_row = rows / 2;
        node->m_pos.m_col =
            (cols / 2 > (int)length ? cols / 2 : (int)length) - (int)(length - 1 - i);
        if (num == 0) {
            /* When num==1,tail and head are the same node.
             * We use node as an intermediate variable to pass the address.*/
            firstNode = node;
            lastNode = node;
            node->m_next = node;
        } else {
            lastNode->m_next = node;
            lastNode = node;
            node->m_next = firstNode;
        }
        num++;
    }
    snake->m_direction = DIR_RIGHT;
    snake->m_head = lastNode;
    snake->m_rows = rows;
    snake->m_cols = cols;
    snake->m_length = length;
    (void)rows;
    (void)cols;
    (void)length;
    return true;
}

Coord snakeNextPosition(const Snake *snake, Direction requested) {
    Coord position;
    position.m_col = snake->m_head->m_pos.m_col;
    position.m_row = snake->m_head->m_pos.m_row;
    if ((snake->m_length > 1) && ((snake->m_direction == DIR_UP && requested == DIR_DOWN) ||
                                  (snake->m_direction == DIR_DOWN && requested == DIR_UP) ||
                                  (snake->m_direction == DIR_LEFT && requested == DIR_RIGHT) ||
                                  (snake->m_direction == DIR_RIGHT && requested == DIR_LEFT))) {
        if (snake->m_direction == DIR_LEFT) {
            position.m_col -= 1;
        } else if (snake->m_direction == DIR_RIGHT) {
            position.m_col += 1;
        } else if (snake->m_direction == DIR_UP) {
            position.m_row -= 1;
        } else if (snake->m_direction == DIR_DOWN) {
            position.m_row += 1;
        }
        return position;
    } else if (requested == DIR_LEFT) {
        position.m_col -= 1;
    } else if (requested == DIR_RIGHT) {
        position.m_col += 1;
    } else if (requested == DIR_UP) {
        position.m_row -= 1;
    } else if (requested == DIR_DOWN) {
        position.m_row += 1;
    } else {
        if (snake->m_direction == DIR_LEFT) {
            position.m_col -= 1;
        } else if (snake->m_direction == DIR_RIGHT) {
            position.m_col += 1;
        } else if (snake->m_direction == DIR_UP) {
            position.m_row -= 1;
        } else if (snake->m_direction == DIR_DOWN) {
            position.m_row += 1;
        }
    }
    return position;
}

bool snakeContains(const Snake *snake, Coord pos) {
    if (snake->m_head == NULL || snake->m_length == 0) {
        return false;
    }
    // check all the elements in the circuit once.
    SnakeNode *current = snake->m_head;
    for (size_t i = 0; i < snake->m_length; i++) {
        if (current->m_pos.m_col == pos.m_col && current->m_pos.m_row == pos.m_row) {
            return true;
        } else {
            current = current->m_next;
        }
    }
    return false;
}

StepResult snakeStep(Snake *snake, Direction requested, bool grow) {
    if (snake->m_head == NULL || snake->m_length == 0) {
        return STEP_WALL;
    }
    // store the new position
    Coord nextPosition = snakeNextPosition(snake, requested);
    if (nextPosition.m_row < BOARD_WALL_THICKNESS ||
        nextPosition.m_row >= snake->m_rows - BOARD_WALL_THICKNESS ||
        nextPosition.m_col < BOARD_WALL_THICKNESS ||
        nextPosition.m_col >= snake->m_cols - BOARD_WALL_THICKNESS) {
        return STEP_WALL;
    }
    // contact with its own old tail position is ok.
    SnakeNode *oldTail = snake->m_head->m_next;
    if (!grow && nextPosition.m_col == oldTail->m_pos.m_col &&
        nextPosition.m_row == oldTail->m_pos.m_row) {
    } else if (snakeContains(snake, nextPosition)) {
        return STEP_SELF;
    }
    if (grow == false) {
        snake->m_head->m_next->m_pos.m_col = nextPosition.m_col;
        snake->m_head->m_next->m_pos.m_row = nextPosition.m_row;
        snake->m_head = snake->m_head->m_next;
    } else if (grow ==
               true) { // I want to create a new circuit first ,then change the address of m_head
        SnakeNode *newHead = malloc(sizeof(SnakeNode));
        if (newHead == NULL) {
            return STEP_NO_MEMORY;
        }
        newHead->m_pos = nextPosition;
        newHead->m_next = oldTail;
        snake->m_head->m_next = newHead;
        snake->m_head = newHead;
        snake->m_length++;
    }
    if (((!grow && snake->m_length > 1) || (grow && snake->m_length - 1 > 1)) &&
        ((snake->m_direction == DIR_UP && requested == DIR_DOWN) ||
         (snake->m_direction == DIR_DOWN && requested == DIR_UP) ||
         (snake->m_direction == DIR_LEFT && requested == DIR_RIGHT) ||
         (snake->m_direction == DIR_RIGHT && requested == DIR_LEFT))) {
    } else if (requested != DIR_DOWN && requested != DIR_UP && requested != DIR_LEFT &&
               requested != DIR_RIGHT) {
    } else {
        snake->m_direction = requested;
    }
    return STEP_OK;
}

void snakeDestroy(Snake *snake) {
    if (snake->m_head == NULL || snake->m_length == 0) {
        return;
    }

    SnakeNode *current = snake->m_head;
    for (size_t i = 0; i < snake->m_length; i++) {
        SnakeNode *next = current->m_next;
        free(current);
        current = next;
    }
    *snake = (Snake){0};
}

// I finish flappy.c first,the function below is the same as the function in flappy.c
unsigned int nextRandom(unsigned int seed) {
    static unsigned int state = RANDOM_INITIAL_SEED;
    if (seed > 0) {
        state = seed % RANDOM_MODULUS;
    } else if (seed == 0) {
        state = state;
    }
    state = (RANDOM_MULTIPLIER * state + RANDOM_INCREMENT) % RANDOM_MODULUS;
    return state;
}
