//
// Created by Thong Nguyen on 11/19/19.
//

#ifndef PROJECT03_BOARD_H
#define PROJECT03_BOARD_H


class Board {
public:
    typedef int Cell;
    typedef int Color;

public:
    static int BOARD_SIZE;
    static int BLACK;
    static int WHITE;

public:
    Board();

    int place(unsigned x, unsigned y, Color color);

private:
    Cell board[9][9]{};

};


#endif //PROJECT03_BOARD_H
