#pragma once

#include <stdexcept>
#include <utility>
#include <vector>

#include "assign_expression.h"
#include "binary_expression.h"
#include "block_statement.h"
#include "call_expression.h"
#include "err.h"
#include "function_definition.h"
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

        Expression::Ptr expression() {
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

        Expression::Ptr and_expression() {
            Expression::Ptr expr = equality();
            while (match(Token_Type::AND)) {
                Token op = previous();
                Expression::Ptr right = equality();
                expr = std::make_shared<Logical_Expression>(op, std::move(expr), std::move(right));
            }
            return expr;
        }

        Expression::Ptr or_expression() {
            Expression::Ptr expr = and_expression();
            while (match(Token_Type::OR)) {
                Token op = previous();
                Expression::Ptr right = and_expression();
                expr = std::make_shared<Logical_Expression>(op, std::move(expr), std::move(right));
            }
            return expr;
        }

        Expression::Ptr assignment() {
            Expression::Ptr expr = or_expression();

            if (match(Token_Type::EQUAL)) {
                Token equals = previous();
                Expression::Ptr value = assignment();
                auto v { dynamic_cast<const Var_Expression *>(expr.get()) };
                if (v) {
                    return std::make_shared<Assign_Expression>(v->name, std::move(value));
                }
                error(equals, "Invalid assignment target.");
            }

            return expr;
        }

        Expression::Ptr equality() {
            Expression::Ptr expr = comparison();
            while (match(Token_Type::BANG_EQUAL, Token_Type::EQUAL_EQUAL)) {
                Token op = previous();
                Expression::Ptr right = comparison();
                expr = std::make_shared<Binary_Expression>(op, std::move(expr), std::move(right));
            }
            return expr;
        }

        Expression::Ptr comparison() {
            Expression::Ptr expr = term();
            while (match(Token_Type::GREATER, Token_Type::GREATER_EQUAL, Token_Type::LESS, Token_Type::LESS_EQUAL)) {
                Token op = previous();
                Expression::Ptr right = term();
                expr = std::make_shared<Binary_Expression>(op, std::move(expr), std::move(right));
            }
            return expr;
        }

        Expression::Ptr term() {
            Expression::Ptr expr = factor();
            while (match(Token_Type::PLUS, Token_Type::MINUS)) {
                Token op = previous();
                Expression::Ptr right = factor();
                expr = std::make_shared<Binary_Expression>(op, std::move(expr), std::move(right));
            }
            return expr;
        }

        Expression::Ptr factor() {
            Expression::Ptr expr = unary();
            while (match(Token_Type::STAR, Token_Type::SLASH)) {
                Token op = previous();
                Expression::Ptr right = unary();
                expr = std::make_shared<Binary_Expression>(op, std::move(expr), std::move(right));
            }
            return expr;
        }

        Expression::Ptr finish_call(Expression::Ptr callee) {
            std::vector<Expression::Ptr> arguments;
            if (! check(Token_Type::RIGHT_PAREN)) {
                do {
                    if (arguments.size() >= 255) {
                        error(peek(), "Can't have more than 255 arguments.");
                    }
                    arguments.push_back(expression());
                } while (match(Token_Type::COMMA));
            }
            Token paren = consume(Token_Type::RIGHT_PAREN, "Expect ')' after arguments.");
            return std::make_shared<Call_Expression>(std::move(callee), paren, std::move(arguments));
        }

        Expression::Ptr call() {
            Expression::Ptr expr = primary();
            for (;;) {
                if (match(Token_Type::LEFT_PAREN)) {
                    expr = finish_call(std::move(expr));
                } else {
                    break;
                }
            }
            return expr;
        }

        Expression::Ptr unary() {
            if (match(Token_Type::BANG, Token_Type::MINUS)) {
                Token op = previous();
                Expression::Ptr right = unary();
                return std::make_shared<Unary>(op, std::move(right));
            }
            return call();
        }


        static Exception error(const Token &token, const std::string& message) {
            ::error(token, message);
            return {};
        }

        Token consume(const Token_Type &expected, const std::string& message) {
            if (check(expected)) { return advance(); }
            throw error(peek(), message);
        }

        Expression::Ptr primary() {
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
                return std::make_shared<Var_Expression>(previous());
            }
            if (match(Token_Type::LEFT_PAREN)) {
                Expression::Ptr expr = expression();
                consume(Token_Type::RIGHT_PAREN, "Expect ')' after expression.");
                return std::make_shared<Grouping>(std::move(expr));
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

        Statement::Ptr if_statement() {
            consume(Token_Type::LEFT_PAREN, "Expect '(' after 'if'.");
            Expression::Ptr condition {expression() };
            consume(Token_Type::RIGHT_PAREN, "Expect ')' after if condition.");
            Statement::Ptr then_branch { statement() };
            Statement::Ptr else_branch;
            if (match(Token_Type::ELSE)) {
                else_branch = statement();
            }
            return std::make_shared<If_Statement>(std::move(condition), std::move(then_branch), std::move(else_branch));
        }

        Statement::Ptr print_statement() {
            Expression::Ptr expr {expression() };
            consume(Token_Type::SEMICOLON, "Expect ';' after value.");
            return std::make_shared<Print_Statement>(std::move(expr));
        }

        Statement::Ptr while_statement() {
            consume(Token_Type::LEFT_PAREN, "Expect '(' after 'while'.");
            Expression::Ptr condition {expression() };
            consume(Token_Type::RIGHT_PAREN, "Expect ')' after while condition.");
            Statement::Ptr body { statement() };
            return std::make_shared<While_Statement>(std::move(condition), std::move(body));
        }

        Block_Statement::Ptr block_statement() {
            std::vector<Statement::Ptr> statements;
            while (! check(Token_Type::RIGHT_BRACE) && ! is_at_end()) {
                statements.push_back(declaration());
            }
            consume(Token_Type::RIGHT_BRACE, "Expect '}' after block.");
            return std::make_shared<Block_Statement>(std::move(statements));
        }

        Statement::Ptr expression_statement() {
            Expression::Ptr expr { expression() };
            consume(Token_Type::SEMICOLON, "Expect ';' after expression.");
            return std::make_shared<Expression_Statement>(std::move(expr));
        }

        Statement::Ptr for_statement() {
            consume(Token_Type::LEFT_PAREN, "Expect '(' after 'for'.");
            Statement::Ptr initializer;
            if (match(Token_Type::SEMICOLON)) {

            } else if (match(Token_Type::VAR)) {
                initializer = var_declaration();
            } else {
                initializer = expression_statement();
            }
            Expression::Ptr condition;
            if (! check(Token_Type::SEMICOLON)) {
                condition = expression();
            }
            consume(Token_Type::SEMICOLON, "Expect ';' after loop condition.");
            Expression::Ptr increment;
            if (! check(Token_Type::LEFT_PAREN)) {
                increment = expression();
            }
            consume(Token_Type::RIGHT_PAREN, "Expect ')' after for clauses.");
            Statement::Ptr body = statement();
            if (increment) {
                std::vector<Statement::Ptr> statements;
                statements.push_back(std::move(body));
                statements.push_back(std::make_shared<Expression_Statement>(std::move(increment)));
                body = std::make_shared<Block_Statement>(std::move(statements));
            }
            if (! condition) { condition = Literal::create(true); }
            body = std::make_shared<While_Statement>(std::move(condition), std::move(body));

            if (initializer) {
                std::vector<Statement::Ptr> statements;
                statements.push_back(std::move(initializer));
                statements.push_back(std::move(body));
                body = std::make_shared<Block_Statement>(std::move(statements));
            }

            return body;
        }

        Statement::Ptr statement() {
            if (match(Token_Type::FOR)) { return for_statement(); }
            if (match(Token_Type::IF)) { return if_statement(); }
            if (match(Token_Type::PRINT)) { return print_statement(); }
            if (match(Token_Type::WHILE)) { return while_statement(); }
            if (match(Token_Type::LEFT_BRACE)) { return block_statement(); }
            return expression_statement();
        }

        Statement::Ptr var_declaration() {
            Token name = consume(Token_Type::IDENTIFIER, "Expect variable name.");
            Expression::Ptr initializer;
            if (match(Token_Type::EQUAL)) {
                initializer = expression();
            }
            consume(Token_Type::SEMICOLON, "Expect ';' after variable declaration.");
            return std::make_shared<Var_Statement>(name, std::move(initializer));
        }

        Statement::Ptr function_definition(const std::string &kind) {
            Token name = consume(Token_Type::IDENTIFIER, "Expect " + kind + " name.");
            consume(Token_Type::LEFT_PAREN, "Expect '(' after " + kind + " name.");
            std::vector<Token> params;
            if (!check(Token_Type::RIGHT_PAREN)) {
                do {
                    if (params.size() >= 255) {
                        error(peek(), "Can't have more than 255 parameters.");
                    }
                    params.push_back(consume(Token_Type::IDENTIFIER, "Expect parameter name."));
                } while (match(Token_Type::COMMA));
            }
            consume(Token_Type::RIGHT_PAREN, "Expect ')' after parameters.");
            consume(Token_Type::LEFT_BRACE, "Expect '{' before " + kind + " body.");
            Block_Statement::Ptr body = block_statement();
            return std::make_shared<Function_Definition>(name, std::move(params), std::move(body));
        }

        Statement::Ptr declaration() {
            try {
                if (match(Token_Type::FUN)) {
                    return function_definition("function");
                }
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

        std::vector<Statement::Ptr> parse() {
            std::vector<Statement::Ptr> statements;
            while (! is_at_end()) {
                statements.push_back(declaration());
            }
            return statements;
        }
};
