#pragma once

#include <stdexcept>
#include <utility>
#include <vector>

#include "assign_expression.h"
#include "binary.h"
#include "block_statement.h"
#include "err.h"
#include "grouping.h"
#include "if_statement.h"
#include "logical_expression.h"
#include "statement.h"
#include "token.h"
#include "unary.h"
#include "print_statement.h"
#include "expression_statement.h"
#include "var_expression.h"
#include "var_statement.h"
#include "while_statement.h"

class Parser {
        class Exception: public std::domain_error {
            public:
                Exception(): std::domain_error { "parse exception" } { }
        };

        const std::vector<Token> tokens_;
        int current_ = 0;

        std::unique_ptr<Expr> expression() {
            return assignment();
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

        std::unique_ptr<Expr> and_expression() {
            std::unique_ptr<Expr> expr = equality();
            while (match(Token_Type::AND)) {
                Token op = previous();
                std::unique_ptr<Expr> right = equality();
                expr = std::make_unique<Logical_Expression>(op, std::move(expr), std::move(right));
            }
            return expr;
        }

        std::unique_ptr<Expr> or_expression() {
            std::unique_ptr<Expr> expr = and_expression();
            while (match(Token_Type::OR)) {
                Token op = previous();
                std::unique_ptr<Expr> right = and_expression();
                expr = std::make_unique<Logical_Expression>(op, std::move(expr), std::move(right));
            }
            return expr;
        }

        std::unique_ptr<Expr> assignment() {
            std::unique_ptr<Expr> expr = or_expression();

            if (match(Token_Type::EQUAL)) {
                Token equals = previous();
                std::unique_ptr<Expr> value = assignment();
                auto v { dynamic_cast<Var_Expression *>(expr.get()) };
                if (v) {
                    return std::make_unique<Assign_Expression>(v->name, std::move(value));
                }
                error(equals, "Invalid assignment target.");
            }

            return expr;
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


        static Exception error(const Token &token, const std::string& message) {
            ::error(token, message);
            return {};
        }

        Token consume(const Token_Type &expected, const std::string& message) {
            if (check(expected)) { return advance(); }
            throw error(peek(), message);
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
            if (match(Token_Type::IDENTIFIER)) {
                return std::make_unique<Var_Expression>(previous());
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

        std::unique_ptr<Statement> if_statement() {
            consume(Token_Type::LEFT_PAREN, "Expect '(' after 'if'.");
            std::unique_ptr<Expr> condition { expression() };
            consume(Token_Type::RIGHT_PAREN, "Expect ')' after if condition.");
            std::unique_ptr<Statement> then_branch { statement() };
            std::unique_ptr<Statement> else_branch;
            if (match(Token_Type::ELSE)) {
                else_branch = statement();
            }
            return std::make_unique<If_Statement>(std::move(condition), std::move(then_branch), std::move(else_branch));
        }

        std::unique_ptr<Statement> print_statement() {
            std::unique_ptr<Expr> expr { expression() };
            consume(Token_Type::SEMICOLON, "Expect ';' after value.");
            return std::make_unique<Print_Statement>(std::move(expr));
        }

        std::unique_ptr<Statement> while_statement() {
            consume(Token_Type::LEFT_PAREN, "Expect '(' after 'while'.");
            std::unique_ptr<Expr> condition { expression() };
            consume(Token_Type::RIGHT_PAREN, "Expect ')' after while condition.");
            std::unique_ptr<Statement> body { statement() };
            return std::make_unique<While_Statement>(std::move(condition), std::move(body));
        }

        std::unique_ptr<Statement> block_statement() {
            std::vector<std::unique_ptr<Statement>> statements;
            while (! check(Token_Type::RIGHT_BRACE) && ! is_at_end()) {
                statements.push_back(declaration());
            }
            consume(Token_Type::RIGHT_BRACE, "Expect '}' after block.");
            return std::make_unique<Block_Statement>(std::move(statements));
        }

        std::unique_ptr<Statement> expression_statement() {
            std::unique_ptr<Expr> expr { expression() };
            consume(Token_Type::SEMICOLON, "Expect ';' after expression.");
            return std::make_unique<Expression_Statement>(std::move(expr));
        }

        std::unique_ptr<Statement> for_statement() {
            consume(Token_Type::LEFT_PAREN, "Expect '(' after 'for'.");
            std::unique_ptr<Statement> initializer;
            if (match(Token_Type::SEMICOLON)) {

            } else if (match(Token_Type::VAR)) {
                initializer = var_declaration();
            } else {
                initializer = expression_statement();
            }
            std::unique_ptr<Expr> condition;
            if (! check(Token_Type::SEMICOLON)) {
                condition = expression();
            }
            consume(Token_Type::SEMICOLON, "Expect ';' after loop condition.");
            std::unique_ptr<Expr> increment;
            if (! check(Token_Type::LEFT_PAREN)) {
                increment = expression();
            }
            consume(Token_Type::RIGHT_PAREN, "Expect ')' after for clauses.");
            std::unique_ptr<Statement> body = statement();
            if (increment) {
                std::vector<std::unique_ptr<Statement>> statements;
                statements.push_back(std::move(body));
                statements.push_back(std::make_unique<Expression_Statement>(std::move(increment)));
                body = std::make_unique<Block_Statement>(std::move(statements));
            }
            if (! condition) { condition = Literal::create(true); }
            body = std::make_unique<While_Statement>(std::move(condition), std::move(body));

            if (initializer) {
                std::vector<std::unique_ptr<Statement>> statements;
                statements.push_back(std::move(initializer));
                statements.push_back(std::move(body));
                body = std::make_unique<Block_Statement>(std::move(statements));
            }

            return body;
        }

        std::unique_ptr<Statement> statement() {
            if (match(Token_Type::FOR)) { return for_statement(); }
            if (match(Token_Type::IF)) { return if_statement(); }
            if (match(Token_Type::PRINT)) { return print_statement(); }
            if (match(Token_Type::WHILE)) { return while_statement(); }
            if (match(Token_Type::LEFT_BRACE)) { return block_statement(); }
            return expression_statement();
        }

        std::unique_ptr<Statement> var_declaration() {
            Token name = consume(Token_Type::IDENTIFIER, "Expect variable name.");
            std::unique_ptr<Expr> initializer;
            if (match(Token_Type::EQUAL)) {
                initializer = expression();
            }
            consume(Token_Type::SEMICOLON, "Expect ';' after variable declaration.");
            return std::make_unique<Var_Statement>(name, std::move(initializer));
        }

        std::unique_ptr<Statement> declaration() {
            try {
                if (match(Token_Type::VAR)) {
                    return var_declaration();
                }
                return statement();
            }
            catch (const Exception &ex) {
                synchronize();
            }
            return { };
        }

    public:
        explicit Parser(const std::vector<Token> &tokens): tokens_ { tokens } { }

        std::vector<std::unique_ptr<Statement>> parse() {
            std::vector<std::unique_ptr<Statement>> statements;
            while (! is_at_end()) {
                statements.push_back(declaration());
            }
            return statements;
        }
};
