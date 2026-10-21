#include "parser.tab.h"
#include <stdexcept>
class Parser {
 const std::vector<Token>& ts;size_t i=0;
 const Token& peek()const{return ts.at(i);}
 bool match(const std::string& k){if(peek().kind==k){++i;return true;}return false;}
 Token need(const std::string& k){if(peek().kind!=k)throw std::runtime_error("Expected '"+k+"' at line "+std::to_string(peek().line));return ts.at(i++);}
 Expr factor(){
  if(match("(")){auto e=expression();need(")");return e;}
  if(match("-")){auto e=factor();return Expr{"-","",{Expr{"number","0",{}},e}};}
  if(peek().kind=="NUM")return Expr{"number",need("NUM").text,{}};
  if(peek().kind=="ID")return Expr{"variable",need("ID").text,{}};
  throw std::runtime_error("Expected expression at line "+std::to_string(peek().line));
 }
 Expr term(){auto a=factor();while(peek().kind=="*"||peek().kind=="/"){auto op=ts.at(i++).kind;auto b=factor();a=Expr{op,"",{a,b}};}return a;}
 Expr expression(){auto a=term();while(peek().kind=="+"||peek().kind=="-"){auto op=ts.at(i++).kind;auto b=term();a=Expr{op,"",{a,b}};}return a;}
public:
 Parser(const std::vector<Token>& t):ts(t){}
 std::vector<Statement> program(){std::vector<Statement> out;while(peek().kind!="EOF"){
  if(match("int")){auto name=need("ID").text;need("=");auto e=expression();need(";");out.push_back({"declare",name,e});}
  else if(match("print")){need("(");auto e=expression();need(")");need(";");out.push_back({"print","",e});}
  else {auto name=need("ID").text;need("=");auto e=expression();need(";");out.push_back({"assign",name,e});}
 }return out;}
};
std::vector<Statement> parse(const std::vector<Token>& tokens){return Parser(tokens).program();}
