#pragma once

#include "expr.h"

#include <memory>

#include "token.h"

class Logical_Expression: public Expr {
public:
    const Token token;
    const std::unique_ptr<Expr> left;
    const std::unique_ptr<Expr> right;

    Logical_Expression(const Token& t, std::unique_ptr<Expr> &&l, std::unique_ptr<Expr> &&r):
            token { t }, left { std::move(l) }, right { std::move(r) }
    { }

    void accept(Expr_Visitor &visitor) const override { visitor.visit(*this); }
};
