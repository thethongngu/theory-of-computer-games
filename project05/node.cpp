#include <cmath>
#include <cstring>
#include <iostream>
#include "node.h"

using namespace std;

Node::Node() {}

Node::~Node() {
    if (child_ptr != nullptr) {
        delete[] child_ptr;
        child_ptr = nullptr;
    }
}


void Node::init_node(int pos, bool color) {
    last_color = color;
    last_pos = pos;
    mean = 0.5;
    count = PARENT_SIMS;
    rave_count = 20;
    rave_mean = 0.5;
    num_child = 0;
    memset(child_pos, -1, sizeof(child_pos));
    child_ptr = nullptr;
    log_count = 1;
}

void Node::add_normal_result(double result) {
    double value = 0.0;
    if (result > 0 && last_color == BLACK) value = 1.0;
    if (result < 0 && last_color == WHITE) value = 1.0;

    mean = (mean * count + value) / (count + 1);
    count += 1;
    log_count = log(count);
}

void Node::add_rave_result(double result) {
    double value = 0.0;
    if (result > 0 && last_color == BLACK) value = 1.0;
    if (result < 0 && last_color == WHITE) value = 1.0;

    rave_mean = (rave_mean * rave_count + value) / (rave_count + 1);
    rave_count += 1;
}

void Node::expansion(Board &board) {

    bool color = !board.last_color();
    num_child = 0;

    for (int pos = 0; pos < NUM_CELL; pos++) {
        if (board.can_move(pos, color)) num_child++;
    }
    if (num_child == 0) return;

    child_ptr = new Node[num_child];

    int child_id = 0;
    for (int pos = 0; pos < NUM_CELL; pos++) {
        if (board.can_move(pos, color)) {
            child_pos[pos] = child_id;
            child_ptr[child_id].init_node(pos, color);
            child_id++;
        }
    }
}

int Node::get_max_move() {

    int best_child = -1;
    double max_count = -1, curr_count;

    for (int i = 0; i < num_child; i++) {
        curr_count = child_ptr[i].count;
        if (curr_count > max_count) {
            max_count = curr_count;
            best_child = i;
        }
    }

    return best_child;
}