#pragma once

#include <vector>

#include "literal.h"

class Interpreter;

class Callable_Literal: public Literal {
public:
    explicit operator std::string() const override { return "<native fn>"; }
    virtual Literal::Ptr call(Interpreter &interpreter, const std::vector<Literal::Ptr> &arguments) const = 0;
    virtual int arity() const = 0;
};
