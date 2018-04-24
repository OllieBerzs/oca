#include <iostream>
#include "oca.hpp"
#include "utils.hpp"

int main(int argc, char** argv) {
    oca::State oca;

    if (argc > 1) {
        oca.script(argv[1]);
    } else {
        oca::enableANSI();

        // start a REPL
        while (true) {
            // prompt
            std::cout << ESC "38;5;15m"
                      << ":: " << ESC "0m";

            // read
            std::string input;
            while (true) {
                std::string line;
                std::getline(std::cin, line);

                if (line.back() == '`') {
                    line.pop_back();
                    input += line + '\n';
                    std::cout << ESC "38;5;15m"
                              << "-: " << ESC "0m";
                } else {
                    input += line + '\n';
                    break;
                }
            }

            if (input == "exit\n")
                return 0;

            // eval
            auto val = oca.eval(input);

            // print
            std::cout << ESC "38;5;8m" << val->tos(false) << ESC "0m\n";
        }
    }

    return 0;
}
