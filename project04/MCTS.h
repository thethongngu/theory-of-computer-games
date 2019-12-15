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
    int total_node;

    std::vector<int> path[2];

    void add_history(int pos, int color) {
        path[color].push_back(pos);
    }

    void clear_tree() {
        if (root != nullptr) delete root;
        root = nullptr;
        root_board.clear_all();
        total_node = 0;
    }

    void init_tree(const Board &board, int color) {
        root = new Node(nullptr, -1, Board::change_color(color));
        root_board = board;
        total_node = 1;
    }

    Node* get_child_move() {
        if (root->children.size() == 0) return nullptr;
        Node* best_child = root->children[0];

        for(Node* child: root->children) {
            if (child->count > best_child->count) {
                best_child = child;
            }
        }

        return best_child;
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
            total_node++;
            node->children.push_back(child);
            node->child_pos[pos] = node->children.size() - 1;
        }

        node = node->get_best_child();
        add_history(node->last_pos, node->last_color);
        return node;
    }

    double simulation(Node* node, Board &board) {

        board.recheck_moves();
        int color = Board::change_color(node->last_color);

        while (true) {
            int pos = board.get_2_go();

            if (pos == -1) break;
            if (!board.can_move(pos, color)) continue;

            board.add_piece(pos, color);
            add_history(pos, color);
            color = Board::change_color(color);
        }

        while (true) {
            int pos = board.get_1_go(color);

            if (pos == -1) break;
            if (!board.can_move(pos, color)) continue;

            board.add_piece(pos, color);
            add_history(pos, color);
            color = Board::change_color(color);
        }

        return (color == WHITE) ? 1 : -1;  // let's not racist
    }

    void backprop(Node* node, double outcome) {

        int color;
        int rave_color;

        while (true) {

            node->add_normal_result(outcome);
            rave_color = Board::change_color(node->last_color);

            for(int pos: path[rave_color]) {
                int pos_child = node->child_pos[pos];
                if (pos_child == -1) continue;
                node->children[pos_child]->add_rave_result(outcome);
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