#include <raylib.h>
#include <stdio.h>

#define BLOCK_SIZE 20
#define GRID_WIDTH 28
#define GRID_HEIGHT 22
#define WINDOW_WIDTH (BLOCK_SIZE * GRID_WIDTH)
#define WINDOW_HEIGHT (BLOCK_SIZE * GRID_HEIGHT)
#define MAX_SNAKE 200

typedef enum {
    STATE_START,
    STATE_PLAY,
    STATE_GAMEOVER,
    STATE_PAUSE
} GameState;

typedef struct {
    int x[MAX_SNAKE];
    int y[MAX_SNAKE];
    int len;
    int dir;
} Snake;

// 打包全部游戏数据，替代全局变量
typedef struct {
    Snake snake;
    int foodX;
    int foodY;
    GameState gameState;
    int score;
} GameData;

// 函数声明，全部增加 GameData* 指针参数
void SpawnFood(GameData* game);
void UpdateSnake(GameData* game);
void DrawStartScreen(GameData* game);
void DrawGameScreen(GameData* game);
void DrawPauseScreen(GameData* game);
void DrawGameOverScreen(GameData* game);

void SpawnFood(GameData* game) {
    int overlap;
    do {
        overlap = 0;
        game->foodX = GetRandomValue(0, GRID_WIDTH - 1);
        game->foodY = GetRandomValue(0, GRID_HEIGHT - 1);
        for (int i = 0; i < game->snake.len; i++) {
            if (game->snake.x[i] == game->foodX && game->snake.y[i] == game->foodY) {
                overlap = 1;
                break;
            }
        }
    } while (overlap);
}

void UpdateSnake(GameData* game) {
    // 蛇身体跟随
    for (int i = game->snake.len - 1; i > 0; i--) {
        game->snake.x[i] = game->snake.x[i - 1];
        game->snake.y[i] = game->snake.y[i - 1];
    }
    // 蛇头移动
    switch (game->snake.dir) {
        case 0: game->snake.y[0] -= 1; break;
        case 1: game->snake.x[0] += 1; break;
        case 2: game->snake.y[0] += 1; break;
        case 3: game->snake.x[0] -= 1; break;
    }
    // 撞墙检测
    if (game->snake.x[0] < 0 || game->snake.x[0] >= GRID_WIDTH || game->snake.y[0] < 0 || game->snake.y[0] >= GRID_HEIGHT) {
        game->gameState = STATE_GAMEOVER;
    }
    // 撞到自己
    for (int i = 1; i < game->snake.len; i++) {
        if (game->snake.x[0] == game->snake.x[i] && game->snake.y[0] == game->snake.y[i]) {
            game->gameState = STATE_GAMEOVER;
            break;
        }
    }
    // 吃到食物
    if (game->snake.x[0] == game->foodX && game->snake.y[0] == game->foodY) {
        game->snake.len++;
        game->score += 1;
        SpawnFood(game);
    }
    // WASD控制方向
    if (IsKeyPressed(KEY_W) && game->snake.dir != 2) game->snake.dir = 0;
    if (IsKeyPressed(KEY_S) && game->snake.dir != 0) game->snake.dir = 2;
    if (IsKeyPressed(KEY_A) && game->snake.dir != 1) game->snake.dir = 3;
    if (IsKeyPressed(KEY_D) && game->snake.dir != 3) game->snake.dir = 1;
}

void DrawStartScreen(GameData* game) {
    DrawText("SNAKE GAME", WINDOW_WIDTH / 2 - 160, WINDOW_HEIGHT / 2 - 80, 50, GREEN);
    DrawText("Press SPACE to Start", WINDOW_WIDTH / 2 - 170, WINDOW_HEIGHT / 2, 28, WHITE);
    DrawText("WASD to move", WINDOW_WIDTH / 2 - 110, WINDOW_HEIGHT / 2 + 40, 22, GRAY);
    DrawText("ESC exit", WINDOW_WIDTH / 2 - 70, WINDOW_HEIGHT / 2 + 70, 22, GRAY);
}

void DrawGameScreen(GameData* game) {
    for (int i = 0; i < game->snake.len; i++) {
        if (i == 0) {
            DrawRectangle(game->snake.x[i] * BLOCK_SIZE, game->snake.y[i] * BLOCK_SIZE, BLOCK_SIZE - 1, BLOCK_SIZE - 1, GREEN);
        } else {
            DrawRectangle(game->snake.x[i] * BLOCK_SIZE, game->snake.y[i] * BLOCK_SIZE, BLOCK_SIZE - 1, BLOCK_SIZE - 1, LIME);
        }
    }
    DrawRectangle(game->foodX * BLOCK_SIZE, game->foodY * BLOCK_SIZE, BLOCK_SIZE - 1, BLOCK_SIZE - 1, RED);
    DrawText(TextFormat("Score: %d", game->score), 10, 10, 20, WHITE);
}

void DrawPauseScreen(GameData* game) {
    DrawRectangle(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, Fade(BLACK, 0.5f));
    DrawText("GAME PAUSED", WINDOW_WIDTH / 2 - MeasureText("GAME PAUSED", 50) / 2, WINDOW_HEIGHT / 2 - 60, 50, WHITE);
    DrawText("Press P to Resume Game", WINDOW_WIDTH / 2 - MeasureText("Press P to Resume Game", 26) / 2, WINDOW_HEIGHT / 2, 26, LIGHTGRAY);
    DrawText("Press R to return to Menu", WINDOW_WIDTH / 2 - MeasureText("Press R to return to Menu", 26) / 2, WINDOW_HEIGHT / 2 + 35, 26, LIGHTGRAY);
    DrawText("Press U to go to Game Over", WINDOW_WIDTH / 2 - MeasureText("Press U to go to Game Over", 26) / 2, WINDOW_HEIGHT / 2 + 70, 26, LIGHTGRAY);
}

void DrawGameOverScreen(GameData* game) {
    DrawText("GAME OVER!", WINDOW_WIDTH / 2 - 110, WINDOW_HEIGHT / 2 - 40, 40, RED);
    DrawText("Press R back to Menu", WINDOW_WIDTH / 2 - 140, WINDOW_HEIGHT / 2 + 10, 24, YELLOW);
    DrawText("ESC to exit", WINDOW_WIDTH / 2 - 80, WINDOW_HEIGHT / 2 + 40, 22, GRAY);
}

int main(void) {
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "像素贪吃蛇 Raylib");
    SetTargetFPS(12);

    // 全部游戏数据放在main里面，局部变量，不再全局
    GameData game;
    game.gameState = STATE_START;

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK);

        // ========= 输入逻辑（放到主循环，不再写在Draw绘制函数内） =========
        if (IsKeyPressed(KEY_SPACE) && game.gameState == STATE_START) {
            game.score = 0;
            game.snake.len = 3;
            game.snake.dir = 1;
            game.snake.x[0] = GRID_WIDTH / 2;
            game.snake.y[0] = GRID_HEIGHT / 2;
            game.snake.x[1] = GRID_WIDTH / 2 - 1;
            game.snake.y[1] = GRID_HEIGHT / 2;
            game.snake.x[2] = GRID_WIDTH / 2 - 2;
            game.snake.y[2] = GRID_HEIGHT / 2;
            SpawnFood(&game);
            game.gameState = STATE_PLAY;
        }
        if (IsKeyPressed(KEY_P)) {
            if (game.gameState == STATE_PLAY) {
                game.gameState = STATE_PAUSE;
            } else if (game.gameState == STATE_PAUSE) {
                game.gameState = STATE_PLAY;
            }
        }
        if (IsKeyPressed(KEY_R)) {
            game.gameState = STATE_START;
        }
        if (IsKeyPressed(KEY_U) && game.gameState == STATE_PAUSE) {
            game.gameState = STATE_GAMEOVER;
        }

        // ========= 状态机：更新 + 绘制 =========
        if (game.gameState == STATE_START) {
            DrawStartScreen(&game);
        } else if (game.gameState == STATE_PLAY) {
            UpdateSnake(&game);
            DrawGameScreen(&game);
        } else if (game.gameState == STATE_PAUSE) {
            DrawGameScreen(&game);
            DrawPauseScreen(&game);
        } else if (game.gameState == STATE_GAMEOVER) {
            DrawGameScreen(&game);
            DrawGameOverScreen(&game);
        }

        EndDrawing();
    }
    CloseWindow();
    return 0;
}
