//
// Created by thethongngu on 12/15/19.
//

#ifndef PROJECT04_MCTS_H
#define PROJECT04_MCTS_H

#endif //PROJECT04_MCTS_H

#include <cmath>
#include <cstring>

#include "global.h"
#include "MCTS.h"

using namespace std;

double MCTS::get_score(Node *node, int child_id) {

    Node *child = (node->child_ptr) + child_id;
    auto &pos = child->last_pos;
    auto &color = child->last_color;
    double &count = child->count;
    double &rcount = child->rave_count;

    return (child->rave_mean * rcount + child->mean * count + sqrt(node->log_count * count) * C_BIAS) /
           (count + rcount);
}

Node *MCTS::get_UTC_RAVE(Node *node) {

    if (node->num_child == 0)return nullptr;

    int i, best_id = 0;
    double max_score, curr_score = get_score(node, 0);
    double diff;

    max_score = curr_score;
    num = 0; chosen[num++] = 0;
    for (i = 1; i < (node->num_child); i++) {
        curr_score = get_score(node, i);
        diff = curr_score - max_score;
        if (diff > -0.0001) {
            if (diff > 0.0001) {
                chosen[0] = i;
                num = 1;
                max_score = curr_score;
            } else chosen[num++] = i;
        }
    }

    best_id = chosen[rand() % num];
    return (node->child_ptr + best_id);
}

void MCTS::selection(Board &board) {

    Node *node = root;
    Board::num_black = 0;  Board::num_white = 0;

    tree_path.clear();
    tree_path.push_back(node);

    while (node->child_ptr != nullptr && node->num_child != 0) {
        node = get_UTC_RAVE(node);
        tree_path.push_back(node);
        board.add_game_path(node->last_pos, node->last_color);
        board.add_piece(node->last_pos, node->last_color);
    }
}

void MCTS::backpropagation(double result, Board &board) {

    for (int i = 0; i < tree_path.size(); i++) {
        tree_path[i]->add_normal_result(result);
        if (tree_path[i]->last_color == 0) {
            for (int j = 0; j < Board::num_white; j++) {
                int k = (tree_path[i]->child_pos[Board::white_path[j]]);
                if (k != -1) ((tree_path[i]->child_ptr) + k)->add_rave_result(result);
            }
        } else {
            for (int j = 0; j < Board::num_black; j++) {
                int k = (tree_path[i]->child_pos[Board::black_path[j]]);
                if (k != -1) ((tree_path[i]->child_ptr) + k)->add_rave_result(result);
            }
        }
    }
}

void MCTS::run_once() {

    double outcome;
    Board board = root_board;

    selection(board);
    Node &leaf = (*(tree_path.back()));
    Node *node;

    if (leaf.num_child == 0 && leaf.count > PARENT_SIMS) {
        leaf.expansion(board);
        if (leaf.num_child != 0) {
            node = get_UTC_RAVE(&leaf);
            tree_path.push_back(node);
            board.add_game_path(node->last_pos, node->last_color);
            board.add_piece(node->last_pos, node->last_color);
        }
    }

    board.recheck_move(black_one, white_one, two, num_black, num_white, num_two);

    if ((board.last_color() == BLACK) && (num_white + num_two) == 0) outcome = 1;
    else if (board.last_color() == WHITE && (num_black + num_two) == 0) outcome = -1;
    else outcome = board.simulate(!board.last_color(), black_one, white_one, two, num_black, num_white, num_two);
    backpropagation(outcome, board);
}

void MCTS::reset_tree(Board &board) {

    root_board = board;
    root = new Node;
    root->last_color = root_board.last_color();
    root->last_pos = 81;
    root->count = PARENT_SIMS;
    root->log_count = 1;

    memset(root->child_pos, -1, sizeof(root->child_pos));
    root->expansion(board);
}

void MCTS::clear() {
    delete root;
}

MCTS::MCTS() {

}

MCTS::~MCTS() {}

