cmake_minimum_required(VERSION 3.18)
project(lox)

set(CMAKE_CXX_STANDARD 20)

add_executable(lox main.cpp scanner.cpp scanner.h err.h err.cpp token.h expr.h binary.h grouping.h literal.h unary.h parser.h interpreter.h statement.h expression_statement.h print_statement.h var_statement.h var_expression.h environment.h assign_expression.h block_statement.h if_statement.h logical_expression.h while_statement.h)
