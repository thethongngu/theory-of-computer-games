//
// Created by thethongngu on 12/14/19.
//

#ifndef PROJECT04_BOARD_H
#define PROJECT04_BOARD_H

#endif //PROJECT04_BOARD_H

#include <cassert>
#include <ctime>
#include <cstdlib>
#include "global.h"
#include "bit_board.h"

class Board {
public:

    static std::vector<int> adj_cells[NUM_CELL];

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
    BitBoard state[2];  // BLACK & WHITE
    int parent[NUM_CELL];

    BitBoard lib[NUM_CELL];
    int lib_count[NUM_CELL];

    BitBoard zero_pos[2];
    BitBoard one_pos[2];

    int two_go[NUM_CELL], one_black_go[NUM_CELL], one_white_go[NUM_CELL];
    int ntwo, none_black, none_white;

    Board() {
        clear_all();
    }

    void print() {
        std::clog << " ";
        for (int i = 0; i < BOARDSIZE - 1; i++) std::clog << " " << (char) (i + 'a');
        std::clog << " j";
        for (int i = 0; i < NUM_CELL; i++) {
            if (i % BOARDSIZE == 0) std::clog << std::endl << i / BOARDSIZE + 1;
            if (state[BLACK].get(i) == 1) std::clog << " B";
            else if (state[WHITE].get(i) == 1) std::clog << " W";
            else std::clog << " +";
        }
        std::clog << std::endl << std::endl;
    }

    int get_root(int i) {
        assert(i >= 0 && i < NUM_CELL);
        while (i != parent[i]) i = parent[i];
        return i;
    }

    void merge(int x, int y) {
        int rx = get_root(x);
        int ry = get_root(y);
        if (rx < ry) parent[ry] = rx; else parent[rx] = ry;
    }

    bool is_occupied(int pos, int color) {
        return state[color].get(pos) == 1;
    }

    bool is_empty(int pos) {
        return state[WHITE].get(pos) == 0 && state[BLACK].get(pos) == 0;
    }

    void clear_all() {
        state[0].clear();
        state[1].clear();
        zero_pos[0].clear();
        zero_pos[1].clear();
        one_pos[0].clear();
        one_pos[1].clear();
        ntwo = none_white = none_black = 0;
        srand(time(NULL));
        for (int i = 0; i < NUM_CELL; i++) {
            lib[i].clear();
            lib_count[i] = 0;
            parent[i] = i;
        }
    }

    void add_piece(int pos, int color) {
        assert(pos >= 0 && pos < NUM_CELL);
        assert(color == BLACK || color == WHITE);

        int xxxxx = change_color(color);
        BitBoard lib_after;

        zero_pos[color].on_bit(pos);
        zero_pos[xxxxx].on_bit(pos);
        state[color].on_bit(pos);

        for (int nei: adj_cells[pos]) {
            if (is_occupied(nei, color)) {
                int root_nei = get_root(nei);
                lib_after |= lib[root_nei];
                lib[root_nei].clear();
                lib_count[root_nei] = 0;
                merge(pos, root_nei);
            } else if (is_occupied(nei, xxxxx)) {
                int root_nei = get_root(nei);
                lib[root_nei].off_bit(pos);
                lib_count[root_nei] = lib[root_nei].count();

                if (lib_count[root_nei] == 1) {
                    zero_pos[color] |= lib[root_nei];
                    one_pos[xxxxx] |= lib[root_nei];
                }
            } else {
                lib_after.on_bit(nei);
                one_pos[xxxxx].on_bit(nei);
            }
        }

        lib_after.off_bit(pos);
        int root_pos = get_root(pos);
        lib[root_pos] = lib_after;
        lib_count[root_pos] = lib_after.count();

        if (lib_count[root_pos] == 1) {
            zero_pos[xxxxx] |= lib_after;
            one_pos[color] |= lib_after;
        }
    }

    bool can_move(int pos, int color) {
        assert(pos >= 0 && pos < NUM_CELL);
        assert(color == BLACK || color == WHITE);

        if (zero_pos[color].get(pos)) return false;  // capture move
        if (one_pos[color].get(pos) == 0) return true;  // 2-Go

        one_pos[color].off_bit(pos);  // now 1-Go
        int xxxxx = change_color(color);

        for (int nei: adj_cells[pos]) {
            if (is_occupied(nei, color) && lib_count[get_root(nei)] > 1) return true;  // no suicide
            if (is_empty(nei)) return true;  // no suicide
        }

        zero_pos[color].on_bit(pos);  // suicide at `pos`
        return false;
    }

    void recheck_moves() {

        ntwo = none_black = none_white = 0;

        for (int pos = 0; pos < NUM_CELL; pos++) {
            if (!is_empty(pos)) continue;
            bool can_black = can_move(pos, BLACK);
            bool can_white = can_move(pos, WHITE);
            if (!can_black && !can_white) continue;

            if (can_black && can_white) {
                two_go[ntwo++] = pos;
            } else {
                if (can_black) one_black_go[none_black++] = pos;
                else one_white_go[none_white++] = pos;
            }
        }
    }

    int get_2_go() {
        if (ntwo == 0) return -1;
        int pos = std::rand() % ntwo;
        int res = two_go[pos];  two_go[pos] = two_go[ntwo - 1];  ntwo--;

        return res;
    }

    void add_1_go(int pos, int color) {
        if (color == BLACK) one_black_go[none_black++] = pos;
        else one_white_go[none_white++] = pos;
    }

    int get_1_go(int color) {
        int res;

        if (color == BLACK) {
            if (none_black == 0) return -1;
            int pos = std::rand() % none_black;
            res = one_black_go[pos];  one_black_go[pos] = one_black_go[none_black - 1];  none_black--;
        } else {
            if (none_white == 0) return -1;
            int pos = std::rand() % none_white;
            res = one_white_go[pos];  one_white_go[pos] = one_white_go[none_white - 1];  none_white--;
        }

        return res;
    }
};

std::vector<int> Board::adj_cells[NUM_CELL];