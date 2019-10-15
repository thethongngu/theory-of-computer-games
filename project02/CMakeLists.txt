cmake_minimum_required(VERSION 3.15)
project(project02)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(project02
        2048.cpp
        action.h
        agent.h
        board.h
        episode.h
        statistic.h
        weight.h)
