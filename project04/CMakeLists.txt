cmake_minimum_required(VERSION 3.15)
project(project04)

set(CMAKE_CXX_STANDARD 14)

add_executable(project04 main.cpp board.cpp board.h episode.cpp episode.h coordinator.cpp coordinator.h test.cpp test.h helper.cpp helper.h)