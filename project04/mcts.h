//
// Created by thethongngu on 12/3/19.
//

#ifndef PROJECT04_MCTS_H
#define PROJECT04_MCTS_H


#include "node.h"

class MCTS {
public:
    MCTS(Board _board, int _depth);

    void set_root(Board curr_board);

    Node *select();

    Node *expand(Node *node);

    int simulate(Node *node);

    void backpropagate(Node *node, int end_value);

    void run();

    static Node *find_child_with_move(Node *node, Board curr_board, Board::Cell pos, Board::Color color);

    static void free_all_children(Node *node);

private:
    Node *root;
    int tree_depth;
};


#endif //PROJECT04_MCTS_H
