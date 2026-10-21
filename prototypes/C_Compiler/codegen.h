#pragma once
#include "ast.h"
#include <string>

// Generates the intermediate code (three-address code) of a program as text.
std::string generateCode(const Program& program);
