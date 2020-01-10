//
// Created by thethongngu on 2020-01-11.
//

#ifndef PROJECT05_BOARD_H
#define PROJECT05_BOARD_H

#endif //PROJECT05_BOARD_H

#include "global.h"
#include "bit_board.h"
#include <vector>
#include <cassert>
#include <string>

using namespace std;

class Board {
public:

    Board();

    static vector<int> adj_cells[NUM_CELL];

    static void generate_all_adjs() {
        for (int i = 0; i < BOARDSIZE; i++) {
            for (int j = 0; j < BOARDSIZE; j++) {
                int pos = i * BOARDSIZE + j;
                if (i > 0) adj_cells[pos].push_back(pos - BOARDSIZE);
                if (j > 0) adj_cells[pos].push_back(pos - 1);
                if (i < BOARDSIZE - 1) adj_cells[pos].push_back(pos + BOARDSIZE);
                if (j < BOARDSIZE - 1) adj_cells[pos].push_back(pos + 1);
            }
        }
    }

    static int change_color(int color) {
        assert(color == BLACK || color == WHITE);
        if (color == BLACK) return WHITE; else return BLACK;
    }

public:

    BitBoard zero_go[2];
    BitBoard not_2_go[2];

    static char black_path[NUM_CELL + 10];
    static int num_black;
    static char white_path[NUM_CELL + 10];
    static int num_white;
    BitBoard state[2];
    char root_region[NUM_CELL];
    BitBoard lib[NUM_CELL];
    char num_lib[NUM_CELL];

    int get_root_region(int pos);

    void merge_region(int x, int y);

    void add_piece(int pos, bool color);

    bool can_move(int pos, bool color);

    int just_play_color();

    void recheck_move(int *bone, int *wone, int *two, int &bsize, int &wsize, int &tsize);

    void clear_all();

    string inttostring(int i);

    double simulate(bool color, int black_one[NUM_CELL], int white_one[NUM_CELL], int two[NUM_CELL], int black_size,
                    int white_size, int two_size);

    inline void add_black_to_path(int k) {
        black_path[num_black] = k;
        num_black++;
    }

    inline void add_white_to_path(int k) {
        white_path[num_white] = k;
        num_white++;
    }
};