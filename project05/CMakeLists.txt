cmake_minimum_required(VERSION 3.15)
project(project05)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(project05
        bit_board.h
        board.cpp
        board.h
        global.h
        main.cpp
        MCTS.cpp
        MCTS.h
        node.cpp
        node.h)
