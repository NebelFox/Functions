cmake_minimum_required(VERSION 3.19)
project(4_functions)

set(CMAKE_CXX_STANDARD 20)

add_executable(4_functions main.cpp Compiler.hpp Function.hpp Token.hpp Grammar.hpp Calculator.hpp)