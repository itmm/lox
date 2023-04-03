#include "scanner.h"

#include "err.h"

std::map<std::string, Token_Type> Scanner::keywords_ = {
        { "and", Token_Type::AND },
        { "class", Token_Type::CLASS },
        { "else", Token_Type::ELSE },
        { "false", Token_Type::FALSE },
        { "for", Token_Type::FOR },
        { "fun", Token_Type::FUN },
        { "if", Token_Type::IF },
        { "nil", Token_Type::NIL },
        { "or", Token_Type::OR },
        { "print", Token_Type::PRINT },
        { "return", Token_Type::RETURN },
        { "super", Token_Type::SUPER },
        { "this", Token_Type::THIS },
        { "true", Token_Type::TRUE },
        { "var", Token_Type::VAR },
        { "while", Token_Type::WHILE }
};

bool Scanner::match(char ch) {
    if (is_at_end()) { return false; }
    if (source_[current_] != ch) { return false; }
    ++current_;
    return true;
}

void Scanner::add_token(Token_Type type) {
    std::string text { source_.substr(start_, current_ - start_) };
    tokens_.emplace_back(type, text, line_);
}

char Scanner::peek() const {
    if (is_at_end()) { return '\0'; }
    return source_[current_];
}

char Scanner::peek_next() const {
    if (current_ + 1 >= source_.length()) { return '\0'; }
    return source_[current_ + 1];
}

bool Scanner::is_digit(char ch) {
    return ch >= '0' && ch <= '9';
}

bool Scanner::is_alpha(char ch) {
    return (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || (ch == '_');
}

bool Scanner::is_alnum(char ch) {
    return is_alpha(ch) || is_digit(ch);
}

void Scanner::parse_string() {
    while (peek() != '"' && !is_at_end()) {
        if (peek() == '\n') { ++line_; }
        advance();
    }
    if (is_at_end()) {
        error(line_, "Unterminated string.");
        return;
    }
    advance();
    std::string value = source_.substr(start_ + 1, current_ - start_ - 2);
    std::string text { source_.substr(start_, current_ - start_) };
    tokens_.emplace_back(Token_Type::STRING, text, value, line_);
}

void Scanner::parse_number() {
    while (is_digit(peek())) { advance(); }
    if (peek() == '.' && is_digit(peek_next())) {
        advance();
        while (is_digit(peek())) { advance(); }
    }

    double value = std::stod(source_.substr(start_, current_ - start_));
    std::string text { source_.substr(start_, current_ - start_) };
    tokens_.emplace_back(Token_Type::NUMBER, text, value, line_);
}

void Scanner::parse_identifier() {
    while (is_alnum(peek())) { advance(); }

    std::string text { source_.substr(start_, current_ - start_) };
    auto got { Scanner::keywords_.find(text) };
    if (got != Scanner::keywords_.end()) {
        add_token(got->second);
    } else {
        add_token(Token_Type::IDENTIFIER);
    }
}

void Scanner::scan_token() {
    char c = advance();
    switch (c) {
        case '(': add_token(Token_Type::LEFT_PAREN); break;
        case ')': add_token(Token_Type::RIGHT_PAREN); break;
        case '{': add_token(Token_Type::LEFT_BRACE); break;
        case '}': add_token(Token_Type::RIGHT_BRACE); break;
        case ',': add_token(Token_Type::COMMA); break;
        case '.': add_token(Token_Type::DOT); break;
        case '-': add_token(Token_Type::MINUS); break;
        case '+': add_token(Token_Type::PLUS); break;
        case ';': add_token(Token_Type::SEMICOLON); break;
        case '*': add_token(Token_Type::STAR); break;
        case '!': add_token(match('=') ? Token_Type::BANG_EQUAL : Token_Type::BANG); break;
        case '=': add_token(match('=') ? Token_Type::EQUAL_EQUAL : Token_Type::EQUAL); break;
        case '<': add_token(match('=') ? Token_Type::LESS_EQUAL : Token_Type::LESS); break;
        case '>': add_token(match('=') ? Token_Type::GREATER_EQUAL : Token_Type::GREATER); break;
        case '/': {
            if (match('/')) {
                while (peek() != '\n' && !is_at_end()) { advance(); }
            } else { add_token(Token_Type::SLASH); }
            break;
        }
        case ' ': case '\r': case '\t': break;
        case '\n': ++line_; break;
        case '"': parse_string(); break;
        default:
            if (is_digit(c)) {
                parse_number();
            } else if (is_alpha(c)) {
                parse_identifier();
            } else {
                error(line_, "Unexpected character."); }
            break;
    }
}

const std::vector<Token> &Scanner::scan_tokens() {
    while (! is_at_end()) {
        start_ = current_;
        scan_token();
    }
    tokens_.emplace_back(Token_Type::END_OF_DATA, "", line_);
    return tokens_;
}
