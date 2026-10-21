#pragma once
#include "ast.h"
#include <string>
#include <vector>

// Converts C source text into tokens (the last token has kind "EOF").
// Handles comments, #include (ignored), simple object-like #define macros,
// int/float/char literals and string literals (only usable inside printf/scanf).
std::vector<Token> tokenize(const std::string& source);
