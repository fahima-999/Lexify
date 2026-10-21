#include "symbol_table.h"
#include <cctype>

void SymbolTable::enterScope() { scopes.emplace_back(); }

void SymbolTable::exitScope() {
    if (!scopes.empty()) scopes.pop_back();
}

// temporaries are called t1, t2 ... and labels L1, L2 ...; keep user names away from them
static bool looksLikeCompilerName(const std::string& n) {
    if (n.size() < 2 || (n[0] != 't' && n[0] != 'L')) return false;
    for (size_t i = 1; i < n.size(); ++i)
        if (!std::isdigit((unsigned char)n[i])) return false;
    return true;
}

int SymbolTable::declare(const std::string& name, Type type, int line) {
    if (scopes.empty()) enterScope();
    auto& cur = scopes.back();
    auto it = cur.find(name);
    if (it != cur.end()) {
        semanticError(line, "redeclaration of '" + name + "' in the same scope (first declared on line " +
                                std::to_string(symbols[it->second].line) + ")");
    }
    Symbol s;
    s.name = name;
    s.type = type;
    s.line = line;
    s.slot = (int)symbols.size();
    std::string base = looksLikeCompilerName(name) ? name + "_v" : name;
    int n = nameUse[base]++;
    s.uniqueName = n == 0 ? base : base + "_" + std::to_string(n);
    symbols.push_back(s);
    cur[name] = s.slot;
    return s.slot;
}

int SymbolTable::lookup(const std::string& name) const {
    for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
        auto f = it->find(name);
        if (f != it->end()) return f->second;
    }
    return -1;
}
