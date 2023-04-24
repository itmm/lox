#pragma once

#include "statement.h"

#include <memory>

#include "expression.h"

class If_Statement: public Statement {
public:
    const Expression::Ptr condition;
    const Statement::Ptr then_branch;
    const Statement::Ptr else_branch;

    explicit If_Statement(Expression::Ptr c, Statement::Ptr tb, Statement::Ptr eb):
        condition { std::move(c) }, then_branch { std::move(tb) }, else_branch { std::move(eb) } { }

    void accept(Statement_Visitor &visitor) const override { visitor.visit(*this); }
};
