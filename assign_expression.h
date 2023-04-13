#pragma once

#include "expr.h"
#include "token.h"

#include <memory>

class Assign_Expression: public Expr {
public:
    const Token name;
    const std::unique_ptr<Expr> value;

    Assign_Expression(const Token &n, std::unique_ptr<Expr> &&v): name { n }, value { std::move(v) } { }

    void accept(Expr_Visitor &visitor) const override { visitor.visit(*this); }
};
