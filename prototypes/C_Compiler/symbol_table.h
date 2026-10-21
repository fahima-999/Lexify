#pragma once
#include "ast.h"
#include <map>
#include <string>
#include <vector>

// One declared variable. Every declaration gets its own slot (no recursion exists,
// so a variable can be given one fixed storage location for the whole run).
struct Symbol {
    std::string name;
    std::string uniqueName;   // name used in TAC / assembly (x, x_1 for a shadowing x, ...)
    Type type = Type::Int;
    int slot = -1;
    int line = 0;
};

// Scoped symbol table: a stack of scopes, each mapping a name to a slot.
class SymbolTable {
    std::vector<std::map<std::string, int>> scopes;
    std::vector<Symbol> symbols;
    std::map<std::string, int> nameUse;

public:
    void enterScope();
    void exitScope();
    // Declares a variable in the innermost scope. Throws if it already exists there.
    int declare(const std::string& name, Type type, int line);
    // Returns the slot of the innermost visible declaration, or -1.
    int lookup(const std::string& name) const;
    const Symbol& symbol(int slot) const { return symbols.at(slot); }
    const std::vector<Symbol>& all() const { return symbols; }
    size_t depth() const { return scopes.size(); }
};
