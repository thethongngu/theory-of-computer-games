//
// Created by thethongngu on 12/15/19.
//

#ifndef PROJECT04_MCTS_H
#define PROJECT04_MCTS_H

#endif //PROJECT04_MCTS_H

#include <vector>
#include "global.h"
#include "node.h"
#include "board.h"

class MCTS {
public:
    Node* root;

    std::vector<int> pos_data;
    std::vector<int> color_data;

    void add_history(int pos, int color) {
        pos_data.push_back(pos);
        color_data.push_back(color);
    }

    Node* selection(Board &board) {
        Node* node = root;
        int color = Board::change_color(node->last_color);

        while (!node->children.empty()) {
            node = get_best_child(node);
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
            Node* child = new Node(pos, color);
            node->children.push_back(child);
        }

        node =  get_best_child(node);
        add_history(node->last_pos, node->last_color);
        return node;
    }



    void run_once() {

        Board board;
        Node* leaf;
        double outcome = 0.0;

        leaf = selection(board);
        leaf = expansion(leaf, board);
        outcome = simulation(leaf, board);
        backprop(leaf, outcome);
    }
};