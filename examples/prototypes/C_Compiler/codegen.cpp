#include "codegen.h"
#include "tac.h"
std::string generateCode(const std::vector<Statement>& program){std::string s;for(auto& line:generateTAC(program))s+=line+"\n";return s;}
