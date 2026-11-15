#include "flappy.h"

// Update all pipe positions first, then recycle off-screen pipes uniformly
// Recycling triggers when the right edge of a pipe is less than or equal to 0
void advancePipes(Game *game, float dt) {
    for (int i = 0; i < PIPE_COUNT; i++) {
        game->m_pipes[i].m_x -= dt * PIPE_SPEED;
    }
    for (int i = 0; i < PIPE_COUNT; i++) {
        if (game->m_pipes[i].m_x + PIPE_WIDTH <= 0) {
            recyclePipe(game, i);
        }
    }
}

// Check if the bird hits obstacles (ceiling, floor, pipes); edge contact is safe
bool birdHitsObstacle(const Game *game) {
    // Check ceiling and floor collision first
    if (gameBirdBox(game).m_y < 0) {
        return true;
    } else if (gameBirdBox(game).m_y + gameBirdBox(game).m_height > FLOOR_Y) {
        return true;
    } else {
        // Check upper/lower pipes separately; construct Box structs for overlap test
        for (int i = 0; i < PIPE_COUNT; i++) {
            Pipe pipe = game->m_pipes[i];
            Box upperPipe = {
                .m_x = pipe.m_x, .m_y = 0, .m_width = PIPE_WIDTH, .m_height = pipe.m_gap_y};
            Box lowerPipe = {.m_x = pipe.m_x,
                             .m_y = pipe.m_gap_y + PIPE_GAP_HEIGHT,
                             .m_width = PIPE_WIDTH,
                             .m_height = FLOOR_Y - pipe.m_gap_y - PIPE_GAP_HEIGHT};
            if (boxesOverlap(gameBirdBox(game), upperPipe) ||
                boxesOverlap(gameBirdBox(game), lowerPipe)) {
                return true;
            }
        }
    }
    return false;
}

// A pipe counts as passed only when its right edge is strictly left of the bird
// m_scored flag prevents duplicate scoring for the same pipe
// Returns the number of new points added in this call
int scorePassedPipes(Game *game) {
    int newScore = 0;
    for (int i = 0; i < PIPE_COUNT; i++) {
        if (game->m_pipes[i].m_x + PIPE_WIDTH < gameBirdBox(game).m_x) {
            if (!game->m_pipes[i].m_scored) {
                game->m_score += 1;
                newScore += 1;
            }
            game->m_pipes[i].m_scored = true;
        }
    }
    return newScore;
}

// Reset internal state when seed > 0; continue with current state when seed == 0
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
