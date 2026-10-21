#pragma once
#include <memory>
#include <string>
#include <vector>
struct Expr {std::string kind,text;std::unique_ptr<Expr> left,right;};
struct Stmt {std::string kind,name,type;std::unique_ptr<Expr> expression;};
struct Program {std::vector<Stmt> statements;};
