#pragma once

#include <string>

void error(int line, std::string message);

class Token;

void error(const Token &token, std::string message);

extern bool had_error;