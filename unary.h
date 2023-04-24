#pragma once

#include "expression.h"
#include "token.h"

class Unary: public Expression {
    public:
        const Token token;
        Expression::Ptr right;

        Unary(const Token &t, Expression::Ptr r): token {t }, right {std::move(r) } { }

        void accept(Expression_Visitor &visitor) const override { visitor.visit(*this); }
};