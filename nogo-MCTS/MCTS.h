//
// Created by thethongngu on 12/15/19.
//

#ifndef PROJECT04_MCTS_H
#define PROJECT04_MCTS_H

#endif //PROJECT04_MCTS_H

#include "global.h"
#include "node.h"

class MCTS {
public:

    Node* root;
    Board root_board;
    int total_node;

    int path[2][NUM_CELL];
    int num_path[2];

    void add_history(int pos, int color) {
        path[color][num_path[color]++] = pos;
    }

    void clear_tree() {
        if (root != nullptr) delete root;
        root = nullptr;
        root_board.clear_all();
        total_node = 0;
    }

    void init_tree(const Board &board, int color) {
        root = new Node();
        root->init_node(nullptr, -1, Board::change_color(color));
        root_board = board;
        total_node = 1;
    }

    Node* get_child_move() {
        if (root->num_child == 0) return nullptr;
        Node* best_child = (root->children + 0);

        for(int i = 0; i < root->num_child; i++) {
            Node *child = (root->children + i);
            if (child->count > best_child->count) {
                best_child = child;
            }
        }

        return best_child;
    }

    Node* selection(Board &board) {
        Node* node = root;
        num_path[BLACK] = num_path[WHITE] = 0;

        while (node->num_child > 0) {
            node = node->get_best_child();
            board.add_piece(node->last_pos, node->last_color);
            add_history(node->last_pos, node->last_color);
        }

        return node;
    }

    Node* expansion(Node* node, Board &board) {

        if (node->count == 0) return node;  // simulation first time

        int color = Board::change_color(node->last_color);
        int num_valid = 0;

        for(int i = 0; i < NUM_CELL; i++) {
            if (board.can_move(i, color)) num_valid++;
        }
        if (num_valid == 0) return node;

        node->children = new Node[num_valid];
        int child_id = 0;

        for(int pos = 0; pos < NUM_CELL; pos++) {
            if (board.can_move(pos, color)) {
                node->child_pos[pos] = child_id;
                (node->children + child_id)->init_node(node, pos, color);
                total_node++;
                node->num_child++;
                child_id++;
            }
        }

        node = node->get_best_child();
        board.add_piece(node->last_pos, node->last_color);
        add_history(node->last_pos, node->last_color);
        return node;
    }

    double simulation(Node* node, Board &board) {

        board.recheck_moves();
        int color = Board::change_color(node->last_color);

        while (true) {
            int pos = board.get_2_go();

            if (pos == -1) break;

            bool black_can = board.can_move(pos, BLACK);
            bool white_can = board.can_move(pos, WHITE);

            if (board.can_move(pos, color)) {
                board.add_piece(pos, color);
                add_history(pos, color);
                color = Board::change_color(color);
            }
            else {
                if (white_can && !black_can) board.add_1_go(pos, WHITE);
                else if (!white_can && black_can) board.add_1_go(pos, BLACK);
            }
        }

        while (true) {
            int pos = board.get_1_go(color);

            if (pos == -1) break;
            if (!board.can_move(pos, color)) continue;

            board.add_piece(pos, color);
            color = Board::change_color(color);
        }

        return (color == WHITE) ? 1 : -1;  // let's not racist
    }

    void backprop(Node* node, double outcome) {

        int rave_color;
        while (true) {

            node->add_normal_result(outcome);
            rave_color = Board::change_color(node->last_color);

            for(int i = 0; i < num_path[rave_color]; i++) {
                int pos = path[rave_color][i];
                int child_id = node->child_pos[pos];
                if (child_id == -1) continue;
                (node->children + child_id)->add_rave_result(outcome);
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