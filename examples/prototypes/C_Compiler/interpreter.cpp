#include "interpreter.h"
#include <iostream>
#include <stdexcept>
int evaluate(const Expr& e,const SymbolTable& symbols){
 if(e.op=="number") return std::stoi(e.value);
 if(e.op=="variable") {if(!symbols.contains(e.value)) throw std::runtime_error("Undefined variable: "+e.value); return symbols.get(e.value);}
 int a=evaluate(e.args.at(0),symbols), b=evaluate(e.args.at(1),symbols);
 if(e.op=="+")return a+b;
 if(e.op=="-")return a-b;
 if(e.op=="*")return a*b;
 if(e.op=="/"){if(b==0)throw std::runtime_error("Division by zero");return a/b;}
 throw std::runtime_error("Unknown operator");
}
void execute(const std::vector<Statement>& program){
 SymbolTable symbols;
 for(const auto& s:program){
  int result=evaluate(s.expr,symbols);
  if(s.kind=="print")std::cout<<result<<'\n';
  else {if(s.kind=="assign"&&!symbols.contains(s.name))throw std::runtime_error("Undefined variable: "+s.name);if(s.kind=="declare"&&symbols.contains(s.name))throw std::runtime_error("Duplicate variable: "+s.name);symbols.set(s.name,result);}
 }
}
