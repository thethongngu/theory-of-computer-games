//
// Created by thethongngu on 12/15/19.
//

#ifndef PROJECT04_MCTS_H
#define PROJECT04_MCTS_H

#endif //PROJECT04_MCTS_H

#include <iostream>
#include <cmath>
#include <cstring>

#include "global.h"
#include "MCTS.h"

using namespace std;

double MCTS::get_score(Node *nodeptr, int child_id) {

    Node *child = (nodeptr->child_ptr) + child_id;
    auto &pos = child->last_pos;
    auto &color = child->last_color;
    double &count = child->count;
    double &rcount = child->rave_count;

    return (child->rave_mean * rcount + child->mean * count + sqrt(nodeptr->log_count * count) * C_BIAS) /
           (count + rcount);
}

Node *MCTS::get_UTC_RAVE(Node *node_ptr) {

    if (node_ptr->num_child == 0)return nullptr;

    int i, best_id = 0;
    double max_score, curr_score = get_score(node_ptr, 0);
    double diff;

    max_score = curr_score;
    num = 0; chosen[num++] = 0;
    for (i = 1; i < (node_ptr->num_child); i++) {
        curr_score = get_score(node_ptr, i);
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
    return (node_ptr->child_ptr + best_id);
}

void MCTS::select(Board &board) {

    bool j = !board.just_play_color();//next to play

    Node *node = root;
    Board::num_black = 0;  Board::num_white = 0;

    path.clear();
    path.push_back(node);

    while (node->child_ptr != nullptr && node->num_child != 0) {
        node = get_UTC_RAVE(node);
        path.push_back(node);

        if (node->last_color == BLACK) {
            board.add_black_to_path(node->last_pos);
            sbnum++;
        } else {
            board.add_white_to_path(node->last_pos);
            swnum++;
        }
        board.add_piece(node->last_pos, node->last_color);
    }
}

void MCTS::update(double result, Board &b) {

    for (int i = 0; i < path.size(); i++) {
        path[i]->add_normal_result(result);
        if (path[i]->last_color == 0) {
            for (int j = 0; j < Board::num_white; j++) {
                int k = (path[i]->child_pos[Board::white_path[j]]);
                if (k != -1) ((path[i]->child_ptr) + k)->add_rave_result(result);
            }
        } else {
            for (int j = 0; j < Board::num_black; j++) {
                int k = (path[i]->child_pos[Board::black_path[j]]);
                if (k != -1) ((path[i]->child_ptr) + k)->add_rave_result(result);
            }
        }
    }
}

void MCTS::run_a_cycle() {

    Board board;
    double result;
    board = root_board;
    sbnum = swnum = 0;
    select(board);
    Node &last = (*(path.back()));
    Node *node;

    if (last.num_child == 0 && last.count > PARENT_SIMS) {
        last.expansion(board);
        if (last.num_child != 0) {
            totalnode += last.num_child;
            node = get_UTC_RAVE(&last);
            path.push_back(node);

            if (node->last_color == 0) {
                board.add_black_to_path(node->last_pos);
                sbnum++;
            } else {
                board.add_white_to_path(node->last_pos);
                swnum++;
            }
            board.add_piece(node->last_pos, node->last_color);

        }
    }

    total += sbnum;
    total += swnum;
    board.recheck_move(bone, wone, two, bsize, wsize, tsize);

    if ((board.just_play_color() == BLACK) && (wsize + tsize) == 0) {
        result = 1;
    } else if (board.just_play_color() == WHITE && (bsize + tsize) == 0) {
        result = -1;
    } else {
        result = board.simulate(!board.just_play_color(), bone, wone, two, bsize, wsize, tsize);
    }
    update(result, board);
}

void MCTS::reset(Board &board) {

    root_board = board;
    root = new Node;
    root->last_color = root_board.just_play_color();
    root->last_pos = 81;
    root->count = 0;
    root->log_count = 1;

    memset(root->child_pos, -1, sizeof(root->child_pos));
    root->expansion(board);
    total = 0;
    totalnode = 0;
}

void MCTS::show_path() {
    Node *nodeptr = root;
    int i, k;
    i = 0;
    while (nodeptr->child_ptr != nullptr && i < 10) {
        k = nodeptr->get_max_move();
        nodeptr = nodeptr->child_ptr + k;
        i++;
        if (nodeptr != nullptr) {
            if (nodeptr->last_color == BLACK)
                cerr << "B(";
            else
                cerr << "W(";
            cerr << inttostring(nodeptr->last_pos) << ") ";
        }
    }
    cerr << endl;
}

void MCTS::clear() {
    delete root;
}

string MCTS::inttoGTPstring(int i) {
    string s = "A1";
    s[0] += i / 9;
    if (s[0] >= 'I')s[0]++;
    s[1] += i % 9;
    return s;
}

string MCTS::inttostring(int i) {
    return inttoGTPstring(i);
}

MCTS::MCTS() {

}

MCTS::~MCTS() {}

