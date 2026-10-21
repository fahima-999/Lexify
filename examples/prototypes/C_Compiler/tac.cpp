#include "tac.h"
static std::string emit(const Expr& e,std::vector<std::string>& out,int& counter){
 if(e.op=="number"||e.op=="variable")return e.value;
 auto a=emit(e.args[0],out,counter),b=emit(e.args[1],out,counter);
 auto temp="t"+std::to_string(++counter);
 out.push_back(temp+" = "+a+" "+e.op+" "+b);return temp;
}
std::vector<std::string> generateTAC(const std::vector<Statement>& program){
 std::vector<std::string> out;int counter=0;
 for(const auto& s:program){auto v=emit(s.expr,out,counter);out.push_back(s.kind=="print"?"print "+v:s.name+" = "+v);}
 return out;
}
