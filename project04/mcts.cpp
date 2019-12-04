//
// Created by thethongngu on 12/3/19.
//

#include "mcts.h"

MCTS::MCTS(Board _board, int _depth) {
    root = new Node(_board);
    root->parent = root;
    tree_depth = _depth;
}

/**
 * Find the leaf node by follow the selection policy in 'get_best_node'
 * @return the pointer to the leaf node
 */
Node *MCTS::select() {
    auto curr_node = root;
    while (curr_node->is_fully_expanded()) {
        Node *child = curr_node->get_best_uct_child();
        curr_node = child;
    }

    return curr_node;
}

/**
 * Expends all the child node of current leaf node.
 * New created nodes will be initialized with fixed values
 * @param node: a pointer to the leaf node
 */
Node *MCTS::expand(Node *node) {

    if (node->board.is_terminated()) return node;

    Board::Color curr_color = Board::get_opponent_color(node->color);
    Board::Cell random_empty_cell = node->board.get_random_empty_cell();
    Board curr_board = node->board;   // TODO: assignment is deep copy or not?

    Node *child = new Node(curr_board, random_empty_cell, curr_color, node);
    node->children.push_back(child);
    child->parent = node;

    return node->get_best_uct_child();
}

/**
 * Return the outcome of simulation from current node
 * @return lose: -1; win: 1
 */
int MCTS::simulate(Node *node) {

    auto curr_board = node->board;
    auto curr_color = node->color;

    while (!node->board.is_terminated()) {
        curr_color = Board::get_opponent_color(curr_color);
        Board::Cell cell = curr_board.get_random_empty_cell();
        node->board.place(cell, curr_color);
    }

    return curr_color == root->color ? 1 : -1;
}


/**
 * Update value back from the leaf to the root of tree
 */
void MCTS::backpropagate(Node *node, int value) {
    do {
        node->update_value(value);
        node = node->get_parent();
    } while (node != root);
}


void MCTS::run() {
    Node *leaf = select();
    leaf = expand(leaf);
    int end_value = simulate(leaf);
    backpropagate(leaf, end_value);
}

Node *MCTS::find_child_with_board(Node *node, Board curr_board) {
    for (auto &child : node->children) {
        if (child->board.equal(curr_board))
            return child;
    }

    return nullptr;
}

void MCTS::free_all_children(Node *node) {
    for (auto &child : node->children) {
        free_all_children(child);
    }
}

void MCTS::set_root(Board curr_board, Board::Color color) {
    root->board = curr_board;
    root->color = color;
}

MCTS::MCTS() {
    tree_depth = 1;
}
