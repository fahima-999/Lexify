#pragma once
#include "ast.h"

// Runs a checked program (tree-walking interpreter).
// Returns the value given to `return` in main (0 if the program just ends).
int execute(const Program& program);
