#pragma once

#include <memory>
#include <string>

#include "expression.h"

class Literal;


class Literal: public Expression, public std::enable_shared_from_this<Literal> {
    public:
        using Ptr = std::shared_ptr<const Literal>;
        virtual explicit operator std::string() const = 0;

        void accept(Expression_Visitor &visitor) const override { visitor.visit(*this); }

        [[nodiscard]] bool as_bool() const;
        [[nodiscard]] std::string as_string() const;
        [[nodiscard]] double as_number() const;

        [[nodiscard]] virtual bool is_bool() const { return false; }
        [[nodiscard]] virtual bool is_string() const { return false; }
        [[nodiscard]] virtual bool is_number() const { return false; }

        static Ptr create();
        static Ptr create(bool value);
        static Ptr create(std::string value);
        static Ptr create(double value);

        static std::string to_string(const Ptr &l) {
            return l ? static_cast<std::string>(*l) : "nil";
        }

        Ptr shared() const { return shared_from_this(); }
};

class Bool_Literal: public Literal {
    public:
        const bool value;
        [[nodiscard]] bool is_bool() const override { return true; }
        explicit Bool_Literal(bool v): value { v } { }
        explicit operator std::string() const override { return value ? "true" : "false"; }
};

class String_Literal: public Literal {
    public:
        const std::string value;
        [[nodiscard]] bool is_string() const override { return true; }
        explicit String_Literal(std::string v): value { std::move(v) } { }
        explicit operator std::string() const override { return value; }
};

class Number_Literal: public Literal {
    public:
        const double value;
        [[nodiscard]] bool is_number() const override { return true; }
        explicit Number_Literal(double v): value { v } { }
        explicit operator std::string() const override { return std::to_string(value); }
};

inline bool Literal::as_bool() const { return dynamic_cast<const Bool_Literal &>(*this).value; }
inline std::string Literal::as_string() const { return dynamic_cast<const String_Literal &>(*this).value; }
inline double Literal::as_number() const { return dynamic_cast<const Number_Literal &>(*this).value; }

inline Literal::Ptr Literal::create() { return {}; }
inline Literal::Ptr Literal::create(bool value) { return std::make_shared<Bool_Literal>(value); }
inline Literal::Ptr Literal::create(std::string value) { return std::make_shared<String_Literal>(std::move(value)); }
inline Literal::Ptr Literal::create(double value) { return std::make_shared<Number_Literal>(value); }
