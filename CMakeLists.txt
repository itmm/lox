cmake_minimum_required(VERSION 3.18)
project(lox)

set(CMAKE_CXX_STANDARD 20)

add_executable(lox main.cpp scanner.cpp scanner.h err.h err.cpp token.h expr.h binary.h grouping.h literal.h unary.h parser.h interpreter.h)
