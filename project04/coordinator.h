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
        unsigned id;
        std::string command;
        std::vector<std::string> arguments;

        Command() {
            id = -1;
            command = "";
            arguments.clear();
        }
    };

public:
    Coordinator();

    void start();

    static std::string preprocess(const std::string& raw_command);
    void run(const std::string& command);
    static Command parse_command(const std::string& command);

    static void response(bool is_success, Command &command, const std::string &mess);
    static bool is_known_command(const std::string &head);
    static bool update_boardsize(const std::vector<std::string>& args);
    void clear_board();
    bool move(const std::vector<std::string>& args);

private:
    bool is_stop;
    Board board;
};


#endif //PROJECT04_COORDINATOR_H
