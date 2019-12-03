//
// Created by thethongngu on 12/3/19.
//

#include "agent.h"

Agent::Agent() {

}

std::string Agent::make_move(Board curr_board, Board::Color color) {
    tree.set_root(curr_board);
}
