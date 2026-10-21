#pragma once
#include "ast.h"
#include "symbol_table.h"
#include <vector>
int evaluate(const Expr& e,const SymbolTable& symbols);
void execute(const std::vector<Statement>& program);
