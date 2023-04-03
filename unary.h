#pragma once

#include "expr.h"
#include "token.h"

class Unary: public Expr {
    public:
        const Token token;
        std::unique_ptr<Expr> right;

        Unary(const Token &t, std::unique_ptr<Expr> &&r): token { t }, right { std::move(r) } { }

        void accept(Expr_Visitor &visitor) const { visitor.visit(*this); }
};