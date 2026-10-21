#pragma once
#include "ast.h"
#include <string>
#include <vector>

// Three-address code.
enum class TacKind { Assign, Binary, Unary, Cast, Label, Goto, IfFalse, IfTrue, Print, Printf, Scanf, Return };

struct TacInstr {
    TacKind kind = TacKind::Assign;
    std::string op;                 // Binary/Unary: operator text. Cast: target type name
    std::string a, b;               // operands (Printf/Scanf: a = format string)
    std::string res;                // destination variable / temporary, or label name
    std::vector<std::string> args;  // Printf: argument operands. Scanf: target variable names
};

std::vector<TacInstr> generateTAC(const Program& program);
std::string formatTAC(const TacInstr& instr);
