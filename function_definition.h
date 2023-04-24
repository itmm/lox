#pragma once

#include "statement.h"

#include <memory>
#include <utility>
#include <vector>

#include "block_statement.h"
#include "expression.h"
#include "token.h"

class Function_Definition: public Statement, public std::enable_shared_from_this<Function_Definition> {
public:
    using Ptr = std::shared_ptr<const Function_Definition>;

    const Token name;
    const std::vector<Token> params;
    const Block_Statement::Ptr body;

    Function_Definition(const Token &n, std::vector<Token> &&p, Block_Statement::Ptr b):
            name { n }, params { std::move(p) }, body { std::move(b) } { }

    void accept(Statement_Visitor &visitor) const override { visitor.visit(*this); }

    Ptr shared() const { return shared_from_this(); }
};
