#include "err.h"

#include <iostream>

#include "token.h"

bool had_error = false;

static void report(int line, const std::string& where, const std::string& message) {
    std::cerr << "[line " << line << "] Error" << where << ": " << message << "\n";
    had_error = true;
}

void error(int line, const std::string& message) {
    report(line, "", message);
}

void error(const Token &token, const std::string& message) {
    if (token.type == Token_Type::END_OF_DATA) {
        report(token.line, " at end", message);
    } else {
        report(token.line, " at '" + token.lexeme + "'", message);
    }
}
