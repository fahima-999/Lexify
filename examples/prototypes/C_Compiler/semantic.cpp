#include "semantic.h"
#include <set>
#include <stdexcept>
static void checkExpr(const Expr& e,const std::set<std::string>& names){
 if(e.op=="variable"&&!names.count(e.value))throw std::runtime_error("Undefined variable: "+e.value);
 for(const auto& a:e.args)checkExpr(a,names);
}
void checkSemantics(const std::vector<Statement>& program){
 std::set<std::string> names;
 for(const auto& s:program){
  if(s.kind=="declare"&&names.count(s.name))throw std::runtime_error("Duplicate declaration: "+s.name);
  if(s.kind=="assign"&&!names.count(s.name))throw std::runtime_error("Assignment to undeclared variable: "+s.name);
  checkExpr(s.expr,names);
  if(s.kind=="declare")names.insert(s.name);
 }
}
