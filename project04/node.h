//
// Created by thethongngu on 12/3/19.
//

#ifndef PROJECT04_NODE_H
#define PROJECT04_NODE_H


#include "board.h"

class Node {
public:
    Board board;
    Board::Color color;
    Board::Cell pos;
    std::vector<Node*> children;
    Node* parent;

    int count;
    int rave_count;
    double mean;
    double rave_mean;
    int num_win;

public:
    Node(Board _board) {
        board = _board;
        pos = 0;
        color = 0;
        count = rave_count = num_win = 0;
        mean = rave_mean = 0.0;
    }

    Node(Board _board, Board::Cell _pos, Board::Color _color, Node* node) {
        board = _board;
        pos = _pos;
        color = _color;
        parent = node;

        count = rave_count = 0;
        mean = rave_mean = 0.0;
        num_win = 0;
    }

    bool is_leaf();
    bool is_fully_expanded();
    Node* get_best_uct_child();
    double get_score();
    void update_value(int value);
    Node* get_parent();

};


#endif //PROJECT04_NODE_H
