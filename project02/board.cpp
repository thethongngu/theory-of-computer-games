#include <iostream>
#include "board.h"

#define debug(a) std::cout << #a << " = " << a << std::endl
#define print(a) std::cout << a << std::endl

const Board::Reward Board::kTileScore[15] = {0, 0, 0, 3, 9, 27, 81, 243, 729, 2187, 6561, 19683, 59049, 177147, 531441};
const Board::Cell Board::kTileValue[15] = {0, 1, 2, 3, 6, 12, 24, 48, 96, 192, 384, 768, 1536, 3072, 6144};

/**
 * place a tile (index value) to the specific position (1-d form index)
 * return 0 if the action is valid, or -1 if not
 */
Board::Reward Board::place(unsigned pos, Board::Cell tile_id) {
    if (pos >= 16) return -1;
    if (tile_id != 1 && tile_id != 2 && tile_id != 3) return -1;
    operator()(pos) = tile_id;
    return 0;
}

/**
 * apply an action to the board
 * return the reward of the action, or -1 if the action is illegal
 */
Board::Reward Board::slide(unsigned opcode) {
    switch (opcode & 0b11u) {
        case 0: return slide_up();
        case 1: return slide_right();
        case 2: return slide_down();
        case 3: return slide_left();
        default: return -1;
    }
}

Board::Reward Board::slide_left() {
    Board prev = *this;
    for (int r = 0; r < 4; r++) {
        auto& row = tile[r];
        for (int c = 0; c < 3; c++) {
            if (Board::can_merge(row[c], row[c + 1])) {
                Cell new_tile = std::max(row[c], row[c + 1]) + 1;
                set_score(get_score() - kTileScore[row[c]] - kTileScore[row[c + 1]] + kTileScore[new_tile]);
                row[c] = new_tile;  row[c + 1] = 0;
            }
            if (row[c] == 0) {  // can slide
                row[c] = row[c + 1];
                row[c + 1] = 0;
            }
        }
    }
    return (*this != prev) ? get_score() : -1;
}

void Board::set_score(Reward new_score) {
    score = new_score;
}

Board::Reward Board::slide_right() {
    reflect_horizontal();
    Reward slide_score = slide_left();
    reflect_horizontal();
    return slide_score;
}
Board::Reward Board::slide_up() {
    rotate_right();
    Reward slide_score = slide_right();
    rotate_left();
    return slide_score;
}
Board::Reward Board::slide_down() {
    rotate_right();
    Reward slide_score = slide_left();
    rotate_left();
    return slide_score;
}

void Board::transpose() {
    for (int r = 0; r < 4; r++) {
        for (int c = r + 1; c < 4; c++) {
            std::swap(tile[r][c], tile[c][r]);
        }
    }
}

void Board::reflect_horizontal() {
    for (int r = 0; r < 4; r++) {
        std::swap(tile[r][0], tile[r][3]);
        std::swap(tile[r][1], tile[r][2]);
    }
}

void Board::reflect_vertical() {
    for (int c = 0; c < 4; c++) {
        std::swap(tile[0][c], tile[3][c]);
        std::swap(tile[1][c], tile[2][c]);
    }
}

/**
 * rotate the board clockwise by given times
 */
void Board::rotate(int r) {
    switch (((r % 4) + 4) % 4) {
        default:
        case 0: break;
        case 1: rotate_right(); break;
        case 2: reverse(); break;
        case 3: rotate_left(); break;
    }
}

void Board::rotate_right() { transpose(); reflect_horizontal(); } // clockwise
void Board::rotate_left() { transpose(); reflect_vertical(); } // counterclockwise
void Board::reverse() { reflect_horizontal(); reflect_vertical(); }

std::ostream& operator <<(std::ostream& out, const Board& b) {
    out << "+------------------------+" << std::endl;
    for (auto& row : b.tile) {
        out << "|" << std::dec;
        for (auto t : row) out << std::setw(6) << (Board::kTileValue[t]);
        out << "|" << std::endl;
    }
    out << "+------------------------+" << std::endl;
    return out;
}

Board::Reward Board::get_score() {
    return score;
}

bool Board::can_merge(Board::Cell cell01, Board::Cell cell02) {
    if (cell01 == 1 && cell02 == 2) return true;
    if (cell01 == 2 && cell02 == 1) return true;
    if (cell01 > 2 && cell02 > 2 && cell01 == cell02) return true;

    return false;
}

