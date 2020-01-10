//
// Created by thethongngu on 2020-01-10.
//

#include <cstdlib>
#include <ctime>
#include <cmath>
#include <cstring>
#include "global.h"
#include "board.h"

class Node {

public:

    int last_pos;
    int last_color;
    int child_pos[NUM_CELL];
    int num_child;
    Node *child_ptr;

    double count, mean, rave_count, rave_mean;
    double log_count;

    void init_node(int pos, int color);
    void add_normal_result(double outcome);
    void add_rave_result(double outcome);
    int get_max_move();

    void expansion(Board &board);

    Node();
    ~Node();
};

void Node::init_node(int pos, int color) {
    last_pos = pos;
    last_color = color;
    mean = 0.5;  count = 0;
    rave_mean = 0.5;  rave_count = 20;
    memset(child_pos, -1, sizeof(child_pos));
    child_ptr = nullptr;
    log_count = 1;
}

void Node::add_normal_result(double outcome) {
    double value = 0.0;
    if (outcome > 0 && last_color == BLACK) value = 1.0;
    if (outcome < 0 && last_color == WHITE) value = 1.0;
    mean = (mean * count + value) / (count + 1);
    count += 1;  log_count = log(count);
}

void Node::add_rave_result(double outcome) {
    double value = 0.0;
    if (outcome > 0 && last_color == BLACK) value = 1.0;
    if (outcome < 0 && last_color == WHITE) value = 1.0;
    rave_mean = (rave_mean * rave_count + value) / (rave_count + 1);
    rave_count += 1;
}

int Node::get_max_move() {

    int max_count = -1, curr_count;
    int child_id;

    for (int i = 0; i < num_child; i++) {
        curr_count = child_ptr[i].count;
        if (max_count < curr_count) {
            max_count = curr_count;
            child_id = i;
        }
    }

    return child_id;
}

void Node::expansion(Board &board) {

    int pos;
    int color = Board::change_color(last_color);
    num_child = 0;

    for (pos = 0; pos < NUM_CELL; pos++) {
        if (board.can_move(pos, color)) num_child++;
    }

    if (num_child == 0) return;

    child_ptr = new Node[num_child];
    int child_id = 0;
    for(pos=0; pos < NUM_CELL; pos++) {
        if(board.can_move(pos, color)) {
            child_pos[pos] = child_id;
            child_ptr[child_id].init_node(pos, color);
            child_id++;
        }
    }
}


Node::Node() {}
Node::~Node() {
    if (child_ptr != nullptr) {
        delete [] child_ptr;
        child_ptr= nullptr;
    }
}

