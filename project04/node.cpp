//
// Created by thethongngu on 12/3/19.
//

#include <cmath>
#include "node.h"

bool Node::is_leaf() {
    return children.empty();
}

Node *Node::get_best_uct_child() {
    if (is_leaf()) return nullptr;
    double curr_score = children[0]->get_score();
    std::vector<Node *> curr_child(1, children[0]);

    for (size_t i = 1; i < children.size(); i++) {
        Node *child = children[i];
        auto score = child->get_score();
        if (curr_score - score > 0.0001) {  /** curr_score > score */
            curr_child.clear();
            curr_child.push_back(child);
            curr_score = score;
        } else if (curr_score - score > -0.0001) {  /** curr_score >= score */
            curr_child.push_back(child);
        }
    }

    return curr_child[rand() % curr_child.size()];
}

/**
 * @return the current score of node
 */
double Node::get_score() {
    return (double)num_win / count + sqrt(log(get_parent()->count) / count);
}

Node *Node::get_parent() {
    return parent;
}

void Node::update_value(int value) {
    num_win += value;
    count++;
}

bool Node::is_fully_expanded() {
    return board.num_empty_cell == children.size();
}
