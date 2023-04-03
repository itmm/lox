#pragma once

#include <string>
#include <memory>
#include <utility>

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

class Literal {
    public:
        virtual explicit operator std::string() const = 0;
        [[nodiscard]] virtual std::unique_ptr<Literal> copy() const = 0;
        virtual ~Literal() = default;
};

class Bool_Literal: public Literal {
        const bool value_;
    public:
        explicit Bool_Literal(bool value): value_ { value } { }
        [[nodiscard]] std::unique_ptr<Literal> copy() const override { return std::make_unique<Bool_Literal>(value_); }
        explicit operator std::string() const override { return value_ ? "true" : "false"; }
};

class String_Literal: public Literal {
        const std::string value_;
    public:
        explicit String_Literal(std::string value): value_ { std::move(value) } { }
        [[nodiscard]] std::unique_ptr<Literal> copy() const override { return std::make_unique<String_Literal>(value_); }
        explicit operator std::string() const override { return value_; }
};

class Number_Literal: public Literal {
        const double value_;
    public:
        explicit Number_Literal(double value): value_ { value } { }
        [[nodiscard]] std::unique_ptr<Literal> copy() const override { return std::make_unique<Number_Literal>(value_); }
        explicit operator std::string() const override { return std::to_string(value_); }
};

class Token {
        const Token_Type type_;
        const std::string lexeme_;
        const std::unique_ptr<Literal> literal_;
        const int line_ = 1;

        static std::unique_ptr<Literal> duplicate_literal(const std::unique_ptr<Literal> &literal) {
            if (! literal) { return {}; }
            return std::move(literal->copy());
        }

    public:
        Token(Token_Type type, std::string lexeme, int line):
            type_ { type }, lexeme_ { std::move(lexeme) }, literal_ { }, line_ { line }
        { }

        Token(Token_Type type, std::string lexeme, bool literal, int line):
            type_ { type }, lexeme_ { std::move(lexeme) },
            literal_ { std::make_unique<Bool_Literal>(literal) }, line_ { line }
        { }

        Token(Token_Type type, std::string lexeme, std::string literal, int line):
            type_ { type }, lexeme_ { std::move(lexeme) },
            literal_ { std::make_unique<String_Literal>(std::move(literal)) }, line_ { line }
        { }

        Token(Token_Type type, std::string lexeme, double literal, int line):
            type_ { type }, lexeme_ { std::move(lexeme) },
            literal_ { std::make_unique<Number_Literal>(literal) }, line_ { line }
        { }

        Token(const Token &token):
            type_ { token.type_ }, lexeme_ { token.lexeme_ },
            literal_ { duplicate_literal(token.literal_) }, line_ { token.line_ }
        { }

        explicit operator std::string() const {
            return std::to_string(static_cast<int>(type_)) + " " + lexeme_ + " " +
                    (literal_ ? static_cast<std::string>(*literal_) : "nil");
        }
};
