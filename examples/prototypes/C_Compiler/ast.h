#pragma once
#include <string>
#include <vector>
struct Token { std::string kind,text; int line; };
struct Expr { std::string op,value; std::vector<Expr> args; };
struct Statement { std::string kind,name; Expr expr; };
