//
// Created by Thong Nguyen on 11/19/19.
//

#include <queue>
#include "board.h"

int Board::BOARD_SIZE = 9;
int Board::NUM_CELL = 81;
int Board::BLACK = 1;
int Board::WHITE = 2;

Board::Board() {
    for(int i = 0; i < NUM_CELL; i++) board[i] = 0;
}

std::vector<Board::Cell> Board::get_liberties(unsigned pos, Board::Color color) {
    std::vector<Board::Cell> res;
    std::queue<Board::Cell> q;
    bool checked[NUM_CELL];
    int d[4] = {-BOARD_SIZE, 1, +BOARD_SIZE, -1};

    for(int i = 0; i < NUM_CELL; i++) checked[i] = false;

    q.push(pos);
    checked[pos] = true;
    while (!q.empty()) {
        Board::Cell cell = q.front();  q.pop();
        for (int offset : d) {
            Cell new_cell = cell + offset;
            if (new_cell < 0 || new_cell >= NUM_CELL || checked[new_cell]) continue;
            if (board[new_cell] == color) {
                checked[new_cell] = true;
                q.push(new_cell);
            } else if (board[new_cell] == 0) {
                checked[new_cell] = true;
                res.push_back(new_cell);
            }
        }
    }

    return res;
}

bool Board::is_capture(unsigned pos, Board::Color color) {

}

bool Board::is_suicide(unsigned pos, Board::Color color) {

}

int Board::place(unsigned pos, Color color) {
    if (board[pos] != 0) return -1;
    if (is_capture(pos, color)) return -1;
    if (is_suicide(pos, color)) return -1;
    board[pos] = color;
    return 1;
}
