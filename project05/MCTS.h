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

    vector<Node *> path;
    int bsize, wsize, tsize;
    int bone[NUM_CELL], wone[NUM_CELL], two[NUM_CELL];

    double get_score(Node *nodeptr, int child_id);

    Node *get_UTC_RAVE(Node *node_ptr);

    void select(Board &board);

    void update(double result, Board &b);

    void run_a_cycle();

    void reset(Board &board);

    void clear();
};