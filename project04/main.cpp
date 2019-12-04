#include <iostream>
#include <cassert>
#include <vector>

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

    std::vector<Region> regions;
    int cell_to_region[NUM_CELL];
};

void init_board(Board &board) {
    board.first_seg = board.first_flag = 0;
    board.second_seg = board.second_flag = 0;
    board.regions.clear();
}

void init_region(Region &region) {
    region.first_seg = region.first_flag = 0;
    region.second_seg = region.second_flag = 0;
}

void get_adj_cells(int pos, std::vector<int> &res) {
    assert(pos >= 0 && pos <= NUM_CELL - 1);

    int d[4] = {-BOARD_SIZE, +1, +BOARD_SIZE, -1};

    for (int i = 0; i < 4; i++) {
        int new_pos = pos + d[i];
        if (new_pos >= 0 && new_pos <= NUM_CELL - 1) res.push_back(new_pos);
    }
}

int get_region_id_by_cell(const Board &tmp, int pos) {
    return tmp.cell_to_region[pos];
}

bool compare_region(Region ra, Region rb) {
    return ra.first_flag == rb.first_flag && ra.second_flag == rb.second_flag &&
           ra.first_seg == rb.first_seg && ra.second_seg == rb.second_seg;
}

void on_bit_region(Region &region, int i) {
    assert(i >= 0 && i <= NUM_CELL - 1);
    if (i < 64) {
        region.first_seg |= (1 << i);
        region.first_flag |= (1 << i);
    } else {
        region.second_seg |= (1 << i);
        region.second_flag |= (1 << i);
    }
}

void off_bit_region(Region &region, int i) {
    assert(i >= 0 && i <= NUM_CELL - 1);
    if (i < 64) {
        region.first_seg &= ~(1 << i);
        region.first_flag &= ~(1 << i);
    } else {
        region.second_seg &= ~(1 << i);
        region.second_flag &= ~(1 << i);
    }
}

void add_black_to_bit(Board &board, int i) {
    assert(i >= 0 && i <= NUM_CELL - 1);
    if (i < 64) {
        board.first_seg &= ~(1 << i);
        board.first_flag |= (1 << i);
    } else {
        board.second_seg &= ~(1 << i);
        board.second_flag |= (1 << i);
    }
}

void add_white_to_bit(Board &board, int i) {
    assert(i >= 0 && i <= NUM_CELL - 1);
    if (i < 64) {
        board.first_seg |= (1 << i);
        board.first_flag |= (1 << i);
    } else {
        board.second_seg |= (1 << i);
        board.second_flag |= (1 << i);
    }
}

void remove_cell_on_board(Board &board, int i) {
    assert(i >= 0 && i <= NUM_CELL - 1);
    if (i < 64) {
        board.first_seg &= ~(1 << i);
        board.first_flag &= ~(1 << i);
    } else {
        board.second_seg &= ~(1 << i);
        board.second_flag &= ~(1 << i);
    }
}

int get_region_cell(const Region &region, int pos) {
    assert(pos >= 0 && pos <= NUM_CELL - 1);
    if (pos < 64) {
        int is_empty = ~((region.first_flag & (1 << pos)) >> pos);
        if (is_empty) return NONE;

        int bit = (region.first_seg & (1 << pos)) >> pos;
        return (bit) ? WHITE : BLACK;

    } else {
        int is_empty = ~((region.second_flag & (1 << pos)) >> pos);
        if (is_empty) return NONE;

        int bit = (region.second_seg & (1 << pos)) >> pos;
        return (bit) ? WHITE : BLACK;
    }
}

int get_board_cell(const Board &board, int pos) {
    assert(pos >= 0 && pos <= NUM_CELL - 1);
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

void merge_region(int fr_id, int sr_id, Board &board) {
    // one cell can not in 2 different regions
    Region &fr = board.regions[fr_id];
    Region &sr = board.regions[sr_id];

    assert((fr.first_flag & sr.first_flag) != 0);
    assert((fr.second_flag & sr.second_flag) != 0);

    // merging region
    fr.first_seg |= sr.first_seg;
    fr.second_seg |= sr.second_seg;
    fr.first_flag |= sr.first_flag;
    fr.second_flag |= sr.second_flag;

    // update mapping cell to region
    // for each cell in second region, change mapping to first region
    for(int i = 0; i < 81; i++) {
        int sr_color = get_region_cell(sr, i);
        // TODO: how to assert color of two region similar?
        if (sr_color == NONE) continue;
        board.cell_to_region[i] = fr_id;
    }

    // remove the second regions out of board state
    board.regions.erase(board.regions.begin() + sr_id);
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
    assert(pos >= 0 && pos <= NUM_CELL - 1);
    assert(color == BLACK || color == WHITE);

    if (get_board_cell(board, pos) != NONE) return -1;

    Board tmp = board;  // copy value
    if (color == BLACK) add_black_to_bit(tmp, pos);
    else add_white_to_bit(tmp, pos);

    // update region information
    std::vector<int> adjs;
    get_adj_cells(pos, adjs);
    int pos_region_id = get_region_id_by_cell(tmp, pos);

    // check whether regions in 4 direction are the same
    for (int &adj : adjs) {
        if (get_board_cell(tmp, adj) != color) continue;
        int adj_region_id = get_region_id_by_cell(tmp, adj);
        if (pos_region_id == adj_region_id) continue;
        merge_region(pos_region_id, adj_region_id, tmp);
    }


}

/** ----------------- Coordinator ----------------------- */



/** ----------------- Node ------------------------- */
/** ----------------- MCTS ----------------------- */
/** ----------------- Agent ----------------------- */


int main() {

    return 0;
}