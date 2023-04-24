#pragma once

#include "statement.h"

#include <memory>

#include "expression.h"
#include "token.h"

class Return_Statement: public Statement {
public:
    const Token keyword;
    const Expression::Ptr value;

    explicit Return_Statement(const Token& t, Expression::Ptr v): keyword { t }, value { std::move(v) } { }

    void accept(Statement_Visitor &visitor) const override { visitor.visit(*this); }
};
