#pragma once

#include "statement.h"

#include <memory>
#include <vector>

class Block_Statement: public Statement {
public:
    const std::vector<std::unique_ptr<Statement>> statements;

    explicit Block_Statement(std::vector<std::unique_ptr<Statement>> &&s): statements { std::move(s) } { }

    void accept(Statement_Visitor &visitor) const override { visitor.visit(*this); }
};
