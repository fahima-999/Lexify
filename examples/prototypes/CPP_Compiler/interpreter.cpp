#include "interpreter.h"
#include "symbol_table.h"
#include <iostream>
#include <stdexcept>
static double evaluate(const Expr* e,const SymbolTable& vars){if(!e)return 0;if(e->kind=="number")return std::stod(e->text);if(e->kind=="identifier")return vars.at(e->text).value;if(e->kind=="unary")return -evaluate(e->left.get(),vars);double a=evaluate(e->left.get(),vars),b=evaluate(e->right.get(),vars);if(e->text=="+")return a+b;if(e->text=="-")return a-b;if(e->text=="*")return a*b;if(b==0)throw std::runtime_error("Division by zero");return a/b;}
void execute(const Program& p){SymbolTable vars;for(auto& s:p.statements){if(s.kind=="declare"){double v=evaluate(s.expression.get(),vars);if(s.type=="int")v=static_cast<long long>(v);vars[s.name]={s.type,v};}else if(s.kind=="assign"){double v=evaluate(s.expression.get(),vars);if(vars.at(s.name).type=="int")v=static_cast<long long>(v);vars.at(s.name).value=v;}else if(s.kind=="print")std::cout<<evaluate(s.expression.get(),vars)<<'\n';else if(s.kind=="return")return;}}
