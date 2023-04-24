#pragma once

class Block_Statement;
class Expression_Statement;
class Function_Definition;
class If_Statement;
class Print_Statement;
class Return_Statement;
class Var_Statement;
class While_Statement;

class Statement_Visitor {
    public:
        virtual ~Statement_Visitor() = default;
        virtual void visit(const Print_Statement &statement) = 0;
        virtual void visit(const Expression_Statement &statement) = 0;
        virtual void visit(const Var_Statement &statement) = 0;
        virtual void visit(const Block_Statement &statement) = 0;
        virtual void visit(const If_Statement &statement) = 0;
        virtual void visit(const While_Statement &statement) = 0;
        virtual void visit(const Function_Definition &statement) = 0;
        virtual void visit(const Return_Statement &statement) = 0;
};

class Statement {
public:
    using Ptr = std::shared_ptr<const Statement>;
    virtual ~Statement() = default;
    virtual void accept(Statement_Visitor &visitor) const = 0;
};
