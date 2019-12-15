#include <iostream>
#include <cassert>
#include <vector>
#include <array>
#include <sstream>
#include <iterator>
#include <cmath>

#include "global.h"
#include "board.h"
#include "MCTS.h"


struct Command {
    int id = -1;
    std::string command = "";
    std::vector<std::string> arguments;
};

struct Log {
    Command command;
    std::string response;
};

std::array<std::string, 11> valid_coms = {
        "protocol_version",
        "name",
        "version",
        "known_command",
        "list_commands",
        "quit",
        "boardsize",
        "showboard",
        "clear_board",
        "play",
        "genmove"
};

Board mainboard;
MCTS tree;
bool is_quit;
std::vector<Log> history;

/** ----------------- Helper ----------------------- */

int get_int_helper(const std::string &s) {
    if (s.find_first_not_of("01234566789") != std::string::npos) return -1;
    return strtol(s.c_str(), nullptr, 0);
}

std::string to_lowercase_helper(const std::string &s) {
    std::string res;
    for (auto x: s) {
        if (x >= 'A' && x <= 'Z') res.append(1, x + 32);
        else res.append(1, x);
    }
    return res;
}

int parse_color_helper(const std::string &arg) {
    auto token = to_lowercase_helper(arg);
    if (token[0] == 'b') return BLACK;
    if (token[0] == 'w') return WHITE;
    return -1;
}

int parse_pos_helper(const std::string &arg) {
    auto token = to_lowercase_helper(arg);
    int row = token[1] - '1';
    int col = token[0] - 'a';

    if (col == 9) col = 8;   // fucking j :)
    if (token[0] == 'i') return -1;

    return row * BOARDSIZE + col;
}

std::string get_move_string(int pos) {
    assert(pos >= 0 && pos <= NUM_CELL - 1);
    std::string res;
    res.append(1, (char) (pos % BOARDSIZE + 'a'));
    if (res[0] == 'i') res[0] = 'j';   // fucking j :)
    res.append(1, (char) ((pos / BOARDSIZE) + '1'));
    return res;
}

/** ------------------ Coordinator ---------------------- */

std::string preprocess_command(const std::string &raw_command) {

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

Command parse_command(const std::string &command) {
    std::istringstream iss(command);
    std::vector<std::string> tokens(std::istream_iterator<std::string>{iss}, std::istream_iterator<std::string>());

    Command res = Command();
    if (tokens.size() == 0) return res;  // NOLINT(readability-container-size-empty)

    int id = get_int_helper(tokens[0]);
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

std::string get_response(bool is_success, Command &command, const std::string &mess) {
    std::string res;
    if (is_success) res.append("="); else res.append("?");
    if (command.id != -1) res.append(std::to_string(command.id));
    if (!mess.empty()) res.append(" ").append(mess);
    res.append("\n\n");
    return res;
}

bool is_known_command(const std::string &c, const std::array<std::string, 11> &known_coms) {
    for (const auto &command : known_coms)
        if (command == c) return true;
    return false;
}

bool make_input_move(Board &board, const std::vector<std::string> &args) {

    if (args.size() < 2) return false;
    int color = parse_color_helper(args[0]);
    int pos = parse_pos_helper(args[1]);

    if (color != BLACK && color != WHITE) return false;
    if (pos < 0 || pos >= NUM_CELL) return false;
    if (!board.can_move(pos, color)) return false;

    board.add_piece(pos, color);
    return true;
}

int make_AI_move(Board &board, int color) {
    tree.init_tree(board, color);
    for(int i = 0; i < SIM_TIMES; i++) {
        tree.run_once();
    }

    Node* child = tree.root->get_best_child();
    if (child == nullptr) return false;

    int pos = child->last_pos;
    board.add_piece(pos, color);
    tree.clear_tree();

    return pos;
}

/** ---------------------- MAIN --------------------------- */

void exec_command(const std::string &raw_command) {

    auto command_string = preprocess_command(raw_command);
    Command command = parse_command(command_string);
    auto head = command.command;
    auto args = command.arguments;

    std::string response;

    if (head == "protocol_version") {
        response = get_response(true, command, "2");

    } else if (head == "name") {
        response = get_response(true, command, "0860832");

    } else if (head == "version") {
        response = get_response(true, command, "0.0");

    } else if (head == "known_command") {
        std::string res = is_known_command(args[0], valid_coms) ? "true" : "false";
        response = get_response(true, command, res);

    } else if (head == "list_commands") {
        std::string res;
        for (const auto &x: valid_coms) {
            res.append(x);
            res.append("\n");
        }
        response = get_response(true, command, res);

    } else if (head == "boardsize") {
        response = get_response(true, command, "");

    } else if (head == "showboard") {
        response = get_response(true, command, "");
        mainboard.print();

    } else if (head == "quit") {
        is_quit = true;
        response = get_response(true, command, "");

    } else if (head == "clear_board") {
        mainboard.clear_all();
        response = get_response(true, command, "");

    } else if (head == "play") {
        bool can_move = make_input_move(mainboard, args);
        response = get_response(can_move, command, can_move ? "" : "illegal move");

    } else if (head == "genmove") {
        // TODO: assume that args always true because the protocol don't specify this
        int color = parse_color_helper(args[0]);
        int pos = make_AI_move(mainboard, color);

        if (color != WHITE && color != BLACK) {
            response = get_response(false, command, "wrong color syntax");
        }

        if (pos != -1) {
            std::string move = get_move_string(pos);
            response = get_response(true, command, move);
        } else {
            response = get_response(true, command, "resign");
        }

    } else {
        response = get_response(false, command, "unknown command");
    }

    history.push_back({command, response});
    std::cout << history.back().response;
}

void init_program() {
    is_quit = false;
    Board::generate_all_adjs();
    mainboard.clear_all();
    tree.clear_tree();
}

/** ------------------ ENTRY POINT ---------------- */
int main() {

    init_program();

    std::string raw_command;
    while (!is_quit) {
        getline(std::cin, raw_command);
        exec_command(raw_command);
        mainboard.print();
    }

    return 0;
}