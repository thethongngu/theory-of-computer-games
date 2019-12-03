//
// Created by thethongngu on 12/3/19.
//

#include "agent.h"

Agent::Agent() {
}

std::string Agent::make_move(Board curr_board, Board::Color color) {
    Node *root = tree.root;
    Node *child = MCTS::find_child_with_board(root, curr_board);
    if (child == nullptr) {
        MCTS::free_all_children(root);
        tree.set_root(curr_board);
    } else {
        for(auto & other_child : root->children)
            if (other_child != child) MCTS::free_all_children(other_child);
        tree.root = child;
    }

    tree.run();
}
