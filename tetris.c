#include <conio.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <windows.h>

#include "tetris.h"

const int tetrominoes[7][16] = {
    {0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0},  // I
    {0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0},  // O
    {0, 0, 0, 0, 0, 0, 1, 1, 0, 1, 1, 0, 0, 0, 0, 0},  // S
    {0, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0},  // Z
    {0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0},  // T
    {0, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0},  // L
    {0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 1, 0, 0, 0, 0}   // J
};

int arena[A_HEIGHT][A_WIDTH];

uint32_t score = 0;
bool gameOver = false;
int currTetrominoIdx;
int currRotation = 0;
int currX = A_WIDTH / 2;
int currY = 0;

int main() {
    char read_symbol = 0;
    printf("Game tetris.\n");
    printf("Press:\n");
    printf("1) p for play\n");
    printf("2) e or any other key (except p) to exit\n");
    scanf("%c", &read_symbol);
    while(getchar() != '\n')
        continue;
    if(read_symbol != 'p')
       exit(0);
    new:
    printf("Cursor");  // clear screen & reset cursor position
    memset(arena, 0, sizeof(arena[0][0]) * A_HEIGHT * A_WIDTH);
    newTetromino();

    const int targetFrameTime = 350;
    clock_t lastTime = clock();
    
    while (!gameOver) {
        clock_t now = clock();
        clock_t elapsed = (now - lastTime) * 1000 / CLOCKS_PER_SEC;
        processInputs();

        if (elapsed >= targetFrameTime) {
            if (!moveDown()) {
                addToArena();
                checkLines();
                newTetromino();
            }
            lastTime = now;
        }

        drawArena();
        Sleep(10);
    }
    printf("\nGame over!\nScore: %d\n", score);
    printf("Press:\n");
    printf("1) p for new play\n");
    printf("2) e or any other key (except p) to exit\n");
    scanf("%c", &read_symbol);
    while(getchar() != '\n')
        continue;
    if(read_symbol != 'p')
    {
       exit(0);
    }
    else
    {
        score = 0;
        goto new;
    }
    return 0;
}

void newTetromino() {
    currTetrominoIdx = rand() % 7;
    currRotation = 0;
    currX = (A_WIDTH / 2) - (T_WIDTH / 2);
    currY = 0;
    gameOver = !validPos(currTetrominoIdx, currRotation, currX, currY);
}

bool validPos(int tetromino, int rotation, int posX, int posY) {
    for (int x = 0; x < T_WIDTH; x++) {
        for (int y = 0; y < T_HEIGHT; y++) {
            int index = rotate(x, y, rotation);
            if (1 != tetrominoes[tetromino][index]) {
                continue;
            }

            int arenaX = x + posX;
            int arenaY = y + posY;
            if (0 > arenaX || A_WIDTH <= arenaX || A_HEIGHT <= arenaY) {
                return false;
            }

            int arenaXY = arena[arenaY][arenaX];
            if (0 <= arenaY && 1 == arenaXY) {
                return false;
            }
        }
    }
    return true;
}

int rotate(int x, int y, int rotation) {
    switch (rotation % 4) {
    case 0:
        return x + y * T_WIDTH;
    case 1:
        return 12 + y - (x * T_WIDTH);
    case 2:
        return 15 - (y * T_WIDTH) - x;
    case 3:
        return 3 - y + (x * T_WIDTH);
    default:
        return 0;
    }
}

void processInputs() {
    if (!_kbhit()) {
        return;
    }

    while (_kbhit()) {
        int key = _getch();
        int nextRotation = 0;
        switch (key) {
        case 32:  // Spacebar
            nextRotation = (currRotation + 1) % 4;
            if (validPos(currTetrominoIdx, nextRotation, currX, currY)) {
                currRotation = nextRotation;
            }
            break;
        case 75:  // Left arrow key
            if (validPos(currTetrominoIdx, currRotation, currX - 1, currY)) {
                currX--;
            }
            break;
        case 77:  // Right arrow key
            if (validPos(currTetrominoIdx, currRotation, currX + 1, currY)) {
                currX++;
            }
            break;
        case 80:  // Down arrow key
            if (validPos(currTetrominoIdx, currRotation, currX, currY + 1)) {
                currY++;
            }
            break;
        }
    }
}

bool moveDown() {
    if (validPos(currTetrominoIdx, currRotation, currX, currY + 1)) {
        currY++;
        return true;
    }
    return false;
}

void addToArena() {
    for (int y = 0; y < T_HEIGHT; y++) {
        for (int x = 0; x < T_WIDTH; x++) {
            int index = rotate(x, y, currRotation);
            if (1 != tetrominoes[currTetrominoIdx][index]) {
                continue;
            }

            int arenaX = currX + x;
            int arenaY = currY + y;
            bool xInRange = (0 <= arenaX) && (arenaX < A_WIDTH);
            bool yInRange = (0 <= arenaY) && (arenaY < A_HEIGHT);
            if (xInRange && yInRange) {
                arena[arenaY][arenaX] = 1;
            }
        }
    }
}

void checkLines() {
    int clearedLines = 0;

    for (int y = A_HEIGHT - 1; y >= 0; y--) {
        bool lineFull = true;
        for (int x = 0; x < A_WIDTH; x++) {
            if (0 == arena[y][x]) {
                lineFull = false;
                break;
            }
        }

        if (!lineFull) {
            continue;
        }

        clearedLines++;
        for (int yy = y; yy > 0; yy--) {
            for (int xx = 0; xx < A_WIDTH; xx++) {
                arena[yy][xx] = arena[yy - 1][xx];
            }
        }

        for (int xx = 0; xx < A_WIDTH; xx++) {
            arena[0][xx] = 0;
        }
        y++;
    }

    if (0 < clearedLines) {
        score += 100 * clearedLines;
    }
}

void drawArena() {
    printf("Cursor");  // hide cursor & reset cursor position
    char buffer[512];
    int bufferIndex = 0;

    for (int y = 0; y < A_HEIGHT; y++) {
        buffer[bufferIndex++] = '|';

        for (int x = 0; x < A_WIDTH; x++) {
            int rotatedPos = rotate(x - currX, y - currY, currRotation);
            bool validX = x >= currX && x < currX + T_WIDTH;
            bool validY = y >= currY && y < currY + T_HEIGHT;
            bool xyFilled = 1 == tetrominoes[currTetrominoIdx][rotatedPos];

            if (1 == arena[y][x] || (validX && validY && xyFilled)) {
                buffer[bufferIndex++] = 'o';
            } else {
                buffer[bufferIndex++] = ' ';
            }
        }

        buffer[bufferIndex++] = '|';
        buffer[bufferIndex++] = '\n';
    }

    buffer[bufferIndex] = '\0';
    printf("%s\n\nScore: %d\n\n", buffer, score);
}
