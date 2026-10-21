#pragma once
#include <map>
#include <string>
struct Symbol {std::string type;double value=0;};
using SymbolTable=std::map<std::string,Symbol>;
