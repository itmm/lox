#pragma once

class Assign_Expression;
class Binary;
class Grouping;
class Literal;
class Unary;
class Var_Expression;

class Expr_Visitor {
public:
    virtual ~Expr_Visitor() = default;
    virtual void visit(const Binary &binary) = 0;
    virtual void visit(const Grouping &grouping) = 0;
    virtual void visit(const Literal &literal) = 0;
    virtual void visit(const Unary &unary) = 0;
    virtual void visit(const Var_Expression &expr) = 0;
    virtual void visit(const Assign_Expression &expr) = 0;
};

class Expr {
    public:
        virtual ~Expr() = default;
        virtual void accept(Expr_Visitor &visitor) const = 0;
};
