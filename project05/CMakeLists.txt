cmake_minimum_required(VERSION 3.15)
project(project05)

set(CMAKE_CXX_STANDARD 14)

add_executable(project05 main.cpp bit_board.h global.h board.cpp MCTS.h node.cpp)