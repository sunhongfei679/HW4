#include "flappy.h"

// 必须先更新全部管道位置，再统一执行回收
// 管道右边缘 <= 0 时触发回收
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

// 检测小鸟是否碰撞障碍物（天花板、地板、管道）,接触边缘是可以的
bool birdHitsObstacle(const Game *game) {
    // 先检测天花板和地板
    if (gameBirdBox(game).m_y < 0) {
        return true;
    } else if (gameBirdBox(game).m_y + gameBirdBox(game).m_height > FLOOR_Y) {
        return true;
    } else {
        // 分别对上下管道检测，建立Box来使用boxesOverlap
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

// 管道右边缘严格小于鸟左边缘才算通过
// m_scored防止同一管道重复计分
// 返回本次新增的得分数量
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

// seed > 0 时重置状态；seed = 0 时沿用当前状态
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
