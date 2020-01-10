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


void Node::init_node(int pos, int color) {
    last_color = color;
    last_pos = pos;
    mean = 0.5;
    count = 0;
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

void Node::expansion(Board &b) {

    int pos, color;
    color = !b.just_play_color();
    num_child = 0;
    for (pos = 0; pos < NUM_CELL; pos++) {
        if (b.can_move(pos, color)) num_child++;
    }
    if (num_child == 0) return;

    child_ptr = new Node[num_child];

    int child_id = 0;
    for (pos = 0; pos < NUM_CELL; pos++) {
        if (b.can_move(pos, color)) {
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

vector<float> Node::getPolicy() {
    vector<float> ret(NUM_CELL, 0);
    float sum = 0;
    for (int i = 0; i < num_child; i++) {
        sum += child_ptr[i].count;
    }
    for (int i = 0; i < num_child; i++) {
        ret[child_ptr[i].last_pos] = child_ptr[i].count / sum;
    }
    return ret;
}

void Node::show_child() {
    for (int i = 0; i < num_child; i++) {
        if (child_ptr[i].count < 10) continue;
        cerr << inttoGTPstring((int) child_ptr[i].last_pos) << ' ' << child_ptr[i].mean << ' ' << child_ptr[i].count
             << ' ';
        //cerr<<sqrt( logc / ((childptr[i].count) - minusnum))* UCB_WEIGHT<<' ' ;
        cerr << (child_ptr[i].rave_mean) << ' ' << (child_ptr[i].rave_count) << endl;
    }
}

float Node::show_inf(int best) {
    cerr << "total : " << count - basenum << endl << "winrate :" << (child_ptr[best].mean);
    //cerr<<"\nlog : "<<sqrt( logc / ((childptr[best].count) - minusnum))* UCB_WEIGHT ;
    cerr << "\nrave : " << (child_ptr[best].rave_mean) << ' ' << (child_ptr[best].rave_count) << endl;
    return child_ptr[best].mean;
}

string Node::inttoGTPstring(int i) {
    string s = "A1";
    s[0] += i / 9;
    if (s[0] >= 'I')s[0]++;
    s[1] += i % 9;
    return s;
}

string Node::inttostring(int i) {
    return inttoGTPstring(i);
}