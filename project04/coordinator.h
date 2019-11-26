//
// Created by thethongngu on 11/20/19.
//

#ifndef PROJECT04_COORDINATOR_H
#define PROJECT04_COORDINATOR_H

#include <string>
#include <vector>
#include <array>
#include "board.h"


class Coordinator {
public:
    static std::array<std::string, 11> known_commands;

    struct Command {
        int id;
        std::string command;
        std::vector<std::string> arguments;

        Command() {
            id = -1;
            command = "";
            arguments.clear();
        }

        Command(int _id, const std::string &_command, const std::string &_arg) {
            id = _id, command = _command, arguments.push_back(_arg);
        }

        std::string get_string() {
            std::string s = std::to_string(id).append(" ").append(command);
            for(auto &arg: arguments) s.append(" ").append(arg);
            return s;
        }
    };

public:
    Coordinator();

    static std::string preprocess(const std::string& raw_command);
    static Command parse_command(const std::string& command);
    static std::string get_response(bool is_success, Command &command, const std::string &mess);
    static bool is_known_command(const std::string &head);
    static bool update_boardsize(const std::vector<std::string>& args);
    static bool is_valid_response(const std::string &s);

    void start();
    void run(const std::string& command);
    void clear_board();
    bool move(const std::vector<std::string>& args);
    std::vector<std::pair<Command, std::string> > get_all_moves(bool only_valid);

private:
    bool is_stop;
    Board board;
    std::vector<Command> command_history;
    std::vector<std::string> response_history;
};


#endif //PROJECT04_COORDINATOR_H
