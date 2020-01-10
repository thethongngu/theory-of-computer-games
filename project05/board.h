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

    BitBoard ban[2];
    BitBoard notsafe[2];

    static char bpath[NUM_CELL + 10];
    static int bpsize;
    static char wpath[NUM_CELL + 10];
    static int wpsize;
    BitBoard bitb[2];
    char parent[NUM_CELL];
    BitBoard air[NUM_CELL];
    char countair[NUM_CELL];

    int get_root(int i);

    void unite(int x, int y);

    void getallair();

    void add_piece(int i, bool j);

    bool can_move(int i, bool j);

    inline bool get(int i, bool j);

    bool just_play_color();

    void getv(int bone[NUM_CELL], int wone[NUM_CELL], int two[NUM_CELL], int &bsize, int &wsize, int &tsize);

    void setdata();

    void clear_all();

    string inttostring(int i);

    double simulate(bool j, int bone[NUM_CELL], int wone[NUM_CELL], int two[NUM_CELL], int bsize, int wsize, int tsize);

    bool isempty();

    inline void addbp(int k) {
        bpath[bpsize] = k;
        bpsize++;
    }

    inline void addwp(int k) {
        wpath[wpsize] = k;
        wpsize++;
    }
};