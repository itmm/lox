#pragma once

#include "statement.h"

#include <memory>
#include <vector>

class Block_Statement: public Statement {
public:
    using Ptr = std::shared_ptr<Block_Statement>;

    const std::vector<Statement::Ptr> statements;

    explicit Block_Statement(std::vector<Statement::Ptr> s): statements { std::move(s) } { }

    void accept(Statement_Visitor &visitor) const override { visitor.visit(*this); }
};
