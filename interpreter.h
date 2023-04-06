#pragma once

#include <utility>

#include "binary.h"
#include "expr.h"
#include "grouping.h"
#include "literal.h"
#include "token.h"
#include "unary.h"

class Interpreter: public Expr_Visitor {
    Literal_p value_;

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

    void interpret(const Expr &expr) {
        try {
            expr.accept(*this);
            std::cout << Literal::to_string(value_) << "\n";
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
