//
// Created by thethongngu on 12/15/19.
//

#ifndef PROJECT04_NODE_H
#define PROJECT04_NODE_H

#endif //PROJECT04_NODE_H

#include <vector>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include "global.h"

class Node {
public:

    int last_pos;
    int last_color;

    double count, mean;
    double rave_count, rave_mean;

    Node *parent;

    std::vector<Node *> children;
    int child_pos[NUM_CELL];

    Node(Node *_parent, int pos, int color) {
        last_color = color;
        last_pos = pos;
        parent = _parent;
        children.clear();
        for (int i = 0; i < NUM_CELL; i++) child_pos[i] = -1;

        mean = 0.5;
        count = 0.0;
        rave_mean = 0.5;
        rave_count = 20;
    }

    void print_info() {
        std::clog << " ========= Node ======== " << std::endl;
        debug(children.size());
        debug(last_pos);
        debug(mean);
        debug(count);
        debug(rave_mean);
        debug(rave_count);
    }

    void print_tree(int d) {

        print_info();
        if (d == 1) return;
        std::clog << " ========================= Child =================== " << std::endl;
        for(Node* child: children) {
            child->print_tree(d - 1);
        }
        std::clog << " ====================== End Child =================== " << std::endl;
    }

    double get_score() {
        int p_count = (parent->count == 0) ? 1 : parent->count;
        double res = (rave_mean * rave_count + mean * count + C_BIAS * sqrt(log(p_count) * count)) / (count + rave_count);
        return res;
    }

    Node *get_best_child() {
        if (children.empty()) return nullptr;

        std::vector<Node *> chosen;
        double curr_score = children[0]->get_score();
        chosen.push_back(children[0]);

        for (Node *child: children) {
            double score = child->get_score();
            if (score - curr_score > -0.0001) {  // score >= curr_score
                if (score - curr_score > 0.0001) {  // score > curr_score
                    chosen.clear();
                    chosen.push_back(child);
                } else {
                    chosen.push_back(child);
                }
            }
        }

        srand(time(NULL));
        return chosen[std::rand() % chosen.size()];
    }

    void add_normal_result(double outcome) {
        double value = 0.0;
        if (outcome > 0 && last_color == BLACK) value = 1.0;
        if (outcome < 0 && last_color == WHITE) value = 1.0;
        mean = (mean * count + value) / (count + 1);
        count += 1;
    }

    void add_rave_result(double outcome) {
        double value = 0.0;
        if (outcome > 0 && last_color == BLACK) value = 1.0;
        if (outcome < 0 && last_color == WHITE) value = 1.0;
        rave_mean = (rave_mean + rave_count + value) / (rave_count + 1);
        rave_count += 1;
    }

    ~Node() {
        for (Node *child: children) delete child;
        children.clear();
    }
};