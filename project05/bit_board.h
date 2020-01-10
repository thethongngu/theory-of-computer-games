//
// Created by thethongngu on 12/14/19.
//

#ifndef PROJECT04_BIT_BOARD_H
#define PROJECT04_BIT_BOARD_H

#endif //PROJECT04_BIT_BOARD_H

#include "global.h"

class BitBoard {
public:

    ull x[2]{};

    BitBoard() {
        x[0] = 0LL;
        x[1] = 0LL;
    }

    void clear() {
        x[0] = 0LL;
        x[1] = 0LL;
    }

    int count() {
        unsigned long long v, v1;
        v = (x[0] & 0x5555555555555555LL) + ((x[0] >> 1) & 0x5555555555555555LL);
        v1 = (x[1] & 0x5555555555555555LL) + ((x[1] >> 1) & 0x5555555555555555LL);
        v = (v & 0x3333333333333333LL) + ((v >> 2) & 0x3333333333333333LL);
        v1 = (v1 & 0x3333333333333333LL) + ((v1 >> 2) & 0x3333333333333333LL);

        v += v1;
        v = (v & 0x0f0f0f0f0f0f0f0fLL) + ( (v >> 4) & 0x0f0f0f0f0f0f0f0fLL);
        v = (v & 0x00ff00ff00ff00ffLL) + ( (v >> 8) & 0x00ff00ff00ff00ffLL);
        v = (v & 0x0000ffff0000ffffLL) + ( (v >> 16) & 0x0000ffff0000ffffLL);
        return (int)((v & 0x00000000fffffffffLL) + ((v >> 32) & 0x00000000ffffffffLL));
    }

    bool get(int i) {
        return (x[i >> 6] & (1LL << (i & 63))) != 0;
    }

    void off_bit(int i) {
        x[i >> 6] &= ~(1LL << (i & 63));
    }

    void on_bit(int i) {
        x[i >> 6] |= (1LL << (i & 63));
    }

    void operator|=(BitBoard b) {
        x[0] |= b.x[0];
        x[1] |= b.x[1];
    }
};