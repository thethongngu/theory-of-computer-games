//
// Created by thethongngu on 2020-01-11.
//

#ifndef PROJECT05_MCTS_H
#define PROJECT05_MCTS_H

#endif //PROJECT05_MCTS_H

#include "global.h"
#include "node.h"

class MCTS {
public:
    int chosen[NUM_CELL];
    int num;
    Node *root;
    Board root_board;

    MCTS();

    ~MCTS();

    vector<Node *> tree_path;
    int num_black, num_white, num_two;
    int black_one[NUM_CELL], white_one[NUM_CELL], two[NUM_CELL];

    double get_score(Node *node, int child_id);

    Node *get_UTC_RAVE(Node *node);

    void select(Board &board);

    void update(double result, Board &board);

    void run_a_cycle();

    void reset(Board &board);

    void clear();
};