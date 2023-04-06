#pragma once

#include <string>

void error(int line, const std::string& message);

class Token;

void error(const Token &token, const std::string& message);
void runtime_error(const Token &token, const std::string &message);

extern bool had_error;
extern bool had_runtime_error;