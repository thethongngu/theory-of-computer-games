//
// Created by thethongngu on 12/3/19.
//

#ifndef PROJECT04_AGENT_H
#define PROJECT04_AGENT_H


#include <string>
#include <vector>
#include "board.h"

class Agent {
public:
    Agent();

    std::string make_move(Board::Color color);
};


#endif //PROJECT04_AGENT_H
