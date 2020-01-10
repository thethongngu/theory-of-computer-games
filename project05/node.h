//
// Created by thethongngu on 2020-01-11.
//

#ifndef PROJECT05_NODE_H
#define PROJECT05_NODE_H

#endif //PROJECT05_NODE_H

#include "global.h"
#include "board.h"

#define basenum 0

class Node {

public:
    int last_pos, last_color;
    int child_pos[NUM_CELL + 1];
    int num_child;
    Node *child_ptr;

    double count, mean;
    double rave_count, rave_mean;
    double log_count;

    void init_node(int pos, int color);

    void add_normal_result(double result);

    void add_rave_result(double result);

    int get_max_move();

    vector<float> getPolicy();

    void show_child();

    float show_inf(int child);

    void expansion(Board &b);

    string inttostring(int i);

    string inttoGTPstring(int i);

    Node();

    ~Node();
};