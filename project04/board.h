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

    void print();

    /** Return a vector of Cell that is liberty when placing stone with color at pos **/
    std::vector<Cell> get_liberties(unsigned pos, Board::Color color);

    static Color get_opponent_color(Color color);

    /** Is capturing opponent if place stone with 'color' at 'pos' **/
    bool is_capturing(int pos, Color color);

    /** Is suiciding if place stone with 'color' at 'pos' **/
    bool is_suiciding(int pos, Color color);

    /** Return -1 if move is not valid, **/
    int place(int pos, Color color);

    int place(int x, int y, Color color);

private:
    Cell board[81]{};

};


#endif //PROJECT03_BOARD_H
