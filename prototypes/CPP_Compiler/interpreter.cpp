#include "interpreter.h"
#include "symbol_table.h"
#include <iostream>
#include <stdexcept>
struct Val{double v;bool isInt;};
static Val evaluate(const Expr* e,const SymbolTable& vars){
 if(!e)return {0,true};
 if(e->kind=="number")return {std::stod(e->text),e->text.find('.')==std::string::npos};
 if(e->kind=="identifier"){const auto& x=vars.at(e->text);return {x.value,x.type=="int"};}
 if(e->kind=="unary"){Val a=evaluate(e->left.get(),vars);return {-a.v,a.isInt};}
 Val a=evaluate(e->left.get(),vars),b=evaluate(e->right.get(),vars);
 bool isInt=a.isInt&&b.isInt;
 if(e->text=="+")return {a.v+b.v,isInt};
 if(e->text=="-")return {a.v-b.v,isInt};
 if(e->text=="*")return {a.v*b.v,isInt};
 if(b.v==0)throw std::runtime_error("Division by zero");
 if(isInt)return {static_cast<double>(static_cast<long long>(a.v)/static_cast<long long>(b.v)),true};
 return {a.v/b.v,false};
}
void execute(const Program& p){SymbolTable vars;for(auto& s:p.statements){if(s.kind=="declare"){double v=evaluate(s.expression.get(),vars).v;if(s.type=="int")v=static_cast<long long>(v);vars[s.name]={s.type,v};}else if(s.kind=="assign"){double v=evaluate(s.expression.get(),vars).v;if(vars.at(s.name).type=="int")v=static_cast<long long>(v);vars.at(s.name).value=v;}else if(s.kind=="print")std::cout<<evaluate(s.expression.get(),vars).v<<'\n';else if(s.kind=="return")return;}}
