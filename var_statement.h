#pragma once

#include "statement.h"

#include <memory>

#include "expression.h"
#include "token.h"

class Var_Statement: public Statement {
public:
    const Token name;
    const Expression::Ptr initializer;

    explicit Var_Statement(const Token &n, Expression::Ptr i): name {n }, initializer {std::move(i) } { }

    void accept(Statement_Visitor &visitor) const override { visitor.visit(*this); }
};
