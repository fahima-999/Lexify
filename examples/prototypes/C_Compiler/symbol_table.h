#pragma once
#include <map>
#include <string>
class SymbolTable {
 std::map<std::string,int> values;
public:
 bool contains(const std::string& n) const { return values.count(n)>0; }
 void set(const std::string& n,int v){values[n]=v;}
 int get(const std::string& n) const {return values.at(n);}
};
