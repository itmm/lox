#pragma once

#include <vector>

#include "literal.h"

class Interpreter;

class Callable_Literal: public Literal {
public:
    explicit operator std::string() const override { return "<native fn>"; }
    virtual Literal_p call(Interpreter &interpreter, const std::vector<Literal_p> &arguments) = 0;
    virtual int arity() = 0;
};
