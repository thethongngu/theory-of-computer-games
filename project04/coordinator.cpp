//
// Created by thethongngu on 11/20/19.
//

#include <cstring>
#include "coordinator.h"

void Coordinator::preprocess(char* raw_command, char* new_command) {

    size_t next_pos = 0;
    bool in_comment = false;
    for(size_t i = 0; i < strlen(raw_command); i++) {
        int code = raw_command[i];
        if (code < 32 && code != 9 && code != 10) continue;  // control char not HT or LF

        if (code == 35) in_comment = true;   // # char
        if (code == 10) in_comment = false;  // LF char
        if (in_comment) continue;  // after # char

        if (code == 9) new_command[next_pos] = (char) 32;
        new_command[next_pos++] = (char) code;
    }
    new_command[next_pos] = '\0';
}

void Coordinator::run(char* raw_command) {
    char new_command[strlen(raw_command)];
    preprocess(raw_command, new_command);
}