#pragma once

#include "callable_literal.h"
#include "function_definition.h"
#include "environment.h"

class Function_Callable: public Callable_Literal {
    Function_Definition::Ptr definition;

public:
    explicit Function_Callable(Function_Definition::Ptr fd): definition { std::move(fd) } { }

    Literal::Ptr call(Interpreter &interpreter, const std::vector<Literal::Ptr> &arguments) const override;

    explicit operator std::string() const override { return "<fn " + definition->name.lexeme + ">"; }

    [[nodiscard]] int arity() const override { return static_cast<int>(definition->params.size()); }
};
