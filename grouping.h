#pragma once

#include "expr.h"

#include <memory>

class Grouping: public Expr {
    public:
        const std::unique_ptr<Expr> expression;

        explicit Grouping(std::unique_ptr<Expr> &&e): expression { std::move(e) } { }

        void accept(Expr_Visitor &visitor) const { visitor.visit(*this); }
};