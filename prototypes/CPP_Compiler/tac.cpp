#include "tac.h"
#include "ast.h"
#include <string>
#include <vector>

// Emits three-address code for one expression and returns the operand
// (a literal, a variable name or a fresh temporary) holding its value.
static std::string emit(const Expr* e, std::vector<std::string>& out, int& counter) {
    if (!e) return "0";
    if (e->kind == "number" || e->kind == "identifier") return e->text;
    std::string a = emit(e->left.get(), out, counter);
    std::string b = e->right ? emit(e->right.get(), out, counter) : "";
    std::string temp = "t" + std::to_string(++counter);
    out.push_back(temp + " = " + (e->kind == "unary" ? "-" + a : a + " " + e->text + " " + b));
    return temp;
}

std::vector<std::string> generateTAC(const Program& program) {
    std::vector<std::string> out;
    int counter = 0;
    for (const auto& s : program.statements) {
        std::string v = emit(s.expression.get(), out, counter);
        if (s.kind == "declare") out.push_back("declare " + s.type + " " + s.name);
        if ((s.kind == "declare" && s.expression) || s.kind == "assign") out.push_back(s.name + " = " + v);
        if (s.kind == "print") out.push_back("print " + v);
        if (s.kind == "return") out.push_back("return " + v);
    }
    return out;
}
