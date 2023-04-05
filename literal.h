#pragma once

#include <memory>
#include <string>

#include "expr.h"

class Literal_Base {
    public:
        virtual explicit operator std::string() const = 0;
        [[nodiscard]] virtual std::unique_ptr<Literal_Base> copy() const = 0;
        virtual ~Literal_Base() = default;
};

class Nil_Literal: public Literal_Base {
    public:
        Nil_Literal() = default;
        [[nodiscard]] std::unique_ptr<Literal_Base> copy() const override { return std::make_unique<Nil_Literal>(); }
        explicit operator std::string() const override { return "nil"; }
};

class Bool_Literal: public Literal_Base {
    public:
        const bool value;
        explicit Bool_Literal(bool v): value { v } { }
        [[nodiscard]] std::unique_ptr<Literal_Base> copy() const override { return std::make_unique<Bool_Literal>(value); }
        explicit operator std::string() const override { return value ? "true" : "false"; }
};

class String_Literal: public Literal_Base {
    public:
        const std::string value;
        explicit String_Literal(std::string v): value { std::move(v) } { }
        [[nodiscard]] std::unique_ptr<Literal_Base> copy() const override { return std::make_unique<String_Literal>(value); }
        explicit operator std::string() const override { return value; }
};

class Number_Literal: public Literal_Base {
    public:
        const double value;
        explicit Number_Literal(double v): value { v } { }
        [[nodiscard]] std::unique_ptr<Literal_Base> copy() const override { return std::make_unique<Number_Literal>(value); }
        explicit operator std::string() const override { return std::to_string(value); }
};

class Literal: public Expr {
    public:
        const std::unique_ptr<Literal_Base> value;
        explicit Literal(std::unique_ptr<Literal_Base> &&v): value { std::move(v) } { }

        void accept(Expr_Visitor &visitor) const override { visitor.visit(*this); }

        static std::unique_ptr<Literal> create() { return std::make_unique<Literal>(std::make_unique<Nil_Literal>()); }
        static std::unique_ptr<Literal> create(bool value) { return std::make_unique<Literal>(std::make_unique<Bool_Literal>(value)); }
        static std::unique_ptr<Literal> create(std::string value) { return std::make_unique<Literal>(std::make_unique<String_Literal>(std::move(value))); }
        static std::unique_ptr<Literal> create(double value) { return std::make_unique<Literal>(std::make_unique<Number_Literal>(value)); }

};