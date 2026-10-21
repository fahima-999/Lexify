#include "assembly.h"
#include <sstream>
std::string assemblyComment(const std::string& tac){std::istringstream in(tac);std::string line,result;while(std::getline(in,line))result+="; "+line+"\n";return result;}
