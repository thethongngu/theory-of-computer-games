#include <ostream>
#include "board.h"

/**
 * place a tile (index value) to the specific position (1-d form index)
 * return 0 if the action is valid, or -1 if not
 */
Board::Reward Board::place(unsigned pos, Board::Cell tile) {
    if (pos >= 16) return -1;
    if (tile != 1 && tile != 2) return -1;
    operator()(pos) = tile;
    return 0;
}

/**
 * apply an action to the board
 * return the reward of the action, or -1 if the action is illegal
 */
Board::Reward Board::slide(unsigned opcode) {
    switch (opcode & 0b11) {
        case 0: return slide_up();
        case 1: return slide_right();
        case 2: return slide_down();
        case 3: return slide_left();
        default: return -1;
    }
}

Board::Reward Board::slide_left() {
    Board prev = *this;
    Board::Reward score = 0;
    for (int r = 0; r < 4; r++) {
        auto& row = tile[r];
        int top = 0, hold = 0;
        for (int c = 0; c < 4; c++) {
            int tile = row[c];
            if (tile == 0) continue;
            row[c] = 0;
            if (hold) {
                if (tile == hold) {
                    row[top++] = ++tile;
                    score += (1 << tile);
                    hold = 0;
                } else {
                    row[top++] = hold;
                    hold = tile;
                }
            } else {
                hold = tile;
            }
        }
        if (hold) tile[r][top] = hold;
    }
    return (*this != prev) ? score : -1;
}
Board::Reward Board::slide_right() {
    reflect_horizontal();
    Reward score = slide_left();
    reflect_horizontal();
    return score;
}
Board::Reward Board::slide_up() {
    rotate_right();
    Reward score = slide_right();
    rotate_left();
    return score;
}
Board::Reward Board::slide_down() {
    rotate_right();
    Reward score = slide_left();
    rotate_left();
    return score;
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
        for (auto t : row) out << std::setw(6) << ((1 << t) & -2u);
        out << "|" << std::endl;
    }
    out << "+------------------------+" << std::endl;
    return out;
}

