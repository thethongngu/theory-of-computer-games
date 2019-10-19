//
// Created by thethongngu on 10/16/19.
//

#ifndef PROJECT02_BOARD_H
#define PROJECT02_BOARD_H

#pragma once
#include <array>
#include <iostream>
#include <iomanip>

/**
 * array-based board for 2048
 *
 * index (1-d form):
 *  (0)  (1)  (2)  (3)
 *  (4)  (5)  (6)  (7)
 *  (8)  (9) (10) (11)
 * (12) (13) (14) (15)
 *
 */


class Board {
public:
    typedef uint32_t Cell;
    typedef std::array<Cell, 4> Row;
    typedef std::array<Row, 4> Grid;
    typedef uint64_t Data;
    typedef int Reward;

public:
    Board() : tile(), attr(0) {}
    Board(const Grid& b, Data v = 0) : tile(b), attr(v) {}
    Board(const Board& b) = default;
    Board& operator =(const Board& b) = default;

    explicit operator Grid&() { return tile; }
    explicit operator const Grid&() const { return tile; }

    Row& operator [](unsigned i) { return tile[i]; }
    const Row& operator [](unsigned i) const { return tile[i]; }
    Cell& operator ()(unsigned i) { return tile[i / 4][i % 4]; }
    const Cell& operator ()(unsigned i) const { return tile[i / 4][i % 4]; }

    Data info() const { return attr; }
    Data info(Data dat) { Data old = attr; attr = dat; return old; }

    static const Cell kTileValue[15];
    static const Reward kTileScore[15];

public:
    bool operator ==(const Board& b) const { return tile == b.tile; }
    bool operator < (const Board& b) const { return tile < b.tile; }
    bool operator !=(const Board& b) const { return !(*this == b); }
    bool operator > (const Board& b) const { return b < *this; }
    bool operator <=(const Board& b) const { return !(b < *this); }
    bool operator >=(const Board& b) const { return !(*this < b); }

public:
    static bool can_merge(Cell cell01, Cell cell02);

    Reward place(unsigned pos, Cell tile_id);
    Reward slide(unsigned opcode);
    Reward slide_left();
    Reward slide_right();
    Reward slide_up();
    Reward slide_down();

    void transpose();
    void reflect_horizontal();
    void reflect_vertical();
    void rotate(int r = 1);
    void rotate_right();
    void rotate_left();
    void reverse();

    friend std::ostream& operator <<(std::ostream& out, const Board& b);

private:
    Grid tile;
    Data attr;
};


#endif //PROJECT02_BOARD_H
