#pragma once

#include <stdexcept>
#include <utility>
#include <vector>

#include "binary.h"
#include "err.h"
#include "grouping.h"
#include "token.h"
#include "unary.h"

class Parser {
        class Exception: public std::domain_error {
            public:
                Exception(): std::domain_error { "parse exception" } { }
        };

        const std::vector<Token> tokens_;
        int current_ = 0;

        std::unique_ptr<Expr> expression() {
            return equality();
        }

        [[nodiscard]] static bool match() {
            return false;
        }

        [[nodiscard]] const Token &peek() const { return tokens_[current_]; }

        [[nodiscard]] const Token &previous() const { return tokens_[current_ - 1]; }

        [[nodiscard]] bool is_at_end() const { return peek().type == Token_Type::END_OF_DATA; }

        [[nodiscard]] bool check(const Token_Type &type) const {
            if (is_at_end()) { return false; }
            return peek().type == type;
        }

        Token advance() {
            if (! is_at_end()) { ++current_; }
            return previous();
        }

        template<typename... Ts> bool match(const Token_Type &type, Ts... other) {
            if (check(type)) {
                advance();
                return true;
            }
            return match(other...);
        }

        std::unique_ptr<Expr> equality() {
            std::unique_ptr<Expr> expr = comparison();
            while (match(Token_Type::BANG_EQUAL, Token_Type::EQUAL_EQUAL)) {
                Token op = previous();
                std::unique_ptr<Expr> right = comparison();
                expr = std::make_unique<Binary>(op, std::move(expr), std::move(right));
            }
            return expr;
        }

        std::unique_ptr<Expr> comparison() {
            std::unique_ptr<Expr> expr = term();
            while (match(Token_Type::GREATER, Token_Type::GREATER_EQUAL, Token_Type::LESS, Token_Type::LESS_EQUAL)) {
                Token op = previous();
                std::unique_ptr<Expr> right = term();
                expr = std::make_unique<Binary>(op, std::move(expr), std::move(right));
            }
            return expr;
        }

        std::unique_ptr<Expr> term() {
            std::unique_ptr<Expr> expr = factor();
            while (match(Token_Type::PLUS, Token_Type::MINUS)) {
                Token op = previous();
                std::unique_ptr<Expr> right = factor();
                expr = std::make_unique<Binary>(op, std::move(expr), std::move(right));
            }
            return expr;
        }

        std::unique_ptr<Expr> factor() {
            std::unique_ptr<Expr> expr = unary();
            while (match(Token_Type::STAR, Token_Type::SLASH)) {
                Token op = previous();
                std::unique_ptr<Expr> right = unary();
                expr = std::make_unique<Binary>(op, std::move(expr), std::move(right));
            }
            return expr;
        }

        std::unique_ptr<Expr> unary() {
            if (match(Token_Type::BANG, Token_Type::MINUS)) {
                Token op = previous();
                std::unique_ptr<Expr> right = unary();
                return std::make_unique<Unary>(op, std::move(right));
            }
            return primary();
        }


        static Exception error(const Token &token, std::string message) {
            ::error(token, std::move(message));
            return {};
        }

        Token consume(const Token_Type &expected, std::string message) {
            if (check(expected)) { return advance(); }
            throw error(peek(), std::move(message));
        }

        std::unique_ptr<Expr> primary() {
            if (match(Token_Type::FALSE)) { return Literal::create(false); }
            if (match(Token_Type::TRUE)) { return Literal::create(true); }
            if (match(Token_Type::NIL)) { return Literal::create(); }
            if (match(Token_Type::NUMBER)) {
                return Literal::create(dynamic_cast<const Number_Literal &>(*previous().literal).value);
            }
            if (match(Token_Type::STRING)) {
                return Literal::create(dynamic_cast<const String_Literal &>(*previous().literal).value);
            }

            if (match(Token_Type::LEFT_PAREN)) {
                std::unique_ptr<Expr> expr = expression();
                consume(Token_Type::RIGHT_PAREN, "Expect ')' after expression.");
                return std::make_unique<Grouping>(std::move(expr));
            }

            throw error(peek(), "Expect expression.");
        }

        void synchronize() {
            advance();
            while (! is_at_end()) {
                if (previous().type == Token_Type::SEMICOLON) { return; }
                switch (peek().type) {
                    case Token_Type::CLASS:
                    case Token_Type::FUN:
                    case Token_Type::VAR:
                    case Token_Type::FOR:
                    case Token_Type::IF:
                    case Token_Type::WHILE:
                    case Token_Type::PRINT:
                    case Token_Type::RETURN:
                        return;
                    default: break;
                }
                advance();
            }
        }

    public:
        explicit Parser(const std::vector<Token> &tokens): tokens_ { tokens } { }

        std::unique_ptr<Expr> parse() {
            try {
                return expression();
            }
            catch (const Exception &ex) {
                return {};
            }
        }
};
