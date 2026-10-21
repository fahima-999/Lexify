#pragma once
#include "ast.h"

// Semantic analysis: resolves every variable to a slot, computes expression types and
// reports errors (undeclared variable, redeclaration, bad operands, break outside loop,
// wrong printf/scanf format ...). Fills in program.varNames / program.varTypes.
void checkSemantics(Program& program);
