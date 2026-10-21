#include "codegen.h"
#include "tac.h"

std::string generateCode(const Program& program) {
    std::string s;
    for (const auto& instr : generateTAC(program)) s += formatTAC(instr) + "\n";
    return s;
}
