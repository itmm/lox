#pragma once

class Block_Statement;
class Expression_Statement;
class If_Statement;
class Print_Statement;
class Var_Statement;

class Statement_Visitor {
    public:
        virtual ~Statement_Visitor() = default;
        virtual void visit(const Print_Statement &statement) = 0;
        virtual void visit(const Expression_Statement &statement) = 0;
        virtual void visit(const Var_Statement &statement) = 0;
        virtual void visit(const Block_Statement &statement) = 0;
        virtual void visit(const If_Statement &statement) = 0;
};

class Statement {
public:
    virtual ~Statement() = default;
    virtual void accept(Statement_Visitor &visitor) const = 0;
};
