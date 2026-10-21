#pragma once
#include "ast.h"
#include "tac.h"
#include <string>
#include <vector>

// Translates TAC into x86-like pseudo assembly (NASM style, 32-bit integer view) for learning.
std::string generateAssembly(const std::vector<TacInstr>& tac, const Program& program);



//picchi
