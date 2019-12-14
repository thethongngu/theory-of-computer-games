//
// Created by thethongngu on 12/15/19.
//

#ifndef PROJECT04_NODE_H
#define PROJECT04_NODE_H

#endif //PROJECT04_NODE_H

#include <vector>
#include "global.h"

class Node {
public:

    int last_pos;
    int last_color;

    std::vector<Node*> children;
};