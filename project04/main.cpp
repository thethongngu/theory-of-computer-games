#include <iostream>
#include <cassert>
#include <vector>

#define ull unsigned long long
#define ui  unsigned int

/** ----------------- Board & Region ----------------------- */
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
 */

void get_adj_cells(int pos, std::vector<int> &res) {
    assert(pos >= 0 && pos <= NUM_CELL - 1);

    int d[4] = {-BOARD_SIZE, +1, +BOARD_SIZE, -1};

    for (int i = 0; i < 4; i++) {
        int new_pos = pos + d[i];
        if (new_pos >= 0 && new_pos <= NUM_CELL - 1) res.push_back(new_pos);
    }
}

struct Region {

    // region info
    ull first_seg = 0;
    ui second_seg = 0;
    ull first_flag = 0;
    ui second_flag = 0;

    // liberty info
    ull first_lib = 0;
    ui second_lib = 0;

};

struct Board {
    ull first_seg = 0;
    ui second_seg = 0;
    ull first_flag = 0;
    ui second_flag = 0;

    std::vector<Region> regions;
    int cell_to_region[NUM_CELL];
};

/**
 * - Change the segment to BLACK (0)
 * - Change the flag to 1
 */
void add_black_bit_region(Region &region, int i) {
    assert(i >= 0 && i <= NUM_CELL - 1);
    if (i < 64) {
        region.first_seg &= ~(1 << i);
        region.first_flag |= (1 << i);
    } else {
        region.second_seg &= ~(1 << i);
        region.second_flag |= (1 << i);
    }
}

/**
 * - Change the segment to WHITE (1)
 * - Change the flag to 1
 */
void add_white_bit_region(Region &region, int i) {
    assert(i >= 0 && i <= NUM_CELL - 1);
    if (i < 64) {
        region.first_seg |= (1 << i);
        region.first_flag |= (1 << i);
    } else {
        region.second_seg |= (1 << i);
        region.second_flag |= (1 << i);
    }
}

/**
 * - Change the segment to BLACK (0)
 * - Change the flag to 1
 */
void add_black_bit_board(Board &board, int i) {
    assert(i >= 0 && i <= NUM_CELL - 1);
    if (i < 64) {
        board.first_seg &= ~(1 << i);
        board.first_flag |= (1 << i);
    } else {
        board.second_seg &= ~(1 << i);
        board.second_flag |= (1 << i);
    }
}

/**
 * - Change the segment to BLACK (0)
 * - Change the flag to 1
 */
void add_while_bit_board(Board &board, int i) {
    assert(i >= 0 && i <= NUM_CELL - 1);
    if (i < 64) {
        board.first_seg |= (1 << i);
        board.first_flag |= (1 << i);
    } else {
        board.second_seg |= (1 << i);
        board.second_flag |= (1 << i);
    }
}

/**
 * - Change the segment to corresponding color
 * - Change the flag to 1
 */
void add_bit_region(Region &region, int pos, int color) {
    assert(color == BLACK || color == WHITE);
    assert(pos >= 0 && pos <= 80);

    if (color == BLACK) add_black_bit_region(region, pos);
    else add_white_bit_region(region, pos);
}

/**
 * - Change the segment to corresponding color
 * - Change the flag to 1
 */
void add_bit_board(Board &board, int pos, int color) {
    assert(color == BLACK || color == WHITE);
    assert(pos >= 0 && pos <= 80);

    if (color == BLACK) add_black_bit_board(board, pos);
    else add_while_bit_board(board, pos);
}

int get_region_id_by_cell(const Board &board, int pos) {
    return board.cell_to_region[pos];
}

/**
 * - Change the segment to 0
 * - Change the flag to 0
 */
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

/**
 * Return color at the position of a Region
 */
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

/**
 * Return color at the position of a Board
 */
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

/**
 * Check and add new liberties in list of adj_cells to region
 * @param region
 * @param board
 * @param adj_cells
 */
void add_liberty_region(Region &region, const Board &board, const std::vector<int> &adj_cells) {
    for (int adj : adj_cells) {
        assert(adj >= 0 && adj <= 80);
        if (get_board_cell(board, adj) != NONE) continue;
        if (adj < 64) region.first_lib |= (1 << adj);
        else region.second_lib |= (1 << adj);
    }
}

/**
 * - Add new region to list of regions
 * - Update cell information of board which is inside the region
 */
void add_region_to_board(Region region, Board &board, int color) {

    board.regions.push_back(region);
    int region_id = board.regions.size() - 1;

    for (int pos = 0; pos < NUM_CELL; pos++) {
        int cell_color = get_region_cell(region, pos);
        if (cell_color == color)
            board.cell_to_region[pos] = region_id;
    }
}

/**
 * Merge regions
 *  - Update cell information of second region into first region
 *  - Update mapping of cell from second region to first region
 *  - Remove second region from board information
 *
 * @param fr_id id of the first region
 * @param sr_id id of the second region
 * @param board the board that contains two regions
 */
void merge_region(int fr_id, int sr_id, Board &board) {

    Region &fr = board.regions[fr_id];
    Region &sr = board.regions[sr_id];

    // one cell can not in 2 different regions
    assert((fr.first_flag & sr.first_flag) != 0);
    assert((fr.second_flag & sr.second_flag) != 0);

    // merging region
    fr.first_seg |= sr.first_seg;
    fr.second_seg |= sr.second_seg;
    fr.first_flag |= sr.first_flag;
    fr.second_flag |= sr.second_flag;

    // update mapping cell to region
    // for each cell in second region, change mapping to first region
    for (int i = 0; i < 81; i++) {
        int sr_color = get_region_cell(sr, i);
        // TODO: how to assert color of two region similar?
        if (sr_color == NONE) continue;
        board.cell_to_region[i] = fr_id;
    }

    // remove the second regions out of board state
    board.regions.erase(board.regions.begin() + sr_id);
}

/**
 * - Update board state (turn on bit in board)
 * - Update regions state
 *      - Update new cell in region
 *      - Update mapping from cell to region
 *      - Update liberties in region
 */
void update_board_info(Board &board, int pos, int color) {

    assert(color == BLACK || color == WHITE);
    assert(pos >= 0 && pos <= 80);

    add_bit_board(board, pos, color);

    std::vector<int> adj_cells;
    int pivot_id = -1;

    // get pivot region for merging other into
    get_adj_cells(pos, adj_cells);
    for (int &adj: adj_cells) {
        if (get_board_cell(board, adj) != color) continue;
        pivot_id = get_region_id_by_cell(board, adj);
        break;
    }

    if (pivot_id == -1) {  // no merging region, new region
        Region region;
        add_bit_region(region, pos, color);
        add_liberty_region(region, board, adj_cells);
        add_region_to_board(region, board, color);

    } else {  // merging region

        for(int &adj: adj_cells) {
            if (get_board_cell(board, adj) != color) continue;
            int next_id = get_region_id_by_cell(board, adj);
            if (pivot_id == next_id) continue;
            merge_region(pivot_id, next_id, board);
        }
    }

    add_bit_region(board.regions[pivot_id], pos, color);
    add_liberty_region(board.regions[pivot_id], board, adj_cells);
}

bool is_suicide(const Board &board, int pos) {
    int region_id = get_region_id_by_cell(board, pos);
    return get_region_liberties(board, region_id);
}

bool is_capture(const Board &board, int pos) {
//    std::vector<int> adjs;
//    get_adj_cells(pos, adjs);

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
    update_board_info(tmp, pos, color);

    if (is_suicide(tmp, pos)) return -1;
    if (is_capture(tmp, pos)) return -1;

    board = tmp;
    return 1;
}

/** ----------------- Coordinator ----------------------- */



/** ----------------- Node ------------------------- */
/** ----------------- MCTS ----------------------- */
/** ----------------- Agent ----------------------- */


int main() {

    return 0;
}