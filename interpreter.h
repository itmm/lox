#pragma once

#include <utility>
#include <vector>

#include "assign_expression.h"
#include "binary.h"
#include "block_statement.h"
#include "environment.h"
#include "expr.h"
#include "expression_statement.h"
#include "grouping.h"
#include "literal.h"
#include "print_statement.h"
#include "statement.h"
#include "token.h"
#include "unary.h"
#include "var_statement.h"
#include "var_expression.h"

class Interpreter;

class Env_Handler {
        Interpreter &interpreter_;
    public:
        explicit Env_Handler(Interpreter &i);
        ~Env_Handler();
};

class Interpreter: public Expr_Visitor, public Statement_Visitor {
    Literal_p value_;
    std::unique_ptr<Environment> environment_;

    static bool is_truthy(const Literal_p &literal) {
        if (! literal) { return false; }
        if (literal->is_bool()) { return literal->as_bool(); }
        return true;
    }

    static bool is_equal(const Literal_p &a, const Literal_p &b) {
        if (! a) { return ! b; }
        if (a->is_bool()) { return b->is_bool() && a->as_bool() == b->as_bool(); }
        if (a->is_string()) { return b->is_string() && a->as_string() == b->as_string(); }
        if (a->is_number()) { return b->is_number() && a->as_number() == b->as_number(); }
        return false;
    }

    static void check_number_operand(const Token &token, const Literal_p &right);
    static void check_number_operands(const Token &token, const Literal_p &left, const Literal_p &right);

    friend class Env_Handler;

public:
    class Exception: public std::domain_error {
        public:
            const Token token;
            Exception(const Token &t, const std::string& m): std::domain_error { m }, token { t } { }
    };

    void visit(const Binary &binary) override {
        binary.left->accept(*this);
        Literal_p left = std::move(value_);
        binary.right->accept(*this);
        Literal_p  right = std::move(value_);

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
        grouping.accept(*this);
    }

    void visit(const Literal &literal) override {
        value_ = literal.copy();
    }

    void visit(const Unary &unary) override {
        unary.right->accept(*this);
        Literal_p right = std::move(value_);
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
        const auto &got { environment_->get(expression.name) };
        value_ = got? got->copy() : Literal_p { };
    }

    void visit(const Print_Statement &statement) override {
        if (statement.expression) { statement.expression->accept(*this); } else { value_ = {}; }
        std::cout << Literal::to_string(value_) << "\n";
    }

    void visit(const Expression_Statement &statement) override {
        if (statement.expression) { statement.expression->accept(*this); }
    }

    void visit(const Var_Statement &statement) override {
        Literal_p initializer;
        if (statement.initializer) {
            statement.initializer->accept(*this);
            initializer = std::move(value_);
        }
        environment_->define(statement.name.lexeme, std::move(initializer));
    }

    void visit(const Assign_Expression &statement) override {
        statement.value->accept(*this);
        Literal_p  value = value_ ? value_->copy() : Literal_p { };
        environment_->assign(statement.name, std::move(value));
    }

    void visit(const Block_Statement &statement) override {
        Env_Handler env_handler { *this };
        for (const auto &s : statement.statements) {
            if (s) { s->accept(*this); }
        }
    }

    void interpret(const std::vector<std::unique_ptr<Statement>> &statements) {
        environment_ = std::make_unique<Environment>();
        try {
            for (const auto &statement : statements) {
                if (statement) { statement->accept(*this); }
            }
        } catch (const Exception &ex) {
            runtime_error(ex.token, ex.what());
        }
    }
};

inline void Interpreter::check_number_operand(const Token &token, const Literal_p &right) {
    if (right && right->is_number()) { return; }
    throw Exception { token, "Operand must be a number." };
}

inline void Interpreter::check_number_operands(const Token &token, const Literal_p &left, const Literal_p &right) {
    if (left && left->is_number() && right && right->is_number()) { return; }
    throw Exception { token, "Operands must be numbers." };
}

inline Env_Handler::Env_Handler(Interpreter &i): interpreter_ { i } { interpreter_.environment_ = std::make_unique<Environment>(std::move(interpreter_.environment_)); }
inline Env_Handler::~Env_Handler() { interpreter_.environment_ = std::move(interpreter_.environment_->enclosing()); }
