//
// Created by thethongngu on 12/14/19.
//

#ifndef PROJECT04_BIT_BOARD_H
#define PROJECT04_BIT_BOARD_H

#endif //PROJECT04_BIT_BOARD_H

#include "global.h"

const unsigned int NUM_VAR = NUM_CELL / 64 + 1;

class BitBoard {
public:

    ull x[NUM_VAR];

    BitBoard() {
        for(int i = 0; i < NUM_VAR; i++) x[i] = 0LL;
    }

    void clear() {
        for(int i = 0; i < NUM_VAR; i++) x[i] = 0LL;
    }

    int count() {
        int res = 0;
        for(int i = 0; i < NUM_VAR; i++) {
            ull v;
            v = (x[i] & 0x5555555555555555LL) + ((x[i] >> 1) & 0x5555555555555555LL);
            v = (v & 0x3333333333333333LL) + ((v >> 2) & 0x3333333333333333LL);
            v = (v & 0x0f0f0f0f0f0f0f0fLL) + ((v >> 4) & 0x0f0f0f0f0f0f0f0fLL);
            v = (v & 0x00ff00ff00ff00ffLL) + ((v >> 8) & 0x00ff00ff00ff00ffLL);
            v = (v & 0x0000ffff0000ffffLL) + ((v >> 16) & 0x0000ffff0000ffffLL);

            res += (int) ((v & 0x00000000fffffffffLL) + ((v >> 32) & 0x00000000ffffffffLL));
        }
        return res;
    }

    bool get(int i) {
        return (x[i >> 6] & (1LL << (i & 63))) != 0;
    }

    void off(int i) {
        x[i >> 6] &= ~(1LL << (i & 63));
    }

    void on(int i) {
        x[i >> 6] |= (1LL << (i & 63));
    }

    void operator|=(BitBoard b) {
        for(int i = 0; i < NUM_VAR; i++) x[i] |= b.x[i];
        return;
    }

    bool is_empty() {
        for(int i = 0; i < NUM_VAR; i++)
            if (x[i] != 0) return false;
        return true;
    }
};