#pragma once

#include <string>
#include <vector>
#include <map>

#include "token.h"

class Scanner {
        static std::map<std::string, Token_Type> keywords_;

        const std::string source_;
        std::vector<Token> tokens_;
        int start_ = 0;
        int current_ = 0;
        int line_ = 1;

        char advance() { return source_[current_++]; }
        bool match(char ch);
        [[nodiscard]] char peek() const;
        [[nodiscard]] char peek_next() const;
        [[nodiscard]] static bool is_digit(char ch);
        [[nodiscard]] static bool is_alpha(char ch);
        [[nodiscard]] static bool is_alnum(char ch);

        void add_token(Token_Type type);

        void parse_string();
        void parse_number();
        void parse_identifier();
    public:
        explicit Scanner(std::string source): source_ { std::move(source) } { }

        [[nodiscard]] bool is_at_end() const { return current_ >= source_.length(); }
        const std::vector<Token> &scan_tokens();
        void scan_token();
};
