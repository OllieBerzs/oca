#include <iostream>
#include "oca.hpp"

int main(int argc, char** argv)
{
    oca::State oca;

    if (argc > 1)
    {
        oca.script(argv[1]);
    }
    else
    {
        // start a REPL
        system("printf ''");
        bool running = true;
        while (running)
        {
            // prompt
            std::cout << "\033[38;5;15m" << ":: " << "\033[0m";

            // read
            std::string input;
            while (true)
            {
                std::string line;
                std::getline(std::cin, line);

                if (line.back() == '`')
                {
                    line.pop_back();
                    input += line + '\n';
                    std::cout << "\033[38;5;15m" << "-: " << "\033[0m";
                }
                else
                {
                    input += line + '\n';
                    break;
                }
            }

            // eval
            auto val = oca.eval(input);

            // print
            std::cout << "\033[38;5;8m" << val->tos(false) << "\033[0m\n";
        }
    }

    return 0;
}
