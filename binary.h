#pragma once

#include "expr.h"

#include <memory>

#include "token.h"

class Binary: public Expr {
    public:
        const Token token;
        const std::unique_ptr<Expr> left;
        const std::unique_ptr<Expr> right;

        Binary(const Token& t, std::unique_ptr<Expr> &&l, std::unique_ptr<Expr> &&r):
            token { t }, left { std::move(l) }, right { std::move(r) }
        { }

        void accept(Expr_Visitor &visitor) const { visitor.visit(*this); }
};