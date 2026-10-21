#include "parser.h"
#include <stdexcept>
#include <utility>
class Parser {
 const std::vector<Token>& t; size_t i=0;
 bool at(const std::string& s) const{return t[i].value==s;}
 bool accept(const std::string& s){if(at(s)){++i;return true;}return false;}
 void expect(const std::string& s){if(!accept(s))throw std::runtime_error("Line "+std::to_string(t[i].line)+": expected '"+s+"', got '"+t[i].value+"'");}
 std::string identifier(){if(t[i].type!="identifier")throw std::runtime_error("Expected identifier at line "+std::to_string(t[i].line));return t[i++].value;}
 std::unique_ptr<Expr> atom(){if(accept("(")){auto e=expr();expect(")");return e;}if(accept("-")){auto n=std::make_unique<Expr>();n->kind="unary";n->text="-";n->left=atom();return n;}if(t[i].type=="number"||t[i].type=="identifier"){auto n=std::make_unique<Expr>();n->kind=t[i].type;n->text=t[i++].value;return n;}throw std::runtime_error("Expected expression at line "+std::to_string(t[i].line));}
 std::unique_ptr<Expr> product(){auto e=atom();while(at("*")||at("/")){auto n=std::make_unique<Expr>();n->kind="binary";n->text=t[i++].value;n->left=std::move(e);n->right=atom();e=std::move(n);}return e;}
 std::unique_ptr<Expr> expr(){auto e=product();while(at("+")||at("-")){auto n=std::make_unique<Expr>();n->kind="binary";n->text=t[i++].value;n->left=std::move(e);n->right=product();e=std::move(n);}return e;}
 public: explicit Parser(const std::vector<Token>& ts):t(ts){}
 Program run(){
  if(accept("using")){expect("namespace");expect("std");expect(";");}
  expect("int");expect("main");expect("(");expect(")");expect("{");Program p;
  while(!at("}")&&!at("<eof>")){
   Stmt s;
   if(at("int")||at("double")){s.kind="declare";s.type=t[i++].value;s.name=identifier();if(accept("="))s.expression=expr();expect(";");}
   else if(accept("cout")){s.kind="print";expect("<<");s.expression=expr();expect(";");}
   else if(accept("return")){s.kind="return";s.expression=expr();expect(";");}
   else {s.kind="assign";s.name=identifier();expect("=");s.expression=expr();expect(";");}
   p.statements.push_back(std::move(s));
  }
  expect("}");if(!at("<eof>"))throw std::runtime_error("Unexpected input after main");return p;
 }
};
Program parse(const std::vector<Token>& t){return Parser(t).run();}
