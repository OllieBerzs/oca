#include <iostream>
#include "oca.hpp"

#if _WIN32 || _WIN64
#include <windows.h>
#endif

#define ESC "\033["

int main(int argc, char** argv)
{
    oca::State oca;

    if (argc > 1)
    {
        oca.script(argv[1]);
    }
    else
    {
        // set console mode for ansi
        #if _WIN32 || _WIN64
        HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
        DWORD dwMode = 0;
        GetConsoleMode(hOut, &dwMode);
        dwMode |= 0x0004;
        SetConsoleMode(hOut, dwMode);
        #endif

        // start a REPL
        while (true)
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
