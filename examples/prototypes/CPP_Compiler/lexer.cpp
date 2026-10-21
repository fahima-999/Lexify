#include "lexer.h"
#include <cctype>
#include <stdexcept>
std::vector<Token> tokenize(const std::string& s) {
 std::vector<Token> out; size_t i=0; int line=1;
 while(i<s.size()) {
  char c=s[i]; if(c=='\n'){++line;++i;continue;} if(isspace((unsigned char)c)){++i;continue;}
  if(c=='/'&&i+1<s.size()&&s[i+1]=='/'){while(i<s.size()&&s[i]!='\n')++i;continue;}
  if(c=='/'&&i+1<s.size()&&s[i+1]=='*'){i+=2;bool closed=false;while(i+1<s.size()){if(s[i]=='\n')++line;if(s[i]=='*'&&s[i+1]=='/'){i+=2;closed=true;break;}++i;}if(!closed)throw std::runtime_error("Unclosed comment");continue;}
  if(isalpha((unsigned char)c)||c=='_'){size_t start=i++;while(i<s.size()&&(isalnum((unsigned char)s[i])||s[i]=='_'))++i;std::string v=s.substr(start,i-start);out.push_back({(v=="int"||v=="double"||v=="cout"||v=="return"||v=="using"||v=="namespace"||v=="std"||v=="main")?"keyword":"identifier",v,line});continue;}
  if(isdigit((unsigned char)c)){size_t start=i++;while(i<s.size()&&(isdigit((unsigned char)s[i])||s[i]=='.'))++i;out.push_back({"number",s.substr(start,i-start),line});continue;}
  if(i+1<s.size()&&(s.substr(i,2)=="<<"||s.substr(i,2)=="::")){out.push_back({"symbol",s.substr(i,2),line});i+=2;continue;}
  if(std::string("+-*/=;(){}<>,").find(c)!=std::string::npos){out.push_back({"symbol",std::string(1,c),line});++i;continue;}
  throw std::runtime_error("Unexpected character at line "+std::to_string(line)+": "+c);
 }
 out.push_back({"eof","<eof>",line});return out;
}
