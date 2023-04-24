#pragma once

#include "expression.h"

#include <memory>

class Grouping: public Expression {
    public:
        const Expression::Ptr expression;

        explicit Grouping(Expression::Ptr e): expression {std::move(e) } { }

        void accept(Expression_Visitor &visitor) const override { visitor.visit(*this); }
};