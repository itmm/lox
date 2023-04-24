#pragma once

#include <iostream>
#include <utility>
#include <vector>

#include "assign_expression.h"
#include "binary_expression.h"
#include "block_statement.h"
#include "call_expression.h"
#include "callable_literal.h"
#include "environment.h"
#include "err.h"
#include "expression.h"
#include "expression_statement.h"
#include "function_callable.h"
#include "grouping.h"
#include "if_statement.h"
#include "literal.h"
#include "logical_expression.h"
#include "print_statement.h"
#include "return_statement.h"
#include "statement.h"
#include "token.h"
#include "unary.h"
#include "var_statement.h"
#include "var_expression.h"
#include "while_statement.h"
#include "clock_callable.h"

class Interpreter;

class Env_Handler {
        Interpreter &interpreter_;
        Environment::Ptr old_;

    public:
        explicit Env_Handler(Interpreter &i);
        Env_Handler(Interpreter &i, Environment::Ptr new_env);
        ~Env_Handler();
};

class Interpreter: public Expression_Visitor, public Statement_Visitor {
    Literal::Ptr value_;
    Environment::Ptr environment_;

    static bool is_truthy(const Literal::Ptr &literal) {
        if (! literal) { return false; }
        if (literal->is_bool()) { return literal->as_bool(); }
        return true;
    }

    static bool is_equal(const Literal::Ptr &a, const Literal::Ptr &b) {
        if (! a) { return ! b; }
        if (a->is_bool()) { return b->is_bool() && a->as_bool() == b->as_bool(); }
        if (a->is_string()) { return b->is_string() && a->as_string() == b->as_string(); }
        if (a->is_number()) { return b->is_number() && a->as_number() == b->as_number(); }
        return false;
    }

    static void check_number_operand(const Token &token, const Literal::Ptr &right);
    static void check_number_operands(const Token &token, const Literal::Ptr &left, const Literal::Ptr &right);

    friend class Env_Handler;

public:
    Environment::Ptr globals;

    class Exception: public std::domain_error {
        public:
            const Token token;
            Exception(const Token &t, const std::string& m): std::domain_error { m }, token { t } { }
    };

    class Return: public std::runtime_error {
        public:
            const Literal::Ptr value;
            explicit Return(Literal::Ptr v) : runtime_error("return called"), value{std::move(v)} { }
    };

    Interpreter() {
        globals = std::make_shared<Environment>();
        globals->define("clock", std::make_shared<Clock_Callable>());
    }

    void visit(const Binary_Expression &binary) override {
        binary.left->accept(*this);
        Literal::Ptr left = std::move(value_);
        binary.right->accept(*this);
        Literal::Ptr right = std::move(value_);

        switch (binary.token.type) {
            case Token_Type::GREATER:
                check_number_operands(binary.token, left, right);
                value_ = Literal::create(left->as_number() > right->as_number());
                return;
            case Token_Type::GREATER_EQUAL:
                check_number_operands(binary.token, left, right);
                value_ = Literal::create(left->as_number() >= right->as_number());
                return;
            case Token_Type::LESS:
                check_number_operands(binary.token, left, right);
                value_ = Literal::create(left->as_number() < right->as_number());
                return;
            case Token_Type::LESS_EQUAL:
                check_number_operands(binary.token, left, right);
                value_ = Literal::create(left->as_number() <= right->as_number());
                return;
            case Token_Type::BANG_EQUAL:
                check_number_operands(binary.token, left, right);
                value_ = Literal::create(!is_equal(left, right));
                return;
            case Token_Type::EQUAL_EQUAL:
                check_number_operands(binary.token, left, right);
                value_ = Literal::create(is_equal(left, right));
                return;
            case Token_Type::MINUS:
                check_number_operands(binary.token, left, right);
                value_ = Literal::create(left->as_number() - right->as_number());
                return;
            case Token_Type::PLUS:
                if (left && right && left->is_number() && right->is_number()) {
                    value_ = Literal::create(left->as_number() + right->as_number());
                    return;
                }
                if (left && right && left->is_string() && right->is_string()) {
                    value_ = Literal::create(left->as_string() + right->as_string());
                    return;
                }
                throw Exception { binary.token, "Operands must be two numbers or two strings." };
            case Token_Type::SLASH:
                check_number_operands(binary.token, left, right);
                value_ = Literal::create(left->as_number() / right->as_number());
                return;
            case Token_Type::STAR:
                check_number_operands(binary.token, left, right);
                value_ = Literal::create(left->as_number() * right->as_number());
                return;
            default: break;
        }
    }

    void visit(const Grouping &grouping) override {
        grouping.expression->accept(*this);
    }

    void visit(const Literal &literal) override {
        value_ = literal.shared();
    }

    void visit(const Unary &unary) override {
        unary.right->accept(*this);
        Literal::Ptr right = std::move(value_);
        switch (unary.token.type) {
            case Token_Type::BANG:
                value_ = Literal::create(! is_truthy(value_));
                return;
            case Token_Type::MINUS:
                check_number_operand(unary.token, right);
                value_ = Literal::create(-right->as_number());
                return;
            default:
                break;
        }
    }

    void visit(const Var_Expression &expression) override {
        value_ = environment_->get(expression.name);
    }

    void visit(const Print_Statement &statement) override {
        if (statement.expression) { statement.expression->accept(*this); } else { value_ = {}; }
        std::cout << Literal::to_string(value_) << "\n";
    }

    void visit(const Expression_Statement &statement) override {
        if (statement.expression) { statement.expression->accept(*this); }
    }

    void visit(const Var_Statement &statement) override {
        Literal::Ptr initializer;
        if (statement.initializer) {
            statement.initializer->accept(*this);
            initializer = std::move(value_);
        }
        environment_->define(statement.name.lexeme, std::move(initializer));
    }

    void visit(const Assign_Expression &statement) override {
        statement.value->accept(*this);
        environment_->assign(statement.name, value_);
    }

    void execute_block(const Block_Statement &statement, Environment::Ptr env) {
        Env_Handler env_handler { *this, std::move(env) };
        for (const auto &s : statement.statements) {
            if (s) { s->accept(*this); }
        }
    }

    void visit(const Block_Statement &statement) override {
        Env_Handler env_handler { *this };
        execute_block(statement, environment_);
    }

    void visit(const If_Statement &statement) override {
        if (statement.condition) { statement.condition->accept(*this); } else { value_ = {}; }
        if (is_truthy(value_)) {
            if (statement.then_branch) { statement.then_branch->accept(*this); }
        } else if (statement.else_branch) {
            statement.else_branch->accept(*this);
        }
        value_ = {};
    }

    void visit(const Logical_Expression &statement) override {
        if (statement.left) { statement.left->accept(*this); } else { value_ = {}; }
        if (statement.token.type == Token_Type::OR) {
            if (is_truthy(value_)) { return; }
        } else {
            if (!is_truthy(value_)) { return; }
        }
        if (statement.right) { statement.right->accept(*this); } else { value_ = {}; }
    }

    void visit(const While_Statement &statement) override {
        for (;;) {
            if (statement.condition) { statement.condition->accept(*this); } else { value_ = {}; }
            if (! is_truthy(value_)) { break; }
            if (statement.body) { statement.body->accept(*this); }
        }
    }

    void visit(const Call_Expression &expr) override {
        if (expr.callee) { expr.callee->accept(*this); } else { value_ = { }; }
        Literal::Ptr callee = std::move(value_);

        std::vector<Literal::Ptr> arguments;
        for (const auto &arg: expr.arguments) {
            if (arg) { arg->accept(*this); } else { value_ = { }; }
            arguments.push_back(std::move(value_));
        }

        auto *fn = dynamic_cast<const Callable_Literal *>(&*callee);
        if (! fn) {
            throw Exception(expr.paren, "Can only call functions and classes.");
        }

        if (arguments.size() != fn->arity()) {
            throw Exception(expr.paren, "Expected " + std::to_string(fn->arity()) + " arguments, but got " + std::to_string(arguments.size()) + ".");
        }
        value_ = fn->call(*this, arguments);
    }

    void visit(const Function_Definition &definition) override {
        auto fn = std::make_shared<Function_Callable>(definition.shared());
        environment_->define(definition.name.lexeme, fn);
        value_ = {};
    }

    void visit (const Return_Statement &return_statement) override {
        Literal::Ptr value;
        if (return_statement.value) { return_statement.value->accept(*this); } else { value_ = {}; }
        value = std::move(value_);
        throw Return(value);
    }

    void interpret(const std::vector<Statement::Ptr> &statements) {
        environment_ = globals;
        try {
            for (const auto &statement : statements) {
                if (statement) { statement->accept(*this); }
            }
        } catch (const Exception &ex) {
            runtime_error(ex.token, ex.what());
        }
    }
};

inline void Interpreter::check_number_operand(const Token &token, const Literal::Ptr &right) {
    if (right && right->is_number()) { return; }
    throw Exception { token, "Operand must be a number." };
}

inline void Interpreter::check_number_operands(const Token &token, const Literal::Ptr &left, const Literal::Ptr &right) {
    if (left && left->is_number() && right && right->is_number()) { return; }
    throw Exception { token, "Operands must be numbers." };
}

inline Env_Handler::Env_Handler(Interpreter &i): interpreter_ { i }, old_ { interpreter_.environment_} {
    interpreter_.environment_ = std::make_shared<Environment>(interpreter_.environment_);
}
inline Env_Handler::Env_Handler(Interpreter &i, Environment::Ptr new_env): interpreter_ { i }, old_ { interpreter_.environment_ } {
    interpreter_.environment_ = std::move(new_env);
}
inline Env_Handler::~Env_Handler() { interpreter_.environment_ = old_; }
