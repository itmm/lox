#pragma once

#include "statement.h"

#include <memory>

#include "expr.h"
#include "token.h"

class Var_Statement: public Statement {
public:
    const Token name;
    const std::unique_ptr<Expr> initializer;

    explicit Var_Statement(const Token &n, std::unique_ptr<Expr> &&i): name { n }, initializer { std::move(i) } { }

    void accept(Statement_Visitor &visitor) const override { visitor.visit(*this); }
};
