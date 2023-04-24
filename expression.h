#pragma once

#include <memory>

class Expression_Visitor;

class Expression {
    public:
        using Ptr = std::shared_ptr<const Expression>;
        virtual ~Expression() = default;
        virtual void accept(Expression_Visitor &visitor) const = 0;
};

class Assign_Expression;
class Binary_Expression;
class Call_Expression;
class Grouping;
class Literal;
class Logical_Expression;
class Unary;
class Var_Expression;

class Expression_Visitor {
public:
    virtual ~Expression_Visitor() = default;
    virtual void visit(const Binary_Expression &binary) = 0;
    virtual void visit(const Grouping &grouping) = 0;
    virtual void visit(const Literal &literal) = 0;
    virtual void visit(const Unary &unary) = 0;
    virtual void visit(const Var_Expression &expr) = 0;
    virtual void visit(const Assign_Expression &expr) = 0;
    virtual void visit(const Logical_Expression &expr) = 0;
    virtual void visit(const Call_Expression &expr) = 0;
};

