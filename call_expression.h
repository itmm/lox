#pragma once

#include "expr.h"

#include <memory>
#include <vector>

#include "token.h"

class Call_Expression: public Expr {
public:
    const std::unique_ptr<Expr> callee;
    const Token paren;
    const std::vector<std::unique_ptr<Expr>> arguments;

    Call_Expression(std::unique_ptr<Expr> &&l, const Token &p, std::vector<std::unique_ptr<Expr>> &&a):
            callee { std::move(l) }, paren { p }, arguments { std::move(a) }
    { }

    void accept(Expr_Visitor &visitor) const override { visitor.visit(*this); }
};
