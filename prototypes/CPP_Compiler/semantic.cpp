#include "semantic.h"
#include <set>
#include <stdexcept>
static void checkExpr(const Expr* e,const std::set<std::string>& vars){if(!e)return;if(e->kind=="identifier"&&!vars.count(e->text))throw std::runtime_error("Undeclared variable: "+e->text);checkExpr(e->left.get(),vars);checkExpr(e->right.get(),vars);}
void checkSemantics(const Program& p){std::set<std::string> vars;for(auto& s:p.statements){if(s.kind=="declare"){if(vars.count(s.name))throw std::runtime_error("Duplicate declaration: "+s.name);checkExpr(s.expression.get(),vars);vars.insert(s.name);}else{if(s.kind=="assign"&&!vars.count(s.name))throw std::runtime_error("Undeclared variable: "+s.name);checkExpr(s.expression.get(),vars);}}}
