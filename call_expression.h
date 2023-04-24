#pragma once

#include "expression.h"

#include <memory>
#include <vector>

#include "token.h"

class Call_Expression: public Expression {
public:
    const Expression::Ptr callee;
    const Token paren;
    const std::vector<Expression::Ptr> arguments;

    Call_Expression(Expression::Ptr l, const Token &p, std::vector<Expression::Ptr> &&a):
            callee { std::move(l) }, paren { p }, arguments { std::move(a) }
    { }

    void accept(Expression_Visitor &visitor) const override { visitor.visit(*this); }
};
