#include <fstream>
#include <iostream>
#include <utility>

#include "err.h"
#include "parser.h"
#include "print_visitor.h"
#include "scanner.h"

void run(std::string source) {
    Scanner scanner { std::move(source) };
    auto tokens { scanner.scan_tokens() };
    Parser parser { tokens };
    auto expr { parser.parse() };
    if (had_error) { return; }
    Print_Visitor visitor { std::cout, *expr };
    std::cout << "\n";
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
