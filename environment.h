#pragma once

#include <map>
#include <string>
#include <memory>
#include <utility>

#include "literal.h"
#include "token.h"

class Environment {
    public:
        using Ptr = std::shared_ptr<Environment>;
        Ptr enclosing_;
        std::map<std::string, std::unique_ptr<Literal>> values_;
    public:
        class Exception : public std::runtime_error {
            public:
                const Token token;

                Exception(const Token &t, const std::string &msg): std::runtime_error { msg }, token { t } { }
        };

        Environment() = default;
        explicit Environment(Ptr enc): enclosing_ { std::move(enc) } { }

        Ptr &enclosing() { return enclosing_; }

        void define(const std::string &name, std::unique_ptr<Literal> &&value) {
            values_[name] = std::move(value);
        }

        [[nodiscard]] const Literal_p &get(const Token &name) const {
            const auto &got { values_.find(name.lexeme) };
            if (got != values_.end()) { return got->second; }
            if (enclosing_) { return enclosing_->get(name); }
            throw Exception { name, "Undefined variable '" + name.lexeme + "'." };
        }

        void assign(const Token &name, std::unique_ptr<Literal> &&value) {
            auto got { values_.find(name.lexeme) };
            if (got != values_.end()) {
                got->second = std::move(value);
            } else if (enclosing_) {
                enclosing_->assign(name, std::move(value));
            } else {
                throw Exception{name, "Undefined variable '" + name.lexeme + "'."};
            }
        }
};