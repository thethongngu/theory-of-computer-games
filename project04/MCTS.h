//
// Created by thethongngu on 12/15/19.
//

#ifndef PROJECT04_MCTS_H
#define PROJECT04_MCTS_H

#endif //PROJECT04_MCTS_H

#include <vector>
#include "global.h"
#include "node.h"

class MCTS {
public:

    Node* root;
    Board root_board;

    std::vector<int> path[2];

    void add_history(int pos, int color) {
        path[color].push_back(pos);
    }

    void clear_tree() {
        if (root != nullptr) delete root;
        root = nullptr;
        root_board.clear_all();
    }

    void init_tree(const Board &board, int color) {
        root = new Node(nullptr, -1, Board::change_color(color));
        root_board = board;
    }

    Node* selection(Board &board) {
        Node* node = root;
        int color = Board::change_color(node->last_color);
        path[BLACK].clear();
        path[WHITE].clear();

        while (!node->children.empty()) {
            node = node->get_best_child();
            board.add_piece(node->last_pos, node->last_color);
            add_history(node->last_pos, node->last_color);
        }

        return node;
    }

    Node* expansion(Node* node, Board &board) {

        int color = Board::change_color(node->last_color);
        std::vector<int> valids;

        for(int i = 0; i < NUM_CELL; i++) {
            if (board.can_move(i, color)) valids.push_back(i);
        }
        if (valids.empty()) return node;

        for(int pos: valids) {
            Node* child = new Node(node, pos, color);
            node->children.push_back(child);
        }

        node = node->get_best_child();
        add_history(node->last_pos, node->last_color);
        return node;
    }

    double simulation(Node* node, Board &board) {

        board.recheck_moves();
        int color;

        while (true) {
            color = Board::change_color(node->last_color);
            int pos = board.get_2_go();
            if (pos == -1) break;
            board.add_piece(pos, color);
            add_history(pos, color);
        }

        while (true) {
            color = Board::change_color(node->last_color);
            int pos = board.get_1_go(color);
            if (pos == -1) break;
            board.add_piece(pos, color);
            add_history(pos, color);
        }

        return (color == WHITE) ? 1 : -1;  // let's not racist
    }

    void backprop(Node* node, double outcome) {
        int color;

        while (true) {

            node->add_normal_result(outcome);
            color = Board::change_color(node->last_color);
            for(int pos: path[color]) {
                node->children[pos]->add_rave_result(outcome);
            }

            if (node->parent == nullptr) break;
            node = node->parent;
        }
    }

    void run_once() {
        Node* leaf;
        double outcome;
        Board board = root_board;

        leaf = selection(board);
        leaf = expansion(leaf, board);
        outcome = simulation(leaf, board);
        backprop(leaf, outcome);
    }
};