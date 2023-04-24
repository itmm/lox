#pragma once

#include <string>
#include <memory>
#include <utility>

#include "literal.h"

enum class Token_Type {
    LEFT_PAREN, RIGHT_PAREN, LEFT_BRACE, RIGHT_BRACE,
    COMMA, DOT, MINUS, PLUS, SEMICOLON, SLASH, STAR,

    BANG, BANG_EQUAL, EQUAL, EQUAL_EQUAL,
    GREATER, GREATER_EQUAL, LESS, LESS_EQUAL,

    IDENTIFIER, STRING, NUMBER,

    AND, CLASS, ELSE, FALSE, FUN, FOR, IF, NIL, OR,
    PRINT, RETURN, SUPER, THIS, TRUE, VAR, WHILE,

    END_OF_DATA
};

inline std::string to_string(const Token_Type &type) {
    switch (type) {
        case Token_Type::LEFT_PAREN: return "(";
        case Token_Type::RIGHT_PAREN: return ")";
        case Token_Type::LEFT_BRACE: return "{";
        case Token_Type::RIGHT_BRACE: return "}";
        case Token_Type::COMMA: return ",";
        case Token_Type::DOT: return ".";
        case Token_Type::MINUS: return "-";
        case Token_Type::PLUS: return "+";
        case Token_Type::SEMICOLON: return ";";
        case Token_Type::SLASH: return "/";
        case Token_Type::STAR: return "*";
        case Token_Type::BANG: return "!";
        case Token_Type::BANG_EQUAL: return "!=";
        case Token_Type::EQUAL: return "=";
        case Token_Type::EQUAL_EQUAL: return "==";
        case Token_Type::GREATER: return ">";
        case Token_Type::GREATER_EQUAL: return ">=";
        case Token_Type::LESS: return "<";
        case Token_Type::LESS_EQUAL: return "<=";
        case Token_Type::IDENTIFIER: return "IDENTIFIER";
        case Token_Type::STRING: return "STRING";
        case Token_Type::NUMBER: return "NUMBER";
        case Token_Type::AND: return "and";
        case Token_Type::CLASS: return "class";
        case Token_Type::ELSE: return "else";
        case Token_Type::FALSE: return "false";
        case Token_Type::FUN: return "fun";
        case Token_Type::FOR: return "for";
        case Token_Type::IF: return "if";
        case Token_Type::NIL: return "nil";
        case Token_Type::OR: return "or";
        case Token_Type::PRINT: return "print";
        case Token_Type::RETURN: return "return";
        case Token_Type::SUPER: return "super";
        case Token_Type::THIS: return "this";
        case Token_Type::TRUE: return "true";
        case Token_Type::VAR: return "var";
        case Token_Type::WHILE: return "while";
        case Token_Type::END_OF_DATA: return "EOF";
        default: return "unknown token " + std::to_string(static_cast<int>(type));
    }
}

class Token {
    public:
        const Token_Type type;
        const std::string lexeme;
        const Literal::Ptr literal;
        const int line;

        Token(Token_Type type, std::string lexeme, int line):
            type { type }, lexeme { std::move(lexeme) }, literal { }, line { line }
        { }

        Token(Token_Type type, std::string lexeme, bool literal, int line):
            type { type }, lexeme { std::move(lexeme) },
            literal { Literal::create(literal) }, line { line }
        { }

        Token(Token_Type type, std::string lexeme, std::string literal, int line):
            type { type }, lexeme { std::move(lexeme) },
            literal { Literal::create(std::move(literal)) }, line { line }
        { }

        Token(Token_Type type, std::string lexeme, double literal, int line):
            type { type }, lexeme { std::move(lexeme) },
            literal { Literal::create(literal) }, line { line }
        { }

        Token(const Token &token) = default;

        explicit operator std::string() const {
            return to_string(type) + " " + lexeme + " " + Literal::to_string(literal);
        }
};
