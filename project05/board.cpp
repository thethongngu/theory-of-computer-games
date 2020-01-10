//
// Created by thethongngu on 12/14/19.
//

#ifndef PROJECT04_BOARD_H
#define PROJECT04_BOARD_H

#endif //PROJECT04_BOARD_H

#include <cassert>
#include <cstdlib>
#include <random>
#include <cstring>

#include "global.h"
#include "bit_board.h"

using namespace std;

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

    void add(int i, bool j);

    bool check(int i, bool j);

    Board();

    inline bool get(int i, bool j);

    bool just_play_color();

    void getv(int bone[NUM_CELL], int wone[NUM_CELL], int two[NUM_CELL], int &bsize, int &wsize, int &tsize);

    void setdata();

    void clear();

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

std::vector<int> Board::adj_cells[NUM_CELL];
char Board::bpath[NUM_CELL + 10];
int Board::bpsize;
char Board::wpath[NUM_CELL + 10];
int Board::wpsize;


int Board::get_root(int i) {
    char &j = parent[i];
    if (j == parent[j])return j;
    return j = get_root(j);
}

void Board::unite(int x, int y) {
    char i = get_root(x), j = get_root(y);
    if (i < j)parent[j] = i;
    else parent[i] = j;
}

void Board::add(int i, bool j)//j=0 black j=1 white
{
    int k, l, lp;
    BitBoard tmp;
#if SEARCH == 1
    zValue ^= ztable.ZHT[j][i];
#endif
    ban[0].on_bit(i);
    ban[1].on_bit(i);
    bitb[j].on_bit(i);
    for (k = 0; k < adj_cells[i].size(); k++) {
        l = adj_cells[i][k];
        if (bitb[j].get(l)) {
            lp = get_root(l);
            tmp |= air[lp];
            air[lp].clear();
            countair[lp] = 0;
            unite(i, lp);
        } else if (bitb[!j].get(l)) {
            lp = get_root(l);
            air[lp].off_bit(i);
            countair[lp] = air[lp].count();
            if (countair[lp] == 1) {
                ban[j] |= air[lp];
                notsafe[!j] |= air[lp];
            }
        } else {
            notsafe[!j].on_bit(l);
            tmp.on_bit(l);
        }
    }
    tmp.off_bit(i);
    k = get_root(i);
    air[k] = tmp;
    countair[k] = air[k].count();
    if (countair[k] == 1) {
        ban[!j] |= air[k];
        notsafe[j] |= air[k];
    }

}

bool Board::check(int i, bool j)//j=0 =>b
{
    //cout<<"check"<<i<<endl;
    bool flag = false;
    int l, k;
    if (ban[j].get(i))return false;
    else if (notsafe[j].get(i) == 0)return true;
    notsafe[j].off_bit(i);
#if search == 2
    // if(checkTriangle(i,j))return true;
#endif
    for (k = 0; k < adj_cells[i].size(); k++) {
        l = adj_cells[i][k];
        if (bitb[j].get(l)) {
            if (countair[get_root(l)] != 1)return true;
        } else if (!bitb[!j].get(l)) {
            return true;
        }
    }
    ban[j].on_bit(i);
    return false;
}

void Board::getallair() {
    int i, j, t, u, r, d, l;
    memset(air, 0, sizeof(air));
    memset(countair, 0, sizeof(countair));
    for (i = 0; i < BOARDSIZE; i++) {
        for (j = 0; j < BOARDSIZE; j++) {
            u = r = d = l = -1;
            t = i * BOARDSIZE + j;
            if (!bitb[0].get(t) && !bitb[1].get(t))//�ť�
            {
                if (i != 0) {
                    if (bitb[0].get(t - BOARDSIZE) || bitb[1].get(t - BOARDSIZE)) {
                        u = get_root(t - BOARDSIZE);
                        air[u].on_bit(t);
                    }
                }
                if (i != (BOARDSIZE - 1)) {
                    if (bitb[0].get(t + BOARDSIZE) || bitb[1].get(t + BOARDSIZE)) {
                        d = get_root(t + BOARDSIZE);
                        if (d != u)air[d].on_bit(t);
                    }
                }
                if (j != 0) {
                    if (bitb[0].get(t - 1) || bitb[1].get(t - 1)) {
                        r = get_root(t - 1);
                        if (r != u && r != d)air[r].on_bit(t);
                    }
                }
                if (j != (BOARDSIZE - 1)) {
                    if (bitb[0].get(t + 1) || bitb[1].get(t + 1)) {
                        l = get_root(t + 1);
                        if (l != u && l != d && l != r)air[l].on_bit(t);
                    }
                }
            }
        }
    }
    for (i = 0; i < NUM_CELL; i++) {
        if (bitb[0].get(i) || bitb[1].get(i)) {
            if (parent[i] == i)countair[i] = air[i].count();
        }
    }
}

Board::Board() {}

inline bool Board::get(int i, bool j) {
    return bitb[j].get(i);
}

bool Board::just_play_color() {
    int bc = bitb[0].count();
    int wc = bitb[1].count();
    if (bc == wc)return 1;
    return 0;
}

void Board::getv(int bone[NUM_CELL], int wone[NUM_CELL], int two[NUM_CELL], int &bsize, int &wsize, int &tsize) {
    bool bc, wc;
    bsize = wsize = tsize = 0;
    for (int i = 0; i < NUM_CELL; i++) {
        if (!bitb[0].get(i) && !bitb[1].get(i)) {
            bc = check(i, 0);
            wc = check(i, 1);
            if (bc) {
                if (wc) {
                    two[tsize] = i;
                    tsize++;
                } else {
                    bone[bsize] = i;
                    bsize++;
                }
            } else if (wc) {
                wone[wsize] = i;
                wsize++;
            }
        }
    }
}

void Board::clear() {
    int i;
    for (i = 0; i < NUM_CELL; i++) {
        parent[i] = i;
    }

    bitb[0].clear();
    bitb[1].clear();
    ban[0].clear();
    ban[1].clear();
    notsafe[0].clear();
    notsafe[1].clear();
    memset(air, 0, sizeof(air));
    memset(countair, 0, sizeof(countair));
}


string Board::inttostring(int i) {
    string s = "A1";
    s[0] += i / BOARDSIZE;
    if (s[0] >= 'H')s[0]++;
    s[1] += i % BOARDSIZE;
    return s;
}

double
Board::simulate(bool j, int bone[NUM_CELL], int wone[NUM_CELL], int two[NUM_CELL], int bsize, int wsize, int tsize) {
    int i, k;
    bool bc, wc;
    FLAG:
    // showboard();
    // system("pause");
    while (tsize > 0) {
        i = rand() % tsize;
        k = two[i];
        two[i] = two[tsize - 1];
        tsize--;
        bc = check(k, 0);
        wc = check(k, 1);
        if (check(k, j)) {
            if (j == 0) {
                bpath[bpsize] = k;
                bpsize++;
            } else {
                wpath[wpsize] = k;
                wpsize++;
            }
            add(k, j);
            j = !j;
            goto FLAG;
        } else {
            if (!bc && wc) {
                wone[wsize] = k;
                wsize++;
            } else if (bc && !wc) {
                bone[bsize] = k;
                bsize++;
            }
        }
    }
    FLAG2 :
    if (j == 0) {

        while (bsize > 0) {
            i = rand() % bsize;
            k = bone[i];
            bone[i] = bone[bsize - 1];
            bsize--;
            if (check(k, j)) {
                add(k, j);
                // bpath[bpsize] = k;
                // bpsize++;
                j = !j;
#if dolog == 1
                cout << inttostring(k);
#endif
                goto FLAG2;
            }
        }
    } else {
        while (wsize > 0) {
            i = rand() % wsize;
            k = wone[i];
            wone[i] = wone[wsize - 1];
            wsize--;
            if (check(k, j)) {
                add(k, j);
                j = !j;
                //  wpath[wpsize] = k;
                //  wpsize++;
#if dolog == 1
                cout<<inttostring(k);
#endif
                goto FLAG2;
            }
        }
    }
    return (j == 1) ? 1 : -1;
}