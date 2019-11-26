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
    static int NONE;

public:
    static Color get_opponent_color(Color color);
    static std::vector<Cell> get_adj_cells(Cell pos);

    Board();

    void print();
    void clear_board();
    int get_boardsize();
    void set_boardsize(int new_size);

    std::vector<Cell> get_liberties(Cell pos, Board::Color color, Cell checking_pos);
    bool is_capturing(Cell pos, Color color);
    bool is_suiciding(Cell pos, Color color);
    int place(Cell pos, Color color);
    int place(int x, int y, Color color);

private:
    Cell board[81]{};

};


#endif //PROJECT03_BOARD_H
