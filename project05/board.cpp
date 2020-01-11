//
// Created by thethongngu on 12/14/19.
//

#ifndef PROJECT04_BOARD_H
#define PROJECT04_BOARD_H

#endif //PROJECT04_BOARD_H

#include <cstdlib>
#include <cstring>

#include "global.h"
#include "board.h"

std::vector<int> Board::adj_cells[NUM_CELL];

char Board::black_path[NUM_CELL + 10];
int Board::num_black;
char Board::white_path[NUM_CELL + 10];
int Board::num_white;

Board::Board() {}

int Board::get_root_region(int pos) {
    char &pa = root_region[pos];
    while (pa != root_region[pa]) pa = root_region[pa];
    return pa;
}

void Board::merge_region(int x, int y) {
    char i = get_root_region(x), j = get_root_region(y);
    if (i < j)root_region[j] = i;
    else root_region[i] = j;
}

void Board::add_piece(int pos, bool color) {

    int i, nei, root_nei;
    BitBoard new_lib;

    zero_go[0].on_bit(pos);
    zero_go[1].on_bit(pos);
    state[color].on_bit(pos);

    for (i = 0; i < adj_cells[pos].size(); i++) {
        nei = adj_cells[pos][i];
        if (state[color].get(nei)) {
            root_nei = get_root_region(nei);
            new_lib |= lib[root_nei];
            lib[root_nei].clear();
            num_lib[root_nei] = 0;
            merge_region(pos, root_nei);

        } else if (state[!color].get(nei)) {

            root_nei = get_root_region(nei);
            lib[root_nei].off_bit(pos);
            num_lib[root_nei] = lib[root_nei].count();

            if (num_lib[root_nei] == 1) {
                zero_go[color] |= lib[root_nei];
                not_2_go[!color] |= lib[root_nei];
            }
        } else {
            not_2_go[!color].on_bit(nei);
            new_lib.on_bit(nei);
        }
    }

    new_lib.off_bit(pos);
    i = get_root_region(pos);
    lib[i] = new_lib;
    num_lib[i] = lib[i].count();

    if (num_lib[i] == 1) {
        zero_go[!color] |= lib[i];
        not_2_go[color] |= lib[i];
    }

}

bool Board::can_move(int pos, bool color) {

    if (zero_go[color].get(pos)) return false;
    else if (not_2_go[color].get(pos) == 0) return true;

    not_2_go[color].off_bit(pos);

    for (int i : adj_cells[pos]) {
        int nei = i;
        if (state[color].get(nei)) {
            if (num_lib[get_root_region(nei)] != 1) return true;
        } else if (!state[!color].get(nei)) return true;
    }

    zero_go[color].on_bit(pos);

    return false;
}

int Board::just_play_color() {
    int bc = state[0].count();
    int wc = state[1].count();
    return (bc == wc) ? WHITE : BLACK;
}

void Board::recheck_move(int *bone, int *wone, int *two, int &bsize, int &wsize, int &tsize) {

    bool can_black, can_white;
    bsize = wsize = tsize = 0;

    for (int i = 0; i < NUM_CELL; i++) {
        if (!state[0].get(i) && !state[1].get(i)) {

            can_black = can_move(i, BLACK);
            can_white = can_move(i, WHITE);

            if (can_black && can_white) {
                two[tsize] = i;
                tsize++;
            } else {
                if (can_black) {
                    bone[bsize] = i;
                    bsize++;
                } else if (can_white) {
                    wone[wsize] = i;
                    wsize++;
                }
            }
        }
    }
}

void Board::clear_all() {
    for (int i = 0; i < NUM_CELL; i++) root_region[i] = i;
    memset(lib, 0, sizeof(lib));
    memset(num_lib, 0, sizeof(num_lib));

    state[0].clear();
    state[1].clear();
    zero_go[0].clear();
    zero_go[1].clear();
    not_2_go[0].clear();
    not_2_go[1].clear();
}

double Board::simulate(
        bool color, int one_black[NUM_CELL], int one_white[NUM_CELL], int two[NUM_CELL],
        int bsize, int wsize, int tsize) {

    int i, pos;
    bool can_black, can_white;

    TWO_GO:
    while (tsize > 0) {
        i = rand() % tsize;
        pos = two[i];
        two[i] = two[tsize - 1];
        tsize--;
        can_black = can_move(pos, BLACK);
        can_white = can_move(pos, WHITE);
        if (can_move(pos, color)) {
            if (color == 0) black_path[num_black++] = pos;
            else white_path[num_white++] = pos;

            add_piece(pos, color);
            color = !color;
            goto TWO_GO;

        } else {
            if (!can_black && can_white) {
                one_white[wsize] = pos;
                wsize++;
            } else if (can_black && !can_white) {
                one_black[bsize] = pos;
                bsize++;
            }
        }
    }

    ONE_GO :
    if (color == 0) {

        while (bsize > 0) {
            i = rand() % bsize;
            pos = one_black[i];
            one_black[i] = one_black[bsize - 1];
            bsize--;
            if (can_move(pos, color)) {
                add_piece(pos, color);
                color = !color;
                goto ONE_GO;
            }
        }
    } else {
        while (wsize > 0) {
            i = rand() % wsize;
            pos = one_white[i];
            one_white[i] = one_white[wsize - 1];
            wsize--;
            if (can_move(pos, color)) {
                add_piece(pos, color);
                color = !color;
                goto ONE_GO;
            }
        }
    }
    return (color == WHITE) ? 1 : -1;
}
