#include"AmazonGame.h"
Move AmazonGame::mostattribution(const std::vector<Move>possiblemoves) {
    Move temp;
    int prev = INT_MIN;
    for (auto& move : possiblemoves) {
        int profit = areacalculate(move);
        if (prev < profit) {
            prev = profit;
            temp = move;
        }
    }
    return temp;
}
int AmazonGame::areacalculate(Move move) {
    char temp[BOARD_SIZE][BOARD_SIZE];
    // 逐元素复制 board 到 temp
    for (int i = 0; i < BOARD_SIZE; ++i) {
        for (int j = 0; j < BOARD_SIZE; ++j) {
            temp[i][j] = board[i][j];
        }
    }
    temp[move.fromX][move.fromY] = '.';
    temp[move.toX][move.toY] = 'C';
    temp[move.arrowX][move.arrowY] = 'X';
    int profit = 0;
    for (int i = move.fromX - 1; i >= 0; i--) {
        if (temp[i][move.fromY] == '.') {
            profit--;
        }
        else break;
    }
    for (int i = move.fromX +1; i<BOARD_SIZE; i++) {
        if (temp[i][move.fromY] == '.') {
            profit--;
        }
        else break;
    }
    for (int i = move.fromY - 1; i >= 0; i--) {
        if (temp[move.fromX][i] == '.') {
            profit--;
        }
        else break;
    }
    for (int i = move.fromY + 1; i < BOARD_SIZE; i++) {
        if (temp[move.fromX][i] == '.') {
            profit--;
        }
        else break;
    }
    for (int i = move.toX - 1; i >= 0; i--) {
        if (temp[i][move.toY] == '.') {
            profit++;
        }
        else break;
    }
    for (int i = move.toX + 1; i < BOARD_SIZE; i++) {
        if (temp[i][move.toY] == '.') {
            profit++;
        }
        else break;
    }
    for (int i = move.toY - 1; i >= 0; i--) {
        if (temp[move.toX][i] == '.') {
            profit++;
        }
        else break;
    }
    for (int i = move.toY + 1; i < BOARD_SIZE; i++) {
        if (temp[move.toX][i] == '.') {
            profit++;
        }
        else break;
    }
    int temX = 0, temY = 0;
    int i = move.arrowX - 1;
    int j = move.arrowX + 1;
    for (; i >= 0; i--) {
        if (temp[i][move.arrowY] == '.') {
            temX++;
        }
        else break;
    }
    for (; j < BOARD_SIZE; j++) {
        if (temp[j][move.arrowY] == '.') {
            temY++;
        }
        else break;
    }
    if (temp[i][move.arrowY] == 'C') {
        profit -= temY;
    }
    else if (temp[i][move.arrowX] == 'H') {
        profit += temY;
    }
    if (temp[j][move.arrowY] == 'C') {
        profit -= temX;
    }
    else if (temp[j][move.arrowY] == 'H') {
        profit += temX;
    }
    i = move.arrowY - 1;
    j = move.arrowY + 1;
    temX = 0; temY = 0;
    for ( ; i >= 0; i--) {
        if (temp[move.arrowX][i] == '.') {
            temX++;
        }
        else {
            break;
        }
    }
    for (int; j < BOARD_SIZE; j++) {
        if (temp[move.arrowX][j] == '.') {
            temY++;
        }
        else {
            break;
        }
    }

    if (temp[move.arrowX][i] == 'C') {
        profit -= temY;
    }
    else if (temp[move.arrowX][i] == 'H') {
        profit += temY;
    }
    if (temp[move.arrowX][j] == 'C') {
        profit -= temX;
    }
    else if (temp[move.arrowX][j] == 'H') {
        profit += temX;
    }
    return profit;
    // 这里可以继续实现后续逻辑
}