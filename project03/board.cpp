//
// Created by Thong Nguyen on 11/19/19.
//

#include "board.h"

int Board::BOARD_SIZE = 9;
int Board::BLACK = 1;
int Board::WHITE = 2;

Board::Board() {
    for(int i = 0; i < BOARD_SIZE; i++) {
        for(int j = 0; j < BOARD_SIZE; j++) {
            board[i][j] = 0;
        }
    }
}

/** Return -1 if move is not valid, */
int Board::place(unsigned x, unsigned y, Board::Color color) {
    if (board[x][y] != 0) return -1;
    board[x][y] = color;
    return 1;
}
