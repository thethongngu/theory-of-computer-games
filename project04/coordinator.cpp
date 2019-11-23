//
// Created by thethongngu on 11/20/19.
//

#include "coordinator.h"
#include "helper.h"
#include <iostream>

#define print(a) std::cout << a << std::endl

static std::array<std::string, 11> known_command = {
        "protocol_version", "name", "version", "known_command" ,"list_commands"
        ,"quit", "boardsize", "clear_board", "komi", "play", "genmove"
};

Coordinator::Coordinator() {
    is_stop = false;
}


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

std::vector<std::string> Coordinator::parse_command(const std::string &command) {
    return std::vector<std::string>();
}

bool Coordinator::is_known_command(const std::vector<std::string>& tokens) {
    for(const auto & command : known_commands)
        if (command == tokens[1]) return true;
    return false;
}

void Coordinator::run(const std::string& raw_command) {
    auto command = preprocess(raw_command);
    auto tokens = parse_command(command);
    if (tokens[0] == "protocol_version") {
        print("2");
    } else if (tokens[0] == "name") {
        print("0860843");
    } else if (tokens[0] == "version") {
        print("0");
    } else if (tokens[0] == "known_command") {
        print(is_known_command(tokens));
    } else if (tokens[0] == "list_commands") {
        for(const auto& x: known_commands) print(x);
    } else if (tokens[0] == "quit") {
        is_stop = true;
    } else if (tokens[0] == "boardsize") {
        update_boardsize(tokens);
    } else if (tokens[0] == "clear_board") {
        clear_board();
    } else if (tokens[0] == "play") {
        move(tokens);
    } else if (tokens[0] == "genmove") {

    }
}

void Coordinator::start() {
    while (!is_stop) {

    }
}

void Coordinator::update_boardsize(const std::vector<std::string> &tokens) {
    if (tokens[1] != "9") {
        print("unacceptable size");
        return;
    }
}

void Coordinator::clear_board() {
    board.clear_board();
}

void Coordinator::move(const std::vector<std::string>& tokens) {
    std::string token;

    token = Helper::to_lowercase(tokens[1]);
    auto color = (token[0] == 'b') ? Board::BLACK: Board::WHITE;

    token = Helper::to_lowercase(tokens[2]);
    int row = token[0] - 'a';
    int col = token[1] - '1';
    if (board.place(row, col, color) == -1) {
        print("illegal move");
    }
}
