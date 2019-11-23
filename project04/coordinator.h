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
public:
    Coordinator();

    void start();

    static std::string preprocess(const std::string& raw_command);
    void run(const std::string& command);
    static std::vector<std::string> parse_command(const std::string& command);

    static bool is_known_command(const std::vector<std::string>& tokens);
    static void update_boardsize(const std::vector<std::string>& tokens);
    void clear_board();
    void move(const std::vector<std::string>& tokens);

private:
    bool is_stop;
    Board board;
};


#endif //PROJECT04_COORDINATOR_H
