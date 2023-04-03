#pragma once

class Binary;
class Grouping;
class Literal;
class Unary;

class Expr_Visitor {
public:
    virtual void visit(const Binary &binary) = 0;
    virtual void visit(const Grouping &grouping) = 0;
    virtual void visit(const Literal &literal) = 0;
    virtual void visit(const Unary &unary) = 0;
};

class Expr {
    public:
        virtual ~Expr() = default;
        virtual void accept(Expr_Visitor &visitor) const = 0;
};
