#pragma once

#include "expression.h"

#include <memory>

#include "token.h"

class Logical_Expression: public Expression {
public:
    const Token token;
    const Expression::Ptr left;
    const Expression::Ptr right;

    Logical_Expression(const Token& t, Expression::Ptr l, Expression::Ptr r):
            token { t }, left { std::move(l) }, right { std::move(r) }
    { }

    void accept(Expression_Visitor &visitor) const override { visitor.visit(*this); }
};
