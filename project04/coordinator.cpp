//
// Created by thethongngu on 11/20/19.
//

#include "coordinator.h"

#include <utility>
#include <iostream>

std::string Coordinator::preprocess(const std::string& raw_command) {

    std::string res;
    bool in_comment = false;
    for(int code : raw_command) {
        if (code < 32 && code != 9 && code != 10) continue;  // control char not HT or LF

        if (code == 35) in_comment = true;   // # char
        if (code == 10) in_comment = false;  // LF char
        if (in_comment) continue;  // after # char

        if (code == 9) res.append(1, (char) 32);  // HT to SPACE
        else res.append(1, (char) code);
    }

    return res;
}

void Coordinator::run(const std::string& raw_command) {
    auto command = preprocess(raw_command);
}