#pragma once

#include "statement.h"

#include <memory>

#include "expression.h"

class Print_Statement: public Statement {
public:
    const Expression::Ptr expression;

    explicit Print_Statement(Expression::Ptr e): expression {std::move(e) } { }

    void accept(Statement_Visitor &visitor) const override { visitor.visit(*this); }
};
