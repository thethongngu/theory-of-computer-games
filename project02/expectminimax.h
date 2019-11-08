//
// Created by thethongngu on 11/8/19.
//

#ifndef PROJECT02_EXPECTMINIMAX_H
#define PROJECT02_EXPECTMINIMAX_H


#include "board.h"
#include "agent.h"

class ExpectMinimax {

private:
    int depth;
    int num_player_action;
    int num_evil_action;

    struct Node {
        double value;
        Board board;
        int last_op;
        std::vector<Node &> children;

        Node() {
            value = 0;
            last_op = -1;
            board = Board();
        }
    };

    Node root;

    int place_pos[4][4] = {
            {12, 13, 14, 15},
            {0,  4,  8,  12},
            {0,  1,  2,  3},
            {3,  7,  11, 15}
    };

    ExpectMinimax(int _depth, int _num_player_action, int _num_evil_action);

    void generate_tree(Node &currNode, int d);

    double search(Node currNode, int d, TDPlayer player);

    Node &get_root();
};


#endif //PROJECT02_EXPECTMINIMAX_H
