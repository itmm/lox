#pragma once

#include "token.h"

class Var_Expression: public Expr {
public:
    const Token name;

    explicit Var_Expression(const Token &n): name { n } { }

    void accept(Expr_Visitor &visitor) const override { visitor.visit(*this); }
};
