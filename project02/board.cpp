#include <iostream>
#include "board.h"

#define debug(a) std::cout << #a << " = " << a << std::endl
#define print(a) std::cout << a << std::endl

const Board::Reward Board::kTileScore[15] = {0, 0, 0, 3, 9, 27, 81, 243, 729, 2187, 6561, 19683, 59049, 177147, 531441};
const Board::Cell Board::kTileValue[15] = {0, 1, 2, 3, 6, 12, 24, 48, 96, 192, 384, 768, 1536, 3072, 6144};
std::map<unsigned long long, unsigned long long> Board::pre_left;

/**
 * place a tile (index value) to the specific position (1-d form index)
 * return 0 if the action is valid, or -1 if not
 */
Board::Reward Board::place(unsigned pos, Board::Cell tile_id) {
    if (pos >= 16) return -1;
    if (get_cell(pos) != 0) return -1;
    if (tile_id != 1 && tile_id != 2 && tile_id != 3) return -1;
    set_cell(pos, tile_id);
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

Board::Reward Board::get_curr_score() {
    int res = 0;
    for(int i = 0; i < 16; i++) {
        res += kTileScore[get_cell(i)];
    }
    return res;
}

Board::Reward Board::slide_left() {

//    debug(*this);

    Board prev = *this;
    for(int i = 0; i < 4; i++) {
        Row row = get_row(i);
        Row new_row = pre_left[row];
        set_row(i, new_row);
    }

//    debug(*this);

    return (*this != prev) ? get_curr_score() : -1;
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
    Board::Cell cell01, cell02;

//    debug(*this);

    cell01 = get_cell(1);  cell02 = get_cell(4);
    set_cell(1, cell02);   set_cell(4, cell01);

    cell01 = get_cell(2);  cell02 = get_cell(8);
    set_cell(2, cell02);   set_cell(8, cell01);

    cell01 = get_cell(3);  cell02 = get_cell(12);
    set_cell(3, cell02);   set_cell(12, cell01);

    cell01 = get_cell(6);  cell02 = get_cell(9);
    set_cell(6, cell02);   set_cell(9, cell01);

    cell01 = get_cell(7);  cell02 = get_cell(13);
    set_cell(7, cell02);   set_cell(13, cell01);

    cell01 = get_cell(11);  cell02 = get_cell(14);
    set_cell(11, cell02);   set_cell(14, cell01);

//    debug(*this);
}

void Board::reflect_horizontal() {

//    debug(*this);
    for (int r = 0; r < 4; r++) {

        Board::Cell cell00 = get_cell((r * 4) + 0);
        Board::Cell cell01 = get_cell((r * 4) + 1);
        Board::Cell cell02 = get_cell((r * 4) + 2);
        Board::Cell cell03 = get_cell((r * 4) + 3);

        set_cell((r * 4) + 0, cell03);
        set_cell((r * 4) + 1, cell02);
        set_cell((r * 4) + 2, cell01);
        set_cell((r * 4) + 3, cell00);
    }
//    debug(*this);
}

void Board::reflect_vertical() {

//    debug(*this);
    Board::Row row00 = get_row(0);
    Board::Row row01 = get_row(1);
    Board::Row row02 = get_row(2);
    Board::Row row03 = get_row(3);

    set_row(0, row03);
    set_row(1, row02);
    set_row(2, row01);
    set_row(3, row00);
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

Board::Cell Board::get_cell(unsigned i) const {
    return (tile >> (i * 4)) & 0b1111ull;
}

void Board::set_cell(unsigned i, Board::Cell value) {
    unsigned long long remove_mask = 18446744073709551615ull - ((1ull << (4 * (i + 1))) - 1) + ((1ull << (4 * i)) - 1);
    unsigned long long add_mask = (unsigned long long)value << (i * 4);
    tile = (tile & remove_mask) | add_mask;
}

Board::Row Board::get_row(unsigned i) const {
    unsigned long long row = (tile >> (i * 16ull)) & 0xffffull;
    return ((row & 0xf000ull) >> 12ull) | ((row & 0x0f00ull) >> 4ull) |
           ((row & 0x00f0ull) << 4ull) | ((row & 0x000full) << 12ull);
}

void Board::set_row(unsigned i, Board::Row value) {
    unsigned long long reflect = ((value & 0xf000ull) >> 12ull) | ((value & 0x0f00ull) >> 4ull) |
                                 ((value & 0x00f0ull) << 4ull) | ((value & 0x000full) << 12ull);

    unsigned long long remove_mask = 18446744073709551615ull - ((1ull << (16 * (i + 1))) - 1) + ((1ull << (16 * i)) - 1);
    unsigned long long add_mask = reflect << (i * 16);
    tile = (tile & remove_mask) | add_mask;
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

                    unsigned long long to = row[3] + (row[2] << 4ull) + (row[1] << 8ull) + (row[0] << 12ull);
                    Board::pre_left[from] = to;
                }
            }
        }
    }
}







