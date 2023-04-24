#pragma once

#include "expression.h"
#include "token.h"

#include <memory>
#include <utility>

class Assign_Expression: public Expression {
public:
    const Token name;
    const Expression::Ptr value;

    Assign_Expression(const Token &n, Expression::Ptr v): name { n }, value { std::move(v) } { }

    void accept(Expression_Visitor &visitor) const override { visitor.visit(*this); }
};
