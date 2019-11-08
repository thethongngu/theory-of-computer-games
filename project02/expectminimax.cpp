//
// Created by thethongngu on 11/8/19.
//

#include <vector>
#include "expectminimax.h"
#include "agent.h"

ExpectMinimax::ExpectMinimax(int _depth, int _num_player_action, int _num_evil_action) {
    depth = _depth;
    num_player_action = _num_player_action;
    num_evil_action = _num_evil_action;
    Node _root;
    root = _root;
    generate_tree(root, depth);
}

void ExpectMinimax::generate_tree(Node& currNode, int d) {
    if (d == 0) {  // leaf
    } else {
        if (d % 2 == 0) {  // max node
            for(int i = 0; i < num_player_action; i++) {
                Node child;
                currNode.children.push_back(child);
                generate_tree(child, d - 1);
            }
        } else {  // chance node
            for(int i = 0; i < num_evil_action; i++) {
                Node child;
                currNode.children.push_back(child);
                generate_tree(child, d - 1);
            }
        }
    }
}

double ExpectMinimax::search(Node currNode, int d, TDPlayer player) {
    if (d == 0) {
        return player.get_v(currNode.board);
    } else {
        if (d % 2 == 0) {  // max node
            double max_value = -1000000000;
            for(int op = 0; op < 4; op++) {
                Node nextNode = currNode.children[op];
                nextNode.board = currNode.board;
                nextNode.last_op = op;
                int valid = Action::Slide(op).apply(nextNode.board);
                if (valid == -1) continue;

                double search_value = search(nextNode, d - 1, player);
                max_value = std::max(max_value, search_value);
            }
            return max_value;
        } else {    // chance node
            double expect_value = 0;
            for(int tile = 1; tile <= 3; tile++) {
                for(int pos_id = 0; pos_id < 4; pos_id++) {
                    Node nextNode = currNode.children[4 * (tile - 1) + pos_id];
                    int pos = place_pos[currNode.last_op][pos_id];
                    int valid = nextNode.board.place(pos, tile);
                    if (valid == -1) continue;

                    double search_value = search(nextNode, d - 1, player);
                    expect_value = expect_value + (1.0 / 12) * search_value;
                }
            }
            return expect_value;
        }
    }
}

ExpectMinimax::Node& ExpectMinimax::get_root() {
    return root;
}