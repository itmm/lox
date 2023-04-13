#pragma once

#include "statement.h"

#include <memory>

#include "expr.h"

class If_Statement: public Statement {
public:
    const std::unique_ptr<Expr> condition;
    const std::unique_ptr<Statement> then_branch;
    const std::unique_ptr<Statement> else_branch;

    explicit If_Statement(std::unique_ptr<Expr> &&c, std::unique_ptr<Statement> &&tb, std::unique_ptr<Statement>&& eb):
        condition { std::move(c) }, then_branch { std::move(tb) }, else_branch { std::move(eb) } { }

    void accept(Statement_Visitor &visitor) const override { visitor.visit(*this); }
};
