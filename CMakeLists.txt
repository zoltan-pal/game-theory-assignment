cmake_minimum_required(VERSION 3.12)
project(game_theory C)

set(CMAKE_C_STANDARD 90)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-long-long -std=c89 -pedantic")

add_executable(game_theory main.c population.c population.h person.h bool.h)