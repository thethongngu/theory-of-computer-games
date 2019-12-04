//
// Created by thethongngu on 12/3/19.
//

#include "agent.h"

Agent::Agent() = default;

/**
 * @return -1 if can not make move, 'pos' if can move
 */
Board::Cell Agent::make_move(Board curr_board, Board::Color color) {
    init_tree(curr_board, color);
    for(int i = 0; i < 100; i++) tree.run();
    auto best_child = tree.root->get_best_uct_child();
    return (best_child == nullptr) ? -1 : best_child->pos;
}

void Agent::init_tree(const Board &curr_board, Board::Color curr_color) {

    Node *root = tree.root;
    Node *chosen_one = MCTS::find_child_with_board(root, curr_board);

    if (chosen_one == nullptr) {
        MCTS::free_all_children(root);
        tree.set_root(curr_board, curr_color);
    } else {
        for(auto & other_child : root->children)
            if (other_child != chosen_one) MCTS::free_all_children(other_child);
        tree.root = chosen_one;
    }
}
