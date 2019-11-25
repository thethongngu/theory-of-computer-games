//
// Created by thethongngu on 11/20/19.
//

#include "coordinator.h"
#include "helper.h"
#include <iostream>
#include <sstream>
#include <iterator>

#define print(a) std::cout << a << std::endl

std::array<std::string, 11> Coordinator::known_commands = {
        "protocol_version", "name", "version", "known_command", "list_commands", "quit", "boardsize", "clear_board",
        "komi", "play", "genmove"
};

Coordinator::Coordinator() {
    is_stop = false;
}


std::string Coordinator::preprocess(const std::string &raw_command) {

    std::string res;
    bool in_comment = false;
    for (int code : raw_command) {
        if (code < 32 && code != 9 && code != 10) continue;  // control char not HT or LF

        if (code == 35) in_comment = true;   // # char
        if (code == 10) in_comment = false;  // LF char
        if (in_comment) continue;  // after # char

        if (code == 9) res.append(1, (char) 32);  // HT to SPACE
        else res.append(1, (char) code);
    }

    return res;
}

Coordinator::Command Coordinator::parse_command(const std::string &command) {
    std::istringstream iss(command);
    std::vector<std::string> tokens(std::istream_iterator<std::string>{iss}, std::istream_iterator<std::string>());

    Command res;
    if (tokens.size() == 0) return res;  // NOLINT(readability-container-size-empty)

    int id = Helper::get_int(tokens[0]);
    if (id >= 0) {
        res.id = id;
        if (tokens.size() > 1) res.command = tokens[1];
        if (tokens.size() > 2) res.arguments.assign(tokens.begin() + 2, tokens.end());
    } else {
        res.command = tokens[0];
        if (tokens.size() > 1) res.arguments.assign(tokens.begin() + 1, tokens.end());
    }
    return res;
}

void Coordinator::response(bool is_success, Coordinator::Command &command, const std::string &mess) {
    if (is_success) std::cout << "="; else std::cout << "?";
    if (command.id != -1) std::cout << command.id;
    if (!mess.empty()) std::cout << " " << mess;
    std::cout << "\n\n";
}

bool Coordinator::is_known_command(const std::string &c) {
    for (const auto &command : known_commands)
        if (command == c) return true;
    return false;
}

void Coordinator::run(const std::string &raw_command) {
    auto command_string = preprocess(raw_command);
    Command command = parse_command(command_string);
    auto head = command.command;
    auto args = command.arguments;

    if (head == "protocol_version") {
        response(true, command, "2");

    } else if (head == "name") {
        response(true, command, "0860832");

    } else if (head == "version") {
        response(true, command, "0.0");

    } else if (head == "known_command") {
        std::string res = is_known_command(args[0]) ? "true" : "false";
        response(true, command, res);

    } else if (head == "list_commands") {
        std::string res;
        for (const auto &x: known_commands) {
            res.append(x);  res.append("\n");
        }
        response(true, command, res);

    } else if (head == "quit") {
        is_stop = true;
        response(true, command, "");

    } else if (head == "boardsize") {
        bool res = update_boardsize(args);
        response(res, command, res ? "" : "unacceptable size");

    } else if (head == "clear_board") {
        clear_board();
        response(true, command, "");

    } else if (head == "play") {
        bool can_move = move(args);
        response(can_move, command, can_move ? "" : "illegal move");

    } else if (head == "genmove") {


    } else {
        response(false, command, "unknown command");
    }
}

void Coordinator::start() {
    std::string raw_command;

    while (!is_stop) {
        std::cout << "Input: ";
        getline(std::cin, raw_command);
        run(raw_command);
    }
}

bool Coordinator::update_boardsize(const std::vector<std::string> &args) {
    return args[0] == "9";
}

void Coordinator::clear_board() {
    board.clear_board();
}

bool Coordinator::move(const std::vector<std::string> &args) {
    std::string token;

    token = Helper::to_lowercase(args[0]);
    auto color = (token[0] == 'b') ? Board::BLACK : Board::WHITE;

    token = Helper::to_lowercase(args[1]);
    int row = token[0] - 'a';
    int col = token[1] - '1';
    return board.place(row, col, color) == -1;
}
