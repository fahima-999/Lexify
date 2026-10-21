#include "lexer.h"
#include "parser.h"
#include "semantic.h"
#include "interpreter.h"
#include "codegen.h"
#include "assembly.h"
#include <fstream>
#include <iostream>
#include <iterator>
#include <stdexcept>
int main(int argc,char** argv){if(argc<2){std::cerr<<"Usage: lexify_cpp <file.cpp> [--tokens|--tac|--asm]\n";return 1;}try{std::ifstream f(argv[1]);if(!f)throw std::runtime_error("Cannot open source file");std::string src((std::istreambuf_iterator<char>(f)),{});auto tokens=tokenize(src);std::string mode=argc>2?argv[2]:"";if(mode=="--tokens"){for(const auto& t:tokens)std::cout<<t.line<<" "<<t.type<<" "<<t.value<<'\n';return 0;}auto program=parse(tokens);checkSemantics(program);if(mode=="--tac"){std::cout<<generateTAC(program);return 0;}if(mode=="--asm"){std::cout<<pseudoAssembly(generateTAC(program));return 0;}if(!mode.empty())throw std::runtime_error("Unknown option: "+mode);execute(program);return 0;}catch(const std::exception& e){std::cerr<<"Lexify error: "<<e.what()<<'\n';return 1;}}
