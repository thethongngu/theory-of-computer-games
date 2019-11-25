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

private:
    static int BOARD_SIZE;
    static int NUM_CELL;

public:
    static int BLACK;
    static int WHITE;

public:
    Board();

    int get_boardsize();

    void set_boardsize(int new_size);

    void clear_board();

    /** Return a vector of Cell that is liberty when placing stone with color at pos **/
    std::vector<Cell> get_liberties(unsigned pos, Board::Color color);

    static Color get_opponent_color(Color color);

    /** Is capturing opponent if place stone with 'color' at 'pos' **/
    bool is_capturing(unsigned pos, Color color);

    /** Is suiciding if place stone with 'color' at 'pos' **/
    bool is_suiciding(unsigned pos, Color color);

    /** Return -1 if move is not valid, **/
    int place(unsigned pos, Color color);

    int place(unsigned x, unsigned y, Color color);

private:
    Cell board[81]{};

};


#endif //PROJECT03_BOARD_H
