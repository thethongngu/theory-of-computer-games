//
// Created by thethongngu on 12/3/19.
//

#include "agent.h"

Agent::Agent() = default;

std::string Agent::make_move(Board curr_board, Board::Color color) {
    init_tree(curr_board);
    tree.run();
}

void Agent::init_tree(const Board &curr_board) {

    Node *root = tree.root;
    Node *chosen_one = MCTS::find_child_with_board(root, curr_board);

    if (chosen_one == nullptr) {
        MCTS::free_all_children(root);
        tree.set_board_root(curr_board);
    } else {
        for(auto & other_child : root->children)
            if (other_child != chosen_one) MCTS::free_all_children(other_child);
        tree.root = chosen_one;
    }
}
