#pragma once

#include <memory>
#include <string>

#include "expr.h"

class Literal;

using Literal_p = std::unique_ptr<Literal>;

class Literal: public Expr {
    public:
        virtual explicit operator std::string() const = 0;
        [[nodiscard]] virtual Literal_p copy() const = 0;

        void accept(Expr_Visitor &visitor) const override { visitor.visit(*this); }

        [[nodiscard]] bool as_bool() const;
        [[nodiscard]] std::string as_string() const;
        [[nodiscard]] double as_number() const;

        [[nodiscard]] virtual bool is_bool() const { return false; }
        [[nodiscard]] virtual bool is_string() const { return false; }
        [[nodiscard]] virtual bool is_number() const { return false; }

        static Literal_p create();
        static Literal_p create(bool value);
        static Literal_p create(std::string value);
        static Literal_p create(double value);

        static std::string to_string(const Literal_p &l) {
            return l ? static_cast<std::string>(*l) : "nil";
        }
};

class Bool_Literal: public Literal {
    public:
        const bool value;
        [[nodiscard]] bool is_bool() const override { return true; }
        explicit Bool_Literal(bool v): value { v } { }
        [[nodiscard]] Literal_p copy() const override { return std::make_unique<Bool_Literal>(value); }
        explicit operator std::string() const override { return value ? "true" : "false"; }
};

class String_Literal: public Literal {
    public:
        const std::string value;
        [[nodiscard]] bool is_string() const override { return true; }
        explicit String_Literal(std::string v): value { std::move(v) } { }
        [[nodiscard]] Literal_p copy() const override { return std::make_unique<String_Literal>(value); }
        explicit operator std::string() const override { return value; }
};

class Number_Literal: public Literal {
    public:
        const double value;
        [[nodiscard]] bool is_number() const override { return true; }
        explicit Number_Literal(double v): value { v } { }
        [[nodiscard]] Literal_p copy() const override { return std::make_unique<Number_Literal>(value); }
        explicit operator std::string() const override { return std::to_string(value); }
};

inline bool Literal::as_bool() const { return dynamic_cast<const Bool_Literal &>(*this).value; }
inline std::string Literal::as_string() const { return dynamic_cast<const String_Literal &>(*this).value; }
inline double Literal::as_number() const { return dynamic_cast<const Number_Literal &>(*this).value; }

inline Literal_p Literal::create() { return {}; }
inline Literal_p Literal::create(bool value) { return std::make_unique<Bool_Literal>(value); }
inline Literal_p Literal::create(std::string value) { return std::make_unique<String_Literal>(std::move(value)); }
inline Literal_p Literal::create(double value) { return std::make_unique<Number_Literal>(value); }
