//
// Created by thethongngu on 11/20/19.
//

#include "coordinator.h"
#include "helper.h"
#include <iostream>
#include <sstream>
#include <iterator>

std::array<std::string, 11> Coordinator::known_commands = {
        "protocol_version", "name", "version", "known_command", "list_commands", "quit", "boardsize", "clear_board",
        "komi", "play", "genmove"
};

Coordinator::Coordinator() {
    is_stop = false;
    ai = Agent();
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

/**
 * Generate a response message
 */
std::string Coordinator::get_response(bool is_success, Coordinator::Command &command, const std::string &mess) {
    std::string res;
    if (is_success) res.append("="); else res.append("?");
    if (command.id != -1) res.append(std::to_string(command.id));
    if (!mess.empty()) res.append(" ").append(mess);
    res.append("\n\n");
    return res;
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
    command_history.push_back(command);

    if (head == "protocol_version") {
        response_history.push_back(get_response(true, command, "2"));

    } else if (head == "name") {
        response_history.push_back(get_response(true, command, "0860832"));

    } else if (head == "version") {
        response_history.push_back(get_response(true, command, "0.0"));

    } else if (head == "known_command") {
        std::string res = is_known_command(args[0]) ? "true" : "false";
        response_history.push_back(get_response(true, command, res));

    } else if (head == "list_commands") {
        std::string res;
        for (const auto &x: known_commands) {
            res.append(x);  res.append("\n");
        }
        response_history.push_back(get_response(true, command, res));

    } else if (head == "quit") {
        is_stop = true;
        response_history.push_back(get_response(true, command, ""));

    } else if (head == "boardsize") {
        bool res = update_boardsize(args);
        response_history.push_back(get_response(res, command, res ? "" : "unacceptable size"));

    } else if (head == "clear_board") {
        clear_board();
        response_history.push_back(get_response(true, command, ""));

    } else if (head == "play") {
        bool can_move = move(args);
        response_history.push_back(get_response(can_move, command, can_move ? "" : "illegal move"));

    } else if (head == "genmove") {
        // TODO: assume that args always true because the protocol don't specify this
        auto color = parse_color(args[0]);
        int pos = ai.make_move(board, color);
        response_history.push_back(get_response(pos != -1, command, pos != -1 ? "" : "resign"));

    } else {
        response_history.push_back(get_response(false, command, "unknown command"));
    }

    std::cout << response_history.back();
}

void Coordinator::start() {
    std::string raw_command;

    while (!is_stop) {
        std::cout << "Input: ";
        getline(std::cin, raw_command);
        run(raw_command);
        board.print();
    }
}

bool Coordinator::update_boardsize(const std::vector<std::string> &args) {
    return args[0] == "9";
}

void Coordinator::clear_board() {
    board.clear_board();
}

Board::Color Coordinator::parse_color(const std::string& arg) {
    auto token = Helper::to_lowercase(arg);
    return (token[0] == 'b') ? Board::BLACK : Board::WHITE;
}

std::pair<int, int> Coordinator::parse_pos(const std::string& arg) {
    auto token = Helper::to_lowercase(arg);
    int row = token[1] - '1';
    int col = token[0] - 'a';
    return {row, col};
}

bool Coordinator::move(const std::vector<std::string> &args) {
    if (args.size() < 2) return false;
    auto color = parse_color(args[0]);
    auto pos = parse_pos(args[1]);
    return board.place(pos.first, pos.second, color) != -1;
}

std::vector<std::pair<Coordinator::Command, std::string> > Coordinator::get_all_moves(bool only_valid) {
    std::vector<std::pair<Command, std::string> > res;

    for (size_t i = 0; i < command_history.size(); i++) {
        auto com = command_history[i];
        if (com.command != "genmove" && com.command != "play") continue;
        if (!only_valid) res.emplace_back(com, response_history[i]);
        else if (is_valid_response(response_history[i])) res.emplace_back(com, response_history[i]);
    }

    return res;
}

bool Coordinator::is_valid_response(const std::string &s) {
    return s[0] == '=';
}
