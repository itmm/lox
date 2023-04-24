#pragma once

#include "statement.h"

#include <memory>

#include "expression.h"

class While_Statement: public Statement {
public:
    const Expression::Ptr condition;
    const Statement::Ptr body;

    explicit While_Statement(Expression::Ptr c, Statement::Ptr b):
            condition { std::move(c) }, body { std::move(b) } { }

    void accept(Statement_Visitor &visitor) const override { visitor.visit(*this); }
};
