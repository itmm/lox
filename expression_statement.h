#pragma once

#include "statement.h"

#include <memory>

#include "expr.h"

class Expression_Statement: public Statement {
    public:
        const std::unique_ptr<Expr> expression;

        explicit Expression_Statement(std::unique_ptr<Expr> &&e): expression { std::move(e) } { }

        void accept(Statement_Visitor &visitor) const override { visitor.visit(*this); }
};