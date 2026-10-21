#include "assembly.h"
#include "codegen.h"
#include "interpreter.h"
#include "lexer.h"
#include "parser.tab.h"
#include "semantic.h"
#include "tac.h"
#include <fstream>
#include <iostream>
#include <iterator>
#include <string>

static void usage() {
    std::cerr << "Lexify - a small C compiler / interpreter (declarations, expressions, if/else, loops, switch)\n"
                 "Usage: lexify <file.c> [option]\n"
                 "  (no option)  compile and run the program\n"
                 "  --tokens     print the token stream\n"
                 "  --ast        print the syntax tree\n"
                 "  --tac        print three-address code\n"
                 "  --asm        print pseudo assembly\n";
}

int main(int argc, char** argv) {
    if (argc < 2) {
        usage();
        return 1;
    }
    std::string opt = argc > 2 ? argv[2] : "";
    if (!opt.empty() && opt != "--tokens" && opt != "--ast" && opt != "--tac" && opt != "--asm") {
        usage();
        return 1;
    }
    try {
        std::ifstream in(argv[1]);
        if (!in) {
            std::cerr << "Lexify: cannot open input file '" << argv[1] << "'\n";
            return 1;
        }
        std::string source((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());

        auto tokens = tokenize(source);
        if (opt == "--tokens") {
            for (auto& t : tokens)
                std::cout << "line " << t.line << "\t" << t.kind << " : " << escapeText(t.text) << "\n";
            return 0;
        }

        Program program = parse(tokens);
        if (opt == "--ast") {
            checkSemantics(program);
            std::cout << dumpProgram(program);
            return 0;
        }

        checkSemantics(program);
        if (opt == "--tac") {
            std::cout << generateCode(program);
            return 0;
        }
        if (opt == "--asm") {
            std::cout << generateAssembly(generateTAC(program), program);
            return 0;
        }
        return execute(program);
    } catch (const std::exception& e) {
        std::cout.flush();
        std::cerr << "Lexify: " << e.what() << "\n";
        return 1;
    }
}
