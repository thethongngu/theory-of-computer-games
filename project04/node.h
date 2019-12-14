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

    int win, count;
    int rave_win, rave_count;

    Node* parent;

    std::vector<Node*> children;

    double get_score() {
        double ucb = (double)win / count + C_BIAS * sqrt(log(parent->count) / count);
        double rave = (double)rave_win / rave_count;
        double beta = (double)rave_count / (count + rave_count + 4 * count * rave_count * SQR_B);
        return (1 - beta) * ucb + beta * rave;
    }

    Node* get_best_child() {
        if (children.empty()) return nullptr;

        std::vector<Node*> chosen;
        double curr_score = children[0]->get_score();
        chosen.push_back(children[0]);

        for(Node* child: children) {
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
};