#pragma once

#include "statement.h"

#include <memory>

#include "expr.h"

class While_Statement: public Statement {
public:
    const std::unique_ptr<Expr> condition;
    const std::unique_ptr<Statement> body;

    explicit While_Statement(std::unique_ptr<Expr> &&c, std::unique_ptr<Statement> &&b):
            condition { std::move(c) }, body { std::move(b) } { }

    void accept(Statement_Visitor &visitor) const override { visitor.visit(*this); }
};
