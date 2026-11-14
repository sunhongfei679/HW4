#include "flappy.h"

void advancePipes(Game *game, float dt) {
    /* TODO 1: Move every pipe left by PIPE_SPEED * dt. Then visit the slots
     * in array order; if a pipe's right edge is <= 0, call recyclePipe. */
    for (int i = 0; i < PIPE_COUNT; i++) {
        game->m_pipes[i].m_x -= dt * PIPE_SPEED;
    }
    for (int i = 0; i < PIPE_COUNT; i++) {
        if (game->m_pipes[i].m_x + PIPE_WIDTH <= 0) {
            recyclePipe(game, i);
        }
    }
    (void)game;
    (void)dt;
}

bool birdHitsObstacle(const Game *game) {
    /* TODO 2: Check the top/floor boundaries and both rectangles of each
     * pipe pair. Use gameBirdBox and boxesOverlap. Do not change the game. */
    if (gameBirdBox(game).m_y < 0) {
        return true;
    } else if (gameBirdBox(game).m_y + gameBirdBox(game).m_height > FLOOR_Y) {
        return true;
    } else {
        for (int i = 0; i < PIPE_COUNT; i++) {
            Pipe pipe = game->m_pipes[i];
            Box Upper_pipe = {
                .m_x = pipe.m_x, .m_y = 0, .m_width = PIPE_WIDTH, .m_height = pipe.m_gap_y};
            Box Lower_pipe = {.m_x = pipe.m_x,
                              .m_y = pipe.m_gap_y + PIPE_GAP_HEIGHT,
                              .m_width = PIPE_WIDTH,
                              .m_height = FLOOR_Y - pipe.m_gap_y - PIPE_GAP_HEIGHT};
            if (boxesOverlap(gameBirdBox(game), Upper_pipe) != 0 ||
                boxesOverlap(gameBirdBox(game), Lower_pipe) != 0) {
                return true;
            }
        }
    }
    (void)game;
    return false;
}

int scorePassedPipes(Game *game) {
    /* TODO 3: A pipe scores once when its right edge is strictly left of
     * BIRD_X. Mark it scored, add to m_score, and return this call's count. */
    int new_score = 0;
    for (int i = 0; i < PIPE_COUNT; i++) {
        if (game->m_pipes[i].m_x + PIPE_WIDTH < gameBirdBox(game).m_x) {
            if (game->m_pipes[i].m_scored != true) {
                game->m_score += 1;
                new_score += 1;
            }
            game->m_pipes[i].m_scored = true;
        }
    }
    (void)game;
    return new_score;
}

unsigned int nextRandom(unsigned int seed) {
    /* TODO: Keep one static unsigned int state, initially RANDOM_INITIAL_SEED.
     * A positive seed resets it modulo RANDOM_MODULUS. Then take one
     * recurrence step and return the new state. Zero continues the sequence. */
    static unsigned int state = RANDOM_INITIAL_SEED;
    if (seed > 0) {
        state = seed % RANDOM_MODULUS;
    } else if (seed == 0) {
        state = state;
    }
    state = (RANDOM_MULTIPLIER * state + RANDOM_INCREMENT) % RANDOM_MODULUS;
    (void)seed;
    return state;
}
