//
// Created by thethongngu on 2020-01-11.
//

#ifndef PROJECT05_NODE_H
#define PROJECT05_NODE_H

#endif //PROJECT05_NODE_H

#include "board.h"

class Node {

public:

    int last_pos;
    int last_color;
    int child_pos[NUM_CELL];
    int num_child;
    Node *child_ptr;

    double count, mean, rave_count, rave_mean;
    double log_count;

    void init_node(int pos, int color);
    void add_normal_result(double outcome);
    void add_rave_result(double outcome);
    int get_max_move();

    void expansion(Board &board);

    Node();
    ~Node();
};