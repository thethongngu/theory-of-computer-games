cmake_minimum_required(VERSION 3.15)
project(project04)

set(CMAKE_CXX_STANDARD 14)

add_executable(project05 main.cpp bit_board.h global.h board.h MCTS.h node.cpp)