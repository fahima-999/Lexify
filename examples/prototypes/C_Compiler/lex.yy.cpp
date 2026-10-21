#include "lexer.h"
#include <cctype>
#include <stdexcept>
std::vector<Token> tokenize(const std::string& s){
 std::vector<Token> out;int line=1;
 for(size_t i=0;i<s.size();){
  char ch=s[i];if(ch=='\n'){++line;++i;continue;}if(std::isspace((unsigned char)ch)){++i;continue;}
  if(ch=='/'&&i+1<s.size()&&s[i+1]=='/'){while(i<s.size()&&s[i]!='\n')++i;continue;}
  if(std::isalpha((unsigned char)ch)||ch=='_'){
   size_t j=i++;while(i<s.size()&&(std::isalnum((unsigned char)s[i])||s[i]=='_'))++i;
   auto word=s.substr(j,i-j);out.push_back({word=="int"||word=="print"?word:"ID",word,line});continue;
  }
  if(std::isdigit((unsigned char)ch)){
   size_t j=i++;while(i<s.size()&&std::isdigit((unsigned char)s[i]))++i;
   out.push_back({"NUM",s.substr(j,i-j),line});continue;
  }
  if(std::string("+-*/=();").find(ch)!=std::string::npos){out.push_back({std::string(1,ch),std::string(1,ch),line});++i;continue;}
  throw std::runtime_error("Unexpected character at line "+std::to_string(line)+": "+ch);
 }
 out.push_back({"EOF","",line});return out;
}
