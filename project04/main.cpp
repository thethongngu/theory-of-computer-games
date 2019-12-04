#include <iostream>
#include <cassert>

#define ull unsigned long long
#define ui  unsigned int

/** ----------------- Board ----------------------- */
const int BOARD_SIZE = 9;
const int NUM_CELL = BOARD_SIZE * BOARD_SIZE;

const int NONE = 0;
const int BLACK = 1;
const int WHITE = 2;

/**
 *   a b c d e f g h i
 * 1 + + + + + + + + +
 * 2 + + + + + + + + +
 * 3 + + + + + + + + +
 * 4 + + + + + + + + +
 * 5 + + + + + + + + +
 * 6 + + + + + + + + +
 * 7 + + + + + + + + +
 * 8 + + + + + + + + +
 * 9 + + + + + + + + +
 *
 *             i7 <---------------------------------------------------------------------- a1
 * first_seg: 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000
 *
 *              i9 <------------------------------ a8
 * second_seg: 0000 0000 0000 0000 0000 0000 0000 0000
 *
 * first_seg, second_seg:
 *  - 0: BLACK
 *  - 1: WHITE
 *
 * first_flag & second_flag:
 *  - 0: empty
 *  - 1: not empty
 *
 * 64 * 2 + 32 * 2 = 192 bit per a region/board
 *
 */
struct Region {
    ull first_seg;
    ui second_seg;
    ull first_flag;
    ui second_flag;
};

struct Board {
    ull first_seg;
    ui second_seg;
    ull first_flag;
    ui second_flag;
};

void init_board(Board &board) {
    board.first_seg = board.first_flag = 0;
    board.second_seg = board.second_flag = 0;
}

void on_bit_region(Region &region, int i) {
    assert(i >= 0 && i <= 80);
    if (i < 64) {
        region.first_seg |= (1 << i);
        region.first_flag |= (1 << i);
    } else {
        region.second_seg |= (1 << i);
        region.second_flag |= (1 << i);
    }
}

void off_bit_region(Region &region, int i) {
    assert(i >= 0 && i <= 80);
    if (i < 64) {
        region.first_seg &= ~(1 << i);
        region.first_flag &= ~(1 << i);
    } else {
        region.second_seg &= ~(1 << i);
        region.second_flag &= ~(1 << i);
    }
}

void on_bit_board(Board &board, int i) {
    assert(i >= 0 && i <= 80);
    if (i < 64) {
        board.first_seg |= (1 << i);
        board.first_flag |= (1 << i);
    } else {
        board.second_seg |= (1 << i);
        board.second_flag |= (1 << i);
    }
}

void off_bit_board(Board &board, int i) {
    assert(i >= 0 && i <= 80);
    if (i < 64) {
        board.first_seg &= ~(1 << i);
        board.first_flag &= ~(1 << i);
    } else {
        board.second_seg &= ~(1 << i);
        board.second_flag &= ~(1 << i);
    }
}

int get_board_cell(const Board &board, int pos) {
    assert(pos >= 0 && pos <= 80);
    if (pos < 64) {
        int is_empty = ~((board.first_flag & (1 << pos)) >> pos);
        if (is_empty) return NONE;

        int bit = (board.first_seg & (1 << pos)) >> pos;
        return (bit) ? WHITE : BLACK;

    } else {
        int is_empty = ~((board.second_flag & (1 << pos)) >> pos);
        if (is_empty) return NONE;

        int bit = (board.second_seg & (1 << pos)) >> pos;
        return (bit) ? WHITE : BLACK;
    }
}

/**
 * Set a cell of board require:
 *  - Empty cell
 *  - No suicide
 *  - No capture
 *
 * Return:
 *  - 1: success
 *  - 2: fail, board not change
 */
int set_board_cell(Board &board, int pos, int color) {
    assert(pos >=0 && pos <= 80);
    assert(color >=0 && color <= 2);

    if (get_board_cell(board, pos) != NONE) return -1;

    Board tmp_board = board;  // copy value
    if (color == BLACK) off_bit_board(board, pos);
    else on_bit_board(board, pos);

}

/** ----------------- Coordinator ----------------------- */



/** ----------------- Node ------------------------- */
/** ----------------- MCTS ----------------------- */
/** ----------------- Agent ----------------------- */


int main() {

    return 0;
}