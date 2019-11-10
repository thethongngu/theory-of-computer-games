#include <iostream>
#include "board.h"

#define debug(a) std::cout << #a << " = " << a << std::endl
#define print(a) std::cout << a << std::endl

const Board::Reward Board::kTileScore[15] = {0, 0, 0, 3, 9, 27, 81, 243, 729, 2187, 6561, 19683, 59049, 177147, 531441};
const Board::Cell Board::kTileValue[15] = {0, 1, 2, 3, 6, 12, 24, 48, 96, 192, 384, 768, 1536, 3072, 6144};
unsigned int Board::pre_left[66000];
unsigned int Board::pre_score[66000];

/**
 * place a tile (index value) to the specific position (1-d form index)
 * return 0 if the action is valid, or -1 if not
 */
Board::Reward Board::place(unsigned pos, Board::Cell tile_id) {
    if (pos >= 16) return -1;
    if (get_cell(pos) != 0) return -1;
    if (tile_id != 1 && tile_id != 2 && tile_id != 3) return -1;
    set_cell(pos, tile_id);
    board_score += kTileScore[tile_id];
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

Board::Reward Board::get_curr_score() const {
    return board_score;
}

Board::Reward Board::slide_left() {

//    debug(*this);

    Board prev = *this;
    board_score = 0;

    for(int i = 0; i < 4; i++) {
        Row row = get_row(i);
        row = ((row & 0xf000ull) >> 12ull) | ((row & 0x0f00ull) >> 4ull) |
                ((row & 0x00f0ull) << 4ull) | ((row & 0x000full) << 12ull);

        Row new_row = pre_left[row];
        board_score += pre_score[new_row];

        new_row = ((new_row & 0xf000ull) >> 12ull) | ((new_row & 0x0f00ull) >> 4ull) |
                  ((new_row & 0x00f0ull) << 4ull) | ((new_row & 0x000full) << 12ull);
        set_row(i, new_row);
    }

//    debug(*this);

    return (*this != prev) ? board_score : -1;
}

Board::Reward Board::slide_right() {
    reflect_horizontal();
    Reward slide_score = slide_left();
    reflect_horizontal();
    return slide_score;
}
Board::Reward Board::slide_up() {
    transpose();
    Reward slide_score = slide_left();
    transpose();
    return slide_score;
}

Board::Reward Board::slide_down() {
    rotate_right();
    Reward slide_score = slide_left();
    rotate_left();
    return slide_score;
}

void Board::transpose() {
    tile = (tile & 0xf0f00f0ff0f00f0full) | ((tile & 0x0000f0f00000f0f0ull) << 12) | ((tile & 0x0f0f00000f0f0000ull) >> 12);
    tile = (tile & 0xff00ff0000ff00ffull) | ((tile & 0x00000000ff00ff00ull) << 24) | ((tile & 0x00ff00ff00000000ull) >> 24);
}

void Board::reflect_horizontal() {

//    debug(*this);
    tile = ((tile & 0x000f000f000f000full) << 12) | ((tile & 0x00f000f000f000f0ull) << 4)
          | ((tile & 0x0f000f000f000f00ull) >> 4) | ((tile & 0xf000f000f000f000ull) >> 12);
//    debug(*this);
}

void Board::reflect_vertical() {

//    debug(*this);
    tile = ((tile & 0x000000000000ffffull) << 48) | ((tile & 0x00000000ffff0000ull) << 16)
          | ((tile & 0x0000ffff00000000ull) >> 16) | ((tile & 0xffff000000000000ull) >> 48);
//    debug(*this);
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
    out << std::endl;
    out << "+------------------------+" << std::endl;
    for (int i = 0; i < 4; i++) {
        out << "|" << std::dec;
        for (int t = i * 4; t < (i + 1) * 4; t++) out << std::setw(6) << (Board::kTileValue[b.get_cell(t)]);
        out << "|" << std::endl;
    }
    out << "+------------------------+" << std::endl;
    return out;
}

bool Board::can_merge(Board::Cell cell01, Board::Cell cell02) {
    if (cell01 == 1 && cell02 == 2) return true;
    if (cell01 == 2 && cell02 == 1) return true;
    return cell01 > 2 && cell02 > 2 && cell01 == cell02;

}

Board::Cell Board::get_cell(unsigned long long i) const {
    return (tile >> (i << 2ull)) & 0x0full;
}

void Board::set_cell(unsigned long long i, Board::Cell value) {
    tile = (tile & ~(0x0full << (i << 2ull))) | (uint64_t(value & 0x0full) << (i << 2ull));
}

Board::Row Board::get_row(unsigned long long i) const {
    return (tile >> (i << 4ull)) & 0xffffull;
}

void Board::set_row(unsigned long long i, Board::Row value) {
    tile = (tile & ~(0xffffull << (i << 4ull))) | (uint64_t(value & 0xffffull) << (i << 4ull));
}

void Board::precompute_left() {

    for(unsigned int a0 = 0; a0 < 15; a0++) {
        for(unsigned int a1 = 0; a1 < 15; a1++) {
            for(unsigned int a2 = 0; a2 < 15; a2++) {
                for(unsigned int a3 = 0; a3 < 15; a3++) {

                    unsigned int row[4] = {a0, a1, a2, a3};
                    unsigned long long from = row[3] + (row[2] << 4ull) + (row[1] << 8ull) + (row[0] << 12ull);

                    for (int c = 0; c < 3; c++) {
                        if (Board::can_merge(row[c], row[c + 1])) {
                            Cell new_tile = std::max(row[c], row[c + 1]) + 1;
                            row[c] = new_tile;  row[c + 1] = 0;
                        }
                        if (row[c] == 0) {  // can slide
                            row[c] = row[c + 1];
                            row[c + 1] = 0;
                        }
                    }
                    long long score = 0;
                    for (unsigned int cell : row) score += Board::kTileScore[cell];

                    unsigned long long to = row[3] + (row[2] << 4ull) + (row[1] << 8ull) + (row[0] << 12ull);
                    Board::pre_left[from] = to;
                    Board::pre_score[to] = score;
                }
            }
        }
    }
}

Board::Grid Board::get_tile() const {
    return tile;
}







