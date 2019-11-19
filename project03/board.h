//
// Created by Thong Nguyen on 11/19/19.
//

#ifndef PROJECT03_BOARD_H
#define PROJECT03_BOARD_H


#include <vector>

class Board {
public:
    typedef int Cell;
    typedef int Color;

public:
    static int BOARD_SIZE;
    static int NUM_CELL;
    static int BLACK;
    static int WHITE;

public:
    Board();

    /** Return a vector of Cell that is liberty when placing stone with color at pos **/
    std::vector<Cell> get_liberties(unsigned pos, Board::Color color);

    bool is_capture(unsigned pos, Board::Color color);
    bool is_suicide(unsigned pos, Board::Color color);

    /** Return -1 if move is not valid, **/
    int place(unsigned pos, Color color);

private:
    Cell board[81]{};

};


#endif //PROJECT03_BOARD_H
