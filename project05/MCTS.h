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
    int selectlist[NUM_CELL];
    int slsize;
    Node *root;
    Board rBoard;
    double rave_num[2][NUM_CELL];
    double rave_wnum[2][NUM_CELL];

    MCTS();

    ~MCTS();

    vector<Node *> path;
    int bsize, wsize, tsize;
    int bone[NUM_CELL], wone[NUM_CELL], two[NUM_CELL];
    int sbnum, swnum;//select num
    int total;
    int totalnode;

    double getscore(Node *nodeptr, int child);

    Node *getbestchild(Node *nodeptr);

    void select(Board &b);

    void update(double result, Board &b);

    void run_a_cycle();

    void reset(Board &b);

    void clear();

    void show_path();

    string inttostring(int i);

    string inttoGTPstring(int i);
};