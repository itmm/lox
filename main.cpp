#include <fstream>
#include <iostream>
#include <utility>

#include "err.h"
#include "scanner.h"
#include "print_visitor.h"

void run(std::string source) {
    Scanner scanner { std::move(source) };
    auto tokens { scanner.scan_tokens() };
    for (const auto &t: tokens) {
        std::cout << static_cast<std::string>(t) << "\n";
    }
}

void run_file(const char *path) {
    std::ifstream in(path);
    std::string source {
        std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>()
    };
    run(source);
    if (had_error) { exit(EXIT_FAILURE); }
}

void run_prompt() {
    std::cout << "> ";
    std::string line;
    while (std::getline(std::cin, line)) {
        run(line);
        std::cout << "> ";
        had_error = false;
    }
}

int main(int argc, const char *argv[]) {
    Binary expression {
        Token(Token_Type::STAR, "*", 1),
        std::make_unique<Unary>(
            Token(Token_Type::MINUS, "-", 1),
            std::make_unique<Literal>(std::make_unique<Number_Literal>(123))
        ),
        std::make_unique<Grouping>(
            std::make_unique<Literal>(std::make_unique<Number_Literal>(45.67))
        )
    };
    Print_Visitor visitor(std::cout, expression); std::cout << "\n";

    switch (argc) {
        case 2:
            run_file(argv[1]);
            break;
        case 1:
            run_prompt();
            break;
        default:
            std::cerr << "Usage: " << argv[0] << " [script]\n";
            exit(EXIT_FAILURE);
    }
    return EXIT_SUCCESS;
}
