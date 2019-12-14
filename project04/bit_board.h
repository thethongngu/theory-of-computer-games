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

    unsigned long long x[NUM_VAR];

    BitBoard() {
        for(int i = 0; i < NUM_VAR; i++) x[i] = 0LL;
    }

    void clear() {
        for(int i = 0; i < NUM_VAR; i++) x[i] = 0LL;
    }

    int count() {

    }
};