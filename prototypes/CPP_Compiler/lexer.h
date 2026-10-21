#pragma once
#include <string>
#include <vector>
struct Token {std::string type, value; int line;};
std::vector<Token> tokenize(const std::string& source);
