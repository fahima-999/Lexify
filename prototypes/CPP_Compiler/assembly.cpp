#include "assembly.h"
#include <sstream>
#include <vector>
static std::string opcode(const std::string& op){
 if(op=="+")return "ADD";
 if(op=="-")return "SUB";
 if(op=="*")return "MUL";
 return "DIV";
}
std::string pseudoAssembly(const std::string& tac){
 std::istringstream in(tac);std::string line,result="; Educational pseudo assembly (not native machine code)\n";
 while(std::getline(in,line)){
  std::istringstream words(line);std::vector<std::string> w;std::string x;while(words>>x)w.push_back(x);
  if(w.empty())continue;
  if(w[0]=="declare"&&w.size()==3)result+="    DECL "+w[1]+" "+w[2]+"\n";
  else if(w[0]=="print"&&w.size()==2)result+="    PRINT "+w[1]+"\n";
  else if(w[0]=="return"&&w.size()==2)result+="    RET "+w[1]+"\n";
  else if(w.size()==3)result+="    MOV "+w[0]+", "+w[2]+"\n";
  else if(w.size()==5)result+="    "+opcode(w[3])+" "+w[0]+", "+w[2]+", "+w[4]+"\n";
  else result+="    ; "+line+"\n";
 }
 return result;
}
