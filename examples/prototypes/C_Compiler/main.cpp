#include "lexer.h"
#include "parser.tab.h"
#include "semantic.h"
#include "interpreter.h"
#include "codegen.h"
#include <fstream>
#include <iostream>
#include <iterator>
#include <stdexcept>
int main(int argc,char** argv){
 if(argc<2){std::cerr<<"Usage: lexify <file.c> [--tokens|--tac]\n";return 1;}
 try{
  std::ifstream in(argv[1]);if(!in)throw std::runtime_error("Cannot open input file");
  std::string source((std::istreambuf_iterator<char>(in)),{});auto tokens=tokenize(source);
  if(argc>2&&std::string(argv[2])=="--tokens"){for(auto& t:tokens)std::cout<<t.kind<<" : "<<t.text<<"\n";return 0;}
  auto program=parse(tokens);checkSemantics(program);
  if(argc>2&&std::string(argv[2])=="--tac"){std::cout<<generateCode(program);return 0;}
  execute(program);return 0;
 }catch(const std::exception& e){std::cerr<<"Lexify error: "<<e.what()<<"\n";return 1;}
}
