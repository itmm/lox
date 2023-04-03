#include "err.h"

#include <iostream>

bool had_error = false;

static void report(int line, std::string where, std::string message) {
    std::cerr << "[line " << line << "] Error" << where << ": " << message << "\n";
    had_error = true;
}

void error(int line, std::string message) {
    report(line, "", message);
}
